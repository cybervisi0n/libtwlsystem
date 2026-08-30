#ifndef NNS_G2D_FONT_DATA_H_
#define NNS_G2D_FONT_DATA_H_

#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifdef _MSC_VER // for VC
#pragma warning(disable : 4200)
#pragma warning(disable : 4201)
#endif //_MSC_VER

#ifndef SDK_ARM9
#include <nitro_win32.h>
#endif //! SDK_ARM9

#ifdef __MWERKS__ // for CW
#pragma warning off(10536)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_BINFILE_SIG_FONTDATA (u32)'NFTR'

#define NNS_G2D_BINBLK_SIG_FINFDATA (u32)'FINF'
#define NNS_G2D_BINBLK_SIG_CGLPDATA (u32)'CGLP'
#define NNS_G2D_BINBLK_SIG_TGLPDATA (u32)'TGLP'
#define NNS_G2D_BINBLK_SIG_TCLPDATA (u32)'TCLP'
#define NNS_G2D_BINBLK_SIG_CWDHDATA (u32)'CWDH'
#define NNS_G2D_BINBLK_SIG_CMAPDATA (u32)'CMAP'

#define NNS_G2D_BINFILE_EXT_FONTDATA "NFTR"

#define NNS_G2D_NFTR_VER_1_0 (u16)(0x0100)
#define NNS_G2D_NFTR_VER_1_1 (u16)(0x0101)
#define NNS_G2D_NFTR_VER_1_2 (u16)(0x0102)

#define NNS_G2D_NFTR_VER NNS_G2D_NFTR_VER_1_2

typedef enum NNSG2dFontType {
  NNS_G2D_FONTTYPE_GLYPH, // BMP
  NNS_G2D_NUM_OF_FONTTYPE
} NNSG2dFontType;

typedef enum NNSG2dFontEncoding {
  NNS_G2D_FONT_ENCODING_UTF8,   // UTF-8
  NNS_G2D_FONT_ENCODING_UTF16,  // UTF-16
  NNS_G2D_FONT_ENCODING_SJIS,   // ShiftJIS
  NNS_G2D_FONT_ENCODING_CP1252, // CP1252
  NNS_G2D_NUM_OF_ENCODING
} NNSG2dFontEncoding;

typedef enum NNSG2dFontMappingMethod {
  NNS_G2D_MAPMETHOD_DIRECT, // index = character code - offset
  NNS_G2D_MAPMETHOD_TABLE,  // index = mapInfo[character code - offset]
  NNS_G2D_MAPMETHOD_SCAN,   // index = search(mapInfo, character code)
  NNS_G2D_NUM_OF_MAPMETHOD
} NNSG2dFontMappingMethod;

typedef enum NNSG2dFontGlyphFlag {
  NNS_G2D_FONT_FLAG_TBRL = (1 << 0),    // Fonts for vertical writing
  NNS_G2D_FONT_FLAG_ROT_0 = (0 << 1),   // 0 degrees clockwise rotation font
  NNS_G2D_FONT_FLAG_ROT_90 = (1 << 1),  // 90 degrees clockwise rotation font
  NNS_G2D_FONT_FLAG_ROT_180 = (2 << 1), // 180 degrees clockwise rotation font
  NNS_G2D_FONT_FLAG_ROT_270 = (3 << 1), // 270 degrees clockwise rotation font
  NNS_G2D_FONT_FLAG_ROT_MASK = (3 << 1) // Rotation mask
} NNSG2dFontGlyphFlag;

typedef struct NNSG2dCharWidths {
  s8 left;       // Left space width of character
  u8 glyphWidth; // Glyph width of character
  s8 charWidth;  // character width = left space width + glyph width + right
                 // space width
} NNSG2dCharWidths;

typedef struct NNSG2dCMapScanEntry {
  u16 ccode; // Character code
  u16 index; // Glyph index
} NNSG2dCMapScanEntry;

typedef struct NNSG2dCMapInfoScan {
  u16 num; // number of <entries> elements
  NNSG2dCMapScanEntry
      entries[]; // mapping list to the glyph index from the character code
} NNSG2dCMapInfoScan;

typedef struct NNSG2dFontGlyph {
  u8 cellWidth;    // number of pixels in 1 glyph image line
  u8 cellHeight;   // number of glyph image lines
  u16 cellSize;    // glyph image data size per 1 glyph
  s8 baselinePos;  // (*) distance from the top edge of the glyph image to the
                   // baseline
  u8 maxCharWidth; // (*) maximum character width
  u8 bpp;          // number of bits per 1 glyph image pixel
  u8 flags;        // NNSG2dFontGlyphFlag

  u8 glyphTable[]; // glyph image array
} NNSG2dFontGlyph;

typedef struct NNSG2dFontWidth {
  u16 indexBegin; // glyph index corresponding to the first entry in widthTable
  u16 indexEnd;   // glyph index corresponding to the last entry in widthTable
  struct NNSG2dFontWidth *pNext; // Pointer to the next NNSG2dFontWidth
  NNSG2dCharWidths widthTable[]; // width information array
} NNSG2dFontWidth;

typedef struct NNSG2dFontCodeMap {
  u16 ccodeBegin; // First character code in the character code boundary that
                  // this block manages
  u16 ccodeEnd; // Last character code in the character code boundary that this
                // block manages
  u16 mappingMethod; // Mapping method (NNSG2dFontMappingMethod type)
  u16 reserved;      // reserved
  struct NNSG2dFontCodeMap *pNext; // pointer to the next NNSG2dFontCodeMap
  u16 mapInfo[]; // The specific details of the character code mapping
                 // information depend on the mappingMethod
} NNSG2dFontCodeMap;

typedef struct NNSG2dFontInformation {
  u8 fontType;        // glyph data type (NNSG2dFontType type)
  s8 linefeed;        // (*)== leading
  u16 alterCharIndex; // (*) glyph index of the glyph for characters not
                      // contained in the font
  NNSG2dCharWidths defaultWidth; // (* ) character width information for glyphs
                                 // that have no character width information
  u8 encoding; // Corresponding character string encoding (NNSG2dFontEncoding
               // type)
  NNSG2dFontGlyph *pGlyph; // Pointer to a unique NNSG2dFontGlyph
  NNSG2dFontWidth *pWidth; // Pointer to the first NNSG2dFontWidth
  NNSG2dFontCodeMap *pMap; // Pointer to the first NNSG2dFontCodeMap
  u8 height;
  u8 width;
  u8 ascent;
  u8 padding_[1];
} NNSG2dFontInformation;

typedef struct NNSG2dFontInformationBlock {
  NNSG2dBinaryBlockHeader blockHeader;
  NNSG2dFontInformation blockBody;
} NNSG2dFontInformationBlock;

typedef struct NNSG2dFontGlyphBlock {
  NNSG2dBinaryBlockHeader blockHeader;
  NNSG2dFontGlyph blockBody;
} NNSG2dFontGlyphBlock;

typedef struct NNSG2dFontWidthBlock {
  NNSG2dBinaryBlockHeader blockHeader;
  NNSG2dFontWidth blockBody;
} NNSG2dFontWidthBlock;

typedef struct NNSG2dFontCodeMapBlock {
  NNSG2dBinaryBlockHeader blockHeader;
  NNSG2dFontCodeMap blockBody;
} NNSG2dFontCodeMapBlock;

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifdef __MWERKS__
#pragma warning reset(10536)
#endif

#ifdef _MSC_VER
#pragma warning(default : 4200)
#pragma warning(default : 4201)
#endif //_MSC_VER

#endif // NNS_G2D_FONT_DATA_H_
