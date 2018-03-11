#ifndef ATTACH_USED_COMPONENTS_HXX_
#define ATTACH_USED_COMPONENTS_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int auc_read_arguments(TC_argument_list_t* arguments, char** statuses_to_ignore);
int auc_has_no_except_statuses(tag_t object, int statuses_count, char** statuses, logical* result);
int find_components_and_add_them(tag_t root_task, tag_t object, int statuses_count, char** statuses);
int auc_convert_status_names_string_to_list(char* status_names_string, int* statuses_count, char*** statuses);
int attach_used_components(EPM_action_message_t msg);
int attachments_contain_object(tag_t object, int attachments_count, tag_t* attachments, logical* result);

#endif
