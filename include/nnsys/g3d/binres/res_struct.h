#ifndef NNSG3D_BINRES_RES_STRUCT_H_
#define NNSG3D_BINRES_RES_STRUCT_H_

#include <nnsys/g3d/config.h>
#define NNS_G3D_RES_INLINE NNS_G3D_INLINE

#ifdef NNS_FROM_TOOL

typedef u32 NNSGfdTexKey;
#else
#include <nnsys/gfd.h>
SDK_COMPILER_ASSERT(sizeof(NNSGfdTexKey) == sizeof(u32));
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G3D_SIGNATURE_NSBMD '0DMB'
#define NNS_G3D_SIGNATURE_NSBTX '0XTB'
#define NNS_G3D_SIGNATURE_NSBCA '0ACB'
#define NNS_G3D_SIGNATURE_NSBVA '0AVB'
#define NNS_G3D_SIGNATURE_NSBMA '0AMB'
#define NNS_G3D_SIGNATURE_NSBTP '0PTB'
#define NNS_G3D_SIGNATURE_NSBTA '0ATB'

#define NNS_G3D_DATABLK_MDLSET '0LDM'
#define NNS_G3D_DATABLK_TEX '0XET'
#define NNS_G3D_DATABLK_JNT_ANM '0TNJ'
#define NNS_G3D_DATABLK_VIS_ANM '0SIV'
#define NNS_G3D_DATABLK_MATC_ANM '0TAM'
#define NNS_G3D_DATABLK_TEXPAT_ANM '0TAP'
#define NNS_G3D_DATABLK_TEXSRT_ANM '0TRS'

typedef u16 NNSG3dItemTag;
#define NNS_G3D_ITEMTAG_MAT_STANDARD 0
#define NNS_G3D_ITEMTAG_SHP_STANDARD 0

typedef struct NNSG3dResFileHeader_ {
  union {
    char signature[4];
    u32 sigVal;
  };
  u16 byteOrder;
  u16 version;
  u32 fileSize;
  u16 headerSize;
  u16 dataBlocks;
} NNSG3dResFileHeader;

typedef struct NNSG3dResDataBlockHeader_ {
  union {
    u32 kind;
    char chr[4];
  };
  u32 size;
} NNSG3dResDataBlockHeader;

typedef struct NNSG3dResDictTreeNode_ {
  u8 refBit;
  u8 idxLeft;
  u8 idxRight;
  u8 idxEntry;
} NNSG3dResDictTreeNode;

typedef struct NNSG3dResDict_ {
  u8 revision;     // revision of dictionary
  u8 numEntry;     // num of entry (not that of tree node)
  u16 sizeDictBlk; // size of this block
  u16 dummy_;
  u16 ofsEntry; // (NNSG3dResDictEntryHeader*)   ((u8*)this + idxEntry)

  NNSG3dResDictTreeNode node[1]; // variable size
} NNSG3dResDict;

#define NNS_G3D_RESNAME_SIZE (16)
#define NNS_G3D_RESNAME_VALSIZE (NNS_G3D_RESNAME_SIZE / sizeof(u32))

typedef union NNSG3dResName_ {
  char name[NNS_G3D_RESNAME_SIZE];
  u32 val[NNS_G3D_RESNAME_VALSIZE];
} NNSG3dResName;

typedef struct NNSG3dResDictEntryHeader_ {
  u16 sizeUnit; // The size (in bytes) of one data entry
  u16 ofsName;  //
  u8 data[4]; // The NNSG3dResDictEntryItem list (The size of each is sizeUnit)
} NNSG3dResDictEntryHeader;

typedef enum {
  NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_MASK = 0x0000ffff,
  NNS_G3D_TEXIMAGE_PARAM_S_SIZE_MASK = 0x00700000,
  NNS_G3D_TEXIMAGE_PARAM_T_SIZE_MASK = 0x03800000,
  NNS_G3D_TEXIMAGE_PARAM_TEXFMT_MASK = 0x1c000000,
  NNS_G3D_TEXIMAGE_PARAM_TR_MASK = 0x20000000,

  NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_SHIFT = 0,
  NNS_G3D_TEXIMAGE_PARAM_S_SIZE_SHIFT = 20,
  NNS_G3D_TEXIMAGE_PARAM_T_SIZE_SHIFT = 23,
  NNS_G3D_TEXIMAGE_PARAM_TEXFMT_SHIFT = 26,
  NNS_G3D_TEXIMAGE_PARAM_TR_SHIFT = 29
} NNSG3dTexImageParam;

