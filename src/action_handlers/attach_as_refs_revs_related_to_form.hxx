#ifndef ATTACH_AS_REFS_REVS_RELATED_TO_FORM_HXX_
#define ATTACH_AS_REFS_REVS_RELATED_TO_FORM_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int read_arguments(TC_argument_list_t* arguments, char** rev_type, char** form_type, char** rel_type, char** attr_name, char** attr_value);
int has_related_form(tag_t revision, tag_t relation_type, tag_t form_type, logical* form_exists, tag_t* form);
int form_attribute_equals_argument(tag_t form, char* attr_name, char* attr_value, logical* result);
int is_applicable_revision(tag_t revision, tag_t relation_type, tag_t form_type, char* attr_name, char* attr_value, logical* result);
int attach_applicable_revisions_of_item(tag_t revision_from_targets, tag_t root_task, tag_t relation_type, tag_t form_type, char* attr_name, char* attr_value);
int attach_as_refs_revs_related_to_form(EPM_action_message_t msg);

#endif
