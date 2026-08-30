#if !defined(NNS_FINALROM)

#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/mcs/baseCommon.h>
#include <nnsys/mcs/base.h>
#include <nnsys/mcs/ringBuffer.h>
#include <nnsys/mcs/config.h>
#ifndef SDK_TWLLTD
#include <isdbglib.h>
#endif
#include "istd_stubi.h"
#include "baseCommoni.h"
#include "basei.h"

static const int SEND_RETRY_MAX = 10;
static const int UIC_WAIT_TIMEOUT_FRAME = 60 * 2;

typedef struct DefRecvCBInfo DefRecvCBInfo;
struct DefRecvCBInfo {
  NNSMcsRecvCBInfo cbInfo;
  NNSMcsRingBuffer ringBuf;
};

typedef struct NNSiMcsEnsWork NNSiMcsEnsWork;
struct NNSiMcsEnsWork {
  NNSMcsRingBuffer rdRb;
  NNSMcsRingBuffer wrRb;
  NNSiMcsEnsMsgBuf msgBuf;
};

static NNSiMcsWork *spMcsWork = NULL; // Becomes non-NULL if initialized

static NNSMcsDeviceCaps sDeviceCaps = {
    NITRODEVID_NULL, // Device recognition ID
    0x00000000,      // Resource identification information flag
};

static NNS_MCS_INLINE BOOL IsInitialized() { return spMcsWork != NULL; }