typedef enum {
  NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_MASK = 0x000007ff,
  NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_MASK = 0x003ff800,
  NNS_G3D_TEXIMAGE_PARAMEX_WHSAME_MASK = 0x80000000,

  NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_SHIFT = 0,
  NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_SHIFT = 11,
  NNS_G3D_TEXIMAGE_PARAMEX_WHSAME_SHIFT = 31
} NNSG3dTexImageParamEx;

typedef struct NNSG3dResDictTexData_ {

  u32 texImageParam; // NNSG3dTexImageParam

  u32 extraParam; // NNSG3dTexImageParamEx
} NNSG3dResDictTexData;

typedef enum { NNS_G3D_RESTEX_LOADED = 0x0001 } NNSG3dResTexFlag;

typedef struct NNSG3dResTexInfo_ {
  NNSGfdTexKey vramKey; // Stores the VRAM key of the gfd library specifications
  u16 sizeTex;          // A value equivalent to the texture data after a 3-bit
                        // right-shift is entered anew
  u16 ofsDict;
  u16 flag; // NNSG3dResTexFlag
  u16 dummy_;
  u32 ofsTex; // The offset to the texture data (The offset from
              // &NNSG3dResTexImage)
} NNSG3dResTexInfo;

typedef enum {
  NNS_G3D_RESTEX4x4_LOADED = 0x0001 // Whether or not it is loaded into VRAM
} NNSG3dResTex4x4Flag;

typedef struct NNSG3dResTex4x4Info_ {
  NNSGfdTexKey vramKey; // Stores the VRAM key of the gfd library specifications
  u16 sizeTex; // The value when then the texture data is 3-bit right-shifted
               // (2-bit for the palette index)
  u16 ofsDict;
  u16 flag; // The 0 bit is the flag for whether or not the texture is loaded
            // into VRAM
  u16 dummy_;
  u32 ofsTex;        // The offset to the texture data (The bytes from
                     // &NNSG3dResTexImage)
  u32 ofsTexPlttIdx; // The offset to the texture palette index data (The bytes
                     // from &NNSG3dResTexImage)
} NNSG3dResTex4x4Info;

typedef struct NNSG3dResDictPlttData_ {
  u16 offset; // That which 3-bit right-shifted the offset towards the origin of
              // the palette data.
  u16 flag;   // A flag that determines whether or not the 0 bit is the Tex4Pltt
} NNSG3dResDictPlttData;

typedef enum {
  NNS_G3D_RESPLTT_LOADED = 0x0001, // Whether or not it is loaded into VRAM
  NNS_G3D_RESPLTT_USEPLTT4 =
      0x8000 // Whether or not the 4-color palette is in the block
} NNSG3dResPlttFlag;

typedef struct NNSG3dResPlttInfo_ {
  NNSGfdTexKey vramKey; // Stores the VRAM key of the gfd library specifications
  u16 sizePltt; // Takes what 3-bit right-shifted the data size of the texture
                // palette.
  u16 flag;     // NNSG3dResPlttFlag
  u16 ofsDict;
  u16 dummy_;
  u32 ofsPlttData;
} NNSG3dResPlttInfo;

typedef struct NNSG3dResTex_ {
  NNSG3dResDataBlockHeader header;
  NNSG3dResTexInfo texInfo;
  NNSG3dResTex4x4Info tex4x4Info;
  NNSG3dResPlttInfo plttInfo;
  NNSG3dResDict dict; // The dictionary for textures

} NNSG3dResTex;

typedef struct NNSG3dResDictMatData_ {
  u32 offset; // &The offset from NNSG3dResMat data
} NNSG3dResDictMatData;

typedef struct NNSG3dResDictTexToMatIdxData_ {
  u16 offset; // The offset from NNSG3dResMat
  u8 numIdx;  // The number of material IDs that correspond to the texture name
  u8 flag;    // The flag for whether or not the texture<->material binding was
              // performed
} NNSG3dResDictTexToMatIdxData;

typedef struct NNSG3dResDictPlttToMatIdxData_ {
  u16 offset; // The offset from NNSG3dResMat
  u8 numIdx;  // The number of material IDs that correspond to the palette name
  u8 flag;    // The flag for whether or not the palette<->material binding was
              // performed
} NNSG3dResDictPlttToMatIdxData;

