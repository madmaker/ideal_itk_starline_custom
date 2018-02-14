#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include "../misc.hxx"
#include "check_group_and_role.hxx"

int cgar_read_arguments(TC_argument_list_t* arguments, char** role)
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
		*role = "";

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
			    if (strcmp("ROLE", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its except");
			        if (!STR_EMPTY(normal_value)) {
			        	*role = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			        	strcpy(*role, normal_value);
			        	WRITE_LOG("%s\n", *role);
			        } else {
			        	*role = "";
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

int check_group_and_role(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments,
		root_task,
		group_member,
		user_group,
		temp_group,
		user_role,
		argument_role = NULL;
	int
		*attachments_types,
		attachments_count = 0,
		attachments_to_change_count = 0;
	char* argument_role_name;

	try
	{
		WRITE_LOG("%s\n", "Asking root task and attachmenmts");

		IFERR_THROW( cgar_read_arguments(msg.arguments, &argument_role_name) );
		IFERR_THROW( SA_find_role(argument_role_name, &argument_role) );
		MEM_free(argument_role_name);
		if(argument_role == NULL) throw SA_role_name_empty;

		IFERR_THROW( SA_ask_current_groupmember(&group_member) );
		IFERR_THROW( SA_ask_groupmember_group(group_member, &user_group) );
		IFERR_THROW( SA_ask_groupmember_role(group_member, &user_role) );


		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );

		int* attachments_types_to_change = (int*) MEM_alloc(sizeof(int) * attachments_count);
		tag_t* attachments_to_change = (tag_t*) MEM_alloc(sizeof(tag_t) * attachments_count);

		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "---");
				IFERR_THROW( AOM_ask_group(attachments[i], &temp_group) );
				if(user_group != temp_group || argument_role != user_role)
				{
					attachments_to_change[attachments_to_change_count] = attachments[i];
					attachments_types_to_change[attachments_to_change_count] = EPM_reference_attachment;
					attachments_to_change_count++;
				}
			}
		}

		IFERR_THROW( EPM_remove_attachments(root_task, attachments_to_change_count, attachments_to_change) );
		IFERR_THROW( EPM_add_attachments(root_task, attachments_to_change_count, attachments_to_change, attachments_types_to_change) );

		MEM_free(attachments);
		MEM_free(attachments_types);
		MEM_free(attachments_to_change);
		MEM_free(attachments_types_to_change);
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}
