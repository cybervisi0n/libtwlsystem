#ifndef G2DI_CHAR_H_
#define G2DI_CHAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NNS_G2D_UNICODE
#define NNS_G2D_TRANSCODE(str) L##str
typedef wchar_t NNSG2dChar;
#else // NNS_G2D_UNICODE
#define NNS_G2D_TRANSCODE(str) str
typedef char NNSG2dChar;
#endif // NNS_G2D_UNICODE

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // G2DI_CHAR_H_