typedef enum {
  NNS_G3D_MATFLAG_TEXMTX_USE =
      0x0001, // Whether or not to use the texture matrix
  NNS_G3D_MATFLAG_TEXMTX_SCALEONE =
      0x0002, // ON if the scale is 1.0 (when using the texture matrix)
  NNS_G3D_MATFLAG_TEXMTX_ROTZERO =
      0x0004, // ON if not rotating  (when using the texture matrix)
  NNS_G3D_MATFLAG_TEXMTX_TRANSZERO =
      0x0008, // ON if not translating (when using the texture matrix)
  NNS_G3D_MATFLAG_ORIGWH_SAME =
      0x0010, // Set if the width and height of the texture is the same as that
              // of the system
  NNS_G3D_MATFLAG_WIREFRAME = 0x0020, // ON if displaying wireframe
  NNS_G3D_MATFLAG_DIFFUSE =
      0x0040, // ON if designating the diffuse with the material
  NNS_G3D_MATFLAG_AMBIENT =
      0x0080, // ON if designating the ambient with the material
  NNS_G3D_MATFLAG_VTXCOLOR =
      0x0100, // ON if designating the vtxcolor flag with the material
  NNS_G3D_MATFLAG_SPECULAR =
      0x0200, // ON if designating the specular with the material
  NNS_G3D_MATFLAG_EMISSION =
      0x0400, // ON if designating the emission with the material
  NNS_G3D_MATFLAG_SHININESS =
      0x0800, // ON if designating the shininess flag with the material
  NNS_G3D_MATFLAG_TEXPLTTBASE =
      0x1000, // ON if designating the texture palette base address
  NNS_G3D_MATFLAG_EFFECTMTX = 0x2000 // ON if effect_mtx exists
} NNSG3dMatFlag;

typedef struct NNSG3dResMatData_ {
  NNSG3dItemTag itemTag; // The material type recognition tag (For this
                         // structure, it must be NNS_G3D_ITEMTAG_MAT_STANDARD)
  u16 size;              // Size
  u32 diffAmb;           // The parameters for the MaterialColor0 command
  u32 specEmi;           // The parameters for the MaterialColor1 command
  u32 polyAttr;          // The parameters for the PolygonAttr command
  u32 polyAttrMask;      // The bit designated by the material is 1
  u32 texImageParam;     // The parameters for the TexImageParam command
  u32 texImageParamMask; // The bit designated by the material is 1
  u16 texPlttBase;       // The parameters for the TexPlttBase command
  u16 flag;              // Cast to NNSG3dMatFlag
  u16 origWidth, origHeight; // Transmitted from the texture image at the time
                             // of initialization
  fx32 magW;                 // The width / origWidth of the bound texture
  fx32 magH;                 // The height / origHeight of the bound texture

} NNSG3dResMatData;

typedef struct NNSG3dResMat_ {
  u16 ofsDictTexToMatList;  // The offset to the texture->material ID string
                            // dictionary
  u16 ofsDictPlttToMatList; // The offset to the palette->material ID string
                            // dictionary
  NNSG3dResDict dict;       // The dictionary to the NNSG3dResDictMatData

} NNSG3dResMat;

typedef struct NNSG3dResDictShpData_ {
  u32 offset; // (u8*)&NNSG3DResShp + offset -> NNSG3dResShpData&
} NNSG3dResDictShpData;

typedef enum {
  NNS_G3D_SHPFLAG_USE_NORMAL =
      0x00000001, // The Normal command exists inside the DL
  NNS_G3D_SHPFLAG_USE_COLOR =
      0x00000002, // The Color command exists inside the DL
  NNS_G3D_SHPFLAG_USE_TEXCOORD =
      0x00000004, // The TexCoord command exists inside the DL
  NNS_G3D_SHPFLAG_USE_RESTOREMTX =
      0x00000008 // The RestoreMtx command exists inside the DL
} NNSG3dShpFlag;

typedef struct NNSG3dResShpData_ {
  NNSG3dItemTag itemTag; // The shape type recognition tag (For this structure,
                         // it must be NNS_G3D_ITEMTAG_SHP_STANDARD)
  u16 size;              // Size
  u32 flag;              // NNSG3dShpFlag
  u32 ofsDL;             // The offset to the display list
  u32 sizeDL;            // The display list size
} NNSG3dResShpData;

typedef struct NNSG3dResShp_ {
  NNSG3dResDict dict; // The dictionary to the NNSG3dResDictShpData

} NNSG3dResShp;

