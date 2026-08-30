#ifndef NNS_FND_HEAPCOMMON_H_
#define NNS_FND_HEAPCOMMON_H_

#include <nitro/types.h>
#include <nnsys/fnd/list.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_FND_HEAP_INVALID_HANDLE NULL

#define NNS_FND_HEAP_DEFAULT_ALIGNMENT 4

#define NNSI_EXPHEAP_SIGNATURE ('EXPH')

#define NNSI_FRMHEAP_SIGNATURE ('FRMH')

#define NNSI_UNTHEAP_SIGNATURE ('UNTH')

#define NNS_FND_HEAP_OPT_0_CLEAR (1 << 0)

#define NNS_FND_HEAP_OPT_DEBUG_FILL (1 << 1)

#define NNS_FND_HEAP_ERROR_PRINT (1 << 0)

enum {
  NNS_FND_HEAP_FILL_NOUSE, // When debug fill is not used
  NNS_FND_HEAP_FILL_ALLOC, // When debug fill is allocated
  NNS_FND_HEAP_FILL_FREE,  // When debug fill is freed

  NNS_FND_HEAP_FILL_MAX
};

typedef struct NNSiFndHeapHead NNSiFndHeapHead;

struct NNSiFndHeapHead {
  u32 signature;

  NNSFndLink link;
  NNSFndList childList;

  void *heapStart; // Heap start address
  void *heapEnd;   // Heap end (+1) address

  u32 attribute; // Attribute
};

typedef NNSiFndHeapHead *NNSFndHeapHandle; // Type to represent heap handle

#define NNS_FndGetHeapStartAddress(heap) ((void *)(heap))

#define NNS_FndGetHeapEndAddress(heap) (((NNSiFndHeapHead *)(heap))->heapEnd)

NNSFndHeapHandle NNS_FndFindContainHeap(const void *memBlock);

#if defined(NNS_FINALROM)
#define NNS_FndDumpHeap(heap) ((void)0)
#else
void NNS_FndDumpHeap(NNSFndHeapHandle heap);
#endif

#if defined(NNS_FINALROM)
#define NNS_FndSetFillValForHeap(type, val) (0)
#else
u32 NNS_FndSetFillValForHeap(int type, u32 val);
#endif

#if defined(NNS_FINALROM)
#define NNS_FndGetFillValForHeap(type) (0)
#else
u32 NNS_FndGetFillValForHeap(int type);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
