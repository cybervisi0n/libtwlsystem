#if !defined(NNS_FINALROM)

#include "istd_stubi.h"

__declspec(weak) void ISTDHIOInit() {}

__declspec(weak) u32 ISTDHIOGetDevMask() { return 0; }

__declspec(weak) BOOL ISTDHIOOpen(u32 /* fDevMask */) { return FALSE; }

__declspec(weak) BOOL ISTDHIOClose() { return FALSE; }

__declspec(weak) BOOL ISTDSIOSend(u16 /* chn */, const void * /* pSrc */,
                                  u32 /* nSize */) {
  return FALSE;
}

__declspec(weak) void ISTDSIOSetRecvCallback(ISTDSIORecvCbFunc /* cbRecv */,
                                             void * /* pUser */) {}

#endif
