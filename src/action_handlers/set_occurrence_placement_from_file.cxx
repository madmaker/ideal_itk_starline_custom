#include <ae/ae.h>
#include <bom/bom.h>
#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/aom.h>
#include <tccore/grm.h>
#include <tccore/aom_prop.h>
#include <tccore/tctype.h>
#include "../misc.hxx"
#include "set_occurrence_placement_from_file.hxx"
//#include "assign_form_property_to_targets.hxx"
#include "../actions/post_remove_all_substitutes.hxx"

int get_placement_for_ref(char* ref, logical* placement) // true = Top, false = Bottom, null = none
{
	int ifail = ITK_ok;

	try
	{

	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int find_and_set_placement(tag_t bom_line)
{
	int ifail = ITK_ok;
	char* occ_designator = NULL;

	try
	{
		IFERR_THROW( AOM_ask_value_string(bom_line, "bl_occ_ref_designator", &occ_designator) );
		WRITE_LOG("od:%s\n", occ_designator);
		IFERR_THROW( AOM_set_value_string(bom_line, "", "") );
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(occ_designator!=NULL) MEM_free(occ_designator); occ_designator = NULL;

	return ifail;
}

int cycle_through_bls_and_set_placement(tag_t bvr)
{
	int ifail = ITK_ok;
	tag_t bom_window = NULLTAG;
	tag_t top_line = NULLTAG;
	int bom_lines_count = 0;
	tag_t* bom_lines = NULL;

	try
	{
		WRITE_LOG("%s\n", ".cycle_through_bls_and_set_placement");
		IFERR_THROW( get_bom_window_and_top_line_from_bvr(bvr, false, &bom_window, &top_line) );
		IFNULLTAG_THROW( bom_window );
		IFNULLTAG_THROW( top_line );
		IFERR_THROW( find_and_read_placements_dataset(top_line) );
		IFERR_THROW( BOM_line_ask_all_child_lines(top_line, &bom_lines_count, &bom_lines) );

		for(int i = 0; i < bom_lines_count; i++)
		{
			IFERR_THROW( find_and_set_placement(bom_lines[i]) );
		}
		WRITE_LOG("%s\n", ".cycle_through_bls_and_set_placement done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(bom_lines!=NULL) MEM_free(bom_lines); bom_lines = NULL;

	return ifail;
}

int find_and_read_placements_dataset(tag_t top_bl)
{
	int ifail = ITK_ok;
	tag_t top_revision = NULLTAG;
	tag_t relation_type = NULLTAG;
	tag_t temp_type = NULLTAG;
	int related_objects_count = 0;
	tag_t* related_objects = NULL;
	char* temp_dataset_name = NULL;
	char* item_id = NULL;
	char* target_dataset_name = NULL;
	char temp_type_name[TCTYPE_name_size_c+1] = "";

	try
	{
		WRITE_LOG("%s\n", ".find_and_read_placements_dataset");
		IFERR_THROW( AOM_ask_value_tag(top_bl, "bl_revision", &top_revision) );
		IFNULLTAG_THROW( top_revision );
		IFERR_THROW( AOM_ask_value_string(top_revision, "item_id", &item_id) );
		WRITE_LOG("item_id:%s\n", item_id);
		target_dataset_name = (char*) MEM_alloc(sizeof(char) * (strlen(item_id) + strlen("_PAP") + 1));
		strcpy(target_dataset_name, item_id);
		strcat(target_dataset_name, "_PAP");
		WRITE_LOG("target_dataset_name:%s\n", target_dataset_name);

		IFERR_THROW( GRM_find_relation_type("IMAN_Motion", &relation_type) );
		IFNULLTAG_THROW( relation_type );
		IFERR_THROW( GRM_list_secondary_objects_only(top_revision, relation_type, &related_objects_count, &related_objects) );
		for (int i = 0; i < related_objects_count; i++)
		{
			WRITE_LOG("i::%d\n", i);
			IFERR_THROW( TCTYPE_ask_object_type (related_objects[i], &temp_type) );
			IFERR_THROW( TCTYPE_ask_name(temp_type, temp_type_name) );
			IFERR_THROW( AOM_ask_value_string(related_objects[i], "object_name", &temp_dataset_name) );
			if ((strcmp(temp_type_name, "Text") == 0) && (strcmp(target_dataset_name, temp_dataset_name) == 0))
			{
				WRITE_LOG("%s\n", "YES");
				read_placements_dataset( related_objects[i] );
			}
		}
		WRITE_LOG("%s\n", ".find_and_read_placements_dataset done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(related_objects!=NULL) MEM_free(related_objects); related_objects = NULL;
	if(target_dataset_name!=NULL) MEM_free(target_dataset_name); target_dataset_name = NULL;
	if(temp_dataset_name!=NULL) MEM_free(temp_dataset_name); temp_dataset_name = NULL;
	if(item_id!=NULL) MEM_free(item_id); item_id = NULL;

	return ifail;
}

int read_placements_dataset(tag_t dataset)
{
	int ifail = ITK_ok;
	tag_t text_file = NULLTAG;
	IMF_file_t
	        file_descriptor = NULL;
	AE_reference_type_t
	        ref_type;
	char
		text_line[SS_MAXLLEN+1];

	try
	{
		WRITE_LOG("%s\n", ".read_placements_dataset");
		//if (dataset == NULLTAG) continue;
		IFERR_THROW( AOM_refresh(dataset, TRUE) );

		IFERR_THROW( AE_ask_dataset_named_ref(dataset, "Text", &ref_type, &text_file) );
		//if (text_file == NULLTAG) continue;
		IFERR_THROW( AOM_refresh(text_file, TRUE) );

		IFERR_THROW( IMF_ask_file_descriptor(text_file, &file_descriptor) );
		IFERR_THROW( IMF_open_file(file_descriptor, SS_RDONLY) );

		while (IMF_read_file_line(file_descriptor, text_line) == ITK_ok)
			WRITE_LOG("%s\n", text_line);

		IFERR_THROW( IMF_close_file(file_descriptor) );

		IFERR_THROW( AOM_unload(text_file) );
		IFERR_THROW( AOM_unload(dataset) );
		WRITE_LOG("%s\n", ".read_placements_dataset done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int set_occurrence_placement_from_file(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	tag_t
		*attachments = NULL,
		root_task,
		target_form,
		bvr_type,
		temp_type;
	int
		*attachments_types = NULL,
		attachments_count = 0;
	bool
		is_type_of = false;

	try
	{
		WRITE_LOG("%s\n", "Into set occurrence placement from file");

		IFERR_THROW( TCTYPE_find_type("BOMView Revision", NULL, &bvr_type) );
		IFNULLTAG_THROW( bvr_type );

		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );
		for(int i = 0; i < attachments_count; i++)
		{
			if(attachments_types[i]==EPM_target_attachment)
			{
				WRITE_LOG("%s\n", "Checking attachment type");
				IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
				IFERR_THROW( TCTYPE_is_type_of(temp_type, bvr_type, &is_type_of) );
				if(is_type_of)
				{
					IFERR_THROW( cycle_through_bls_and_set_placement(attachments[i]) );
				}
			}
		}

		WRITE_LOG("%s\n", "Into done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(attachments!=NULL) MEM_free(attachments); attachments = NULL;
	if(attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;

	return ifail;
}