typedef struct NNSG3dResEvpMtx_ {
  MtxFx43 invM;
  MtxFx33 invN;
} NNSG3dResEvpMtx;

typedef enum {
  NNS_G3D_SBCTYPE_NORMAL = 0 // There is currently only one type
} NNSG3dSbcType;

typedef enum {
  NNS_G3D_SCALINGRULE_STANDARD = 0, // General matrix calculations are OK
  NNS_G3D_SCALINGRULE_MAYA = 1, // A node exists where Maya's Segment Scaling
                                // Compensate must be taken into consideration
  NNS_G3D_SCALINGRULE_SI3D =
      2 // Scaling calculations for Softimage3D must be performed
} NNSG3dScalingRule;

typedef enum {
  NNS_G3D_TEXMTXMODE_MAYA = 0x00, // Applies the Maya calculation method
  NNS_G3D_TEXMTXMODE_SI3D = 0x01, // Applies the Softimage3D calculation method
  NNS_G3D_TEXMTXMODE_3DSMAX = 0x02, // Applies the 3dsMax calculation method
  NNS_G3D_TEXMTXMODE_XSI = 0x03     // Applies the XSI calculation method
} NNSG3dTexMtxMode;

typedef struct NNSG3dResMdlInfo_ {
  u8 sbcType;     // Structure Byte Code type identifier (NNSG3dSbcType)
  u8 scalingRule; // scaling rule identifier (NNSG3dScalingRule)
  u8 texMtxMode;  // The texture matrix calculation formula identifier
                  // (NNSG3dTexMtxMode)
  u8 numNode;

  u8 numMat;
  u8 numShp;
  u8 firstUnusedMtxStackID;
  u8 dummy_;

  fx32 posScale;
  fx32 invPosScale;

  u16 numVertex;   // The value of the IMD file <output_info>::vertex_size
  u16 numPolygon;  // The value of the IMD file <output_info>::polygon_size
  u16 numTriangle; // The value of the IMD file <output_info>::triangle_size
  u16 numQuad;     // The value of the IMD file <output_info>::quad_size

  fx16 boxX, boxY,
      boxZ; // These become the actual coordinates if multiplied by boxPosScale
  fx16 boxW, boxH,
      boxD; // These become the actual coordinates if multiplied by boxPosScale

  fx32 boxPosScale;
  fx32 boxInvPosScale;
} NNSG3dResMdlInfo;

typedef struct NNSG3dResDictNodeData_ {
  u32 offset; // The offset data from &NNSG3dResNodeInfo
} NNSG3dResDictNodeData;

typedef enum {
  NNS_G3D_SRTFLAG_TRANS_ZERO = 0x0001, // Trans=(0,0,0), the output of the
                                       // translation data is abbreviated
  NNS_G3D_SRTFLAG_ROT_ZERO = 0x0002,   // Rot=Identity,  the output of the
                                       // rotation matrix data is abbreviated
  NNS_G3D_SRTFLAG_SCALE_ONE =
      0x0004, // Scale=(1,1,1), the output of the scaling data is abbreviated
  NNS_G3D_SRTFLAG_PIVOT_EXIST =
      0x0008, // A +1 or -1 element (Pivot) exists in the rotation matrix
  NNS_G3D_SRTFLAG_IDXPIVOT_MASK =
      0x00f0, // The location of the +1 / -1 element (Pivot)
  NNS_G3D_SRTFLAG_PIVOT_MINUS =
      0x0100,                         // If ON, the Pivot is -1, if OFF it is +1
  NNS_G3D_SRTFLAG_SIGN_REVC = 0x0200, // If set, C is the opposite code of B
  NNS_G3D_SRTFLAG_SIGN_REVD = 0x0400, // If set, D is the opposite code of A
  NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK =
      0xf800, // In the stack if 0-30, not in the stack if 31

  NNS_G3D_SRTFLAG_IDENTITY = NNS_G3D_SRTFLAG_TRANS_ZERO |
                             NNS_G3D_SRTFLAG_ROT_ZERO |
                             NNS_G3D_SRTFLAG_SCALE_ONE,

  NNS_G3D_SRTFLAG_IDXPIVOT_SHIFT = 4,
  NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT = 11
} NNSG3dSRTFlag;

typedef struct NNSG3dResNodeData_ {
  u16 flag; // NNSG3dSRTFlag
  fx16 _00;

} NNSG3dResNodeData;

typedef struct NNSG3dResNodeInfo_ {
  NNSG3dResDict dict;

} NNSG3dResNodeInfo;

