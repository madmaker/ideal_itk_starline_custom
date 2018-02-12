#ifndef ATTACH_PREVIOUS_REVISIONS_HXX_
#define ATTACH_PREVIOUS_REVISIONS_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int read_arguments(TC_argument_list_t* arguments, char** statuses_to_ignore);
int has_no_except_statuses(tag_t object, int statuses_count, char** statuses, logical* result);
int compare_dates(const date_t *date1, const date_t *date2);
int find_prev_revisions_and_add_them(tag_t root_task, tag_t object, int statuses_count, char** statuses);
int convert_status_names_string_to_list(char* status_names_string, int* statuses_count, char*** statuses);
int attach_previous_revisions(EPM_action_message_t msg);

#endif
