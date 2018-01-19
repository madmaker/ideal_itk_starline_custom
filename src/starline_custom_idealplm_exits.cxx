#include <iostream>
#include <string>
#include <base_utils/ResultCheck.hxx>
#include <tc/tc.h>
#include <tccore/custom.h>
#include "misc.hxx"
#include "starline_custom_idealplm_exits.hxx"
#include "actions/actions.hxx"

//cppcheck-suppress unusedFunction
extern "C" __declspec (dllexport) int starline_custom_idealplm_register_callbacks()
{
	int erc = ITK_ok;

	try
	{
		printf("\Loading idealplm custom library...\n");

		erc = CUSTOM_register_exit("starline_custom_idealplm", "USER_init_module", starline_custom_idealplm_register_actions);
	}
	catch (...)
	{
		return erc;
	}
	return erc;
}
