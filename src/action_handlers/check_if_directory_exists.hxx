#ifndef CHECK_IF_DIRECTORY_EXISTS_HXX_
#define CHECK_IF_DIRECTORY_EXISTS_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

/**
 * Handler проверяет, существует ли папка, переданная аргументом
 */

int cide_read_arguments(TC_argument_list_t* arguments, char** directory);
EPM_decision_t check_if_directory_exists(EPM_rule_message_t);
logical directory_exists(char* directory);

#endif