typedef struct NNSG3dResMdl_ {
  u32 size;   // The size of the model
  u32 ofsSbc; // The offset of the SBC that used &NNSG3dResMdl as the origin
  u32 ofsMat; // The offset of the NNSG3dResMat that used &NNSG3dResMdl as the
              // origin
  u32 ofsShp; // The offset of the NNSG3dResShp that used &NNSG3dResMdl as the
              // origin
  u32 ofsEvpMtx; // The offset of NNSG3dEvpMtx that uses &NNSG3dResMdl as the
                 // origin
  NNSG3dResMdlInfo info;      // The model information (Fixed length)
  NNSG3dResNodeInfo nodeInfo; // The node information

} NNSG3dResMdl;

typedef struct NNSG3dResDictMdlSetData_ {
  u32 offset;
} NNSG3dResDictMdlSetData;

typedef struct NNSG3dResMdlSet_ {
  NNSG3dResDataBlockHeader header;
  NNSG3dResDict dict;

} NNSG3dResMdlSet;

#define NNS_G3D_SBC_NOP 0x00      // NOP
#define NNS_G3D_SBC_RET 0x01      // RET
#define NNS_G3D_SBC_NODE 0x02     // NODE      idxNode, visibility
#define NNS_G3D_SBC_MTX 0x03      // MTX       stackIdx
#define NNS_G3D_SBC_MAT 0x04      // MAT       idxMat
#define NNS_G3D_SBC_SHP 0x05      // SHP       idxShp
#define NNS_G3D_SBC_NODEDESC 0x06 // NODEDESC  idxNode, idxMtxDest, idxMtxParent
#define NNS_G3D_SBC_BB 0x07
#define NNS_G3D_SBC_BBY 0x08
#define NNS_G3D_SBC_NODEMIX                                                    \
  0x09 // NODEMIX   idxNode, idxMtxDest, numMtx, idxMtx1, ratio1, ..., idxMtxN,
       // ratioN
#define NNS_G3D_SBC_CALLDL 0x0a   // CALLDL    u32(relative), u32(size)
#define NNS_G3D_SBC_POSSCALE 0x0b // POSSCALE
#define NNS_G3D_SBC_ENVMAP 0x0c
#define NNS_G3D_SBC_PRJMAP 0x0d

#define NNS_G3D_SBC_NOCMD                                                      \
  0x1f // does not exist in compiled structure byte code mnemonic

#define NNS_G3D_SBC_COMMAND_NUM 0x20

#define NNS_G3D_SBCFLG_000 0x00
#define NNS_G3D_SBCFLG_001 0x20
#define NNS_G3D_SBCFLG_010 0x40
#define NNS_G3D_SBCFLG_011 0x60
#define NNS_G3D_SBCFLG_100 0x80
#define NNS_G3D_SBCFLG_101 0xa0
#define NNS_G3D_SBCFLG_110 0xc0
#define NNS_G3D_SBCFLG_111 0xe0

#define NNS_G3D_SBCCMD_MASK 0x1f
#define NNS_G3D_SBCFLG_MASK 0xe0

typedef enum {
  NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_APPLY = 0x01,
  NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_PARENT = 0x02
} NNSG3dSbcNodeDescFlag;

#define NNS_G3D_GET_SBCCMD(x) ((x) & NNS_G3D_SBCCMD_MASK)
#define NNS_G3D_GET_SBCFLG(x) ((u32)((x) & NNS_G3D_SBCFLG_MASK))

typedef struct NNSG3dResAnmHeader_ {

  u8 category0;
  u8 revision;

  u16 category1;
} NNSG3dResAnmHeader;

typedef struct {
  NNSG3dResAnmHeader anmHeader;
  u16 numFrame;
  u16 dummy_;
} NNSG3dResAnmCommon;

typedef struct {
  u32 offset; // The offset for accomplishing individual animations is stored
              // here.
} NNSG3dResDictAnmSetData;

typedef struct {
  NNSG3dResDataBlockHeader header;
  NNSG3dResDict dict; // file name -> each AnmSetData
} NNSG3dResAnmSet;

typedef struct NNSG3dResVisAnm_ {
  NNSG3dResAnmHeader anmHeader;
  u16 numFrame;
  u16 numNode;
  u16 size;
  u16 dummy_;

  u32 visData[1];
} NNSG3dResVisAnm;

typedef NNSG3dResDictAnmSetData NNSG3dResDictVisAnmSetData;

