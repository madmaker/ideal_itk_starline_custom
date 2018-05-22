#ifndef EXPORT_PLMXML_HXX_
#define EXPORT_PLMXML_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

void rename_file(char* oldname);
int ep_read_arguments(TC_argument_list_t* arguments, char** transfer_mode, char** rev_rule, char** export_dir);
int get_related_sap_form(tag_t primary_object, tag_t relation, tag_t* sap_form);
int export_plmxml(EPM_action_message_t);

#endif
