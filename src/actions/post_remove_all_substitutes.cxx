#include <epm/epm.h>
#include <sa/sa.h>
#include <ics.h>
#include <ics2.h>
#include <ps.h>
#include <grm.h>
#include <bom.h>
#include <form.h>
#include <tctype.h>
#include <epm/epm_task_template_itk.h>
#include <user_exits/epm_toolkit_utils.h>
#include <tc_util.h>
#include "post_remove_all_substitutes.hxx"
#include "../misc.hxx"
#include <base_utils/ResultCheck.hxx>

int set_substitute_base_data(tag_t replacement_form, tag_t revision)
{
	int erc = ITK_ok;
	tag_t temp_relation_type;
	tag_t temp_relation;

	try
	{

		erc = GRM_find_relation_type("IMAN_Motion", &temp_relation_type);
		erc = GRM_create_relation(replacement_form, revision, temp_relation_type, NULLTAG, &temp_relation);
		erc = GRM_save_relation(temp_relation);
	}
	catch (int exfail)
	{
		return exfail;
	}

	return erc;
}

int create_form(tag_t* form_r)
{
	int erc = ITK_ok;
	char* form_type = "Pm8_GeneralNoteForm";
	tag_t form;
	tag_t form_create_input;

	try
	{
		WRITE_LOG("%s", "creating form... ");
		tag_t form_type_tag = NULLTAG;
		erc = TCTYPE_find_type(form_type, NULL, &form_type_tag);

		tag_t form_create_input_tag = NULLTAG;
		erc = TCTYPE_construct_create_input(form_type_tag, &form_create_input_tag);
		erc = AOM_set_value_string(form_create_input_tag, "object_desc", "description");
		erc = AOM_set_value_string(form_create_input_tag, "object_name", "name");

		erc = TCTYPE_create_object(form_create_input, &form);
		erc = AOM_save(form);
		*form_r = form;
		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		return exfail;
	}

	return erc;
}

int relate(tag_t primary, tag_t secondary)
{
	int erc = ITK_ok;
	tag_t relation;
	tag_t relation_type;

	try
	{
		WRITE_LOG("%s", "creating relation... ");
		erc = GRM_find_relation_type("IMAN_Rendering", &relation_type);
		erc = GRM_create_relation(primary, secondary, relation_type, NULLTAG, &relation);
		erc = GRM_save_relation(relation);
		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		return exfail;
	}

	return erc;
}

int get_bom_window_and_top_line_from_bvr(tag_t bvr, tag_t* bom_window, tag_t* top_line)
{
	int erc = ITK_ok;
	tag_t bom_window_r;
	tag_t top_line_r;

	try
	{
		erc = BOM_create_window(&bom_window_r);
		erc = BOM_set_window_top_line_bvr (bom_window_r, bvr, &top_line_r);
		*bom_window = bom_window_r;
		*top_line = top_line_r;
	}
	catch (int exfail)
	{
		return exfail;
	}

	return erc;
}

int list_bom_substitutes(tag_t bom_line, tag_t top_replacement_form)
{
	int erc = ITK_ok;
	int sub_number = 0;
	tag_t* substitutes;
	tag_t replacement_form;
	tag_t revision;

	try
	{
		char* bl_line_name;
		erc = AOM_ask_value_string(bom_line, "bl_line_name", &bl_line_name);
		WRITE_LOG("bl_line_name:%s\n", bl_line_name);
		MEM_free(bl_line_name);

		erc = BOM_line_list_substitutes(bom_line, &sub_number, &substitutes);
		if(sub_number>0)
		{
			erc = create_form(&replacement_form);
			erc = relate(top_replacement_form, replacement_form);

			erc = AOM_ask_value_tag(bom_line, "bl_revision", &revision);
			erc = set_substitute_base_data(replacement_form, revision);
		}
		for(int i = 0; i < sub_number; i++)
		{
			char* bl_sub_line_name;
			erc = AOM_ask_value_string(substitutes[i], "bl_line_name", &bl_sub_line_name);
			WRITE_LOG("bl_sub_line_name:%s\n", bl_sub_line_name);
			MEM_free(bl_sub_line_name);

			erc = AOM_ask_value_tag(substitutes[i], "bl_revision", &revision);
			if(revision!=NULLTAG)
			{
				char* object_name;
				erc = AOM_ask_value_string(revision, "object_name", &object_name);
				WRITE_LOG("RELATING FORM TO:%s\n", object_name);
				MEM_free(object_name);

				erc = relate(replacement_form, revision);
				erc = BOM_line_cut(substitutes[i]);
			}
			else
			{
				WRITE_LOG("%s\n", "revision is null =(");
			}
		}
	}
	catch (int exfail)
	{
		return exfail;
	}

	if(sub_number > 0) MEM_free(substitutes);

	return erc;
}

int post_remove_all_substitutes(METHOD_message_t *msg, va_list args)
{
	int erc = ITK_ok;
	int occ_number = 0;
	tag_t primary_object;
	tag_t secondary_object;
	tag_t* occurences;

	tag_t bvr_type;
	tag_t primary_type;
	tag_t secondary_type;
	char primary_type_name[TCTYPE_name_size_c+1];
	char secondary_type_name[TCTYPE_name_size_c+1];

	try
	{
		WRITE_LOG("%s\n", "-POST remove all substitutes called");

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
			WRITE_LOG("%s\n", "occurences for bvr");

			tag_t top_replacement_form;
			erc = create_form(&top_replacement_form);
			erc = relate(primary_object, top_replacement_form);

			tag_t bom_window;
			tag_t top_line;
			int child_count = 0;
			tag_t* children;
			erc = get_bom_window_and_top_line_from_bvr(primary_object, &bom_window, &top_line);
			erc = BOM_line_ask_all_child_lines(top_line, &child_count, &children);
			for(int i = 0; i < child_count; i++)
			{
				WRITE_LOG("%s\n", "-line");
				list_bom_substitutes(children[i], top_replacement_form);
				WRITE_LOG("%s\n", "-line done");
			}
			erc = BOM_save_window(bom_window);
			erc = BOM_close_window(bom_window);
		}

		erc = AOM_save(primary_object);

		WRITE_LOG("%s\n", "-POST done");
	}
	catch (int exfail)
	{
		return exfail;
	}

	if(occ_number > 0) MEM_free(occurences);

	return erc;
}
