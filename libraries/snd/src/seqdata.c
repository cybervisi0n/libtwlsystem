#include <nnsys/snd/seqdata.h>

#ifndef NNS_FROM_TOOL

#include <nnsys/misc.h>

#else

#define NNS_NULL_ASSERT(exp) ((void)0)

#ifdef _MSC_VER
#pragma warning(disable : 4018) // warning: signed/unsigned mismatch
#pragma warning(disable                                                        \
                : 4311) // warning: pointer truncation from 'type' to 'type'
#pragma warning(                                                               \
    disable                                                                    \
    : 4312) // warning: conversion from 'type' to 'type' of greater size
#endif

#endif // NNS_FROM_TOOL

u32 NNSi_SndSeqArcGetSeqCount(const NNSSndSeqArc *seqArc) {
  NNS_NULL_ASSERT(seqArc);
  return seqArc->count;
}

const NNSSndSeqArcSeqInfo *NNSi_SndSeqArcGetSeqInfo(const NNSSndSeqArc *seqArc,
                                                    int index) {
  NNS_NULL_ASSERT(seqArc);

  if (index < 0)
    return NULL;
  if (index >= seqArc->count)
    return NULL;
  if (seqArc->info[index].offset == NNS_SND_SEQ_ARC_INVALID_OFFSET)
    return NULL;

  return &seqArc->info[index];
}
