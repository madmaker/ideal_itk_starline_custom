#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <res/res_itk.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/releasestatus.h>
#include "../misc.hxx"
#include "attach_as_refs_related_wo.hxx"
#include "rename_items_for_revisions_from_targets.hxx"

int aarrw_read_arguments(TC_argument_list_t* arguments, char** relations)
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
		*relations = "";

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
			    	WRITE_LOG("%s\n", "Its relation");
			    	if (!STR_EMPTY(normal_value)) {
			    		*relations = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*relations, normal_value);
			    		WRITE_LOG("%s\n", *relations);
			    	} else {
			    		*relations = "";
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

int cycle_through_relations(tag_t primary, tag_t root_task, tag_t wo_type, int rel_types_count, tag_t* rel_types)
{
	int ifail = ITK_ok;

	try
	{
		WRITE_LOG("%s\n", ".cycle_through_relations");
		for(int i = 0; i < rel_types_count; i++)
		{
			attach_related_locked_wo(primary, root_task, wo_type, rel_types[i]);
		}
		WRITE_LOG("%s\n", ".cycle_through_relations done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int attach_related_locked_wo(tag_t primary, tag_t root_task, tag_t wo_type, tag_t rel_type)
{
	int ifail = ITK_ok;

	int related_objects_count = 0;
	tag_t* related_objects = NULL;
	tag_t temp_type = NULLTAG;
	logical is_type_of = false;
	logical is_locked = false;

	try
	{
		WRITE_LOG("%s\n", ".attach_related_locked_wo");
		IFERR_THROW( GRM_list_secondary_objects_only(primary, rel_type, &related_objects_count, &related_objects) );

		for(int i = 0; i < related_objects_count; i++)
		{
			IFERR_THROW( TCTYPE_ask_object_type(related_objects[i], &temp_type) );
			IFNULLTAG_THROW( temp_type );
			IFERR_THROW( TCTYPE_is_type_of(temp_type, wo_type, &is_type_of) );
			IFERR_THROW( RES_is_checked_out(related_objects[i], &is_locked) );
			if(is_type_of && is_locked)
			{
				move_object_to_refs(related_objects[i], root_task);
			}
		}
		WRITE_LOG("%s\n", ".attach_related_locked_wo done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(related_objects!=NULL) MEM_free(related_objects); related_objects = NULL;

	return ifail;
}

int convert_relation_names_string_to_list(char* relation_names_string, int* relations_count, tag_t** relations)
{
	int ifail = ITK_ok;
	int relation_names_count = 1;
	int count = 0;
	char* delim = ",";
	char* temp;
	char* templine = NULL;
	tag_t* temp_relations = NULL;
	tag_t temp_type = NULLTAG;

	try
	{
		WRITE_LOG("%s\n", ".convert_relation_names_string_to_list");
		if(strlen(relation_names_string) == 0)
		{
			*relations_count = 0;
			*relations = NULL;
			return ITK_ok;
		}

		templine = (char*) MEM_alloc(sizeof(*relation_names_string) * (strlen(relation_names_string)+1));
		strcpy(templine, relation_names_string);

		for(int i = 0; i < strlen(templine); i++)
		{
			if(templine[i] == ',') relation_names_count++;
			WRITE_LOG("%s\n", "One more status");
		}

		WRITE_LOG("%s\n", "Converting status string to list");
		temp = strtok (templine, delim);

		WRITE_LOG("%s\n", "Allocating space for list");
		temp_relations = (tag_t*) MEM_alloc(sizeof(*temp_relations) * relation_names_count);

		while (temp != NULL)
		{
			WRITE_LOG("%s\n", "Allocating space for list entry");
			IFERR_THROW( TCTYPE_find_type(temp, NULL, &temp_type) );
			IFNULLTAG_THROW( temp_type );
			temp_relations[count] = temp_type;
			count++;

		    temp = strtok (NULL, delim);
		}

		*relations_count = relation_names_count;
		*relations = temp_relations;
		WRITE_LOG("%s\n", ".convert_relation_names_string_to_list done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(templine!=NULL) MEM_free(templine); templine = NULL;

	return ifail;
}

int attach_as_refs_related_wo(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments = NULL,
		root_task = NULLTAG,
		temp_type = NULLTAG,
		wo_type = NULLTAG,
		*rel_types;
	int
		*attachments_types = NULL,
		attachments_count = 0,
		rel_types_count = 0;
	char
		*rel_types_string = NULL;

	try
	{
		WRITE_LOG("%s\n", ".attach_as_refs_related_wo");
		IFERR_THROW( aarrw_read_arguments(msg.arguments, &rel_types_string) );
		IFERR_THROW( convert_relation_names_string_to_list(rel_types_string, &rel_types_count, &rel_types) );

		IFERR_THROW( TCTYPE_find_type("WorkspaceObject", NULL, &wo_type) );

		WRITE_LOG("%s\n", "Asking root task and attachments");
		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		for(int i = 0; i < attachments_count; i++)
		{
			IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
			IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
			if(attachments_types[i]==EPM_target_attachment)
			{
				IFERR_THROW( cycle_through_relations(attachments[i], root_task, wo_type, rel_types_count, rel_types) );
			}
		}
		WRITE_LOG("%s\n", ".attach_as_refs_related_wo done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(attachments!=NULL) MEM_free(attachments); attachments = NULL;
	if(attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;
	if(rel_types_string != NULL) MEM_free(rel_types_string); rel_types_string = NULL;
	if(rel_types!=NULL) MEM_free(rel_types); rel_types = NULL;

	return ifail;
}
