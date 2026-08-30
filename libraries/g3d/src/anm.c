#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/kernel.h>

#include <nnsys/g3d/anm/nsbca.h>
#include <nnsys/g3d/anm/nsbma.h>
#include <nnsys/g3d/anm/nsbta.h>
#include <nnsys/g3d/anm/nsbtp.h>
#include <nnsys/g3d/anm/nsbva.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>

#ifdef FIX_SCALEEX_BLEND_BUG
#include <nnsys/g3d/sbc.h>
#include <nnsys/g3d/util.h>
#endif // FIX_SCALEEX_BLEND_BUG

NNSG3dFuncAnmBlendMat NNS_G3dFuncBlendMatDefault = &NNSi_G3dAnmBlendMat;
NNSG3dFuncAnmBlendJnt NNS_G3dFuncBlendJntDefault = &NNSi_G3dAnmBlendJnt;
NNSG3dFuncAnmBlendVis NNS_G3dFuncBlendVisDefault = &NNSi_G3dAnmBlendVis;

NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBmaDefault =
#ifndef NNS_G3D_NSBMA_DISABLE
    &NNSi_G3dAnmCalcNsBma;
#else
    NULL;
#endif

NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBtpDefault =
#ifndef NNS_G3D_NSBTP_DISABLE
    &NNSi_G3dAnmCalcNsBtp;
#else
    NULL;
#endif

NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBtaDefault =
#ifndef NNS_G3D_NSBTA_DISABLE
    &NNSi_G3dAnmCalcNsBta;
#else
    NULL;
#endif

NNSG3dFuncAnmJnt NNS_G3dFuncAnmJntNsBcaDefault =
#ifndef NNS_G3D_NSBCA_DISABLE
    &NNSi_G3dAnmCalcNsBca;
#else
    NULL;
#endif

NNSG3dFuncAnmVis NNS_G3dFuncAnmVisNsBvaDefault =
#ifndef NNS_G3D_NSBVA_DISABLE
    &NNSi_G3dAnmCalcNsBva;
#else
    NULL;
#endif

