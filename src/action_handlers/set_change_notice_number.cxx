#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/aom.h>
#include <tccore/grm.h>
#include <tccore/aom_prop.h>
#include <tccore/tctype.h>
#include "../misc.hxx"
#include "set_change_notice_number.hxx"
#include "assign_form_property_to_targets.hxx"

#define CHANGE_NOTICE_NUMBER_LENGTH 8

int find_new_value(char* attr_name, char** value)
{
	int ifail = ITK_ok;
	tag_t
		query = NULLTAG,
		*found_objects = NULL;
	int
		found_objects_count = 0;
	int
		*existing_values = NULL,
		existing_values_count = 0;

	try
	{
		IFERR_THROW( QRY_find("__IDEALPLM - FindCNForms", &query) );
		IFNULLTAG_THROW( query );
		IFERR_THROW( QRY_execute(query, 0, NULL, NULL, &found_objects_count, &found_objects) );
		if (found_objects_count != 0)
		{
			TC_write_syslog("%s\n", "results>0");
			char* temp_value;
			int temp_value_int = 0;
			existing_values = (int*) MEM_alloc(sizeof(int) * found_objects_count);
			for(int j = 0; j < found_objects_count; j++)
			{
				IFERR_THROW( AOM_ask_value_string(found_objects[j], attr_name, &temp_value) );
				if( !STR_EMPTY(temp_value) )
				{
					temp_value_int = atoi( temp_value );
					existing_values[j] = temp_value_int;
					existing_values_count++;
				}
				else
				{
					existing_values[j] = 0;
					existing_values_count++;
				}
				MEM_free(temp_value);
			}
			if(existing_values_count==0)
			{
				*value = (char*) MEM_alloc(sizeof(char) * (CHANGE_NOTICE_NUMBER_LENGTH + 1));
				strcpy(*value, "00000000");
			}
			else
			{
				qsort(existing_values, found_objects_count, sizeof(int), cmpfunc);
				int new_number = existing_values[found_objects_count-1] + 1;
				WRITE_LOG("New number:%d\n", new_number);
				char new_number_string[CHANGE_NOTICE_NUMBER_LENGTH+1];
				sprintf(new_number_string, "%08d", new_number);
				*value = (char*) MEM_alloc(sizeof(char) * (CHANGE_NOTICE_NUMBER_LENGTH + 1));
				strcpy(*value, new_number_string);
			}
		}
		else
		{
			TC_write_syslog("%s\n", "results=0");
			*value = (char*) MEM_alloc(sizeof(char) * (CHANGE_NOTICE_NUMBER_LENGTH + 1));
			strcpy(*value, "00000000");
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(found_objects!=NULL) MEM_free(found_objects);

	return ifail;
}

int set_form_value_if_empty(tag_t form, char* attr_name)
{
	int ifail = ITK_ok;
	char
		*new_value = NULL;

	try
	{
		IFERR_THROW( find_new_value(attr_name, &new_value) );
		IFERR_THROW( set_property(form, attr_name, new_value) );
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(new_value!=NULL) MEM_free(new_value);

	return ifail;
}

int set_change_notice_number(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments = NULL,
		root_task,
		target_form,
		form_type,
		temp_type;
	int
		*attachments_types = NULL,
		attachments_count = 0;
	bool
		is_type_of = false;
	char
		*form_type_name = "SL4_ChangeNoticeForm",
		*attribute_name = "sl4_ChangeNoticeNumber";

	try
	{
		WRITE_LOG("%s\n", "Into set change notice number");

		IFERR_THROW( TCTYPE_find_type(form_type_name, NULL, &form_type) );
		IFNULLTAG_THROW( form_type );

		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "Checking attachment type");
				IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
				IFERR_THROW( TCTYPE_is_type_of(temp_type, form_type, &is_type_of) );
				if(is_type_of)
				{
					IFERR_THROW( set_form_value_if_empty(attachments[i], attribute_name) );
				}
			}
		}

	}
	catch (int exfail)
	{
		ifail = exfail;
	}
	if(attachments!=NULL) MEM_free(attachments); attachments = NULL;
	if(attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;

	return ifail;
}
