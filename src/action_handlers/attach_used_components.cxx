#include <bom/bom.h>
#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <ps/ps.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/releasestatus.h>
#include "../misc.hxx"
#include "attach_used_components.hxx"
#include "../actions/post_remove_all_substitutes.hxx"

tag_t auc_item_revision_type = NULLTAG;

int auc_read_arguments(TC_argument_list_t* arguments, char** statuses_to_ignore)
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
		*statuses_to_ignore = "";

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
			    if (strcmp("EXCEPT", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its except");
			        if (!STR_EMPTY(normal_value)) {
			        	*statuses_to_ignore = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			        	strcpy(*statuses_to_ignore, normal_value);
			        	WRITE_LOG("%s\n", *statuses_to_ignore);
			        } else {
			        	*statuses_to_ignore = "";
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

int auc_has_no_except_statuses(tag_t object, int statuses_count, char** statuses, logical* result)
{
	int ifail = ITK_ok;
	logical result_r = true;
	char release_status_type[WSO_name_size_c+1];

	try
	{
		WRITE_LOG("%s\n", "Checking statuses");
		int rs_count;
		tag_t* rs_list;
		IFERR_THROW( WSOM_ask_release_status_list(object, &rs_count, &rs_list) );
		for(int j = 0; j < statuses_count; j++)
		{
			if(rs_count==0 && strcmp(statuses[j], "NOT")==0) result_r = false;

			for(int i = 0; i < rs_count; i++)
			{
				IFERR_THROW( CR_ask_release_status_type(rs_list[i], release_status_type) );
				WRITE_LOG("Comparing [%s]==[%s]\n", statuses[j], release_status_type);
				if(strcmp(release_status_type, statuses[j]) == 0)
				{
					WRITE_LOG("%s\n", "Found exception status");
					result_r = false;
				}
			}
		}
		*result = result_r;
	}
	catch(int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int attachments_contain_object(tag_t object, int attachments_count, tag_t* attachments, logical* result)
{
	int ifail = ITK_ok;

	try
	{
		*result = false;
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments[i] == object) *result = true;
		}
	}
	catch(int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int find_components_and_add_them(tag_t root_task, tag_t object, int statuses_count, char** statuses, int attachments_count, tag_t* attachments)
{
	int ifail = ITK_ok;
	tag_t object_type;
	logical is_type_of;

	try
	{
		WRITE_LOG("%s\n", "Looking for previous revisions");
		IFERR_THROW( TCTYPE_ask_object_type(object, &object_type) );
		IFERR_THROW( TCTYPE_is_type_of(object_type, auc_item_revision_type, &is_type_of) );

		if(is_type_of)
		{
			WRITE_LOG("%s\n", "Is ItemRevision");

			int bvrs_count = 0;
			tag_t* bvrs = NULL;
			IFERR_THROW( ITEM_rev_list_bom_view_revs(object, &bvrs_count, &bvrs) );

			for(int i = 0; i < bvrs_count; i++)
			{
				tag_t bom_window = NULLTAG;
				tag_t top_line = NULLTAG;
				int child_lines_count = 0;
				tag_t* child_lines = NULLTAG;
				tag_t revision = NULLTAG;
				logical result = false;
				logical already_is_attachment = false;

				IFERR_THROW( get_bom_window_and_top_line_from_bvr(bvrs[i], true, &bom_window, &top_line) );
				IFERR_THROW( BOM_line_ask_all_child_lines(top_line, &child_lines_count, &child_lines) );
				int number_to_add = 0;
				int* attachments_types_to_add = (int*) MEM_alloc(sizeof(int) * child_lines_count);
				tag_t* attachments_to_add = (tag_t*) MEM_alloc(sizeof(tag_t) * child_lines_count);

				for(int j = 0; j < child_lines_count; j++)
				{
					IFERR_THROW( AOM_ask_value_tag(child_lines[j], "bl_revision", &revision) );
					IFERR_THROW( auc_has_no_except_statuses(revision, statuses_count, statuses, &result) );
					IFERR_THROW( attachments_contain_object(revision, attachments_count, attachments, &already_is_attachment) );
					if(result && !already_is_attachment)
					{
						WRITE_LOG("%s\n", "Adding to attachments");
						try
						{
							attachments_to_add[number_to_add] = revision;
							attachments_types_to_add[number_to_add] = EPM_reference_attachment;
							number_to_add++;
						}
						catch(int exfail)
						{
							WRITE_LOG("%s\n", "Adding to attachments failed");
						}
					}
				}
				IFERR_THROW( BOM_close_window(bom_window) );
				IFERR_THROW( EPM_add_attachments(root_task, number_to_add, attachments_to_add, attachments_types_to_add) );
				MEM_free(attachments_types_to_add);
				MEM_free(attachments_to_add);

				if(child_lines_count > 0) MEM_free(child_lines);
			}

			if(bvrs_count>0) MEM_free(bvrs);
		}
	}
	catch(int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int auc_convert_status_names_string_to_list(char* status_names_string, int* statuses_count, char*** statuses)
{
	int ifail = ITK_ok;
	int status_names_count = 1;
	int count = 0;
	char* delim = ",";
	char* temp;
	char* templine = NULL;
	char** temp_statuses;


	try
	{
		if(strlen(status_names_string) == 0)
		{
			*statuses_count = 0;
			*statuses = NULL;
			return ITK_ok;
		}

		templine = (char*) MEM_alloc(sizeof(*status_names_string) * (strlen(status_names_string)+1));
		strcpy(templine, status_names_string);

		for(int i = 0; i < strlen(templine); i++)
		{
			if(templine[i] == ',') status_names_count++;
			WRITE_LOG("%s\n", "One more status");
		}

		WRITE_LOG("%s\n", "Converting status string to list");
		temp = strtok (templine, delim);

		WRITE_LOG("%s\n", "Allocating space for list");
		temp_statuses = (char**) MEM_alloc(sizeof(*temp_statuses) * status_names_count);

		while (temp != NULL)
		{
			WRITE_LOG("%s\n", "Allocating space for list entry");
			temp_statuses[count] = (char*) MEM_alloc(sizeof(*temp_statuses[count]) * (strlen(temp)+1));
			strcpy(temp_statuses[count++], temp);

		    temp = strtok (NULL, delim);
		}

		*statuses_count = status_names_count;
		*statuses = temp_statuses;
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(templine!=NULL) MEM_free(templine); templine = NULL;

	return ifail;
}

int attach_used_components(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments = NULL,
		root_task = NULLTAG;
	int
		*attachments_types = NULL,
		attachments_count = 0,
		statuses_count = 0;
	char
		*status_names_to_ignore_string,
		**status_names_to_ignore_list;

	try
	{
		IFERR_THROW( auc_read_arguments(msg.arguments, &status_names_to_ignore_string) );

		IFERR_THROW( TCTYPE_find_type("ItemRevision", NULL, &auc_item_revision_type) );
		IFERR_THROW( auc_convert_status_names_string_to_list(status_names_to_ignore_string, &statuses_count, &status_names_to_ignore_list) );

		WRITE_LOG("%s\n", "Asking root task and attachmenmts");
		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "Working with target");
				IFERR_THROW( find_components_and_add_them(root_task, attachments[i], statuses_count, status_names_to_ignore_list, attachments_count, attachments) );
			}
		}

	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(attachments!=NULL) MEM_free(attachments); attachments = NULL;
	if(attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;
	for(int i = 0; i < statuses_count; i++)
	{
		MEM_free(status_names_to_ignore_list[i]);
	}

	return ifail;
}
