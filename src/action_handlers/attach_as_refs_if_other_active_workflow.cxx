#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <epm/epm.h>
#include "../misc.hxx"
#include "no_reference_attachments.hxx"

int is_in_another_active_workflow(tag_t object, tag_t root_task, logical* result)
{
	int ifail = ITK_ok;
	int process_stages_count = 0;
	tag_t* process_stages = NULL;
	tag_t temp_root_task = NULLTAG;

	try
	{
		*result = false;
		IFERR_THROW( AOM_ask_value_tags(object, "process_stage_list", &process_stages_count, &process_stages) );
		for(int i = 0; i < process_stages_count; i++)
		{
			WRITE_LOG("%s", "Asking root task...");
			IFERR_THROW( EPM_ask_root_task(process_stages[i], &temp_root_task) );
			WRITE_LOG("%s\n", " done");
			if(root_task != temp_root_task) *result = true;
		}
	}
	catch (int exfail)
	{
		WRITE_LOG("%s\n", " failed");
		ifail = exfail;
	}

	if(process_stages!=NULL) MEM_free(process_stages); process_stages = NULL;

	return ifail;
}

int attach_as_refs_if_other_active_workflow(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		current_task = NULLTAG,
		root_task = NULLTAG,
		*attachments = NULL,
		*attachments_to_change = NULL;
	int
		*attachments_types,
		attachments_count = 0,
		attachments_to_change_count = 0,
		*attachments_types_to_change;
	logical in_another_wf = false;

	try
	{
		WRITE_LOG("%s\n", "Into no other workflows");
		current_task = msg.task;

		IFERR_THROW( EPM_ask_root_task(current_task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );

		attachments_types_to_change = (int*) MEM_alloc(sizeof(int) * attachments_count);
		attachments_to_change = (tag_t*) MEM_alloc(sizeof(tag_t) * attachments_count);

		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "Found target");
				IFERR_THROW( is_in_another_active_workflow(attachments[i], root_task, &in_another_wf) );
				if(in_another_wf)
				{
					WRITE_LOG("%s\n", "IN ANOTHER WF");
					attachments_to_change[attachments_to_change_count] = attachments[i];
					attachments_types_to_change[attachments_to_change_count] = EPM_reference_attachment;
					attachments_to_change_count++;
					break;
				}
			}
		}

		IFERR_THROW( EPM_remove_attachments(root_task, attachments_to_change_count, attachments_to_change) );
		IFERR_THROW( EPM_add_attachments(root_task, attachments_to_change_count, attachments_to_change, attachments_types_to_change) );
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	// Memory handling
	if(attachments_count>0)
	{
		MEM_free(attachments);
		MEM_free(attachments_types);
	}

	if(attachments_to_change!=NULL) MEM_free(attachments_to_change);
	if(attachments_types_to_change!=NULL) MEM_free(attachments_types_to_change);

	return ifail;
}