static NNS_MCS_INLINE uint32_t RoundUp(uint32_t value, uint32_t alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

static NNS_MCS_INLINE uint32_t RoundDown(uint32_t value, uint32_t alignment) {
  return value & ~(alignment - 1);
}

static NNS_MCS_INLINE u32 min_u32(u32 a, u32 b) { return a < b ? a : b; }

static NNS_MCS_INLINE void Lock() {
  if (OS_IsThreadAvailable()) {
    OS_LockMutex(&spMcsWork->mutex);
  }
}

static NNS_MCS_INLINE void Unlock() {
  if (OS_IsThreadAvailable()) {
    OS_UnlockMutex(&spMcsWork->mutex);
  }
}

static NNSMcsRecvCBInfo *GetRecvCBInfo(NNSFndList *pList, uint32_t channel) {
  NNSMcsRecvCBInfo *pInfo = NULL;

  while (NULL != (pInfo = NNS_FndGetNextListObject(pList, pInfo))) {
    if (pInfo->channel == channel) {
      return pInfo;
    }
  }

  return NULL;
}

static void CallbackRecv(u32 userData, u32 channel, const void *pRecv,
                         uint32_t recvSize) {
  const NNSiMcsMsg *const pMsg = (NNSiMcsMsg *)pRecv;
  const u32 dataSize = recvSize - NNSi_MCS_MSG_HEAD_SIZE;

  Lock();

  if (recvSize < NNSi_MCS_MSG_HEAD_SIZE ||
      NNSi_MCS_MSG_HEAD_VERSION != pMsg->head.version) {

    spMcsWork->bProtocolError = TRUE;
  } else {
    if (NNSi_MCS_SYSMSG_CHANNEL == channel) {
      u32 bConnect;

      if (dataSize == sizeof(bConnect)) {
        bConnect = *(u32 *)pMsg->data;
        spMcsWork->bHostDataRecived = (u8)bConnect;

      } else {
      }
    } else {
      NNSFndList *const pList = (NNSFndList *)userData;
      NNSMcsRecvCBInfo *const pInfo = GetRecvCBInfo(pList, channel);

      if (pInfo) {
        (*pInfo->cbFunc)(pMsg->data, dataSize, pInfo->userData,
                         pMsg->head.offset, pMsg->head.totalLen);
      }
    }
  }

  Unlock();
}

static void CallbackRecvDummy(uintptr_t /* userData */, uint32_t /* channel */,
                              const void * /* pRecv */, uint32_t /* recvSize */
) {}

static void DataRecvCallback(const void *pData, u32 dataSize, u32 userData,
                             u32 offset, u32 totalSize) {
  DefRecvCBInfo *cbInfo = (DefRecvCBInfo *)userData;

  if (NNS_McsGetRingBufferWritableBytes(cbInfo->ringBuf) < offset + dataSize) {

    NNS_WARNING(FALSE,
                "NNS mcs error : buffer is not enough. writable %d, data size "
                "%d, offset %d, total size %d\n",
                NNS_McsGetRingBufferWritableBytes(cbInfo->ringBuf), dataSize,
                offset, totalSize);
    return;
  }

  {
    BOOL ret = NNS_McsWriteRingBufferEx(cbInfo->ringBuf, pData, offset,
                                        dataSize, totalSize);
    NNS_ASSERT(ret);
  }
}

static NNS_MCS_INLINE NNSiMcsEnsWork *GetEnsWorkPtr() {
  return (NNSiMcsEnsWork *)NNSi_MCS_ENS_WRITE_RB_END;
}

static void SetMaskResource(u32 maskResource) {
  const BOOL preIRQ = OS_DisableIrq();
  sDeviceCaps.maskResource = maskResource;
  (void)OS_RestoreIrq(preIRQ);
}

static void WaitSendData(void) {
  NNS_McsPollingIdle(); // Set so that the PC-side is not blocked by the write
                        // wait

  NNSi_McsSleep(8);
}

static void WaitDeviceEnable(void) {
  NNS_McsPollingIdle();

  NNSi_McsSleep(16);
}

int _isdbusmgr_isresourceavailable(int /*typeResource*/);

__declspec(weak) int _isdbusmgr_isresourceavailable(int /*typeResource*/) {
  return FALSE;
}

static BOOL IsDebuggerPresent() {
  static const int ISDRESOURCE_SW_DEBUGGER = 0x00200;

  return _isdbusmgr_isresourceavailable(ISDRESOURCE_SW_DEBUGGER);
}

static NNS_MCS_INLINE BOOL IsRunOnTwlHw() {
  return OS_IsRunOnTwl() || OSi_IsNitroModeOnTwl();
}

static NNS_MCS_INLINE BOOL IsRunOnEnsata() {
#ifdef SDK_TWL
  return FALSE; // Ensata will run only for programs built for NITRO
#else
  return OS_IsRunOnEmulator();
#endif
}

static BOOL Open_Ensata(NNSMcsDeviceCaps *pCaps) {
#ifndef SDK_TWL
  const NNSMcsRingBuffer rdRb = (NNSMcsRingBuffer)NNSi_MCS_ENS_READ_RB_START;
  const NNSMcsRingBuffer wrRb = (NNSMcsRingBuffer)NNSi_MCS_ENS_WRITE_RB_START;

  if (!NNS_McsCheckRingBuffer(rdRb) || !NNS_McsCheckRingBuffer(wrRb)) {
    return FALSE;
  }

  {
    NNSiMcsEnsWork *pWork = GetEnsWorkPtr();
    pWork->rdRb = rdRb;
    pWork->wrRb = wrRb;
  }

  pCaps->deviceID = 0;
  pCaps->maskResource = NITROMASK_RESOURCE_POLL;

  return TRUE;
#else
  (void)pCaps;
  return FALSE;
#endif
}

static BOOL Open_NITRO(NNSMcsDeviceCaps *pCaps) {
#ifndef SDK_TWLLTD

  enum {
    DEV_PRI_NONE,     // No device
    DEV_PRI_UNKNOWN,  // Unknown device
    DEV_PRI_NITRODBG, // IS-NITRO-DEBUGGER
    DEV_PRI_NITROUIC, // IS-NITRO-UIC
    DEV_PRI_NITROUSB, // IS-NITRO-EMULATOR

    DEV_PRI_MAX
  };

  int devPri = DEV_PRI_NONE;
  int devID = 0;
  const int devNum = NNS_McsGetMaxCaps();
  int findDevID = 0;
  const NITRODEVCAPS *pNITROCaps;

  for (findDevID = 0; findDevID < devNum; ++findDevID) {
    int findDevPri = DEV_PRI_NONE;
    pNITROCaps = NITROToolAPIGetDeviceCaps(findDevID);
    switch (pNITROCaps->m_nDeviceID) {
    case NITRODEVID_NITEMULATOR: // IS-NITRO-EMULATOR
      if (IsDebuggerPresent()) {
        findDevPri = DEV_PRI_NITRODBG;
      } else // When not starting from IS-NITRO-DEBUGGER software
      {
        findDevPri = DEV_PRI_NITROUSB;
      }
      break;
    case NITRODEVID_NITUIC: // IS-NITRO-UIC CARTRIDGE
      findDevPri = DEV_PRI_NITROUIC;
      break;
    default:
      findDevPri = DEV_PRI_UNKNOWN;
    }

    if (devPri < findDevPri) {
      devPri = findDevPri;
      devID = findDevID;
    }
  }

  if (devPri == DEV_PRI_NONE || devPri == DEV_PRI_UNKNOWN) {
    OS_Warning("no device.\n"); // No connectable device found
    return FALSE;
  }

  pNITROCaps = NITROToolAPIGetDeviceCaps(devID);
  if (!NITROToolAPIOpen(pNITROCaps)) {
    return FALSE; // Failed to initialize and open the device
  }

  pCaps->deviceID = pNITROCaps->m_nDeviceID;
  pCaps->maskResource = pNITROCaps->m_dwMaskResource;
  spMcsWork->bLengthEnable = FALSE;

  {

    BOOL bSuccess = NITROToolAPISetReceiveStreamCallBackFunction(
        CallbackRecv, (u32)&spMcsWork->recvCBInfoList);
    NNS_ASSERT(bSuccess);
  }

  return TRUE;
#else
  (void)pCaps;
  return FALSE;
#endif
}

static BOOL Open_TWL(NNSMcsDeviceCaps *pCaps) {
  u32 nDevMask;

  ISTDSIOSetRecvCallback((ISTDSIORecvCbFunc)CallbackRecv,
                         (void *)&spMcsWork->recvCBInfoList);

  nDevMask = ISTDHIOGetDevMask();
  if (nDevMask == 0 || !ISTDHIOOpen(nDevMask)) {
    OS_Printf(" ISHIO: device open failure. - DevMask %08X\n", nDevMask);
    return FALSE;
  }

  pCaps->deviceID = 0;
  pCaps->maskResource = NITROMASK_RESOURCE_POLL;

  return TRUE;
}

static NNS_MCS_INLINE BOOL Close_Ensata() {
#ifndef SDK_TWL
  NNSiMcsEnsWork *pWork = GetEnsWorkPtr();
  pWork->rdRb = 0;
  pWork->wrRb = 0;

  return TRUE;
#else
  return FALSE;
#endif
}

static BOOL Close_NITRO() {
#ifndef SDK_TWLLTD
  return NITROToolAPIClose();
#else
  return FALSE;
#endif
}

static BOOL Close_TWL() { return ISTDHIOClose(); }

static void Init_TWL() {
  OS_InitTick();
  OS_InitAlarm();

  ISTDHIOInit();
}

static void Init_Ensata() {}

static void Init_NITRO() {
#ifndef SDK_TWLLTD
  NITROToolAPIInit();
#endif
}

static void Init_InVariousDevice() {
  if (IsRunOnTwlHw())
    Init_TWL();
  else if (IsRunOnEnsata())
    Init_Ensata();
  else
    Init_NITRO();
}

void NNS_McsInit(void *workMem) {
  if (IsInitialized()) {
    return;
  }

  NNS_NULL_ASSERT(workMem);

  Init_InVariousDevice();

  spMcsWork = (NNSiMcsWork *)workMem;

  spMcsWork->bProtocolError = FALSE;
  spMcsWork->bLengthEnable = FALSE;
  spMcsWork->bHostDataRecived = FALSE;
  OS_InitMutex(&spMcsWork->mutex);
  NNS_FND_INIT_LIST(&spMcsWork->recvCBInfoList, NNSMcsRecvCBInfo, link);
}

void NNS_McsFinalize() {
  if (!IsInitialized()) {
    return;
  }

  spMcsWork = NULL;
}

static int GetMaxCaps_TWL() { return 1; }

static int GetMaxCaps_Ensata() { return 1; }

static int GetMaxCaps_NITRO() {
#ifndef SDK_TWLLTD
  return NITROToolAPIGetMaxCaps();
#else
  return 0;
#endif
}

static int GetMaxCaps_InVariousDevice() {
  if (IsRunOnTwlHw())
    return GetMaxCaps_TWL();
  else if (IsRunOnEnsata())
    return GetMaxCaps_Ensata();
  else
    return GetMaxCaps_NITRO();
}

int NNS_McsGetMaxCaps() {
  int num;

  NNS_ASSERT(IsInitialized());
  Lock();

  num = GetMaxCaps_InVariousDevice();

  Unlock();
  return num;
}

static BOOL Open_InVariousDevice(NNSMcsDeviceCaps *pCaps) {
  if (IsRunOnTwlHw())
    return Open_TWL(pCaps);
  else if (IsRunOnEnsata())
    return Open_Ensata(pCaps);
  else
    return Open_NITRO(pCaps);
}

BOOL NNS_McsOpen(NNSMcsDeviceCaps *pCaps) {
  BOOL bSuccess;

  NNS_ASSERT(IsInitialized());
  Lock();

  bSuccess = Open_InVariousDevice(pCaps);

  if (bSuccess) {
    sDeviceCaps.deviceID = pCaps->deviceID;
    SetMaskResource(pCaps->maskResource);

    spMcsWork->bHostDataRecived = FALSE;
  }

  Unlock();
  return bSuccess;
}

static BOOL Close_InVariousDevice() {
  if (IsRunOnTwlHw())
    return Close_TWL();
  else if (IsRunOnEnsata())
    return Close_Ensata();
  else
    return Close_NITRO();
}

BOOL NNS_McsClose() {
  BOOL bSuccess;

  NNS_ASSERT(IsInitialized());
  Lock();

  bSuccess = Close_InVariousDevice();

  if (bSuccess) {
    sDeviceCaps.deviceID =
        NITRODEVID_NULL;         // Initializes the device identification ID
    SetMaskResource(0x00000000); // Flag initialization of resource
                                 // identification information
  }

  Unlock();
  return bSuccess;
}

void NNS_McsRegisterRecvCallback(NNSMcsRecvCBInfo *pInfo, u16 channel,
                                 NNSMcsRecvCallback cbFunc, u32 userData) {
  NNS_ASSERT(IsInitialized());
  Lock();

  NNS_ASSERT(
      NULL ==
      GetRecvCBInfo(&spMcsWork->recvCBInfoList,
                    channel)); // The same channel should not already be in use

  pInfo->channel = channel;
  pInfo->cbFunc = cbFunc;
  pInfo->userData = userData;

  NNS_FndAppendListObject(&spMcsWork->recvCBInfoList, pInfo);

  Unlock();
}

void NNS_McsRegisterStreamRecvBuffer(u16 channel, void *buf, u32 bufSize) {
  uintptr_t start = (uintptr_t)buf;
  uintptr_t end = start + bufSize;
  u8 *pBBuf;
  DefRecvCBInfo *pInfo;

  NNS_ASSERT(IsInitialized());
  Lock();

  start = RoundUp(start, 4);
  end = RoundDown(end, 4);

  NNS_ASSERT(start < end);

  buf = (void *)start;
  bufSize = end - start;

  NNS_ASSERT(bufSize >= sizeof(DefRecvCBInfo) +
                            sizeof(NNSiMcsRingBufferHeader) + sizeof(uint32_t) +
                            sizeof(uint32_t) + sizeof(uint32_t));

  pBBuf = buf;
  pInfo = buf;

  pInfo->ringBuf = NNS_McsInitRingBuffer(pBBuf + sizeof(DefRecvCBInfo),
                                         bufSize - sizeof(DefRecvCBInfo));

  NNS_McsRegisterRecvCallback(&pInfo->cbInfo, channel, DataRecvCallback,
                              (uintptr_t)pInfo);

  Unlock();
}

void NNS_McsUnregisterRecvResource(u16 channel) {
  NNSMcsRecvCBInfo *pInfo = NULL;

  NNS_ASSERT(IsInitialized());
  Lock();

  pInfo = GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel);
  NNS_ASSERT(pInfo);

  NNS_FndRemoveListObject(&spMcsWork->recvCBInfoList, pInfo);

  Unlock();
}

