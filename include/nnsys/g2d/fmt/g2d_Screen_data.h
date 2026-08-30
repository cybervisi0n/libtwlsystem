#ifndef NNS_G2D_SCREEN_DATA_H_
#define NNS_G2D_SCREEN_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifndef SDK_ARM9
#include <nitro_win32.h>
#endif // SDK_ARM9

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_BINFILE_SIG_SCRDATA (u32)'NSCR'

#define NNS_G2D_BINBLK_SIG_SCRDATA (u32)'SCRN'

#define NNS_G2D_BINFILE_EXT_SCRDATA "NSCR"

#define NNS_G2D_NSCR_MAJOR_VER (u8)1
#define NNS_G2D_NSCR_MINOR_VER (u8)0

#define NNS_G2D_NSCR_VER                                                       \
  (u16)((NNS_G2D_NSCR_MAJOR_VER << 8) | NNS_G2D_NSCR_MINOR_VER)

typedef enum NNSG2dColorMode {
  NNS_G2D_SCREENCOLORMODE_16x16, // use 16 colors 16 palettes character data
  NNS_G2D_SCREENCOLORMODE_256x1, // use 256 colors 1 palette character data
  NNS_G2D_SCREENCOLORMODE_256x16 // use 256 colors 16 palettes character data

} NNSG2dColorMode;

typedef enum NNSG2dScreenFormat {
  NNS_G2D_SCREENFORMAT_TEXT,      // Text BG
  NNS_G2D_SCREENFORMAT_AFFINE,    // (normal) Affine BG
  NNS_G2D_SCREENFORMAT_AFFINEEXT, // Affine Extended (256x16) BG
  NNS_G2D_SCREENFORMAT_PLTBMP,    // PaLeTte color BitMaP
  NNS_G2D_SCREENFORMAT_DCBMP      // Direct Color BitMaP

} NNSG2dScreenFormat;

typedef struct NNSG2dScreenData {

  u16 screenWidth;  // screen width (pixels) of rawData
  u16 screenHeight; // screen height (pixels) of rawData
  u16 colorMode;    // character data color mode
  u16 screenFormat; // screen data format
  u32 szByte;       // size of rawData
  u32 rawData[1];   // screen data (variable length)

} NNSG2dScreenData;

typedef struct NNSG2dScreenDataBlock {

  NNSG2dBinaryBlockHeader blockHeader;
  NNSG2dScreenData screenData;

} NNSG2dScreenDataBlock;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_SCREEN_DATA_H_
