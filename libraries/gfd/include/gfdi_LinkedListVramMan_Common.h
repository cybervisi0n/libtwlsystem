#ifndef NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_
#define NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_

#include <nnsys/gfd.h>

typedef struct NNSiGfdLnkVramBlock NNSiGfdLnkVramBlock;

struct NNSiGfdLnkVramBlock {
  u32 addr;   // Region start address
  u32 szByte; // Region size (zero may not be used)

  NNSiGfdLnkVramBlock
      *pBlkPrev; // Previous region (No address-positional relation)
  NNSiGfdLnkVramBlock *pBlkNext; // Next region (No address-positional relation)
};

typedef struct NNSiGfdLnkMemRegion {
  u32 start;
  u32 end;

} NNSiGfdLnkMemRegion;

typedef struct NNSiGfdLnkVramMan {
  NNSiGfdLnkVramBlock *pFreeList; // Unused region block list

} NNSiGfdLnkVramMan;

void NNSi_GfdDumpLnkVramManFreeListInfo(
    const NNSiGfdLnkVramBlock *pFreeListHead, u32 szReserved);

void NNSi_GfdDumpLnkVramManFreeListInfoEx(
    const NNSiGfdLnkVramBlock *pFreeListHead, NNSGfdLnkDumpCallBack pFunc,
    void *pUserData);

void NNSi_GfdInitLnkVramMan(NNSiGfdLnkVramMan *pMgr);

NNSiGfdLnkVramBlock *
NNSi_GfdInitLnkVramBlockPool(NNSiGfdLnkVramBlock *pArrayHead,
                             u32 lengthOfArray);

BOOL NNSi_GfdAddNewFreeBlock(NNSiGfdLnkVramMan *pMan,
                             NNSiGfdLnkVramBlock **ppBlockPoolList,
                             u32 baseAddr, u32 szByte);

BOOL NNSi_GfdAllocLnkVram(NNSiGfdLnkVramMan *pMan,
                          NNSiGfdLnkVramBlock **ppBlockPoolList, u32 *pRetAddr,
                          u32 szByte);

BOOL NNSi_GfdAllocLnkVramAligned(NNSiGfdLnkVramMan *pMan,
                                 NNSiGfdLnkVramBlock **ppBlockPoolList,
                                 u32 *pRetAddr, u32 szByte, u32 alignment);

void NNSi_GfdMergeAllFreeBlocks(NNSiGfdLnkVramMan *pMan,
                                NNSiGfdLnkVramBlock **ppBlockPoolList);

BOOL NNSi_GfdFreeLnkVram(NNSiGfdLnkVramMan *pMan,
                         NNSiGfdLnkVramBlock **ppBlockPoolList, u32 addr,
                         u32 szByte);

#endif // NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_