u32 NNS_McsGetStreamReadableSize(u16 channel) {
  u32 size;

  NNS_ASSERT(IsInitialized());
  Lock();

  {
    DefRecvCBInfo *pCBInfo =
        (DefRecvCBInfo *)GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel)
            ->userData;

    NNS_ASSERT(pCBInfo);

    size = NNS_McsGetRingBufferReadableBytes(pCBInfo->ringBuf);
  }

  Unlock();
  return size;
}

u32 NNS_McsGetTotalStreamReadableSize(u16 channel) {
  u32 size;

  NNS_ASSERT(IsInitialized());
  Lock();

  {
    DefRecvCBInfo *pCBInfo =
        (DefRecvCBInfo *)GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel)
            ->userData;

    NNS_ASSERT(pCBInfo);

    size = NNS_McsGetRingBufferTotalReadableBytes(pCBInfo->ringBuf);
  }

  Unlock();
  return size;
}

BOOL NNS_McsReadStream(u16 channel, void *data, u32 size, u32 *pReadSize) {
  BOOL bSuccess;

  NNS_ASSERT(IsInitialized());
  Lock();

  {
    DefRecvCBInfo *pCBInfo =
        (DefRecvCBInfo *)GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel)
            ->userData;

    NNS_ASSERT(pCBInfo);

    bSuccess = NNS_McsReadRingBuffer(pCBInfo->ringBuf, data, size, pReadSize);
  }

  Unlock();
  return bSuccess;
}

