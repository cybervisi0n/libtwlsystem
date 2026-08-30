#ifndef NNS_MCS_ISTD_STUBI_H_
#define NNS_MCS_ISTD_STUBI_H_

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NNS_FINALROM)

#ifdef SDK_LINK_ISTD

#include <istdbglib.h>

#else

void ISTDHIOInit(void);
u32 ISTDHIOGetDevMask(void);
BOOL ISTDHIOOpen(u32 fDevMask);
BOOL ISTDHIOClose(void);
BOOL ISTDSIOSend(u16 chn, const void *pSrc, u32 nSize);

typedef void (*ISTDSIORecvCbFunc)(void *pUser, u16 chn, const void *pBuf,
                                  u32 nSize);
void ISTDSIOSetRecvCallback(ISTDSIORecvCbFunc cbRecv, void *pUser);

enum { ISTDSIO_MAX_PAYLOAD_SIZE = 16384 };

#endif

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
