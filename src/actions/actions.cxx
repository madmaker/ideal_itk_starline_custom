#include <iostream>
#include <string>
#include <tc/tc.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include "../misc.hxx"
#include "actions.hxx"
#include "post_remove_all_substitutes.hxx"
#include "pre_return_all_substitutes.hxx"
#include "post_rename_item.hxx"
#include "post_set_eda_item_id.hxx"

int starline_custom_idealplm_register_actions(int * decision, va_list args)
{
	int ifail = ITK_ok;

	try
	{
		WRITE_LOG("%s", "Registering actions... ");

		*decision = ALL_CUSTOMIZATIONS;

		METHOD_id_t post_remove_all_substitutes_method;
		METHOD_id_t pre_return_all_substitutes_method;
		METHOD_id_t post_rename_item_method;
		METHOD_id_t post_set_eda_item_id_method;

		IFERR_THROW( METHOD_find_method("IMAN_RES_checkout", "createPost#Teamcenter::CreateInput,*", &post_remove_all_substitutes_method) );
		IFERR_THROW( METHOD_add_action(post_remove_all_substitutes_method, METHOD_post_action_type, (METHOD_function_t) post_remove_all_substitutes, NULL) );

		IFERR_THROW( METHOD_find_method("IMAN_RES_checkout", "IMAN_delete", &pre_return_all_substitutes_method) );
		IFERR_THROW( METHOD_add_action(pre_return_all_substitutes_method, METHOD_pre_action_type, (METHOD_function_t) pre_return_all_substitutes, NULL) );

		IFERR_THROW(METHOD_find_prop_method("ItemRevision", "object_name", "PROP_set_value_string", &post_rename_item_method));
		if(post_rename_item_method.id == 0) WRITE_LOG("%s", "cant find method for object_name on ItemRevision");
		IFERR_THROW( METHOD_add_action(post_rename_item_method, METHOD_post_action_type, (METHOD_function_t) post_rename_item, NULL) );

		IFERR_THROW(METHOD_find_prop_method("Item", "item_id", "PROP_set_value_string", &post_set_eda_item_id_method));
		if(post_set_eda_item_id_method.id == 0) WRITE_LOG("%s", "cant find method for item_id on Item");
		IFERR_THROW( METHOD_add_action(post_set_eda_item_id_method, METHOD_post_action_type, (METHOD_function_t) post_set_eda_item_id, NULL) );

		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		WRITE_LOG("%s\n", "failed");
		ifail = exfail;
	}

	return ifail;
}
