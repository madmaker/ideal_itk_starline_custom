#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include "../misc.hxx"
#include "check_user_and_group_rh.hxx"

EPM_decision_t check_user_and_group_rh(EPM_rule_message_t msg)
{
	int ifail = ITK_ok;
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
		attachments_count = 0;

	EPM_decision_t decision = EPM_go;

	try
	{
		IFERR_THROW( SA_ask_current_groupmember(&group_member) );
		IFERR_THROW( SA_ask_groupmember_group(group_member, &user_group) );

		WRITE_LOG("%s\n", "Asking root task and attachmenmts");
		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		IFERR_THROW( AOM_ask_value_tag(root_task, "owning_user", &user) );


		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "---");
				IFERR_THROW( AOM_ask_group(attachments[i], &temp_group) );
				IFERR_THROW( AOM_ask_value_tag(attachments[i], "owning_user", &temp_user) );
				if(user_group != temp_group || user != temp_user)
				{
					decision = EPM_nogo;
				}
			}
		}

		MEM_free(attachments);
		MEM_free(attachments_types);
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return decision;
}
