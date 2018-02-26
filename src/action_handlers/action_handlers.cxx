#include <iostream>
#include <string>
#include <tc/tc.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include "../misc.hxx"
#include "assign_form_property_to_targets.hxx"
#include "attach_previous_revisions.hxx"
#include "attach_where_used_assemblies.hxx"
#include "attach_used_components.hxx"
#include "check_user_and_group.hxx"
#include "check_group_and_role.hxx"
#include "rename_items_for_revisions_from_targets.hxx"
#include "export_plmxml.hxx"

int starline_custom_idealplm_register_action_handlers(int * decision, va_list args)
{
	int ifail = ITK_ok;

	try
	{
		WRITE_LOG("%s", "Registering action handlers... ");

		*decision = ALL_CUSTOMIZATIONS;

		IFERR_THROW( EPM_register_action_handler("IDEALPLM_AssignFormPropertyToTargets", "", assign_form_property_to_targets) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_AttachPreviousRevisions", "", attach_previous_revisions) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_AttachWhereUsedAssemblies", "", attach_where_used_assemblies) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_AttachUsedComponents", "", attach_used_components) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_CheckUserAndGroup", "", check_user_and_group) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_CheckGroupAndRole", "", check_group_and_role) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_RenameItemsForRevisionsFromTargets", "", rename_items_for_revisions_from_targets) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_ExportPLMXMLforSAP", "", export_plmxml) );

		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		WRITE_LOG("%s\n", "failed");
		ifail = exfail;
	}

	return ifail;
}