static BOOL GetStreamWritableLength_TWL(u32 *pLength) {
  *pLength = ISTDSIO_MAX_PAYLOAD_SIZE;
  return TRUE;
}

static BOOL GetStreamWritableLength_Ensata(u32 *pLength) {
#ifndef SDK_TWL
  *pLength = NNS_McsGetRingBufferWritableBytes(GetEnsWorkPtr()->wrRb);
  return TRUE;
#else
  (void)pLength;
  return FALSE;
#endif
}

static BOOL GetStreamWritableLength_NITRO(u32 *pLength) {
#ifndef SDK_TWLLTD
  BOOL ret = FALSE;
  u32 i;

  for (i = 0; i < UIC_WAIT_TIMEOUT_FRAME; ++i) {
    ret = 0 != NITROToolAPIStreamGetWritableLength(pLength);

    if (!ret) {
      OS_Printf("NNS Mcs error: faild NITROToolAPIStreamGetWritableLength()\n");
      break;
    }

    if (spMcsWork->bLengthEnable) {
      break;
    }

    if (*pLength > 0) {
      spMcsWork->bLengthEnable = TRUE;
      break;
    }

    WaitDeviceEnable();
  }

  return ret;
#else
  (void)pLength;
  return FALSE;
#endif
}

static BOOL GetStreamWritableLength_InVariousDevice(u32 *pLength) {
  if (IsRunOnTwlHw())
    return GetStreamWritableLength_TWL(pLength);
  else if (IsRunOnEnsata())
    return GetStreamWritableLength_Ensata(pLength);
  else
    return GetStreamWritableLength_NITRO(pLength);
}

