#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/aom.h>
#include <tccore/grm.h>
#include <tccore/aom_prop.h>
#include <tccore/tctype.h>
#include "../misc.hxx"
#include "copy_target_form_props_to_ref_form.hxx"

int copy_target_form_props_to_ref_form(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments,
		root_task,
		target_form = NULLTAG,
		ref_form = NULLTAG,
		form_type,
		temp_type,
		temp_relation,
		relation_type = NULLTAG,
		primary_rel_part = NULL,
		replacement_rel_part = NULL,
		*related_objects;
	int
		*attachments_types,
		attachments_count = 0,
		related_objects_count = 0;
	bool
		is_type_of = false;
	char
		*form_type_name = "SL4_ReplacementForm",
		*relation_type_name = "SL4_ReplacementRel";

	try
	{
		WRITE_LOG("%s\n", "Into copy_target_form_props_to_ref_form");
		IFERR_THROW( TCTYPE_find_type(form_type_name, NULL, &form_type) );
		IFNULLTAG_THROW( form_type );
		IFERR_THROW( TCTYPE_find_type(relation_type_name, NULL, &relation_type) );
		IFNULLTAG_THROW( relation_type );

		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );

		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
				IFERR_THROW( TCTYPE_is_type_of(temp_type, form_type, &is_type_of) );
				if(is_type_of)
				{
					target_form = attachments[i];
				}
			}
			else if (attachments_types[i]==EPM_reference_attachment)
			{
				IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
				IFERR_THROW( TCTYPE_is_type_of(temp_type, form_type, &is_type_of) );
				if(is_type_of)
				{
					ref_form = attachments[i];
				}
			}
		}

		if(target_form == NULLTAG || ref_form == NULLTAG)
		{
			WRITE_LOG("%s\n", ".target form or ref form is null");
		}
		else
		{
			IFERR_THROW( AOM_ask_value_tag(target_form, "sl4_ReplacementPrimary", &primary_rel_part) );
			if(primary_rel_part!=NULLTAG)
			{
				IFERR_THROW( AOM_set_value_tag(ref_form, "sl4_ReplacementPrimary", primary_rel_part) );
			}

			IFERR_THROW( GRM_list_secondary_objects_only(target_form, relation_type, &related_objects_count, &related_objects) );
			for(int i = 0; i < related_objects_count; i++)
			{
				IFERR_THROW( GRM_create_relation(ref_form, related_objects[i], relation_type, NULLTAG, &temp_relation) );
				IFERR_THROW( GRM_save_relation(temp_relation) );
			}
		}

		if(attachments_count > 0)
		{
			MEM_free(attachments);
			MEM_free(attachments_types);
		}
		if(related_objects_count > 0)
		{
			MEM_free(related_objects);
		}
		WRITE_LOG("%s\n", "Into done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}
