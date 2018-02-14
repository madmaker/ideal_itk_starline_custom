#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/releasestatus.h>
#include "../misc.hxx"
#include "attach_previous_revisions.hxx"

tag_t item_revision_type = NULLTAG;

int read_arguments(TC_argument_list_t* arguments, char** statuses_to_ignore)
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

int has_no_except_statuses(tag_t object, int statuses_count, char** statuses, logical* result)
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

int compare_dates(const date_t *date1, const date_t *date2)
{
	if (date1->year != date2->year)
        return date1->year > date2->year ? 1 : -1;

    if (date1->month != date2->month)
        return date1->month > date2->month ? 1 : -1;

    if (date1->day != date2->day)
        return date1->day > date2->day ? 1 : -1;

    if (date1->hour != date2->hour)
        return date1->hour > date2->hour ? 1 : -1;

    if (date1->minute != date2->minute)
        return date1->minute > date2->minute ? 1 : -1;

    if (date1->second != date2->second)
        return date1->second > date2->second ? 1 : -1;

    return 0;
}

int find_prev_revisions_and_add_them(tag_t root_task, tag_t object, int statuses_count, char** statuses)
{
	int ifail = ITK_ok;
	tag_t object_type = NULLTAG;
	tag_t item = NULLTAG;
	tag_t *revisions = NULL;
	logical is_type_of = false;
	int revisions_count = 0;
	date_t object_date;
	date_t temp_date;
	int* attachments_types_to_add = NULL;
	tag_t* attachments_to_add = NULL;

	try
	{
		WRITE_LOG("%s\n", "Looking for previous revisions");
		IFERR_THROW( TCTYPE_ask_object_type(object, &object_type) );
		IFERR_THROW( TCTYPE_is_type_of(object_type, item_revision_type, &is_type_of) );
		if(is_type_of)
		{
			WRITE_LOG("%s\n", "Is ItemRevision");
			IFERR_THROW( ITEM_ask_item_of_rev(object, &item) );
			IFERR_THROW( AOM_ask_value_date(object, "creation_date", &object_date) );
			IFERR_THROW( AOM_ask_value_tags(object, "revision_list", &revisions_count, &revisions) );
			int number_to_add = 0;
			logical result = false;
			attachments_types_to_add = (int*) MEM_alloc(sizeof(int) * revisions_count);
			attachments_to_add = (tag_t*) MEM_alloc(sizeof(tag_t) * revisions_count);

			for(int i = 0; i < revisions_count; i++)
			{
				IFERR_THROW( AOM_ask_value_date(revisions[i], "creation_date", &temp_date) );
				if(compare_dates(&object_date, &temp_date) > 0)
				{
					IFERR_THROW( has_no_except_statuses(revisions[i], statuses_count, statuses, &result) );
					if(result && object!=revisions[i])
					{
						WRITE_LOG("%s\n", "Adding to attachments");
						attachments_to_add[number_to_add] = revisions[i];
						attachments_types_to_add[number_to_add] = EPM_reference_attachment;
						number_to_add++;
					}
				}
			}

			if(number_to_add > 0) IFERR_THROW( EPM_add_attachments(root_task, number_to_add, attachments_to_add, attachments_types_to_add) );

		}
	}
	catch(int exfail)
	{
		ifail = exfail;
	}

	if(attachments_types_to_add!=NULL) MEM_free(attachments_types_to_add);
	if(attachments_to_add!=NULL) MEM_free(attachments_to_add);
	if(revisions!=NULL) MEM_free(revisions);

	return ifail;
}

int convert_status_names_string_to_list(char* status_names_string, int* statuses_count, char*** statuses)
{
	int ifail = ITK_ok;
	int status_names_count = 1;
	int count = 0;
	char* delim = ",";
	char *temp = "";

	try
	{
		WRITE_LOG("%s\n", "Converting status string to list");
		if(STR_EMPTY(status_names_string))
		{
			*statuses_count = 0;
			*statuses = NULL;
			return ITK_ok;
		}
		temp = strtok (status_names_string, delim);

		for(int i = 0; i < strlen(status_names_string); i++)
		{
			if(status_names_string[i] == ',') status_names_count++;
		}
		WRITE_LOG("%s\n", "Allocating space for list");
		*statuses = (char**) MEM_alloc(sizeof(statuses) * status_names_count);
		while (temp != NULL)
		{
			WRITE_LOG("%s\n", "Allocating space for list entry");
			*statuses[count] = (char*) MEM_alloc(sizeof(statuses[count]) * (strlen(temp)+1));
			strcpy(*statuses[count++], temp);
		    temp = strtok (NULL, delim);
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int attach_previous_revisions(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments,
		root_task;
	int
		*attachments_types,
		attachments_count = 0,
		statuses_count;
	char
		*status_names_to_ignore_string,
		**status_names_to_ignore_list;

	try
	{
		IFERR_THROW( read_arguments(msg.arguments, &status_names_to_ignore_string) );

		IFERR_THROW( TCTYPE_find_type("ItemRevision", NULL, &item_revision_type) );
		IFERR_THROW( convert_status_names_string_to_list(status_names_to_ignore_string, &statuses_count, &status_names_to_ignore_list) );

		WRITE_LOG("%s\n", "Asking root task and attachmenmts");
		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "Working with target");
				IFERR_THROW( find_prev_revisions_and_add_them(root_task, attachments[i], statuses_count, status_names_to_ignore_list) );
			}
		}

		MEM_free(attachments);
		MEM_free(attachments_types);
		for(int i = 0; i < statuses_count; i++)
		{
			MEM_free(status_names_to_ignore_list[i]);
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}
