#ifndef ASSERT_UTIL_H_
#define ASSERT_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_IS_ALIGNED(addr, align) (((u32)(addr) & ((align) - 1)) == 0)

#define NNS_G2D_ALIGN_ASSERT(addr, align)                                      \
  NNS_G2D_ASSERTMSG(NNS_G2D_IS_ALIGNED((addr), (align)),                       \
                    "'%s'=%p must be %d byte aligned.", #addr, (addr))

#ifdef SDK_TWL

#define NNS_G2D_IS_VALID_POINTER(p)                                            \
  (((HW_ITCM_IMAGE <= ((u32)(p))) && (((u32)(p)) <= HW_CTRDG_RAM_END)) ||      \
   (OS_IsRunOnTwl() && (HW_TWL_MAIN_MEM_EX <= ((u32)(p))) &&                   \
    (((u32)(p)) <= HW_TWL_MAIN_MEM_EX_END)))

#else // SDK_TWL

#define NNS_G2D_IS_VALID_POINTER(p)                                            \
  ((HW_ITCM_IMAGE <= ((u32)(p))) && (((u32)(p)) <= HW_CTRDG_RAM_END))

#endif // SDK_TWL

#define NNS_G2D_POINTER_ASSERT(p)                                              \
  NNS_G2D_ASSERTMSG(NNS_G2D_IS_VALID_POINTER(p),                               \
                    "'%s'=%p is invalid memory address.", #p, (p));

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // ASSERT_UTIL_H_