BOOL NNS_McsGetStreamWritableLength(u32 *pLength) {
  BOOL ret = FALSE;
  u32 length;

  NNS_ASSERT(IsInitialized());
  Lock();

  ret = GetStreamWritableLength_InVariousDevice(&length);

  if (ret) {
    if (length < NNSi_MCS_MSG_HEAD_SIZE) {
      *pLength = 0;
    } else {
      *pLength = length - NNSi_MCS_MSG_HEAD_SIZE;
    }
  }

  Unlock();
  return ret;
}

static BOOL WriteStream_TWL(u16 channel, NNSiMcsMsg *pBlock, u32 length) {
  if (!ISTDSIOSend(channel, (const void *)pBlock,
                   NNSi_MCS_MSG_HEAD_SIZE + length)) {
    NNS_WARNING(FALSE, "NNS Mcs error: failed ISTDSIOSend()\n");
    return FALSE;
  }

  return TRUE;
}

static BOOL WriteStream_Ensata(u16 channel, NNSiMcsMsg *pBlock, u32 length) {
#ifndef SDK_TWL
  NNSiMcsEnsWork *const pWork = GetEnsWorkPtr();

  pWork->msgBuf.channel = channel;
  MI_CpuCopy8(pBlock, pWork->msgBuf.buf, NNSi_MCS_MSG_HEAD_SIZE + length);
  if (!NNS_McsWriteRingBuffer(pWork->wrRb, &pWork->msgBuf,
                              offsetof(NNSiMcsEnsMsgBuf, buf) +
                                  NNSi_MCS_MSG_HEAD_SIZE + length)) {
    NNS_WARNING(FALSE, "NNS Mcs error: send error\n");
    return FALSE;
  }

  return TRUE;
#else
  (void)channel;
  (void)pBlock;
  (void)length;
  return FALSE;
#endif
}

