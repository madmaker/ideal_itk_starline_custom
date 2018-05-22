#ifndef CHECK_GROUP_AND_ROLE_RH_HXX_
#define CHECK_GROUP_AND_ROLE_RH_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int cgar_rh_read_arguments(TC_argument_list_t* arguments, char** role);
EPM_decision_t check_group_and_role_rh(EPM_rule_message_t msg);

#endif
