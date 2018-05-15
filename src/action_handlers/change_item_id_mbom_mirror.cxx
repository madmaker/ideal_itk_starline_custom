#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include "../misc.hxx"
#include "change_item_id_mbom_mirror.hxx"

int change_item_id_mbom_mirror(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments = NULL,
		root_task = NULLTAG,
		item_type = NULLTAG,
		temp_type = NULLTAG;
	int
		*attachments_types = NULL,
		attachments_count = 0;
	char* item_id = NULL;
	logical is_type_of;

	try
	{
		WRITE_LOG("%s\n", ".change_item_id_mbom_mirror");

		WRITE_LOG("%s\n", "Asking root task and attachmenmts");
		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFNULLTAG_THROW( root_task );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		IFERR_THROW( TCTYPE_find_type("Item", NULL, &item_type) );

		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "---");
				IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
				IFERR_THROW( TCTYPE_is_type_of(temp_type, item_type, &is_type_of) );
				if(is_type_of)
				{
					IFERR_THROW( AOM_ask_value_string(attachments[i], "item_id", &item_id) );

					MEM_free( item_id );
				}
			}
		}

		WRITE_LOG("%s\n", ".change_item_id_mbom_mirror done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(attachments!=NULL) MEM_free(attachments); attachments = NULL;
	if(attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;
	if(item_id!=NULL) MEM_free(item_id); item_id = NULL;

	return ifail;
}
