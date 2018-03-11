#ifndef SET_CHANGE_NOTICE_NUMBER_HXX_
#define SET_CHANGE_NOTICE_NUMBER_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int find_new_value(char* attr_name, char** value);
int set_form_value_if_empty(tag_t form, char* attr_name);
int set_change_notice_number(EPM_action_message_t);

#endif
