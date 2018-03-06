#include <epm/epm.h>
#include "../misc.hxx"
#include "no_reference_attachments.hxx"

EPM_decision_t no_reference_attachments(EPM_rule_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		current_task = NULLTAG,
		root_task = NULLTAG,
		*attachments;
	int
		*attachments_types,
		attachments_count = 0;

	int erc = ITK_ok;
	EPM_decision_t decision = EPM_go;

	try
	{
		WRITE_LOG("%s\n", "Into no reference attachments");
		current_task = msg.task;

		erc = EPM_ask_root_task(current_task, &root_task);
		erc = EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types);
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_reference_attachment)
			{
				WRITE_LOG("%s\n", "Found reference");
				decision = EPM_nogo;
				break;
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
