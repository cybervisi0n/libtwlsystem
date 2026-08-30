#ifndef NNS_GFD_VRAMMAN_H_
#define NNS_GFD_VRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

#ifdef SDK_DEBUG

#define NNS_GFD_PRINTF(...) OS_Printf(__VA_ARGS__)
#define NNS_GFD_WARNING(...) OS_Warning(__VA_ARGS__)

#else // SDK_DEBUG

#define NNS_GFD_PRINTF(...) ((void)0)
#define NNS_GFD_WARNING(...) ((void)0)

#endif // SDK_DEBUG

#ifdef NNS_GFD_DEBUG
#define NNS_GFD_DBGMSG(...) OS_Printf(__VA_ARGS__)
#else // NNS_GFD_DEBUG
#define NNS_GFD_DBGMSG(...) ((void)0)
#endif // NNS_GFD_DEBUG

enum {
  NNS_GFD_ALLOC_FROM_HIGH = 0, // Allocate memory from the top of VRAM
  NNS_GFD_ALLOC_FROM_LOW = 1   // Allocate memory from the bottom of VRAM
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_VRAMMAN_H_
