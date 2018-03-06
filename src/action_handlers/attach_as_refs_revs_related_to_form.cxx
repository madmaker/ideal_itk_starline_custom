#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/releasestatus.h>
#include "../misc.hxx"
#include "attach_as_refs_revs_related_to_form.hxx"
#include "rename_items_for_revisions_from_targets.hxx"

int read_arguments(TC_argument_list_t* arguments, char** rev_type, char** form_type, char** rel_type, char** attr_name, char** attr_value)
{
	int ifail = ITK_ok;
	int
		arguments_count = 0;
	char
		*flag = NULL,
		Flag[256],
		*value= NULL,
		*normal_value= NULL;

	try
	{
		WRITE_LOG("%s\n", "Reading arguments");
		arguments_count = TC_number_of_arguments(arguments);
		if (arguments_count != 1)
		{
			throw EPM_wrong_number_of_arguments;
		}
		*rev_type = "";
		*form_type = "";
		*rel_type = "";
		*attr_name = "";
		*attr_value = "";

		for (int i = 0; i < arguments_count; i++)
		{
			WRITE_LOG("%s\n", "Asking named value");
			IFERR_THROW( ITK_ask_argument_named_value(TC_next_argument(arguments), &flag, &value) );
			WRITE_LOG("%s\n", "No errors");
			IFERR_THROW( EPM_substitute_keyword(value, &normal_value) );
			if (normal_value != NULL)
			{
				WRITE_LOG("%s\n", "Normal value not NULL");
			    StrCpy(Flag,256,flag);
			    StrUpr(Flag);
			    if (strcmp("REV_TYPE", Flag)==0)
			    {
			        if (!STR_EMPTY(normal_value)) {
			        	*rev_type = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			        	strcpy(*rev_type, normal_value);
			        } else {
			        	*rev_type = NULL;
			        }
			    }
			    else if (strcmp("FORM_TYPE", Flag)==0)
			    {
			    	if (!STR_EMPTY(normal_value)) {
			    		*form_type = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*form_type, normal_value);
			    	} else {
			    		*form_type = NULL;
			    	}
			    }
			    else if (strcmp("REL_TYPE", Flag)==0)
			    {
			    	if (!STR_EMPTY(normal_value)) {
			    		*rel_type = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*rel_type, normal_value);
			    	} else {
			    		*rel_type = NULL;
			    	}
			    }
			    else if (strcmp("ATTR_NAME", Flag)==0)
			    {
			    	if (!STR_EMPTY(normal_value)) {
			    		*attr_name = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*attr_name, normal_value);
			    	} else {
			    		*attr_name = NULL;
			    	}
			    }
			    else if (strcmp("ATTR_VALUE", Flag)==0)
			    {
			    	if (!STR_EMPTY(normal_value)) {
			    		*attr_value = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*attr_value, normal_value);
			    	} else {
			    		*attr_value = NULL;
			    	}
			    }
		        MEM_free(normal_value);
			    normal_value = NULL;
			}
			else
			{
				WRITE_LOG("%s\n", "Empty attribute");
		   	}
			if (flag != NULL) {
				MEM_free(flag);
				flag = NULL;
			}
			if (value != NULL) {
				MEM_free(value);
				value = NULL;
			}
		}
		WRITE_LOG("%s\n", "Reading arguments is done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int has_related_form(tag_t revision, tag_t relation_type, tag_t form_type, logical* form_exists, tag_t* form)
{
	int ifail = ITK_ok;
	int
		related_objects_count = 0;
	tag_t
		*related_objects = NULL,
		temp_type;
	logical
		is_type_of = false;

	try
	{
		*form_exists = false;
		IFERR_THROW( GRM_list_secondary_objects_only(revision, relation_type, &related_objects_count, &related_objects) );
		for(int i = 0; i < related_objects_count; i++)
		{
			IFERR_THROW( TCTYPE_ask_object_type(related_objects[i], &temp_type) );
			IFERR_THROW( TCTYPE_is_type_of(temp_type, form_type, &is_type_of) );
			if(is_type_of)
			{
				*form_exists = true;
				*form = related_objects[i];
			}
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(related_objects!=NULL) MEM_free(related_objects);

	return ifail;
}

int form_attribute_equals_argument(tag_t form, char* attr_name, char* attr_value, logical* result)
{
	int ifail = ITK_ok;
	char*
		form_value = NULL;

	try
	{
		*result = false;
		IFERR_THROW( AOM_ask_value_string(form, attr_name, &form_value) );
		if(strcmp(form_value, attr_value)==0)
		{
			*result = true;
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(form_value!=NULL) MEM_free(form_value);

	return ifail;
}

int is_applicable_revision(tag_t revision, tag_t relation_type, tag_t form_type, char* attr_name, char* attr_value, logical* result)
{
	int ifail = ITK_ok;
	tag_t
		form = NULLTAG;
	logical
		has_form = false,
		has_equal_attributes = false;

	try
	{
		*result = false;
		IFERR_THROW( has_related_form(revision, relation_type, form_type, &has_form, &form) );
		if(has_form)
		{
			IFERR_THROW( form_attribute_equals_argument(form, attr_name, attr_value, &has_equal_attributes) );
		}

		if(!has_form && STR_EMPTY(attr_value))
		{
			*result = true;
		}
		else if (has_form && has_equal_attributes)
		{
			*result = true;
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int attach_applicable_revisions_of_item(tag_t revision_from_targets, tag_t root_task, tag_t relation_type, tag_t form_type, char* attr_name, char* attr_value)
{
	int ifail = ITK_ok;
	int
		revisions_count = 0;
	tag_t
		item = NULLTAG,
		*revisions = NULL;
	logical
		is_applicable = false;

	try
	{
		IFERR_THROW( ITEM_ask_item_of_rev(revision_from_targets, &item) );
		IFNULLTAG_THROW( item );
		IFERR_THROW( AOM_ask_value_tags(item, "revision_list", &revisions_count, &revisions) );
		if(revisions_count > 1)
		{
			for(int i = 0; i < revisions_count; i++)
			{
				IFERR_THROW( is_applicable_revision(revisions[i], relation_type, form_type, attr_name, attr_value, &is_applicable) );
				if(revisions[i]!=revision_from_targets && is_applicable)
				{
					IFERR_THROW( move_object_to_refs(revisions[i], root_task) );
				}
			}
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(revisions!=NULL) MEM_free(revisions);

	return ifail;
}

int attach_as_refs_revs_related_to_form(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments = NULL,
		root_task = NULLTAG,
		rev_type = NULLTAG,
		form_type = NULLTAG,
		rel_type = NULLTAG,
		temp_type = NULLTAG,
		item_revision_type = NULLTAG,
		pd_tag = NULLTAG;
	int
		*attachments_types = NULL,
		attachments_count = 0;
	char
		*rev_type_name = NULL,
		*form_type_name = NULL,
		*rel_type_name = NULL,
		*attr_name = NULL,
		*attr_value = NULL;
	logical
		is_type_of = false;

	try
	{
		IFERR_THROW( read_arguments(msg.arguments, &rev_type_name, &form_type_name, &rel_type_name, &attr_name, &attr_value) );

		IFERR_THROW( TCTYPE_find_type("ItemRevision", NULL, &item_revision_type) );
		IFERR_THROW( TCTYPE_find_type(rev_type_name, NULL, &rev_type) );
		IFERR_THROW( TCTYPE_find_type(form_type_name, NULL, &form_type) );
		IFERR_THROW( TCTYPE_find_type(rel_type_name, NULL, &rel_type) );
		IFNULLTAG_THROW( item_revision_type );
		IFNULLTAG_THROW( rev_type );
		IFNULLTAG_THROW( form_type );
		IFNULLTAG_THROW( rel_type );
		if( attr_name==NULL || STR_EMPTY(attr_name) ) throw EPM_invalid_argument_value;
		if( attr_value==NULL ) STR_INIT( attr_value );

		IFERR_THROW( TCTYPE_ask_property_by_name(form_type, attr_name, &pd_tag) ); //Check if property exists on form
		IFNULLTAG_THROW( pd_tag );

		WRITE_LOG("%s\n", "Asking root task and attachments");
		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		for(int i = 0; i < attachments_count; i++)
		{
			IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
			IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
			IFERR_THROW( TCTYPE_is_type_of(temp_type, item_revision_type, &is_type_of) );
			if(attachments_types[i]==EPM_target_attachment && is_type_of)
			{
				WRITE_LOG("%s\n", "Working with target of ItemRevision subtype");
				IFERR_THROW( attach_applicable_revisions_of_item(attachments[i], root_task, rel_type, form_type, attr_name, attr_value) );
			}
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(attachments!=NULL) MEM_free(attachments); attachments = NULL;
	if(attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;
	if(rev_type_name != NULL) MEM_free(rev_type_name);
	if(form_type_name != NULL) MEM_free(form_type_name);
	if(rel_type_name != NULL) MEM_free(rel_type_name);
	if(attr_name != NULL) MEM_free(attr_name);
	if(attr_value != NULL) MEM_free(attr_value);

	return ifail;
}
