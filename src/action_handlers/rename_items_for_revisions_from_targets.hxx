#ifndef RENAME_ITEMS_FOR_REVISIONS_FROM_TARGETS_HXX_
#define RENAME_ITEMS_FOR_REVISIONS_FROM_TARGETS_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int rename_item_as_revision(tag_t revision, tag_t item, tag_t root_task);
int move_object_to_refs(tag_t object, tag_t root_task);
int rename_item_or_move_to_refs_if_locked(tag_t revision, tag_t root_task);
int rename_items_for_revisions_from_targets(EPM_action_message_t msg);

#endif
