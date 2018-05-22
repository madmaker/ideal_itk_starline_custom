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
#include "attach_as_refs_revs_related_to_form.hxx"
#include "set_change_notice_number.hxx"
#include "no_reference_attachments.hxx"
#include "attach_as_refs_if_other_active_workflow.hxx"
#include "remove_relations_of_targets.hxx"
#include "set_occurrence_placement_from_file.hxx"
#include "attach_as_refs_related_wo.hxx"
#include "check_if_directory_exists.hxx"
#include "check_group_and_role_rh.hxx"
#include "check_user_and_group_rh.hxx"

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
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_AttachRevsAsRefsIfRelatedToForm", "", attach_as_refs_revs_related_to_form) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_AttachAsRefsIfOtherActiveWorkflow", "", attach_as_refs_if_other_active_workflow) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_SetChangeNoticeNumber", "", set_change_notice_number) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_RemoveRelationsOfTargets", "", remove_relations_of_targets) );
		IFERR_THROW( EPM_register_rule_handler("IDEALPLM_NO_REFERENCE_ATTACHMENTS", "Success if there are no reference attachments", no_reference_attachments) );
		IFERR_THROW( EPM_register_rule_handler("IDEALPLM_CheckIfDirectoryExists", "Success if directory exists", check_if_directory_exists) );

		IFERR_THROW( EPM_register_rule_handler("IDEALPLM_CheckUserAndGroup", "Success if user and group are the same", check_user_and_group_rh) );
		IFERR_THROW( EPM_register_rule_handler("IDEALPLM_CheckGroupAndRole", "Success if group and role are the same", check_group_and_role_rh) );

		IFERR_THROW( EPM_register_action_handler("IDEALPLM_SetOccurrencePlacementFromFile", "", set_occurrence_placement_from_file) );
		IFERR_THROW( EPM_register_action_handler("IDEALPLM_AttachAsRefsRelatedWO", "", attach_as_refs_related_wo) );

		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		WRITE_LOG("%s\n", "failed");
		ifail = exfail;
	}

	return ifail;
}