typedef NNSG3dResAnmSet NNSG3dResVisAnmSet;

typedef enum {
  NNS_G3D_MATCANM_ELEM_CONST = 0x20000000,  // ON when the data is not animating
  NNS_G3D_MATCANM_ELEM_STEP_1 = 0x00000000, // When there is data in every frame
  NNS_G3D_MATCANM_ELEM_STEP_2 =
      0x40000000, // When there is data in every 2nd frame
  NNS_G3D_MATCANM_ELEM_STEP_4 =
      0x80000000, // When there is data in every 4th frame

  NNS_G3D_MATCANM_ELEM_STEP_MASK = 0xc0000000,
  NNS_G3D_MATCANM_ELEM_LAST_INTERP_MASK = 0x1fff0000,
  NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_MASK = 0x0000ffff,

  NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_SHIFT = 0,
  NNS_G3D_MATCANM_ELEM_LAST_INTERP_SHIFT = 16
} NNSG3dMatCElem;

typedef struct NNSG3dResDictMatCAnmData_ {

  u32 diffuse;
  u32 ambient;
  u32 specular;
  u32 emission;
  u32 polygon_alpha;
} NNSG3dResDictMatCAnmData;

typedef enum {
  NNS_G3D_MATCANM_OPTION_INTERPOLATION =
      0x0001, // Corresponds to the <mat_color_info>::interpolation
  NNS_G3D_MATCANM_OPTION_END_TO_START_INTERPOLATION =
      0x0002 // Corresponds to the <mat_color_info>::interp_end_to_start
} NNSG3dMatCAnmOption;

typedef struct NNSG3dResMatCAnm_ {
  NNSG3dResAnmHeader anmHeader;
  u16 numFrame;
  u16 flag; // NNSG3dMatCAnmOption
  NNSG3dResDict dict;
} NNSG3dResMatCAnm;

typedef NNSG3dResDictAnmSetData NNSG3dResDictMatCAnmSetData;

typedef NNSG3dResAnmSet NNSG3dResMatCAnmSet;

typedef struct NNSG3dResTexPatAnmFV_ {
  u16 idxFrame;
  u8 idTex;
  u8 idPltt;
} NNSG3dResTexPatAnmFV;

typedef enum {
  NNS_G3D_TEXPATANM_OPTION_NOPLTT =
      0x0001 // ON when the palette is not animating
} NNSG3dTexPatAnmOption;

typedef struct NNSG3dResDictTexPatAnmData_ {
  u16 numFV;           // The number of NNSG3dResTexPatAnmFV
  u16 flag;            // NNSG3dTexPatAnmOption (Not currently used)
  fx16 ratioDataFrame; // This is the numFV / numFrame. It is a hint for data
                       // searching.
  u16 offset; // The offset to the FV data that used &NNSG3dResTexPatAnm as the
              // origin
} NNSG3dResDictTexPatAnmData;

typedef struct NNSG3dResTexPatAnm_ {
  NNSG3dResAnmHeader anmHeader;
  u16 numFrame;
  u8 numTex;      // The number of textures used
  u8 numPltt;     // The number of palettes used
  u16 ofsTexName; // The offset to the texture name array (numTex NNSG3dResNames
                  // are listed)
  u16 ofsPlttName;    // The offset to the palette name array (numPltt
                      // NNSG3dResNames are listed)
  NNSG3dResDict dict; // The dictionary that references
                      // NNSG3dResDictTexPatAnmData from the material name

} NNSG3dResTexPatAnm;

typedef NNSG3dResDictAnmSetData NNSG3dResDictTexPatAnmSetData;

typedef NNSG3dResAnmSet NNSG3dResTexPatAnmSet;

typedef enum {
  NNS_G3D_TEXSRTANM_ELEM_FX16 = 0x10000000, // ON when the data is held by the
                                            // fx16 vector (Always OFF when rot)
  NNS_G3D_TEXSRTANM_ELEM_CONST =
      0x20000000, // ON when the data is not animating
  NNS_G3D_TEXSRTANM_ELEM_STEP_1 =
      0x00000000, // When there is data in every frame
  NNS_G3D_TEXSRTANM_ELEM_STEP_2 =
      0x40000000, // When there is data in every 2nd frame
  NNS_G3D_TEXSRTANM_ELEM_STEP_4 =
      0x80000000, // When there is data in every 4th frame

  NNS_G3D_TEXSRTANM_ELEM_STEP_MASK = 0xc0000000,
  NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_MASK = 0x0000ffff,

  NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_SHIFT = 0
} NNSG3dTexSRTElem;