static BOOL WriteStream_NITRO(u16 channel, NNSiMcsMsg *pBlock, u32 length) {
#ifndef SDK_TWLLTD
  if (!NITROToolAPIWriteStream(channel, pBlock,
                               NNSi_MCS_MSG_HEAD_SIZE + length)) {
    NNS_WARNING(FALSE, "NNS Mcs error: failed NITROToolAPIWriteStream()\n");
    return FALSE;
  }

  return TRUE;
#else
  (void)channel;
  (void)pBlock;
  (void)length;
  return FALSE;
#endif
}

static BOOL WriteStream_InVariousDevice(u16 channel, NNSiMcsMsg *pBlock,
                                        u32 length) {
  if (IsRunOnTwlHw())
    return WriteStream_TWL(channel, pBlock, length);
  else if (IsRunOnEnsata())
    return WriteStream_Ensata(channel, pBlock, length);
  else
    return WriteStream_NITRO(channel, pBlock, length);
}

BOOL NNS_McsWriteStream(u16 channel, const void *data, u32 size) {
  const u8 *const pSrc = (const u8 *)data;
  int retryCnt = 0;
  u32 offset = 0;

  NNS_ASSERT(IsInitialized());
  Lock();

  while (offset < size) {
    const u32 restSize = size - offset;
    u32 length;

    if (!NNS_McsGetStreamWritableLength(
            &length)) // Gets the amount of data that can be written at once
    {
      break;
    }

    if (restSize > length && length < NNSi_MCS_MSG_DATA_SIZE_MIN) {
      if (++retryCnt > SEND_RETRY_MAX) {
        NNS_WARNING(
            FALSE,
            "NNS Mcs error: send time out writable bytes %d, rest bytes %d\n",
            length, restSize);
        break;
      }
      WaitSendData(); // Waits until it can be written
    } else {
      NNSiMcsMsg *pBlock = &spMcsWork->writeBuf;

      length = min_u32(min_u32(restSize, length), NNSi_MCS_MSG_DATA_SIZE_MAX);

      pBlock->head.version = NNSi_MCS_MSG_HEAD_VERSION;
      pBlock->head.reserved = 0;
      pBlock->head.offset = offset;
      pBlock->head.totalLen = size;
      MI_CpuCopy8(pSrc + offset, pBlock->data, length);

      if (!WriteStream_InVariousDevice(channel, pBlock, length)) {
        break;
      }

      offset += length;
    }
  }

  Unlock();
  return offset == size; // If it succeeds in writing to the end, returns TRUE.
}

static void ClearBuffer_TWL() {

  ISTDSIOSetRecvCallback((ISTDSIORecvCbFunc)CallbackRecvDummy, (void *)0);

  ISTDSIOSetRecvCallback((ISTDSIORecvCbFunc)CallbackRecv,
                         (void *)&spMcsWork->recvCBInfoList);
}

static void ClearBuffer_Ensata() {
#ifndef SDK_TWL
  NNSiMcsEnsWork *pWork = GetEnsWorkPtr();
  NNS_McsClearRingBuffer(pWork->rdRb);
#endif
}

static void ClearBuffer_NITRO() {
#ifndef SDK_TWLLTD

  BOOL bSuccess =
      NITROToolAPISetReceiveStreamCallBackFunction(CallbackRecvDummy, 0);
  NNS_ASSERT(bSuccess);

  NITROToolAPIPollingIdle();

  bSuccess = NITROToolAPISetReceiveStreamCallBackFunction(
      CallbackRecv, (u32)&spMcsWork->recvCBInfoList);
  NNS_ASSERT(bSuccess);
#endif
}

static void ClearBuffer_InVariousDevice() {
  if (IsRunOnTwlHw())
    ClearBuffer_TWL();
  else if (IsRunOnEnsata())
    ClearBuffer_Ensata();
  else
    ClearBuffer_NITRO();
}

