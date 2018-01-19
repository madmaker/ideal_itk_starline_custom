#include <epm/epm.h>
#include <sa/sa.h>
#include <ics.h>
#include <ics2.h>
#include <ps.h>
#include <grm.h>
#include <epm/epm_task_template_itk.h>
#include <user_exits/epm_toolkit_utils.h>
#include <tc_util.h>
#include "post_remove_all_substitutes.hxx"
#include "../misc.hxx"
#include <base_utils/ResultCheck.hxx>

int list_substitutes(tag_t bvr, tag_t occurence)
{
	int erc = ITK_ok;
	int sub_number;
	tag_t* sub_items;
	tag_t* sub_views;

	char* object_name;
	char* bl_line_name;

	tag_t occurence_type;
	char occurence_type_name[TCTYPE_name_size_c+1];

	try
	{
		erc = TCTYPE_ask_object_type(occurence, &occurence_type);
		erc = TCTYPE_ask_name(occurence_type, occurence_type_name);
		WRITE_LOG("ot:%s\n", occurence_type_name);

		erc = AOM_ask_value_string(occurence, "occurrence_name", &bl_line_name);
		WRITE_LOG("bl_line_name:%s\n", bl_line_name);
		MEM_free(bl_line_name);

		erc = PS_list_substitutes (bvr, occurence, &sub_number, &sub_items, &sub_views);
		WRITE_LOG("%s:%i\n", "Number of substitutes", sub_number);
		for(int i = 0; i < sub_number; i++)
		{
			erc = AOM_ask_value_string(sub_items[i], "object_name", &object_name);
			WRITE_LOG("sub:%s\n", object_name);
			MEM_free(object_name);
		}
	}
	catch (int exfail)
	{
		return exfail;
	}

	if(sub_number>0)
	{
		MEM_free(sub_items);
		MEM_free(sub_views);
	}

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
			erc = PS_list_occurrences_of_bvr(primary_object, &occ_number, &occurences);
			for(int i = 0; i < occ_number; i++)
			{
				list_substitutes(primary_object, occurences[i]);
			}
		}

		WRITE_LOG("%s\n", "-POST done");
	}
	catch (int exfail)
	{
		return exfail;
	}

	if(occ_number > 0) MEM_free(occurences);

	return erc;
}
