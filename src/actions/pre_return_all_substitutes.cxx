#include <epm/epm.h>
#include <sa/sa.h>
#include <ics.h>
#include <ics2.h>
#include <ps.h>
#include <grm.h>
#include <bom.h>
#include <epm/epm_task_template_itk.h>
#include <user_exits/epm_toolkit_utils.h>
#include <tc_util.h>
#include "pre_return_all_substitutes.hxx"
#include "post_remove_all_substitutes.hxx"
#include "../misc.hxx"
#include <base_utils/ResultCheck.hxx>

int get_substitute_base_data(tag_t replacement_form, tag_t* rev, tag_t* item)
{
	int erc = ITK_ok;
	tag_t temp_relation_type;
	tag_t temp_relation;
	int related_count = 0;
	tag_t* related;

	try
	{
		erc = AOM_ask_value_tag(replacement_form, "sl4_ReplacementPrimary", rev);
		if(rev == NULLTAG)
		{
			WRITE_LOG("%s\n", "BASE IS MISSING");
		}
		else
		{
			erc = ITEM_ask_item_of_rev(*rev, item);
		}
		/*erc = GRM_find_relation_type("IMAN_Motion", &temp_relation_type);
		erc = GRM_list_secondary_objects_only(replacement_form, temp_relation_type, &related_count, &related);
		if(related_count > 0)
		{
			*rev = related[0];
			erc = ITEM_ask_item_of_rev(related[0], item);

			char* name;
			erc = AOM_ask_value_string(related[0], "object_name", &name);
			WRITE_LOG("base name:%s\n", name);
			MEM_free(name);
		}
		else
		{
			WRITE_LOG("%s\n", "BASE IS MISSSING");
		}
		for(int i = 0; i < related_count; i++)
		{
			erc = GRM_find_relation(replacement_form, related[0], temp_relation_type, &temp_relation);
			erc = GRM_delete_relation(temp_relation);
		}*/
	}
	catch (int exfail)
	{
		return exfail;
	}

	if(related_count > 0) MEM_free(related);

	return erc;
}

int restore_all_substitutes(tag_t bvr, tag_t top_replacement_form, int child_bom_lines_count, tag_t* child_bom_lines)
{
	int erc = ITK_ok;
	tag_t relation_type;
	int replacement_forms_count = 0;
	tag_t* replacement_forms;

	try
	{
		//erc = GRM_find_relation_type("IMAN_Rendering", &relation_type);
		erc = GRM_find_relation_type("SL4_ReplacementRel", &relation_type);
		erc = GRM_list_secondary_objects_only(top_replacement_form, relation_type, &replacement_forms_count, &replacement_forms);

		// For every item revision related to replacemnet form, we restore substitute, delete relation and delete form
		for(int i = 0; i < replacement_forms_count; i++)
		{
			WRITE_LOG("%s\n", "-- Working with replacement form");
			tag_t substitute_base_rev;
			tag_t substitute_base_item;
			WRITE_LOG("%s\n", "-- looking for base data");
			erc = get_substitute_base_data(replacement_forms[i], &substitute_base_rev, &substitute_base_item);

			int substitutes_count = 0;
			tag_t* substitutes;
			erc = GRM_list_secondary_objects_only(replacement_forms[i], relation_type, &substitutes_count, &substitutes);

			for(int j = 0; j < substitutes_count; j++)
			{
				WRITE_LOG("%s\n", "-- working with substitute");
				tag_t substitute_item;
				erc = ITEM_ask_item_of_rev(substitutes[j], &substitute_item);
				for(int k = 0; k < child_bom_lines_count; k++)
				{
					WRITE_LOG("%s\n", "-- through bom");
					tag_t temp_substitute;
					tag_t child_item;
					tag_t child_revision;
					erc = AOM_ask_value_tag(child_bom_lines[k], "bl_item", &child_item);
					erc = AOM_ask_value_tag(child_bom_lines[k], "bl_revision", &child_revision);
					if(substitute_base_item == child_item)
					{
						WRITE_LOG("%s\n", "FOUND MISSING SUBSTITUTE");
						erc = BOM_line_add_substitute (child_bom_lines[k], substitute_item, substitutes[j], NULLTAG, &temp_substitute);
					}
				}
				tag_t relation;
				erc = GRM_find_relation(replacement_forms[i], substitutes[j], relation_type, &relation);
				erc = GRM_delete_relation(relation);
			}

			tag_t relation;
			erc = GRM_find_relation(top_replacement_form, replacement_forms[i], relation_type, &relation);
			erc = GRM_delete_relation(relation);
			erc = AOM_delete(replacement_forms[i]);

			if(substitutes_count > 0) MEM_free(substitutes);
		}

	}
	catch (int exfail)
	{
		return exfail;
	}

	if(replacement_forms_count > 0)MEM_free(replacement_forms);

	return erc;
}

