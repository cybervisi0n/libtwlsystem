#ifndef NNS_FND_ARCHIVE_BLOCK_H_
#define NNS_FND_ARCHIVE_BLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u32 blockType; // block type
  u32 blockSize; // block size

} NNSiFndArchiveBlockHeader;

typedef NNSiFndArchiveBlockHeader NNSiFndArchiveDirBlockHeader;
typedef NNSiFndArchiveBlockHeader NNSiFndArchiveImgBlockHeader;

typedef struct {
  u32 blockType; // block type
  u32 blockSize; // block size
  u16 numFiles;  // number of files
  u16 reserved;  // Reserved

} NNSiFndArchiveFatBlockHeader;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
