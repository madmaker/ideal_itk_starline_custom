#include <bom/bom.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <form/form.h>
#include <ics/ics.h>
#include <ics/ics2.h>
#include <ps/ps.h>
#include <sa/sa.h>
#include <tc/tc_util.h>
#include <tccore/grm.h>
#include <tccore/tctype.h>
#include <user_exits/epm_toolkit_utils.h>
#include "../misc.hxx"
#include "post_remove_all_substitutes.hxx"

int set_substitute_base_data(tag_t replacement_form, tag_t revision)
{
	int ifail = ITK_ok;

	try
	{
		IFERR_THROW( AOM_refresh(replacement_form, TRUE) );
		IFERR_THROW( AOM_set_value_tag(replacement_form, "sl4_ReplacementPrimary", revision) );
		IFERR_THROW( AOM_save(replacement_form) );
		IFERR_THROW( AOM_refresh(replacement_form, FALSE) );
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int create_form(tag_t* form_r)
{
	int ifail = ITK_ok;
	char* form_type = "SL4_ReplacementForm";
	tag_t form = NULLTAG;
	tag_t form_type_tag = NULLTAG;
	tag_t form_create_input = NULLTAG;

	try
	{
		WRITE_LOG("%s", "creating form... ");
		IFERR_THROW( TCTYPE_find_type(form_type, NULL, &form_type_tag) );
		IFERR_THROW( TCTYPE_construct_create_input(form_type_tag, &form_create_input) );
		IFERR_THROW( AOM_set_value_string(form_create_input, "object_desc", "description") );
		IFERR_THROW( AOM_set_value_string(form_create_input, "object_name", "name") );

		IFERR_THROW( TCTYPE_create_object(form_create_input, &form) );
		IFERR_THROW( AOM_save(form) );
		*form_r = form;
		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		WRITE_LOG("%s\n", "failed");
		ifail = exfail;
	}

	return ifail;
}

int relate(tag_t primary, tag_t secondary)
{
	int ifail = ITK_ok;
	tag_t relation = NULLTAG;
	tag_t relation_type = NULLTAG;

	try
	{
		WRITE_LOG("%s", "creating relation... ");
		IFERR_THROW( GRM_find_relation_type("SL4_ReplacementRel", &relation_type) );
		IFNULLTAG_THROW( relation_type );
		IFERR_THROW( GRM_create_relation(primary, secondary, relation_type, NULLTAG, &relation) );
		IFERR_THROW( GRM_save_relation(relation) );
		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		WRITE_LOG("%s\n", "failed");
		ifail = exfail;
	}

	return ifail;
}

int get_bom_window_and_top_line_from_bvr(tag_t bvr, logical packed, tag_t* bom_window, tag_t* top_line)
{
	int ifail = ITK_ok;
	tag_t bom_window_r = NULLTAG;
	tag_t top_line_r = NULLTAG;

	try
	{
		IFERR_THROW( BOM_create_window(&bom_window_r) );
		IFERR_THROW( BOM_set_window_top_line_bvr(bom_window_r, bvr, &top_line_r) );
		*bom_window = bom_window_r;
		*top_line = top_line_r;
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int list_bom_substitutes(tag_t bom_line, tag_t top_replacement_form)
{
	int ifail = ITK_ok;
	int sub_number = 0;
	tag_t* substitutes;
	tag_t replacement_form;
	tag_t revision;

	try
	{
		IFERR_THROW( BOM_line_list_substitutes(bom_line, &sub_number, &substitutes) );
		if(sub_number>0)
		{
			IFERR_THROW( create_form(&replacement_form) );
			IFERR_THROW( relate(top_replacement_form, replacement_form) );
			IFERR_THROW( AOM_ask_value_tag(bom_line, "bl_revision", &revision) );
			IFNULLTAG_THROW( revision );
			IFERR_THROW( set_substitute_base_data(replacement_form, revision) );
		}
		for(int i = 0; i < sub_number; i++)
		{
			IFERR_THROW( AOM_ask_value_tag(substitutes[i], "bl_revision", &revision) );
			IFNULLTAG_THROW( revision );
			IFERR_THROW( relate(replacement_form, revision) );
			IFERR_THROW( BOM_line_cut(substitutes[i]) );
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(sub_number > 0) MEM_free(substitutes);

	return ifail;
}

int check_related_object_types(tag_t primary_object, tag_t secondary_object, logical* result)
{
	int ifail = ITK_ok;
	tag_t bvr_type = NULLTAG;
	tag_t primary_type = NULLTAG;
	tag_t secondary_type = NULLTAG;

	try
	{
		IFERR_THROW( TCTYPE_find_type("BOMView Revision", NULL, &bvr_type) );

		IFERR_THROW( TCTYPE_ask_object_type(primary_object, &primary_type) );
		IFERR_THROW( TCTYPE_ask_object_type(secondary_object, &secondary_type) );

		if(primary_type == bvr_type)
		{
			*result = true;
		}
		else
		{
			*result = false;
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int post_remove_all_substitutes(METHOD_message_t *msg, va_list args)
{
	int ifail = ITK_ok;
	int occ_number = 0;
	tag_t primary_object = NULLTAG;
	tag_t secondary_object = NULLTAG;
	logical primary_is_bvr = false;
	tag_t* occurences;

	try
	{
		WRITE_LOG("%s\n", "-POST remove all substitutes called");

		IFERR_THROW( GRM_ask_primary(msg->object_tag, &primary_object) );
		IFERR_THROW( GRM_ask_secondary(msg->object_tag, &secondary_object) );
		IFERR_THROW( check_related_object_types(primary_object, secondary_object, &primary_is_bvr) );

		if(primary_is_bvr)
		{
			tag_t top_replacement_form;
			IFERR_THROW( create_form(&top_replacement_form) );
			IFERR_THROW( relate(primary_object, top_replacement_form) );

			tag_t bom_window;
			tag_t top_line;
			int child_count = 0;
			tag_t* children;
			IFERR_THROW( get_bom_window_and_top_line_from_bvr(primary_object, false, &bom_window, &top_line) );
			IFERR_THROW( BOM_line_ask_all_child_lines(top_line, &child_count, &children) );
			for(int i = 0; i < child_count; i++)
			{
				WRITE_LOG("%s\n", "-line");
				logical is_substitute;
				IFERR_THROW( BOM_line_ask_is_substitute(children[i], &is_substitute) );
				if(!is_substitute) list_bom_substitutes(children[i], top_replacement_form);
				WRITE_LOG("%s\n", "-line done");
			}
			IFERR_THROW( BOM_save_window(bom_window) );
			IFERR_THROW( BOM_close_window(bom_window) );
		}

		IFERR_THROW( AOM_save(primary_object) );

		WRITE_LOG("%s\n", "-POST done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(occ_number > 0) MEM_free(occurences);

	return ifail;
}
