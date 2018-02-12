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
	WRITE_LOG("%s\n", "Reading arguments");
	int erc = ITK_ok;
	int
		arguments_count = 0;
	char
		*flag = NULL,
		Flag[256],
		*value= NULL,
		*normal_value= NULL;

	try
	{
		arguments_count = TC_number_of_arguments(arguments);
		if (arguments_count != 1)
		{
			throw EPM_wrong_number_of_arguments;
		}

		for (int numctr=0; numctr<arguments_count && erc==ITK_ok;numctr++)
		{
			erc = ITK_ask_argument_named_value(TC_next_argument(arguments), &flag, &value);
			if (erc == ITK_ok)
			{
				erc = EPM_substitute_keyword(value,&normal_value);
			    if (normal_value != NULL)
			    {
			    	StrCpy(Flag,256,flag);
			        StrUpr(Flag);
			        if (erc == ITK_ok)
			        {
			        	if (strcmp("ROLE", Flag)==0)
			        	{
			        		if (!STR_EMPTY(normal_value)) {
			        			*role = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
				                strcpy(*role, normal_value);
			        		} else {
			        			*role = "";
			        		}
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
			else
			{
				WRITE_LOG("Error: rcode1=%i\n", erc);
			}
		}
	}
	catch (int exfail)
	{
		return exfail;
	}

	return ITK_ok;
}

int check_group_and_role(EPM_action_message_t msg)
{
	int erc = ITK_ok;
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

		erc = cgar_read_arguments(msg.arguments, &argument_role_name);
		erc = SA_find_role(argument_role_name, &argument_role);
		MEM_free(argument_role_name);
		if(argument_role == NULL) throw SA_role_name_empty;

		erc = SA_ask_current_groupmember(&group_member);
		erc = SA_ask_groupmember_group(group_member, &user_group);
		erc = SA_ask_groupmember_role(group_member, &user_role);


		erc = EPM_ask_root_task(msg.task, &root_task);
		erc = EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types);

		int* attachments_types_to_change = (int*) MEM_alloc(sizeof(int) * attachments_count);
		tag_t* attachments_to_change = (tag_t*) MEM_alloc(sizeof(tag_t) * attachments_count);

		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "---");
				erc = AOM_ask_group(attachments[i], &temp_group);
				if(user_group != temp_group || argument_role != user_role)
				{
					attachments_to_change[attachments_to_change_count] = attachments[i];
					attachments_types_to_change[attachments_to_change_count] = EPM_reference_attachment;
					attachments_to_change_count++;
				}
			}
		}

		erc = EPM_remove_attachments(root_task, attachments_to_change_count, attachments_to_change);
		erc = EPM_add_attachments(root_task, attachments_to_change_count, attachments_to_change, attachments_types_to_change);

		MEM_free(attachments);
		MEM_free(attachments_types);
		MEM_free(attachments_to_change);
		MEM_free(attachments_types_to_change);
	}
	catch (int exfail)
	{
		return exfail;
	}

	return ITK_ok;
}
