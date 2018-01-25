#include <grm.h>
#include <epm.h>
#include <epm_toolkit_tc_utils.h>
#include <base_utils/ResultCheck.hxx>
#include "../misc.hxx"
#include "assign_form_property_to_targets.hxx"

int assign_form_property_to_targets(EPM_action_message_t msg)
{
	int erc = ITK_ok;

	try
	{
		tag_t
			*attachments,
			root_task,
			zayav_t,
			zayavRel_t,
			M9_ZayavForm_type_t,
			temp_type_t;
		int
			*attachments_types,
			attachments_count = 0;
		bool
			is_M9_ZayavForm_type = false;

		erc = TCTYPE_find_type("M9_ZayavForm", NULL, &M9_ZayavForm_type_t);

		erc = EPM_ask_root_task(msg.task, &root_task);
		erc = EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types);
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{

			}
		}

		/*int* attachments_types_to_add = (int*) MEM_alloc(sizeof(int)*related_objs_count);
		for(int j = 0; j < related_objs_count; j++){
			attachments_types_to_add[j] = EPM_reference_attachment;
		}
		erc = EPM_add_attachments(root_task, related_objs_count, related_objs, attachments_types_to_add);*/

		MEM_free(attachments);
		MEM_free(attachments_types);
		MEM_free(attachments_types_to_add);
	}
	catch (int exfail)
	{
		return exfail;
	}

	return ITK_ok;
}
