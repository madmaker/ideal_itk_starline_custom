#include <bom/bom.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <ics/ics.h>
#include <ics/ics2.h>
#include <ps/ps.h>
#include <sa/sa.h>
#include <tc/tc_util.h>
#include <tccore/grm.h>
#include <user_exits/epm_toolkit_utils.h>
#include "../misc.hxx"
#include "pre_return_all_substitutes.hxx"
#include "post_remove_all_substitutes.hxx"

int get_substitute_base_data(tag_t replacement_form, tag_t* rev, tag_t* item)
{
	int ifail = ITK_ok;
	tag_t temp_relation_type;
	tag_t temp_relation;
	int related_count = 0;
	tag_t* related;

	try
	{
		IFERR_THROW( AOM_ask_value_tag(replacement_form, "sl4_ReplacementPrimary", rev) );
		IFNULLTAG_THROW( *rev );
		IFERR_THROW( ITEM_ask_item_of_rev(*rev, item) );
		IFNULLTAG_THROW( *item );
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(related_count > 0) MEM_free(related);

	return ifail;
}

int restore_all_substitutes(tag_t bvr, tag_t top_replacement_form, int child_bom_lines_count, tag_t* child_bom_lines)
{
	int ifail = ITK_ok;
	tag_t relation_type;
	int replacement_forms_count = 0;
	tag_t* replacement_forms;

	try
	{
		IFERR_THROW( GRM_find_relation_type("SL4_ReplacementRel", &relation_type) );
		IFERR_THROW( GRM_list_secondary_objects_only(top_replacement_form, relation_type, &replacement_forms_count, &replacement_forms) );

		// For every item revision related to replacement form, we restore substitute, delete relation and delete form
		for(int i = 0; i < replacement_forms_count; i++)
		{
			WRITE_LOG("%s\n", "-- Working with replacement form");
			tag_t substitute_base_rev;
			tag_t substitute_base_item;
			WRITE_LOG("%s\n", "-- looking for base data");
			IFERR_THROW( get_substitute_base_data(replacement_forms[i], &substitute_base_rev, &substitute_base_item) );

			int substitutes_count = 0;
			tag_t* substitutes;
			IFERR_THROW( GRM_list_secondary_objects_only(replacement_forms[i], relation_type, &substitutes_count, &substitutes) );

			for(int j = 0; j < substitutes_count; j++)
			{
				WRITE_LOG("%s\n", "-- working with substitute");
				tag_t substitute_item;
				IFERR_THROW( ITEM_ask_item_of_rev(substitutes[j], &substitute_item) );
				for(int k = 0; k < child_bom_lines_count; k++)
				{
					WRITE_LOG("%s\n", "-- through bom");
					tag_t temp_substitute;
					tag_t child_item;
					tag_t child_revision;
					IFERR_THROW( AOM_ask_value_tag(child_bom_lines[k], "bl_item", &child_item) );
					IFERR_THROW( AOM_ask_value_tag(child_bom_lines[k], "bl_revision", &child_revision) );
					if(substitute_base_item == child_item)
					{
						WRITE_LOG("%s\n", "FOUND MISSING SUBSTITUTE");
						IFERR_THROW( BOM_line_add_substitute (child_bom_lines[k], substitute_item, substitutes[j], NULLTAG, &temp_substitute) );
					}
				}
				tag_t relation;
				IFERR_THROW( GRM_find_relation(replacement_forms[i], substitutes[j], relation_type, &relation) );
				IFERR_THROW( GRM_delete_relation(relation) );
			}

			tag_t relation;
			IFERR_THROW( GRM_find_relation(top_replacement_form, replacement_forms[i], relation_type, &relation) );
			IFERR_THROW( GRM_delete_relation(relation) );
			IFERR_THROW( AOM_delete(replacement_forms[i]) );

			if(substitutes_count > 0) MEM_free(substitutes);
		}

	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(replacement_forms_count > 0) MEM_free(replacement_forms);

	return ifail;
}

int pre_return_all_substitutes(METHOD_message_t *msg, va_list args)
{
	int ifail = ITK_ok;
	tag_t primary_object;
	tag_t secondary_object;
	logical primary_is_bvr = false;
	tag_t relation_type;
	int top_replacement_forms_count = 0;
	tag_t* top_replacement_forms;
	int child_bom_lines_count = 0;
	tag_t* child_bom_lines = NULL;

	try
	{
		WRITE_LOG("%s\n", "-PRE return all substitutes called");

		IFERR_THROW( GRM_ask_primary(msg->object_tag, &primary_object) );
		IFERR_THROW( GRM_ask_secondary(msg->object_tag, &secondary_object) );
		IFERR_THROW( check_related_object_types(primary_object, secondary_object, &primary_is_bvr) );
		IFERR_THROW( GRM_find_relation_type("SL4_ReplacementRel", &relation_type) );

		if(primary_is_bvr)
		{
			tag_t bom_window;
			tag_t top_line;

			IFERR_THROW( get_bom_window_and_top_line_from_bvr(primary_object, false, &bom_window, &top_line) );
			IFERR_THROW( BOM_line_ask_all_child_lines(top_line, &child_bom_lines_count, &child_bom_lines) );

			IFERR_THROW( GRM_list_secondary_objects_only(primary_object, relation_type, &top_replacement_forms_count, &top_replacement_forms) );
			if(top_replacement_forms_count > 0)
			{
				for(int i = 0; i < top_replacement_forms_count; i++)
				{
					WRITE_LOG("%s\n", "-line");
					IFERR_THROW( restore_all_substitutes(primary_object, top_replacement_forms[i], child_bom_lines_count, child_bom_lines) );
					tag_t relation;
					IFERR_THROW( GRM_find_relation(primary_object, top_replacement_forms[i], relation_type, &relation) );
					IFERR_THROW( GRM_delete_relation(relation) );
					IFERR_THROW( AOM_delete(top_replacement_forms[i]) );
					WRITE_LOG("%s\n", "-form done");
				}
			}
			IFERR_THROW( BOM_save_window(bom_window) );
			IFERR_THROW( BOM_close_window(bom_window) );
		}

		WRITE_LOG("%s\n", "-PRE done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(child_bom_lines != NULL) MEM_free(child_bom_lines); child_bom_lines = NULL;
	if(top_replacement_forms_count > 0) MEM_free(top_replacement_forms);

	return ifail;
}