BOOL NNSi_G3dAnmBlendMat(NNSG3dMatAnmResult *pResult,
                         const NNSG3dAnmObj *pAnmObj, u32 matID) {
  BOOL rval = FALSE;
  NNS_G3D_NULL_ASSERT(pResult);
  NNS_G3D_ASSERT(!pAnmObj || matID < pAnmObj->numMapData);

  if (pAnmObj) {
    const NNSG3dAnmObj *p = pAnmObj;
    do {
      if (matID < p->numMapData) {

        u32 dataIdx = p->mapData[matID];
        if ((dataIdx & (NNS_G3D_ANMOBJ_MAPDATA_EXIST |
                        NNS_G3D_ANMOBJ_MAPDATA_DISABLED)) ==
            NNS_G3D_ANMOBJ_MAPDATA_EXIST) {
          NNSG3dFuncAnmMat func = (NNSG3dFuncAnmMat)p->funcAnm;

          NNS_G3D_NULL_ASSERT(func);
          if (func) {
            (*func)(pResult, p, dataIdx &NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
            rval = TRUE;
          }
        }
      }
      p = p->next;
    } while (p);
  }

  return rval;
}

#include <nitro/code32.h>
static void blendScaleVec_(VecFx32 *v0, const VecFx32 *v1, fx32 ratio,
                           BOOL isV1One) {
  if (isV1One) {

    v0->x += ratio;
    v0->y += ratio;
    v0->z += ratio;
  } else {
    v0->x += ratio * v1->x >> FX32_SHIFT;
    v0->y += ratio * v1->y >> FX32_SHIFT;
    v0->z += ratio * v1->z >> FX32_SHIFT;
  }
}
#include <nitro/codereset.h>

#include <nitro/code32.h>
BOOL NNSi_G3dAnmBlendJnt(NNSG3dJntAnmResult *pResult,
                         const NNSG3dAnmObj *pAnmObj, u32 nodeID) {
  NNS_G3D_NULL_ASSERT(pResult);

  if (!pAnmObj) {
    return FALSE;
  }

  if (!pAnmObj->next) {

    u32 dataIdx;
    if (nodeID < pAnmObj->numMapData) {
      dataIdx = pAnmObj->mapData[nodeID];
      if ((dataIdx &
           (NNS_G3D_ANMOBJ_MAPDATA_EXIST | NNS_G3D_ANMOBJ_MAPDATA_DISABLED)) ==
          NNS_G3D_ANMOBJ_MAPDATA_EXIST) {
        NNSG3dFuncAnmJnt func = (NNSG3dFuncAnmJnt)pAnmObj->funcAnm;

        NNS_G3D_NULL_ASSERT(func);
        if (func) {
          (*func)(pResult, pAnmObj, dataIdx &NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
          return TRUE;
        } else {
          return FALSE;
        }
      } else {

        return FALSE;
      }
    } else {
      return FALSE;
    }
  } else {

    const NNSG3dAnmObj *p;
    NNSG3dJntAnmResult r;
    fx32 sumOfRatio = 0;
    const NNSG3dAnmObj *pLastAnmObj;
    int numBlend = 0;
#if 1 // FIX
    int i1 = 0;
    VecFx32 keepAxisX;
    VecFx32 keepAxisZ;
#endif
#ifdef FIX_SCALEEX_BLEND_BUG
    u8 scalingRule = NNS_G3D_SCALINGRULE_STANDARD;
    if (NNS_G3dRS->pRenderObj && NNS_G3dRS->pRenderObj->resMdl) {
      scalingRule = NNS_G3dRS->pRenderObj->resMdl->info.scalingRule;
    }
#endif // FIX_SCALEEX_BLEND_BUG

    p = pAnmObj;
    do {

      if (nodeID < p->numMapData) {
        u32 dataIdx = p->mapData[nodeID];
        if ((dataIdx & (NNS_G3D_ANMOBJ_MAPDATA_EXIST |
                        NNS_G3D_ANMOBJ_MAPDATA_DISABLED)) ==
            NNS_G3D_ANMOBJ_MAPDATA_EXIST) {
          NNS_G3D_ASSERT(p->ratio >= 0 && p->ratio <= FX32_ONE);

          if (p->ratio > FX32_ONE)
            sumOfRatio += FX32_ONE;
          else if (p->ratio > 0)
            sumOfRatio += p->ratio;

          pLastAnmObj = p;
          ++numBlend;
        }
      }
      p = p->next;
    } while (p);

    if (sumOfRatio == 0) {

      return FALSE;
    }

    if (numBlend == 1) {

      NNSG3dFuncAnmJnt func = (NNSG3dFuncAnmJnt)pLastAnmObj->funcAnm;
      u32 dataIdx = pLastAnmObj->mapData[nodeID];

      NNS_G3D_NULL_ASSERT(func);
      if (func) {
        (*func)(pResult, pLastAnmObj,
                dataIdx &NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
        return TRUE;
      } else {
        return FALSE;
      }
    }

    MI_CpuClearFast(pResult, sizeof(*pResult));
    pResult->flag =
        (NNSG3dJntAnmResultFlag)-1; // Only the flags must all be set

    p = pAnmObj;
    do {
      if (nodeID < p->numMapData) {

        u32 dataIdx = p->mapData[nodeID];
        if ((dataIdx & (NNS_G3D_ANMOBJ_MAPDATA_EXIST |
                        NNS_G3D_ANMOBJ_MAPDATA_DISABLED)) ==
                NNS_G3D_ANMOBJ_MAPDATA_EXIST &&
            (p->ratio > 0)) {
          fx32 ratio;
          NNSG3dFuncAnmJnt func = (NNSG3dFuncAnmJnt)p->funcAnm;

          NNS_G3D_NULL_ASSERT(func);
          if (func) {
            (*func)(&r, p, dataIdx &NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
#if 1 // FIX
            if (i1 == 0) {
              MI_CpuCopy32(&r.rot._00, &keepAxisX, sizeof(VecFx32));
              MI_CpuCopy32(&r.rot._20, &keepAxisZ, sizeof(VecFx32));
            }
#endif

            if (sumOfRatio != FX32_ONE) {
              ratio = FX_Div(p->ratio, sumOfRatio);
            } else {
              ratio = p->ratio;
            }

            blendScaleVec_(&pResult->scale, &r.scale, ratio,
                           r.flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE);

#ifdef FIX_SCALEEX_BLEND_BUG

#ifndef NNS_G3D_SI3D_DISABLE
            if (scalingRule == NNS_G3D_SCALINGRULE_SI3D) {
              blendScaleVec_(&pResult->scaleEx0, &r.scaleEx0, ratio,
                             r.flag & NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE);
              if (nodeID == 0) {

                NNS_G3dRS->isScaleCacheOne[0] = 1; // Endian-dependent
              }
            }
#endif // NNS_G3D_SI3D_DISABLE

#else  // FIX_SCALEEX_BLEND_BUG
            blendScaleVec_(&pResult->scaleEx0, &r.scaleEx0, ratio,
                           r.flag & NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE);

            blendScaleVec_(&pResult->scaleEx1, &r.scaleEx1, ratio,
                           r.flag & NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE);
#endif // FIX_SCALEEX_BLEND_BUG

            if (!(r.flag & NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO)) {
              pResult->trans.x += (fx32)((fx64)ratio * r.trans.x >> FX32_SHIFT);
              pResult->trans.y += (fx32)((fx64)ratio * r.trans.y >> FX32_SHIFT);
              pResult->trans.z += (fx32)((fx64)ratio * r.trans.z >> FX32_SHIFT);
            }

            if (!(r.flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO)) {
              pResult->rot._00 += ratio * r.rot._00 >> FX32_SHIFT;
              pResult->rot._01 += ratio * r.rot._01 >> FX32_SHIFT;
              pResult->rot._02 += ratio * r.rot._02 >> FX32_SHIFT;
              pResult->rot._10 += ratio * r.rot._10 >> FX32_SHIFT;
              pResult->rot._11 += ratio * r.rot._11 >> FX32_SHIFT;
              pResult->rot._12 += ratio * r.rot._12 >> FX32_SHIFT;
            } else {
              pResult->rot._00 += ratio;
              pResult->rot._11 += ratio;
            }

            pResult->flag &= r.flag;
          }
        }
      }

      p = p->next;
#if 1
      ++i1;
#endif
    } while (p);

#ifdef FIX_SCALEEX_BLEND_BUG

#ifndef NNS_G3D_MAYA_DISABLE
    if (scalingRule == NNS_G3D_SCALINGRULE_MAYA) {

      const u8 *cmd = NNS_G3dRS->c;
      u8 opFlag = *(cmd + 3);

      if (opFlag & NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_PARENT) {
        if (pResult->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE) {

          NNSi_G3dBitVecSet(&NNS_G3dRS->isScaleCacheOne[0], nodeID);
        } else {

          NNSi_G3dBitVecReset(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

          NNS_G3dRSOnGlb.scaleCache[nodeID].inv.x = FX_Inv(pResult->scale.x);
          NNS_G3dRSOnGlb.scaleCache[nodeID].inv.y = FX_Inv(pResult->scale.y);
          NNS_G3dRSOnGlb.scaleCache[nodeID].inv.z = FX_Inv(pResult->scale.z);
        }
      }

      if (opFlag & NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_APPLY) {
        u32 parentID = *(cmd + 2);
        if (!NNSi_G3dBitVecCheck(&NNS_G3dRS->isScaleCacheOne[0], parentID)) {

          pResult->scaleEx0 = NNS_G3dRSOnGlb.scaleCache[parentID].inv;
        }
      }
    }
#endif // NNS_G3D_MAYA_DISABLE

#ifndef NNS_G3D_SI3D_DISABLE
    if (scalingRule == NNS_G3D_SCALINGRULE_SI3D) {

      if (pResult->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE) {

        if (pResult->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE) {

          NNSi_G3dBitVecSet(&NNS_G3dRS->isScaleCacheOne[0], nodeID);
        } else {

          NNSi_G3dBitVecReset(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

          NNS_G3dRSOnGlb.scaleCache[nodeID].s = pResult->scaleEx0;

          NNS_G3dRSOnGlb.scaleCache[nodeID].inv.x = FX_Inv(pResult->scaleEx0.x);
          NNS_G3dRSOnGlb.scaleCache[nodeID].inv.y = FX_Inv(pResult->scaleEx0.y);
          NNS_G3dRSOnGlb.scaleCache[nodeID].inv.z = FX_Inv(pResult->scaleEx0.z);
        }
      } else {

        NNSi_G3dBitVecReset(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

        pResult->scaleEx1.x = FX_Inv(pResult->scaleEx0.x);
        pResult->scaleEx1.y = FX_Inv(pResult->scaleEx0.y);
        pResult->scaleEx1.z = FX_Inv(pResult->scaleEx0.z);
        MI_CpuCopy32(&pResult->scaleEx0, &NNS_G3dRSOnGlb.scaleCache[nodeID],
                     sizeof(VecFx32) + sizeof(VecFx32));
      }
    }
#endif // NNS_G3D_SI3D_DISABLE

#endif // FIX_SCALEEX_BLEND_BUG

    VEC_CrossProduct((VecFx32 *)&pResult->rot._00, (VecFx32 *)&pResult->rot._10,
                     (VecFx32 *)&pResult->rot._20);

#if 0
        VEC_Normalize((VecFx32*)&pResult->rot._00, (VecFx32*)&pResult->rot._00);
        VEC_Normalize((VecFx32*)&pResult->rot._20, (VecFx32*)&pResult->rot._20);
#else // FIX
    if ((pResult->rot._00 == 0) && (pResult->rot._01 == 0) &&
        (pResult->rot._02 == 0)) {

      MI_CpuCopy32(&keepAxisX, &pResult->rot._00, sizeof(VecFx32));
    } else {
      VEC_Normalize((VecFx32 *)&pResult->rot._00, (VecFx32 *)&pResult->rot._00);
    }

    if ((pResult->rot._20 == 0) && (pResult->rot._21 == 0) &&
        (pResult->rot._22 == 0)) {

      MI_CpuCopy32(&keepAxisZ, &pResult->rot._20, sizeof(VecFx32));
    } else {
      VEC_Normalize((VecFx32 *)&pResult->rot._20, (VecFx32 *)&pResult->rot._20);
    }
#endif

    VEC_CrossProduct((VecFx32 *)&pResult->rot._20, (VecFx32 *)&pResult->rot._00,
                     (VecFx32 *)&pResult->rot._10);

    return TRUE;
  }
}
#include <nitro/codereset.h>

BOOL NNSi_G3dAnmBlendVis(NNSG3dVisAnmResult *pResult,
                         const NNSG3dAnmObj *pAnmObj, u32 nodeID) {
  BOOL rval = FALSE;
  const NNSG3dAnmObj *p;
  NNSG3dVisAnmResult tmp;
  NNS_G3D_NULL_ASSERT(pResult);

  p = pAnmObj;
  pResult->isVisible = FALSE;
  do {
    if (nodeID < p->numMapData) {

      u32 dataIdx = p->mapData[nodeID];

      if ((dataIdx &
           (NNS_G3D_ANMOBJ_MAPDATA_EXIST | NNS_G3D_ANMOBJ_MAPDATA_DISABLED)) ==
          NNS_G3D_ANMOBJ_MAPDATA_EXIST) {
        NNSG3dFuncAnmVis func = (NNSG3dFuncAnmVis)p->funcAnm;
        NNS_G3D_NULL_ASSERT(func);
        if (func) {
          (*func)(&tmp, p, dataIdx &NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
          pResult->isVisible |= tmp.isVisible;
          rval = TRUE;
        }
      }
    }
    p = p->next;
  } while (p);

  return rval;
}

u32 NNS_G3dAnmFmtNum = 5;

NNSG3dAnmObjInitFunc NNS_G3dAnmObjInitFuncArray[NNS_G3D_ANMFMT_MAX] = {
#ifndef NNS_G3D_NSBMA_DISABLE
    {'M', 0, 'MA', &NNSi_G3dAnmObjInitNsBma},
#else
    {'M', 0, 'MA', NULL},
#endif
#ifndef NNS_G3D_NSBTP_DISABLE
    {'M', 0, 'TP', &NNSi_G3dAnmObjInitNsBtp},
#else
    {'M', 0, 'TP', NULL},
#endif
#ifndef NNS_G3D_NSBTA_DISABLE
    {'M', 0, 'TA', &NNSi_G3dAnmObjInitNsBta},
#else
    {'M', 0, 'TA', NULL},
#endif
#ifndef NNS_G3D_NSBVA_DISABLE
    {'V', 0, 'VA', &NNSi_G3dAnmObjInitNsBva},
#else
    {'V', 0, 'VA', NULL},
#endif
#ifndef NNS_G3D_NSBCA_DISABLE
    {'J', 0, 'CA', &NNSi_G3dAnmObjInitNsBca}
#else
    {'J', 0, 'CA', NULL}
#endif
};
