#ifndef NNS_G2D_ANIM_DATA_H_
#define NNS_G2D_ANIM_DATA_H_

#include <nitro/types.h>
#include <nitro/fx/fx.h>

#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_BINFILE_SIG_CELLANIM (u32)'NANR'
#define NNS_G2D_BINFILE_SIG_MULTICELLANIM (u32)'NMAR'

#define NNS_G2D_BLKSIG_ANIMBANK (u32)'ABNK'
#define NNS_G2D_USEREXBLK_ANMATTR (u32)'UAAT'

#define NNS_G2D_BINFILE_EXT_CELLANIM "NANR"
#define NNS_G2D_BINFILE_EXT_MULTICELLANIM "NMAR"

#define NNS_G2D_NANR_MAJOR_VER 1
#define NNS_G2D_NANR_MINOR_VER 0

#define NNS_G2D_ANIMTYPE_SHIFT 16
#define NNS_G2D_ANIMTYPE_MASK 0xFF00
#define NNS_G2D_ANIMELEM_MASK 0x00FF

typedef enum NNSG2dAnimationType {
  NNS_G2D_ANIMATIONTYPE_INVALID = 0x0,     // Invalid type
  NNS_G2D_ANIMATIONTYPE_CELL,              // Cell
  NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION, // MultiCell
  NNS_G2D_ANIMATIONTYPE_MAX
} NNSG2dAnimationType;

typedef enum NNSG2dAnimationElement {
  NNS_G2D_ANIMELEMENT_INDEX = 0x0, // Index only
  NNS_G2D_ANIMELEMENT_INDEX_SRT,   // Index + SRT
  NNS_G2D_ANIMELEMENT_INDEX_T,     // Index + T
  NNS_G2D_ANIMELEMENT_MAX
} NNSG2dAnimationElement;

typedef enum NNSG2dAnimationPlayMode {
  NNS_G2D_ANIMATIONPLAYMODE_INVALID = 0x0, // Disabled
  NNS_G2D_ANIMATIONPLAYMODE_FORWARD,       // one-time playback (forward)
  NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP,  // repeat playback (forward loop)
  NNS_G2D_ANIMATIONPLAYMODE_REVERSE, // reverse playback (reverse (forward +
                                     // backward))
  NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP, // reverse playback repeat (reverse
                                          // (forward + backward) loop)
  NNS_G2D_ANIMATIONPLAYMODE_MAX
} NNSG2dAnimationPlayMode;

typedef u16 NNSG2dAnimData; // index only

typedef struct NNSG2dAnimDataSRT // index + SRT information
{
  u16 index; // index

  u16 rotZ; // rotation

  fx32 sx; // scale x
  fx32 sy; // scale y

  s16 px; // position x
  s16 py; // position y

} NNSG2dAnimDataSRT;

typedef struct NNSG2dAnimDataT // index + T information
{
  u16 index; // index
  u16 pad_;  // rotation

  s16 px; // position x
  s16 py; // position y

} NNSG2dAnimDataT;

typedef struct NNSG2dAnimFrameData {
  void *pContent; // pointer to animation result
  u16 frames;     // animation frame continuation time (units: video frames)
  u16 pad16;      // padding

} NNSG2dAnimFrameData;

typedef struct NNSG2dAnimSequenceData {
  u16 numFrames;         // number of animation frames forming the sequence
  u16 loopStartFrameIdx; // loop start animation frame number

  u32 animType; // animation type (upper 16 bits)

  NNSG2dAnimationPlayMode playMode;    // animation sequence playback method
  NNSG2dAnimFrameData *pAnmFrameArray; // offset from the head of pFrameArray.

} NNSG2dAnimSequenceData;

typedef struct NNSG2dAnimBankData {

  u16 numSequences;   // number of animation sequences
  u16 numTotalFrames; // total number of animation frames
  NNSG2dAnimSequenceData
      *pSequenceArrayHead;              // pointer to animation sequence array
  NNSG2dAnimFrameData *pFrameArrayHead; // pointer to animation frame array
  void *pAnimContents;                  // pointer to animation result array
  void *pStringBank;   // pointer to string bank (set during execution)
  void *pExtendedData; // pointer to library expansion area (not used)

} NNSG2dAnimBankData;

typedef struct NNSG2dAnimBankDataBlock {
  NNSG2dBinaryBlockHeader blockHeader; // binary header
  NNSG2dAnimBankData animBankData;     // Animation Bank

} NNSG2dAnimBankDataBlock;

typedef struct NNSG2dUserExAnimFrameAttr {
  u32 *pAttr;

} NNSG2dUserExAnimFrameAttr;

typedef struct NNSG2dUserExAnimSequenceAttr {
  u16 numFrames;
  u16 pad16;
  u32 *pAttr;
  NNSG2dUserExAnimFrameAttr *pAnmFrmAttrArray;

} NNSG2dUserExAnimSequenceAttr;

