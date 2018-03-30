#ifndef ATTACH_AS_REFS_RELATED_WO_HXX_
#define ATTACH_AS_REFS_RELATED_WO_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int aarrw_read_arguments(TC_argument_list_t* arguments, char** rel_type);
int cycle_through_relations(tag_t primary, tag_t root_task, tag_t wo_type, int rel_types_count, tag_t* rel_types);
int attach_related_locked_wo(tag_t primary, tag_t root_task, tag_t wo_type, tag_t rel_type);
int convert_relation_names_string_to_list(char* relation_names_string, int* relations_count, tag_t** relations);
int attach_as_refs_related_wo(EPM_action_message_t msg);

#endif
