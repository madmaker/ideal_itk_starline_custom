#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/releasestatus.h>
#include "../misc.hxx"
#include "attach_previous_revisions.hxx"

int rename_item_as_revision(tag_t revision, tag_t item, tag_t root_task)
{
	int ifail = ITK_ok;
	char* revision_name = NULL;
	int number_to_add = 1;
	int attachments_types_to_add[] = { EPM_target_attachment };
	tag_t attachments_to_add[] = { item };

	try
	{
		WRITE_LOG("%s\n", "Renaming");
		IFERR_THROW( EPM_add_attachments(root_task, number_to_add, attachments_to_add, attachments_types_to_add) );
		IFERR_THROW( ITEM_ask_rev_name2(revision, &revision_name) );
		IFERR_THROW( AOM_refresh(item, TRUE) );
		IFERR_THROW( ITEM_set_name(item, revision_name) );
		IFERR_THROW( AOM_save(item) );
		IFERR_THROW( AOM_refresh(item, FALSE) );
		IFERR_THROW( EPM_remove_attachments(root_task, number_to_add, attachments_to_add) );
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(revision_name!=NULL) MEM_free(revision_name); revision_name = NULL;

	return ifail;
}

int move_item_to_refs(tag_t item, tag_t root_task)
{
	int ifail = ITK_ok;
	int number_to_add = 1;
	int attachments_types_to_add[] = { EPM_reference_attachment };
	tag_t attachments_to_add[] = { item };

	try
	{
		WRITE_LOG("%s\n", "Moving to refs");
		IFERR_THROW( EPM_add_attachments(root_task, number_to_add, attachments_to_add, attachments_types_to_add) );
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int rename_item_or_move_to_refs_if_locked(tag_t revision, tag_t root_task)
{
	int ifail = ITK_ok;
	tag_t item = NULLTAG;
	char* checked_out = NULL;

	try
	{
		WRITE_LOG("%s\n", "Asking item");
		IFERR_THROW( ITEM_ask_item_of_rev(revision, &item) );
		IFNULLTAG_THROW(item);
		WRITE_LOG("%s\n", "Asking if is checked out");
		IFERR_THROW( AOM_ask_value_string(item, "checked_out", &checked_out) );
		if(strcmp(checked_out, "Y")==0)
		{
			IFERR_THROW( move_item_to_refs(item, root_task) );
		}
		else
		{
			IFERR_THROW( rename_item_as_revision(revision, item, root_task) );
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(checked_out!=NULL) MEM_free(checked_out); checked_out = NULL;

	return ifail;
}

int rename_items_for_revisions_from_targets(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments = NULL,
		root_task = NULLTAG,
		item_revision_type = NULLTAG,
		temp_type = NULLTAG;
	int
		*attachments_types = NULL,
		attachments_count = 0;
	logical
		is_type_of = false;

	try
	{
		IFERR_THROW( TCTYPE_find_type("ItemRevision", NULL, &item_revision_type) );

		WRITE_LOG("%s\n", "Asking root task and attachmenmts");
		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "Working with target");
				IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
				IFERR_THROW( TCTYPE_is_type_of(temp_type, item_revision_type, &is_type_of) );
				if(is_type_of)
				{
					IFERR_THROW( rename_item_or_move_to_refs_if_locked(attachments[i], root_task) );
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
