#include <epm/epm.h>
#include <sa/sa.h>
#include <ics.h>
#include <ics2.h>
#include <ps.h>
#include <grm.h>
#include <epm/epm_task_template_itk.h>
#include <user_exits/epm_toolkit_utils.h>
#include <tc_util.h>
#include "pre_return_all_substitutes.hxx"
#include "../misc.hxx"
#include <base_utils/ResultCheck.hxx>

int pre_return_all_substitutes(METHOD_message_t *msg, va_list args)
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
		WRITE_LOG("%s\n", "-PRE remove all substitutes called");

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
		}

		WRITE_LOG("%s\n", "-PRE done");
	}
	catch (int exfail)
	{
		return exfail;
	}

	if(occ_number > 0) MEM_free(occurences);

	return erc;
}