void NNS_McsClearBuffer() {
  NNSMcsRecvCBInfo *pInfo = NULL;

  NNS_ASSERT(IsInitialized());
  Lock();

  if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_POLL) {
    ClearBuffer_InVariousDevice();
  }

  while (NULL != (pInfo = NNS_FndGetNextListObject(&spMcsWork->recvCBInfoList,
                                                   pInfo))) {
    if (pInfo->cbFunc == DataRecvCallback) // Function using the ring buffer
    {
      DefRecvCBInfo *pDefCBInfo = (DefRecvCBInfo *)pInfo->userData;
      NNS_McsClearRingBuffer(pDefCBInfo->ringBuf);
    }
  }

  Unlock();
}

BOOL NNS_McsIsServerConnect() {
  BOOL bSuccess;

  NNS_ASSERT(IsInitialized());
  Lock();

  NNS_McsPollingIdle();

  bSuccess = spMcsWork->bHostDataRecived;

  Unlock();
  return bSuccess;
}

static void PollingIdle_TWL() {}

static void PollingIdle_Ensata() {
#ifndef SDK_TWL
  NNSiMcsEnsWork *pWork = GetEnsWorkPtr();
  uint32_t readableBytes;
  uint32_t readBytes;

  while (0 < (readableBytes = NNS_McsGetRingBufferReadableBytes(pWork->rdRb))) {
    NNS_ASSERT(offsetof(NNSiMcsEnsMsgBuf, buf) < readableBytes &&
               readableBytes <= sizeof(NNSiMcsEnsMsgBuf));

    {
      BOOL bRet = NNS_McsReadRingBuffer(pWork->rdRb, &pWork->msgBuf,
                                        readableBytes, &readBytes);
      NNS_ASSERT(bRet);
    }

    CallbackRecv((u32)&spMcsWork->recvCBInfoList, pWork->msgBuf.channel,
                 pWork->msgBuf.buf,
                 readBytes - offsetof(NNSiMcsEnsMsgBuf, buf));
  }
#endif
}

static void PollingIdle_NITRO() {
#ifndef SDK_TWLLTD
  NITROToolAPIPollingIdle();
#endif
}

static void PollingIdle_InVariousDevice() {
  if (IsRunOnTwlHw())
    PollingIdle_TWL();
  else if (IsRunOnEnsata())
    PollingIdle_Ensata();
  else
    PollingIdle_NITRO();
}

void NNS_McsPollingIdle() {
  NNS_ASSERT(IsInitialized());
  Lock();

  if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_POLL) {
    PollingIdle_InVariousDevice();

    if (spMcsWork->bProtocolError) {
      const u32 data = TRUE; // Connection
      u32 length;
      if (NNS_McsGetStreamWritableLength(&length) || sizeof(data) <= length) {

        if (NNS_McsWriteStream(NNSi_MCS_SYSMSG_CHANNEL, &data, sizeof(data))) {

          OS_Panic("mcs message version error.\n");
        }
      }
    }
  }

  Unlock();
}

static void VBlankInterrupt_TWL() {}

static void VBlankInterrupt_Ensata() {}

static void VBlankInterrupt_NITRO() {
#ifndef SDK_TWLLTD
  NITROToolAPIVBlankInterrupt();
#endif
}

static void VBlankInterrupt_InVariousDevice() {
  if (IsRunOnTwlHw())
    VBlankInterrupt_TWL();
  else if (IsRunOnEnsata())
    VBlankInterrupt_Ensata();
  else
    VBlankInterrupt_NITRO();
}

void NNS_McsVBlankInterrupt() {
  if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_VBLANK) {
    VBlankInterrupt_InVariousDevice();
  }
}

static void CartridgeInterrupt_TWL() {}

static void CartridgeInterrupt_Ensata() {}

static void CartridgeInterrupt_NITRO() {
#ifndef SDK_TWLLTD
  NITROToolAPICartridgeInterrupt();
#endif
}

static void CartridgeInterrupt_InVariousDevice() {
  if (IsRunOnTwlHw())
    CartridgeInterrupt_TWL();
  else if (IsRunOnEnsata())
    CartridgeInterrupt_Ensata();
  else
    CartridgeInterrupt_NITRO();
}

void NNS_McsCartridgeInterrupt() {
  if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_CARTRIDGE) {
    CartridgeInterrupt_InVariousDevice();
  }
}

#endif
