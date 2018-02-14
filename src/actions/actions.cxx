#include <iostream>
#include <string>
#include <tc/tc.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include "../misc.hxx"
#include "actions.hxx"
#include "post_remove_all_substitutes.hxx"
#include "pre_return_all_substitutes.hxx"

int starline_custom_idealplm_register_actions(int * decision, va_list args)
{
	int ifail = ITK_ok;

	try
	{
		WRITE_LOG("%s", "Registering actions... ");

		*decision = ALL_CUSTOMIZATIONS;

		METHOD_id_t post_remove_all_substitutes_method;
		METHOD_id_t pre_return_all_substitutes_method;

		IFERR_THROW( METHOD_find_method("IMAN_RES_checkout", "createPost#Teamcenter::CreateInput,*", &post_remove_all_substitutes_method) );
		//IFNULLTAG_THROW( (int) post_remove_all_substitutes_method.id );
		IFERR_THROW( METHOD_add_action(post_remove_all_substitutes_method, METHOD_post_action_type, (METHOD_function_t) post_remove_all_substitutes, NULL) );

		IFERR_THROW( METHOD_find_method("IMAN_RES_checkout", "IMAN_delete", &pre_return_all_substitutes_method) );
		//IFNULLTAG_THROW( (int) pre_return_all_substitutes_method.id );
		IFERR_THROW( METHOD_add_action(pre_return_all_substitutes_method, METHOD_pre_action_type, (METHOD_function_t) pre_return_all_substitutes, NULL) );

		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		WRITE_LOG("%s\n", "failed");
		ifail = exfail;
	}

	return ifail;
}
