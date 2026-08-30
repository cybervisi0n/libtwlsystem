#ifndef NNS_FND_ARCHIVE_H_
#define NNS_FND_ARCHIVE_H_

#include <nitro/os.h>
#include <nitro/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u32 fileTop;    // Offset to start of file image
  u32 fileBottom; // offset of file image end

} NNSiFndArchiveFatEntry;

typedef struct {
  u32 blockType; // block type
  u32 blockSize; // block size
  u16 numFiles;  // number of files
  u16 reserved;  // Reserved
  NNSiFndArchiveFatEntry fatEntries[1];

} NNSiFndArchiveFatData;

typedef struct {
  u32 signature;  // signature (NARC)
  u16 byteOrder;  // byte order (0xfeff)
  u16 version;    // Version number
  u32 fileSize;   // archive file size
  u16 headerSize; // archive file header size
  u16 dataBlocks; // number of data blocks

} NNSiFndArchiveHeader;

typedef struct {
  FSArchive fsArchive;             // Work for file system
  NNSiFndArchiveHeader *arcBinary; // Start of archive binary data
  NNSiFndArchiveFatData *fatData;  // Start of archive FAT data
  u32 fileImage;                   // Start of archive file image

} NNSFndArchive;

BOOL NNS_FndMountArchive(NNSFndArchive *archive, const char *arcName,
                         void *arcBinary);

BOOL NNS_FndUnmountArchive(NNSFndArchive *archive);

void *NNS_FndGetArchiveFileByName(const char *path);

void *NNS_FndGetArchiveFileByIndex(NNSFndArchive *archive, u32 index);

BOOL NNS_FndOpenArchiveFileByIndex(FSFile *file, NNSFndArchive *archive,
                                   u32 index);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