typedef struct NNSG3dResDictTexSRTAnmData_ {

  u32 scaleS;   // NNSG3dTexSRTElem
  u32 scaleSEx; // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST,
                // otherwise, the offset to the data string

  u32 scaleT;   // NNSG3dTexSRTElem
  u32 scaleTEx; // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST,
                // otherwise, the offset to the data string

  u32 rot;   // NNSG3dTexSRTElem
  u32 rotEx; // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST, otherwise,
             // the offset to the data string

  u32 transS;   // NNSG3dTexSRTElem
  u32 transSEx; // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST,
                // otherwise, the offset to the data string

  u32 transT;   // NNSG3dTexSRTElem
  u32 transTEx; // A constant value when NNS_G3D_TEXSRTANM_ELEM_CONST,
                // otherwise, the offset to the data string
} NNSG3dResDictTexSRTAnmData;

typedef enum NNSG3dTexSRTAnmOption_ {
  NNS_G3D_TEXSRTANM_OPTION_INTERPOLATION =
      0x01, // Corresponds to <tex_srt_info>::interpolation
  NNS_G3D_TEXSRTANM_OPTION_END_TO_START_INTERPOLATION =
      0x02 // Corresponds to <tex_srt_info>::interp_end_to_start
} NNSG3dTexSRTAnmOption;

typedef struct NNSG3dResTexSRTAnm_ {
  NNSG3dResAnmHeader anmHeader;
  u16 numFrame;
  u8 flag;       // NNSG3dTexSRTAnmOption
  u8 texMtxMode; // NNSG3dTexMtxMode
  NNSG3dResDict
      dict; // The material name->NNSG3dResDictTexSRTAnmData dictionary
} NNSG3dResTexSRTAnm;

typedef NNSG3dResDictAnmSetData NNSG3dResDictTexSRTAnmSetData;

typedef NNSG3dResAnmSet NNSG3dResTexSRTAnmSet;

typedef enum {
  NNS_G3D_JNTANM_SRTINFO_IDENTITY =
      0x00000001, // ON when there are no changes to the SRT
  NNS_G3D_JNTANM_SRTINFO_IDENTITY_T =
      0x00000002, // ON when Trans remains (0,0,0)
  NNS_G3D_JNTANM_SRTINFO_BASE_T =
      0x00000004, // ON when the value of the model is used in Trans
  NNS_G3D_JNTANM_SRTINFO_CONST_TX = 0x00000008,   // ON when Tx is a constant
  NNS_G3D_JNTANM_SRTINFO_CONST_TY = 0x00000010,   // ON when Ty is a constant
  NNS_G3D_JNTANM_SRTINFO_CONST_TZ = 0x00000020,   // ON when Tz is a constant
  NNS_G3D_JNTANM_SRTINFO_IDENTITY_R = 0x00000040, // ON when there is no Rot
  NNS_G3D_JNTANM_SRTINFO_BASE_R =
      0x00000080, // ON when the value of the model is used in Rot
  NNS_G3D_JNTANM_SRTINFO_CONST_R = 0x00000100, // ON when Rot is a constant
  NNS_G3D_JNTANM_SRTINFO_IDENTITY_S =
      0x00000200, // ON when the Scale is not used
  NNS_G3D_JNTANM_SRTINFO_BASE_S =
      0x00000400, // ON when the value of the model is used in Scale
  NNS_G3D_JNTANM_SRTINFO_CONST_SX = 0x00000800, // ON when Sx is a constant
  NNS_G3D_JNTANM_SRTINFO_CONST_SY = 0x00001000, // ON when Sy is a constant
  NNS_G3D_JNTANM_SRTINFO_CONST_SZ = 0x00002000, // ON when Sz is a constant

  NNS_G3D_JNTANM_SRTINFO_NODE_MASK =
      0xff000000, // Takes the node ID. Set so that it is OK if not defined by
                  // all nodes.
  NNS_G3D_JNTANM_SRTINFO_NODE_SHIFT = 24
} NNSG3dJntAnmSRTTag;

