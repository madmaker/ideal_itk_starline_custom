#ifndef ASSIGN_FORM_PROPERTY_TO_TARGETS_HXX_
#define ASSIGN_FORM_PROPERTY_TO_TARGETS_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int read_arguments(TC_argument_list_t* arguments, char** type, char** attribute_name, char** attribute_value);
int set_property(tag_t object, char* prop_name, char* prop_value);
int assign_form_property_to_targets(EPM_action_message_t);

#endif
