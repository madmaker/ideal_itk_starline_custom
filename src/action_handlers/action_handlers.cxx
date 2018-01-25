#include <iostream>
#include <string>
#include <base_utils/ResultCheck.hxx>
#include <tc/tc.h>
#include <property/prop_msg.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include "../misc.hxx"
#include "assign_form_property_to_targets.hxx"

int m9_1c_lib_register_action_handlers(int * decision, va_list args)
{
	int erc = ITK_ok;

	try
	{
		WRITE_LOG("%s\n", "Registering action handlers...");

		*decision = ALL_CUSTOMIZATIONS;

		erc = EPM_register_action_handler("IDEALPLM_AssignFormPropertyToTargets", "", assign_form_property_to_targets);
	}
	catch (int exfail)
	{
		return exfail;
	}

	return ITK_ok;
}