typedef enum {
  NNS_G3D_JNTANM_TINFO_STEP_1 = 0x00000000, // When there is data in every frame
  NNS_G3D_JNTANM_TINFO_STEP_2 =
      0x40000000, // When there is data in every 2nd frame
  NNS_G3D_JNTANM_TINFO_STEP_4 =
      0x80000000, // When there is data in every 4th frame
  NNS_G3D_JNTANM_TINFO_FX16ARRAY =
      0x20000000, // ON when the animation data is an fx16 array

  NNS_G3D_JNTANM_TINFO_LAST_INTERP_MASK = 0x1fff0000,
  NNS_G3D_JNTANM_TINFO_STEP_MASK = 0xc0000000,

  NNS_G3D_JNTANM_TINFO_LAST_INTERP_SHIFT = 16,
  NNS_G3D_JNTANM_TINFO_STEP_SHIFT = 30
} NNSG3dJntAnmTInfo;

typedef enum {
  NNS_G3D_JNTANM_RINFO_STEP_1 = 0x00000000, // When there is data in every frame
  NNS_G3D_JNTANM_RINFO_STEP_2 =
      0x40000000, // When there is data in every 2nd frame
  NNS_G3D_JNTANM_RINFO_STEP_4 =
      0x80000000, // When there is data in every 4th frame

  NNS_G3D_JNTANM_RINFO_LAST_INTERP_MASK = 0x1fff0000,
  NNS_G3D_JNTANM_RINFO_STEP_MASK = 0xc0000000,

  NNS_G3D_JNTANM_RINFO_LAST_INTERP_SHIFT = 16,
  NNS_G3D_JNTANM_RINFO_STEP_SHIFT = 30
} NNSG3dJntAnmRInfo;

typedef enum {
  NNS_G3D_JNTANM_PIVOTINFO_IDXPIVOT_MASK =
      0x000f, // The location of the element that is +1/-1
  NNS_G3D_JNTANM_PIVOTINFO_MINUS = 0x0010,     // If ON -1/ If OFF +1
  NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVC = 0x0020, // ON if C=-B
  NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVD = 0x0040, // ON if D=-A

  NNS_G3D_JNTANM_PIVOT_INFO_IDXPIVOT_SHIFT = 0
} NNSG3dJntAnmPivotInfo;

typedef enum {
  NNS_G3D_JNTANM_RIDX_PIVOT = 0x8000,
  NNS_G3D_JNTANM_RIDX_IDXDATA_MASK = 0x7fff,
  NNS_G3D_JNTANM_RIDX_IDXDATA_SHIFT = 0
} NNSG3dJntAnmRIdx;

typedef enum {
  NNS_G3D_JNTANM_SINFO_STEP_1 = 0x00000000, // When there is data in every frame
  NNS_G3D_JNTANM_SINFO_STEP_2 =
      0x40000000, // When there is data in every 2nd frame
  NNS_G3D_JNTANM_SINFO_STEP_4 =
      0x80000000, // When there is data in every 4th frame
  NNS_G3D_JNTANM_SINFO_FX16ARRAY =
      0x20000000, // When the data value is an fx16 type

  NNS_G3D_JNTANM_SINFO_LAST_INTERP_MASK = 0x1fff0000,
  NNS_G3D_JNTANM_SINFO_STEP_MASK = 0xc0000000,

  NNS_G3D_JNTANM_SINFO_LAST_INTERP_SHIFT = 16,
  NNS_G3D_JNTANM_SINFO_STEP_SHIFT = 30
} NNSG3dJntAnmSInfo;

typedef struct NNSG3dResJntAnmSRTInfo_ {
  u32 info; // NNSG3dJntAnmTInfo or NNSG3dJntAnmRInfo or NNSG3dJntAnmSInfo
  u32 offset;
} NNSG3dResJntAnmSRTInfo;

typedef struct NNSG3dResJntAnmSRTTag_ {
  u32 tag; // NNSG3dJntAnmSRTTag;
} NNSG3dResJntAnmSRTTag;

typedef enum {
  NNS_G3D_JNTANM_OPTION_INTERPOLATION = 0x01, // <node_anm_info>::interpolation
  NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION =
      0x02 // <node_anm_info>::interp_end_to_start
} NNSG3dJntAnmOption;

typedef struct NNSG3dResJntAnm_ {
  NNSG3dResAnmHeader anmHeader;
  u16 numFrame;
  u16 numNode;
  u32 flag;
  u32 ofsRot3;
  u32 ofsRot5;

} NNSG3dResJntAnm;

typedef NNSG3dResDictAnmSetData NNSG3dResDictJntAnmSetData;

typedef NNSG3dResAnmSet NNSG3dResJntAnmSet;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNSG3D_BINRES_RES_STRUCT_H_
