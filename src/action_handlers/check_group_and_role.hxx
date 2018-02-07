#ifndef CHECK_GROUP_AND_ROLE_HXX_
#define CHECK_GROUP_AND_ROLE_HXX_

#include <tc/tc.h>
#include <epm.h>

int cgar_read_arguments(TC_argument_list_t* arguments, char** role);
int check_group_and_role(EPM_action_message_t msg);

#endif