int pre_return_all_substitutes(METHOD_message_t *msg, va_list args)
{
	int erc = ITK_ok;
	tag_t primary_object;
	tag_t secondary_object;
	tag_t relation_type;
	int top_replacement_forms_count = 0;
	tag_t* top_replacement_forms;

	tag_t bvr_type;
	tag_t primary_type;
	tag_t secondary_type;
	char primary_type_name[TCTYPE_name_size_c+1];
	char secondary_type_name[TCTYPE_name_size_c+1];

	try
	{
		WRITE_LOG("%s\n", "-PRE return all substitutes called");

		erc = GRM_ask_primary(msg->object_tag, &primary_object);
		erc = GRM_ask_secondary(msg->object_tag, &secondary_object);

		erc = TCTYPE_find_type("BOMView Revision", NULL, &bvr_type);

		erc = TCTYPE_ask_object_type(primary_object, &primary_type);
		erc = TCTYPE_ask_object_type(secondary_object, &secondary_type);
		erc = TCTYPE_ask_name(primary_type, primary_type_name);
		erc = TCTYPE_ask_name(secondary_type, secondary_type_name);

		WRITE_LOG("p:%s\n", primary_type_name);
		WRITE_LOG("s:%s\n", secondary_type_name);

		if(secondary_type == bvr_type) WRITE_LOG("%s\n", "secondary is bvr");

		if(primary_type == bvr_type)
		{
			WRITE_LOG("%s\n", "primary is bvr");

			tag_t bom_window;
			tag_t top_line;
			int child_bom_lines_count = 0;
			tag_t* child_bom_lines;
			erc = get_bom_window_and_top_line_from_bvr(primary_object, false, &bom_window, &top_line);
			erc = BOM_line_ask_all_child_lines(top_line, &child_bom_lines_count, &child_bom_lines);

			//erc = GRM_find_relation_type("IMAN_Rendering", &relation_type);
			erc = GRM_find_relation_type("SL4_ReplacementRel", &relation_type);
			erc = GRM_list_secondary_objects_only(primary_object, relation_type, &top_replacement_forms_count, &top_replacement_forms);
			if(top_replacement_forms_count > 0)
			{
				for(int i = 0; i < top_replacement_forms_count; i++)
				{
					WRITE_LOG("%s\n", "-line");
					erc = restore_all_substitutes(primary_object, top_replacement_forms[i], child_bom_lines_count, child_bom_lines);
					tag_t relation;
					erc = GRM_find_relation(primary_object, top_replacement_forms[i], relation_type, &relation);
					erc = GRM_delete_relation(relation);
					erc = AOM_delete(top_replacement_forms[i]);
					WRITE_LOG("%s\n", "-form done");
				}
			}
			erc = BOM_save_window(bom_window);
			erc = BOM_close_window(bom_window);
			if(child_bom_lines_count > 0) MEM_free(child_bom_lines);
		}

		WRITE_LOG("%s\n", "-PRE done");
	}
	catch (int exfail)
	{
		return exfail;
	}

	if(top_replacement_forms_count > 0) MEM_free(top_replacement_forms);

	return erc;
}
