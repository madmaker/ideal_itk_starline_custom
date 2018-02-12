#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include "../misc.hxx"
#include "check_user_and_group.hxx"

int check_user_and_group(EPM_action_message_t msg)
{
	int erc = ITK_ok;
	tag_t
		*attachments,
		root_task,
		group_member,
		user_group,
		temp_group,
		user,
		temp_user;
	int
		*attachments_types,
		attachments_count = 0,
		attachments_to_change_count = 0;

	try
	{
		erc = SA_ask_current_groupmember(&group_member);
		erc = SA_ask_groupmember_group(group_member, &user_group);

		WRITE_LOG("%s\n", "Asking root task and attachmenmts");
		erc = EPM_ask_root_task(msg.task, &root_task);
		erc = EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types);
		erc = AOM_ask_value_tag(root_task, "owning_user", &user);

		int* attachments_types_to_change = (int*) MEM_alloc(sizeof(int) * attachments_count);
		tag_t* attachments_to_change = (tag_t*) MEM_alloc(sizeof(tag_t) * attachments_count);

		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "---");
				erc = AOM_ask_group(attachments[i], &temp_group);
				erc = AOM_ask_value_tag(attachments[i], "owning_user", &temp_user);
				if(user_group != temp_group || user != temp_user)
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
