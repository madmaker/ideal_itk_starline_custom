#ifndef EXPORT_PLMXML_HXX_
#define EXPORT_PLMXML_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int get_key_object_data(tag_t object, char* key, char** key_values);
void rename_file(char* oldname);
int ep_read_arguments(TC_argument_list_t* arguments, char** transfer_mode, char** rev_rule, char** export_dir, char** type_obj_uid_file_name, char** type_obj_export, char** export_bom, char** type_obj_key, char** key);
int export_plmxml(EPM_action_message_t);

#endif
