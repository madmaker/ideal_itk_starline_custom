#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include "../misc.hxx"
#include "check_group_and_role.hxx"

int rrot_read_arguments(TC_argument_list_t* arguments, char** relation_name, char** secondary_type_name)
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
		if (arguments_count != 2)
		{
			throw EPM_wrong_number_of_arguments;
		}
		*relation_name = "";
		*secondary_type_name = "";

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
			    if (strcmp("RELATION", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its role");
			        if (!STR_EMPTY(normal_value)) {
			        	*relation_name = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			        	strcpy(*relation_name, normal_value);
			        	WRITE_LOG("%s\n", *relation_name);
			        } else {
			        	*relation_name = "";
			        }
			    }
			    if (strcmp("SECONDARY", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its role");
			    	if (!STR_EMPTY(normal_value)) {
			    		*secondary_type_name = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*secondary_type_name, normal_value);
			    		WRITE_LOG("%s\n", *secondary_type_name);
			    	} else {
			    		*secondary_type_name = "";
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

int remove_relation_to_object(tag_t primary, tag_t relation_type, tag_t secondary_type)
{
	int ifail = ITK_ok;
	int
		related_objects_count = 0;
	tag_t
		*related_objects = NULL,
		temp_type = NULLTAG,
		relation = NULLTAG;
	logical
		is_type_of = false;

	try
	{
		IFERR_THROW( GRM_list_secondary_objects_only(primary, relation_type, &related_objects_count, &related_objects) );
		for(int i = 0; i < related_objects_count; i++)
		{
			IFERR_THROW( TCTYPE_ask_object_type(related_objects[i], &temp_type) );
			IFERR_THROW( TCTYPE_is_type_of(temp_type, secondary_type, &is_type_of) );
			if(is_type_of)
			{
				IFERR_THROW( GRM_find_relation(primary, related_objects[i], relation_type, &relation) );
				IFNULLTAG_THROW( relation );
				IFERR_THROW( GRM_delete_relation(relation) );
			}
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(related_objects!=NULL) MEM_free(related_objects); related_objects = NULL;

	return ifail;
}

int remove_relations_of_targets(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments = NULL,
		root_task = NULLTAG,
		relation_type = NULLTAG,
		secondary_type = NULLTAG;
	int
		*attachments_types = NULL,
		attachments_count = 0;
	char
		*relation_name,
		*secondary_type_name;

	try
	{
		WRITE_LOG("%s\n", "Asking root task and attachmenmts");

		IFERR_THROW( rrot_read_arguments(msg.arguments, &relation_name, &secondary_type_name) );
		IFERR_THROW( TCTYPE_find_type(secondary_type_name, NULL, &secondary_type) );
		IFERR_THROW( TCTYPE_find_type(relation_name, NULL, &relation_type) );
		IFNULLTAG_THROW( secondary_type );
		IFNULLTAG_THROW( relation_type );

		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );

		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "---");
				IFERR_THROW( remove_relation_to_object(attachments[i], relation_type, secondary_type) );
			}
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(relation_name!=NULL) MEM_free(relation_name);
	if(secondary_type_name!=NULL) MEM_free(secondary_type_name);
	if(attachments!=NULL) MEM_free(attachments); attachments = NULL;
	if(attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;

	return ifail;
}
