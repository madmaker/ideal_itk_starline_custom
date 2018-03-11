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

EPM_decision_t no_other_active_workflows(EPM_rule_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		current_task = NULLTAG,
		root_task = NULLTAG,
		*attachments;
	int
		*attachments_types,
		attachments_count = 0;
	logical in_another_wf = false;

	EPM_decision_t decision = EPM_go;

	try
	{
		WRITE_LOG("%s\n", "Into no other workflows");
		current_task = msg.task;

		IFERR_THROW( EPM_ask_root_task(current_task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "Found target");
				IFERR_THROW( is_in_another_active_workflow(attachments[i], root_task, &in_another_wf) );
				if(in_another_wf)
				{
					WRITE_LOG("%s\n", "IN ANOTHER WF");
					IFERR_THROW( EMH_store_error_s2(EMH_severity_error, EPM_worfklow_launched_in_background, "EMH_severity_error", "EPM_nogo") );
					decision = EPM_nogo;
					break;
				}
			}
		}
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

	return decision;
}
