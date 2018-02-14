#include <iostream>
#include <string>
#include <tc/tc.h>
#include <tccore/custom.h>
#include "misc.hxx"
#include "starline_custom_idealplm_exits.hxx"
#include "actions/actions.hxx"
#include "action_handlers/action_handlers.hxx"

//#ifdef __cplusplus
//extern "C" {
//#endif

//int starline_custom_idealplm_register_callbacks()
extern "C" __declspec (dllexport) int starline_custom_idealplm_register_callbacks()
{
	int ifail = ITK_ok;

	try
	{
		WRITE_LOG("%s", "Loading idealplm custom library... ");

		IFERR_THROW( CUSTOM_register_exit("starline_custom_idealplm", "USER_init_module", starline_custom_idealplm_register_actions) );
		IFERR_THROW( CUSTOM_register_exit("starline_custom_idealplm", "USER_gs_shell_init_module", starline_custom_idealplm_register_action_handlers) );

		WRITE_LOG("%s\n", "done");
	}
	catch (int exfail)
	{
		WRITE_LOG("%s\n", "failed");
		ifail = exfail;
	}
	return ifail;
}

//#ifdef __cplusplus
//}
//#endif
