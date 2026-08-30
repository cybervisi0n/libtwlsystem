#if !defined(NNS_FINALROM)

#include <nitro.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <nnsys/misc.h>
#include <nnsys/mcs/baseCommon.h>
#include <nnsys/mcs/base.h>
#include <nnsys/mcs/print.h>
#include <nnsys/mcs/config.h>

#include "basei.h"
#include "printi.h"

static const int SEND_RETRY_MAX = 10;

static NNSMcsRecvCBInfo sCBInfo;
static void *spBuffer = NULL;
static u32 sBufferSize = 0;

static NNS_MCS_INLINE BOOL IsInitialized(void) { return NULL != spBuffer; }

static void CallbackRecvDummy(const void * /*pRecv*/, u32 /*dwRecvSize*/,
                              u32 /*dwUserData*/, u32 /*offset*/,
                              u32 /*totalSize*/
) {}

static NNS_MCS_INLINE BOOL CheckConnect() {
  if (NNS_McsIsServerConnect()) {
    return TRUE;
  } else {
    OS_Printf("NNS Mcs Print: not server connect\n");
    return FALSE;
  }
}

static BOOL WriteStream(const void *buf, u32 length) {
  int retryCount;

  for (retryCount = 0; retryCount < SEND_RETRY_MAX; ++retryCount) {
    u32 writableLength;
    if (!NNS_McsGetStreamWritableLength(&writableLength)) {
      return FALSE;
    }

    if (length <= writableLength) {
      return NNS_McsWriteStream(NNSi_MCS_PRINT_CHANNEL, buf, length);
    } else {

      NNSi_McsSleep(16);
    }
  }

  OS_Printf("NNS Mcs Print: send time out\n");
  return FALSE;
}

void NNS_McsInitPrint(void *pBuffer, u32 size) {
  NNS_ASSERT(pBuffer != NULL);
  NNS_ASSERT(size > 1);

  if (IsInitialized()) {
    return;
  }

  spBuffer = pBuffer; // Also means that initialization is complete
  sBufferSize = size;

  NNS_McsRegisterRecvCallback(&sCBInfo, NNSi_MCS_PRINT_CHANNEL,
                              CallbackRecvDummy, 0);
}

void NNS_McsFinalizePrint() {
  if (!IsInitialized()) {
    return;
  }

  NNS_McsUnregisterRecvResource(NNSi_MCS_PRINT_CHANNEL);

  spBuffer = NULL;
  sBufferSize = 0;
}

BOOL NNS_McsPutString(const char *string) {
  NNS_ASSERT(IsInitialized());
  NNS_ASSERT(string);

  if (!CheckConnect()) {
    return FALSE;
  }

  return WriteStream(string, strlen(string));
}

BOOL NNS_McsPrintf(const char *format, ...) {
  int writeChars;
  va_list args;

  NNS_ASSERT(IsInitialized());
  NNS_ASSERT(format);

  if (!CheckConnect()) {
    return FALSE;
  }

  va_start(args, format);

  writeChars = vsnprintf(spBuffer, sBufferSize, format, args);

  va_end(args);

  if (writeChars > 1) {
    return WriteStream(spBuffer, strlen(spBuffer));
  }

  return FALSE;
}

#endif