typedef struct NNSG2dUserExAnimAttrBank {
  u16 numSequences; // number of animation sequences
  u16 numAttribute; // Attribute count: Currently fixed at 1
  NNSG2dUserExAnimSequenceAttr *pAnmSeqAttrArray;

} NNSG2dUserExAnimAttrBank;

NNS_G2D_INLINE u32 NNSi_G2dMakeAnimType(NNSG2dAnimationType animType,
                                        NNSG2dAnimationElement elmType) {
  return (u32)animType << NNS_G2D_ANIMTYPE_SHIFT |
         ((u32)elmType & NNS_G2D_ANIMELEM_MASK);
}

NNS_G2D_INLINE NNSG2dAnimationType
NNSi_G2dGetAnimSequenceAnimType(u32 animType) {
  return (NNSG2dAnimationType)(animType >> NNS_G2D_ANIMTYPE_SHIFT);
}

NNS_G2D_INLINE NNSG2dAnimationElement
NNSi_G2dGetAnimSequenceElementType(u32 animType) {
  return (NNSG2dAnimationElement)(animType & NNS_G2D_ANIMELEM_MASK);
}

NNS_G2D_INLINE NNSG2dAnimationType
NNS_G2dGetAnimSequenceAnimType(const NNSG2dAnimSequenceData *pAnimSeq) {

  return NNSi_G2dGetAnimSequenceAnimType(pAnimSeq->animType);
}

NNS_G2D_INLINE NNSG2dAnimationElement
NNS_G2dGetAnimSequenceElementType(const NNSG2dAnimSequenceData *pAnimSeq) {

  return NNSi_G2dGetAnimSequenceElementType(pAnimSeq->animType);
}

NNS_G2D_INLINE u32 NNS_G2dCalcAnimSequenceTotalVideoFrames(
    const NNSG2dAnimSequenceData *pAnimSeq) {
  u32 i;
  u32 total = 0;

  for (i = 0; i < pAnimSeq->numFrames; i++) {
    total += pAnimSeq->pAnmFrameArray[i].frames;
  }
  return total;
}

#ifndef NNS_FROM_TOOL
NNS_G2D_INLINE
    u16 NNS_G2dGetAnimSequenceIndex(const NNSG2dAnimBankData *pAnimBank,
                                    const NNSG2dAnimSequenceData *pAnimSeq) {
  return (u16)(((u32)pAnimSeq - (u32)pAnimBank->pSequenceArrayHead) /
               (u32)sizeof(NNSG2dAnimSequenceData));
}
#else

#endif

NNS_G2D_INLINE const NNSG2dUserExAnimAttrBank *
NNS_G2dGetUserExAnimAttrBank(const NNSG2dAnimBankData *pAnimBank) {

  const NNSG2dUserExDataBlock *pBlk = NNSi_G2dGetUserExDataBlkByID(
      pAnimBank->pExtendedData, NNS_G2D_USEREXBLK_ANMATTR);

  if (pBlk != NULL) {
    return (const NNSG2dUserExAnimAttrBank *)(pBlk + 1);
  } else {
    return NULL;
  }
}

NNS_G2D_INLINE const NNSG2dUserExAnimSequenceAttr *
NNS_G2dGetUserExAnimSequenceAttr(const NNSG2dUserExAnimAttrBank *pAnimAttrBank,
                                 u16 idx) {
  NNS_G2D_NULL_ASSERT(pAnimAttrBank);

  if (idx < pAnimAttrBank->numSequences) {
    return &pAnimAttrBank->pAnmSeqAttrArray[idx];
  } else {
    return NULL;
  }
}

NNS_G2D_INLINE const NNSG2dUserExAnimFrameAttr *
NNS_G2dGetUserExAnimFrameAttr(const NNSG2dUserExAnimSequenceAttr *pAnimSeqAttr,
                              u16 idx) {
  NNS_G2D_NULL_ASSERT(pAnimSeqAttr);

  if (pAnimSeqAttr->numFrames) {
    return &pAnimSeqAttr->pAnmFrmAttrArray[idx];
  } else {
    return NULL;
  }
}

NNS_G2D_INLINE u32 NNS_G2dGetUserExAnimSeqAttrValue(
    const NNSG2dUserExAnimSequenceAttr *pAnimSeqAttr) {
  NNS_G2D_NULL_ASSERT(pAnimSeqAttr);
  return pAnimSeqAttr->pAttr[0];
}

NNS_G2D_INLINE u32
NNS_G2dGetUserExAnimFrmAttrValue(const NNSG2dUserExAnimFrameAttr *pFrmAttr) {
  NNS_G2D_NULL_ASSERT(pFrmAttr);
  return pFrmAttr->pAttr[0];
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_ANIM_DATA_H_
