#ifndef NNS_G2D_OAMEX_H_
#define NNS_G2D_OAMEX_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2dEntryNewOamEx NNS_G2dEntryOamManExOam
#define NNS_G2dEntryNewOamWithAffineIdxEx NNS_G2dEntryOamManExOamWithAffineIdx
#define NNS_G2dEntryNewAffineParamsEx NNS_G2dEntryOamManExAffine
#define NNS_G2dApplyToBaseModuleEx NNS_G2dApplyOamManExToBaseModule

#define NNS_G2dResetOamInstanceEx NNS_G2dResetOamManExBuffer
#define NNS_G2dGetNewOamInstanceEx NNS_G2dGetOamManExInstance

#define NNS_G2dSetOamEntryFunctionsEx NNS_G2dSetOamManExEntryFunctions

#define NNS_G2D_OAMEX_HW_ID_NOT_INIT 0xFFFF

typedef enum NNSG2dOamExDrawOrder {
  NNSG2D_OAMEX_DRAWORDER_BACKWARD =
      0x0, // Register to external module, reversing the order used in
           // registration
  NNSG2D_OAMEX_DRAWORDER_FORWARD = 0x1 // Register to external module, using the
                                       // same order used for registration

} NNSG2dOamExDrawOrder;

typedef u16 (*NNSG2dGetOamCpacityFuncPtr)();
typedef u16 (*NNSG2dGetOamAffineCpacityFuncPtr)();
typedef BOOL (*NNSG2dEntryNewOamFuncPtr)(const GXOamAttr *pOam, u16 index);
typedef u16 (*NNSG2dEntryNewOamAffineFuncPtr)(const MtxFx22 *mtx, u16 index);

typedef struct NNSG2dOamExEntryFunctions {
  NNSG2dGetOamCpacityFuncPtr getOamCapacity;
  NNSG2dGetOamCpacityFuncPtr getAffineCapacity;
  NNSG2dEntryNewOamFuncPtr entryNewOam;
  NNSG2dEntryNewOamAffineFuncPtr entryNewAffine;

} NNSG2dOamExEntryFunctions;

typedef struct NNSG2dOamChunk {

  GXOamAttr oam;      // OAM attributes
  u16 affineProxyIdx; // NNSG2dAffineParamProxy Index value referenced by OAM
  u16 pad16_;         // Padding
  struct NNSG2dOamChunk *pNext; // Link for building list structure

} NNSG2dOamChunk;

typedef struct NNSG2dOamChunkList {
  u16 numChunks; // Total number of NNSG2dOamChunk elements in the pChunks list
  u16 numLastFrameDrawn; // Number of the NNSG2dOamChunk that was registered for
                         // rendering in the previous frame.

  u16 numDrawn; // Number of chunks to render
  u16 bDrawn;   // Should this ChunkList be registered for rendering?

  NNSG2dOamChunk *pChunks;        // NNSG2dOamChunk list
  NNSG2dOamChunk *pAffinedChunks; // Affine transformed NNSG2dOamChunk list

  NNSG2dOamChunk *pLastChunk; // NNSG2dOamChunk list end
  NNSG2dOamChunk
      *pLastAffinedChunk; // affine-transformed NNSG2dOamChunk list end

} NNSG2dOamChunkList;

typedef struct NNSG2dAffineParamProxy {
  MtxFx22 mtxAffine; // Affine Parameters
  u16 affineHWIndex; // AffineIndex value when loaded in HW
  u16 pad16_;        // Padding
} NNSG2dAffineParamProxy;

typedef struct NNSG2dOamManagerInstanceEx {

  NNSG2dOamChunkList
      *pOamOrderingTbl; // Pointer to the start of the rendering priority table

  u16 numPooledOam;               // Total number of OamChunks
  u16 numUsedOam;                 // Number of OamChunks already used
  NNSG2dOamChunk *pPoolOamChunks; // Pointer to the common OamChunks array

  u16 lengthOfOrderingTbl; // Size of the rendering priority table: (from 0 to
                           // 255: initialized as a u8 value)
  u16 lengthAffineBuffer;  // Total number of NNSG2dAffineParamProxy instances
  u16 numAffineBufferUsed; // Number of AffineParamProxy instances already used
  u16 lastFrameAffineIdx;  // Number for NNSG2dAffineParamProxy registered in a
                           // previous frame
  NNSG2dAffineParamProxy *pAffineBuffer; // Common AffineParamProxy array

  NNSG2dOamExEntryFunctions oamEntryFuncs; // Registration function

  u16 lastRenderedOrderingTblIdx;
  u16 lastRenderedChunkIdx;
  NNSG2dOamExDrawOrder drawOrderType; // Rendering order type

} NNSG2dOamManagerInstanceEx;

void NNS_G2dResetOamManExBuffer(NNSG2dOamManagerInstanceEx *pOam);

BOOL NNS_G2dGetOamManExInstance(NNSG2dOamManagerInstanceEx *pOam,
                                NNSG2dOamChunkList *pOamOrderingTbl,
                                u8 lengthOfOrderingTbl, u16 numPooledOam,
                                NNSG2dOamChunk *pPooledOam,
                                u16 lengthAffineBuffer,
                                NNSG2dAffineParamProxy *pAffineBuffer);

BOOL NNS_G2dEntryOamManExOam(NNSG2dOamManagerInstanceEx *pMan,
                             const GXOamAttr *pOam, u8 priority);

BOOL NNS_G2dEntryOamManExOamWithAffineIdx(NNSG2dOamManagerInstanceEx *pMan,
                                          const GXOamAttr *pOam, u8 priority,
                                          u16 affineIdx);

u16 NNS_G2dEntryOamManExAffine(NNSG2dOamManagerInstanceEx *pMan,
                               const MtxFx22 *mtx);

void NNS_G2dApplyOamManExToBaseModule(NNSG2dOamManagerInstanceEx *pMan);

void NNSG2d_SetOamManExDrawOrderType(NNSG2dOamManagerInstanceEx *pOam,
                                     NNSG2dOamExDrawOrder drawOrderType);

void NNS_G2dSetOamManExEntryFunctions(NNSG2dOamManagerInstanceEx *pMan,
                                      const NNSG2dOamExEntryFunctions *pSrc);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_OAMEX_H_
