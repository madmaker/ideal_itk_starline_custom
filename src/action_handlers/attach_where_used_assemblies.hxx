#ifndef ATTACH_WHERE_USED_ASSEMBLIES_HXX_
#define ATTACH_WHERE_USED_ASSEMBLIES_HXX_

#include <tc/tc.h>
#include <epm.h>

int awua_read_arguments(TC_argument_list_t* arguments, char** statuses_to_ignore);
int awua_has_no_except_statuses(tag_t object, int statuses_count, char** statuses, logical* result);
int find_assemblies_and_add_them(tag_t root_task, tag_t object, int statuses_count, char** statuses);
int awua_convert_status_names_string_to_list(char* status_names_string, int* statuses_count, char*** statuses);
int attach_where_used_assemblies(EPM_action_message_t msg);

#endif
