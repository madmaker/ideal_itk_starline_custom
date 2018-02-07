#include <iostream>
#include <string>
#include <tc/tc.h>
#include <property/prop_msg.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include "../misc.hxx"
#include "actions.hxx"
#include "post_remove_all_substitutes.hxx"
#include "pre_return_all_substitutes.hxx"

int starline_custom_idealplm_register_actions(int * decision, va_list args)
{
	int erc = ITK_ok;
	try
	{
		WRITE_LOG("%s\n", "Registering actions...");

		*decision = ALL_CUSTOMIZATIONS;

		METHOD_id_t post_remove_all_substitutes_method;
		METHOD_id_t pre_return_all_substitutes_method;

		erc = METHOD_find_method("IMAN_RES_checkout", "createPost#Teamcenter::CreateInput,*", &post_remove_all_substitutes_method);
		if (post_remove_all_substitutes_method.id != NULLTAG)
		{
			WRITE_LOG(" %s\n", "POST remove all substitutes");
			erc = METHOD_add_action(post_remove_all_substitutes_method, METHOD_post_action_type, (METHOD_function_t) post_remove_all_substitutes, NULL);
		}
		else
		{
			fprintf(stdout, "Method NOT found! (createPost#Teamcenter::CreateInput,*) for (IMAN_RES_checkout)\n", erc);
		}

		erc = METHOD_find_method("IMAN_RES_checkout", "IMAN_delete", &pre_return_all_substitutes_method);
		if (pre_return_all_substitutes_method.id != NULLTAG)
		{
			WRITE_LOG(" %s\n", "PRE return all substitutes");
			erc = METHOD_add_action(pre_return_all_substitutes_method, METHOD_pre_action_type, (METHOD_function_t) pre_return_all_substitutes, NULL);
		}
		else
		{
			fprintf(stdout, "Method NOT found! (IMAN_delete) for (IMAN_RES_checkout)\n", erc);
		}
	}
	catch (int exfail)
	{
		return exfail;
	}
	return ITK_ok;
}
