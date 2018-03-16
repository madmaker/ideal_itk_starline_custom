#ifndef REMOVE_RELATIONS_OF_TARGETS_HXX_
#define REMOVE_RELATIONS_OF_TARGETS_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int rrot_read_arguments(TC_argument_list_t* arguments, char** relation_name, char** secondary_type_name);
int remove_relation_to_object(tag_t primary, tag_t relation_type, tag_t secondary_type);
int remove_relations_of_targets(EPM_action_message_t msg);

#endif
