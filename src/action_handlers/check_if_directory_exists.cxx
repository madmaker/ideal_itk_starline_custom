#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tc/emh.h>
#include <dirent.h>
#include "../misc.hxx"
#include "check_if_directory_exists.hxx"

int cide_read_arguments(TC_argument_list_t* arguments, char** directory)
{
	int ifail = ITK_ok;
	int
		arguments_count = 0;
	char
		*flag = NULL,
		Flag[256],
		*value= NULL,
		*normal_value= NULL;

	try
	{
		WRITE_LOG("%s\n", "Reading arguments");
		arguments_count = TC_number_of_arguments(arguments);
		if (arguments_count != 1)
		{
			throw EPM_wrong_number_of_arguments;
		}
		*directory = "";

		for (int i = 0; i < arguments_count; i++)
		{
			WRITE_LOG("%s\n", "Asking named value");
			IFERR_THROW( ITK_ask_argument_named_value(TC_next_argument(arguments), &flag, &value) );
			WRITE_LOG("%s\n", "No errors");
			IFERR_THROW( EPM_substitute_keyword(value, &normal_value) );
			if (normal_value != NULL)
			{
				WRITE_LOG("%s\n", "Normal value not NULL");
			    StrCpy(Flag,256,flag);
			    StrUpr(Flag);
			    if (strcmp("EXPORT_DIR", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its directory");
			        if (!STR_EMPTY(normal_value)) {
			        	*directory = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			        	strcpy(*directory, normal_value);
			        	WRITE_LOG("%s\n", *directory);
			        } else {
			        	*directory = "";
			        }
			    }
		        MEM_free(normal_value);
			    normal_value = NULL;
			}
			else
			{
				WRITE_LOG("%s\n", "Empty attribute");
		   	}
			if (flag != NULL) {
				MEM_free(flag);
				flag = NULL;
			}
			if (value != NULL) {
				MEM_free(value);
				value = NULL;
			}
		}
		WRITE_LOG("%s\n", "Reading arguments is done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

EPM_decision_t check_if_directory_exists(EPM_rule_message_t msg)
{
	int ifail = ITK_ok;
	EPM_decision_t decision = EPM_go;

	char* directory = NULL;

	try
	{
		WRITE_LOG("%s\n", "Into check if directory exists");
		IFERR_THROW( cide_read_arguments(msg.arguments, &directory) );

		if(!directory_exists(directory))
		{
			WRITE_LOG("Directory %s does not exist.\n", directory);
			decision = EPM_nogo;
		}

		WRITE_LOG("%s\n", "Done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(directory!=NULL) MEM_free(directory);

	return decision;
}

logical directory_exists(char* directory)
{
	logical result = false;

	DIR* dir = opendir(directory);
	if (dir)
	{
	    result = true;
	    closedir(dir);
	}

	return result;
}
