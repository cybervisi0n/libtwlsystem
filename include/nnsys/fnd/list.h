#ifndef NNS_FND_LIST_H_
#define NNS_FND_LIST_H_

#include <stddef.h>
#include <nitro/types.h>
#include <nnsys/misc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void *prevObject; // Pointer to the previous linked object.
  void *nextObject; // Pointer to the next linked object.

} NNSFndLink;

typedef struct {
  void *headObject; // Pointer for the object linked to the top of the list.
  void *tailObject; // Pointer for the object linked to the end of the list.
  u16 numObjects;   // Number of objects linked in the list.
  u16 offset;       // Offset for NNSFndLink type structure member.

} NNSFndList;

#define NNS_FND_INIT_LIST(list, structName, linkName)                          \
  NNS_FndInitList(list, offsetof(structName, linkName))

void NNS_FndInitList(NNSFndList *list, u16 offset);

void NNS_FndAppendListObject(NNSFndList *list, void *object);

void NNS_FndPrependListObject(NNSFndList *list, void *object);

void NNS_FndInsertListObject(NNSFndList *list, void *target, void *object);

void NNS_FndRemoveListObject(NNSFndList *list, void *object);

void *NNS_FndGetNextListObject(const NNSFndList *list, const void *object);

void *NNS_FndGetPrevListObject(const NNSFndList *list, const void *object);

void *NNS_FndGetNthListObject(const NNSFndList *list, u16 index);

inline u16 NNS_FndGetListSize(const NNSFndList *list) {
  NNS_NULL_ASSERT(list);

  return list->numObjects;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
