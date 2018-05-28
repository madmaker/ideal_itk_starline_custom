#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include "epm/epm_task_template_itk.h"
#include <tccore/grm.h>
#include <tccore/aom_prop.h>
#include <tccore/aom.h>
#include <tccore/tctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <vector>
#include <string.h>
#include "../misc.hxx"
#include "export_plmxml.hxx"

int ep_read_arguments(TC_argument_list_t* arguments, char** transfer_mode, char** rev_rule, char** export_dir, char** type_obj_uid_file_name, char** type_obj_export, char** export_bom, char** type_obj_key, char** key)
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
		if (arguments_count < 3)
		{
			throw EPM_wrong_number_of_arguments;
		}

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
			    if (strcmp("TRANSFER_MODE", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its TRANSFER_MODE");
			        if (!STR_EMPTY(normal_value)) {
			        	*transfer_mode = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			        	strcpy(*transfer_mode, normal_value);
			        	WRITE_LOG("%s\n", *transfer_mode);
			        }
			    }
			    if (strcmp("REV_RULE", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its REV_RULE");
			    	if (!STR_EMPTY(normal_value)) {
			    		*rev_rule = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*rev_rule, normal_value);
			    		WRITE_LOG("%s\n", *rev_rule);
			    	}
			    }
			    if (strcmp("EXPORT_DIR", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its EXPORT_DIR");
			    	if (!STR_EMPTY(normal_value)) {
			    		*export_dir = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*export_dir, normal_value);
			    		WRITE_LOG("%s\n", *export_dir);
			    	}
			    }
			    if (strcmp("TYPE_OBJ_UID_FILE_NAME", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its TYPE_OBJ_UID_FILE_NAME");
			    	if (!STR_EMPTY(normal_value)) {
			    		*type_obj_uid_file_name = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*type_obj_uid_file_name, normal_value);
			    		WRITE_LOG("%s\n", *type_obj_uid_file_name);
			    	}
			    }
			    if (strcmp("TYPE_OBJ_EXPORT", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its TYPE_OBJ_EXPORT");
			    	if (!STR_EMPTY(normal_value)) {
			    		*type_obj_export = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*type_obj_export, normal_value);
			    		WRITE_LOG("%s\n", *type_obj_export);
			    	}
			    }
			    if (strcmp("EXPORT_BOM", Flag)==0)
			    {
					WRITE_LOG("%s\n", "Its EXPORT_BOM");
					if (!STR_EMPTY(normal_value)) {
						*export_bom = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
						strcpy(*export_bom, normal_value);
						WRITE_LOG("%s\n", *export_bom);
					}
			    }
			    if (strcmp("TYPE_OBJ_KEY", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its TYPE_OBJ_KEY");
			    	if (!STR_EMPTY(normal_value)) {
			    		*type_obj_key = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*type_obj_key, normal_value);
			    		WRITE_LOG("%s\n", *type_obj_key);
			    	}
			    }
			    if (strcmp("KEY", Flag)==0)
			    {
					WRITE_LOG("%s\n", "Its KEY");
					if (!STR_EMPTY(normal_value)) {
						*key = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
						strcpy(*key, normal_value);
						WRITE_LOG("%s\n", *key);
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
		if(transfer_mode == NULL)
		{
			*transfer_mode = (char *) MEM_alloc(sizeof(char));
			strcpy(*transfer_mode, "");
		}
		if(rev_rule == NULL)
		{
			*rev_rule = (char *) MEM_alloc(sizeof(char));
			strcpy(*rev_rule, "");
		}
		if(export_dir == NULL)
		{
			*export_dir = (char *) MEM_alloc(sizeof(char));
			strcpy(*export_dir, "");
		}
		if(type_obj_uid_file_name == NULL)
		{
			*type_obj_uid_file_name = (char *) MEM_alloc(sizeof(char));
			strcpy(*type_obj_uid_file_name, "");
		}
		if(type_obj_export == NULL)
		{
			*type_obj_export = (char *) MEM_alloc(sizeof(char));
			strcpy(*type_obj_export, "");
		}
		if(export_bom == NULL)
		{
			*export_bom = (char *) MEM_alloc(sizeof(char));
			strcpy(*export_bom, "");
		}
		if(type_obj_key == NULL)
		{
			*type_obj_key = (char *) MEM_alloc(sizeof(char));
			strcpy(*type_obj_key, "");
		}
		if(key == NULL)
		{
			*key = (char *) MEM_alloc(sizeof(char));
			strcpy(*key, "");
		}
		WRITE_LOG("%s\n", "Reading arguments is done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	return ifail;
}

int get_key_object_data(tag_t object, char* key, char** key_values_result)
{
	int ifail = ITK_ok;
	char* temp_value = NULL;
	char* key_values = NULL;

	try
	{
		key_values = (char *) malloc (sizeof (char));
		strcpy (key_values, "");

		const std::string value (key);
		std::vector < std::string > splited;
		char delim[2];
		delim[0] = 44;
		delim[1] = 0;

		size_t old_pos = 0;
		size_t curr_pos = value.find (delim);

		if(curr_pos == std::string::npos && strlen(key) > 0)
		{
		    splited.push_back ( key );
		}
		else if(curr_pos != std::string::npos)
		{
		    while (curr_pos != std::string::npos)
		    {
		    	splited.push_back (value.substr (old_pos, curr_pos - old_pos));
		    	old_pos = curr_pos + 1;
		    	curr_pos = value.find (delim, curr_pos + 1);
		    }
		    splited.push_back (value.substr (old_pos, value.length () - old_pos));
		}


		for (int i = 0; i < splited.size (); i++)
		{
			if (i < splited.size () - 1)
			{
				IFERR_THROW( AOM_ask_value_string(object, splited[i].c_str(), &temp_value) );
				key_values = (char *) realloc (key_values, sizeof (char) * (strlen (key_values) + strlen(splited[i].c_str()) + strlen(temp_value) + 2)); // 2 because we add , and =
			    strcat (key_values, splited[i].c_str ());
			    strcat (key_values, "=");
			    strcat (key_values, temp_value);
			    strcat (key_values, ",");
			    if(temp_value!=NULL) MEM_free(temp_value); temp_value = NULL;
			}
		    else
			{
		    	IFERR_THROW( AOM_ask_value_string(object, splited[i].c_str(), &temp_value) );
		    	key_values = (char *) realloc (key_values, sizeof (char) * (strlen (key_values) + strlen (splited[i].c_str()) + strlen(temp_value) + 1)); // 1 because we add =
		    	strcat (key_values, splited[i].c_str ());
		    	strcat (key_values, "=");
		    	strcat (key_values, temp_value);
		    	if(temp_value!=NULL) MEM_free(temp_value); temp_value = NULL;
			}
		}

		*key_values_result = (char*) MEM_alloc(sizeof(char) * (strlen(key_values) + 1));
		strcpy(*key_values_result, key_values);
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(temp_value!=NULL) MEM_free(temp_value); temp_value = NULL;
	if(key_values!=NULL) free (key_values); key_values = NULL;

	return ifail;
}

void rename_file(char* oldname)
{
	int result;

	std::string value(oldname);

	int pos = value.find_last_of('/');
	value.erase(pos+1, 1);

	printf("%s\n", value.c_str());

	result = rename( oldname , value.c_str() );
	if ( result != 0 )
		WRITE_LOG("%s\n", "Error renaming file");
}

int export_plmxml(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	char
	 	*task_uid = NULL,
	 	*obj_uid = NULL,
	 	*obj_export_uid = NULL,
	 	//util[] = "%TC_BIN%\\plmxml_export", // FOR WINDOWS
	 	util[] = "plmxml_export", //FOR SOLARIS
	    login[] = "-u=infodba -p=StarlineParts -g=dba",
	    cmd[1024] = " ",
		*transfer_mode = NULL,
		*rev_rule = NULL,
		*export_dir = NULL,
		*type_obj_uid_file_name = NULL,
		*type_obj_export = NULL,
		*export_bom = NULL,
		*type_obj_key = NULL,
		*key = NULL,
		*key_values = NULL,
		*rev = NULL,
		*site_uid = NULL;
	tag_t
		*attachments = NULL,
		root_task = NULLTAG,
		obj_uid_type = NULLTAG,
		obj_export_type = NULLTAG,
		obj_key_type = NULLTAG,
		temp_type = NULLTAG;
	int
		*attachments_types = NULL,
		attachments_count = 0,
		num_of_args = 0;
	bool
		is_obj_uid_type = false,
		is_obj_export_type = false,
		is_obj_key_type = false,
		do_export_bom = false;

	try
	{
		WRITE_LOG("%s\n", "Into export plmxml");
		IFERR_THROW( ep_read_arguments(msg.arguments, &transfer_mode, &rev_rule, &export_dir, &type_obj_uid_file_name, &type_obj_export, &export_bom, &type_obj_key, &key) );

		if(STR_EMPTY(transfer_mode))
		{
			transfer_mode = (char *) MEM_alloc(sizeof(char)*(strlen("ConfiguredDataExportDefault")+1));
			strcpy(transfer_mode, "ConfiguredDataExportDefault");
		}
		if(STR_EMPTY(rev_rule))
		{
			rev_rule = (char *) MEM_alloc(sizeof(char)*(strlen("Latest Working")+1));
			strcpy(rev_rule, "Latest Working");
		}
		if(STR_EMPTY(export_dir))
		{
			export_dir = (char *) MEM_alloc(sizeof(char)*(strlen("C:/Temp")+1));
			strcpy(export_dir, "C:/Temp");
		}

		if(strcmp(StrUpr(export_bom), "YES") == 0)
			do_export_bom = true;

		if(!STR_EMPTY(type_obj_uid_file_name))
		{
			IFERR_THROW( TCTYPE_find_type(type_obj_uid_file_name, NULL, &obj_uid_type) );
			IFNULLTAG_THROW( obj_uid_type );
		}
		if(!STR_EMPTY(type_obj_export))
		{
			IFERR_THROW( TCTYPE_find_type(type_obj_export, NULL, &obj_export_type) );
			IFNULLTAG_THROW( obj_export_type );
		}
		if(do_export_bom && !STR_EMPTY(type_obj_key))
		{
			IFERR_THROW( TCTYPE_find_type(type_obj_key, NULL, &obj_key_type) );
			IFNULLTAG_THROW( obj_key_type );
		}

		ITK__convert_tag_to_uid(msg.task, &task_uid);

		WRITE_LOG("%s\n", "Getting root task and attachments");
		IFERR_THROW( EPM_ask_root_task(msg.task, &root_task) );
		IFERR_THROW( EPM_ask_all_attachments(root_task, &attachments_count, &attachments, &attachments_types) );

		if (attachments_count > 0)
		{
			for(int i = 0; i < attachments_count; i++)
			{
				if(attachments_types[i] == EPM_target_attachment)
				{
					IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type) );
					if(do_export_bom && obj_key_type!=NULL)
					{
						IFERR_THROW( TCTYPE_is_type_of(temp_type, obj_key_type, &is_obj_key_type) );
						if(is_obj_key_type)
						{
							get_key_object_data(attachments[i], key, &key_values);
						}
					}
					else
					{
						WRITE_LOG("EXPORT_BOM=YES, but TYPE_OBJ_KEY does not exist.");
					}

					IFERR_THROW( TCTYPE_is_type_of(temp_type, obj_uid_type, &is_obj_uid_type) );
					if(is_obj_uid_type)
					{
						ITK__convert_tag_to_uid(attachments[i], &obj_uid);
					}

					IFERR_THROW( TCTYPE_is_type_of(temp_type, obj_export_type, &is_obj_export_type) );
					if(is_obj_export_type)
					{
						if(do_export_bom)
						{
							ifail = AOM_ask_value_string(attachments[i], "item_revision_id", &rev);
						}
						else
						{
							ITK__convert_tag_to_uid(attachments[i], &obj_export_uid);
						}

						tag_t owning_site = NULL;
						IFERR_THROW( AOM_ask_value_tag(attachments[i], "owning_site", &owning_site) );
						if(owning_site!=NULL)
						{
							ITK__convert_tag_to_uid(owning_site, &site_uid);
						}
						else
						{
							site_uid = (char *) MEM_alloc(sizeof(char)*(strlen("AAAAAAAAAAAAAA")+1));
							strcpy(site_uid, "AAAAAAAAAAAAAA");
						}
					}
				}
			}

			WRITE_LOG("%s\n", "Forming cmd line");
			sprintf(cmd, "%s %s -transfermode=\"%s\" -rev_rule=\"%s\"", util, login, transfer_mode, rev_rule);

			char filepath[256] = "";
			sprintf(filepath, "%s/.%s-%s%s.xml", export_dir, obj_uid, task_uid, site_uid);
			strcat(cmd, " -xml_file='");
			strcat(cmd, filepath);
			strcat(cmd, "'");

			if(do_export_bom)
			{
				strcat(cmd, " -export_bom=yes");
				strcat(cmd, " -key=");
				strcat(cmd, key_values);
				strcat(cmd, " -rev=");
				strcat(cmd, rev);
			}
			else
			{
				strcat(cmd, " -uid=");
				strcat(cmd, "'");
				strcat(cmd, obj_export_uid);
				strcat(cmd, "'");
			}

			WRITE_LOG("%s\n", cmd);
			WRITE_LOG("%s\n", "Executing");
			system(cmd);
			rename_file(filepath);
		}
		else
		{
			throw EPM_attachments_not_found;
		}
		WRITE_LOG("%s\n", "Done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	WRITE_LOG("%s\n", "1");
	if (transfer_mode!=NULL) MEM_free(transfer_mode); transfer_mode = NULL;
	WRITE_LOG("%s\n", "2");
	if (export_dir!=NULL) MEM_free(export_dir); export_dir = NULL;
	WRITE_LOG("%s\n", "3");
	if (rev_rule!=NULL) MEM_free(rev_rule); rev_rule = NULL;
	WRITE_LOG("%s\n", "4");
	if (type_obj_uid_file_name!=NULL) MEM_free(type_obj_uid_file_name); type_obj_uid_file_name = NULL;
	WRITE_LOG("%s\n", "5");
	if (type_obj_export!=NULL) MEM_free(type_obj_export); type_obj_export = NULL;
	WRITE_LOG("%s\n", "6");
	if (export_bom!=NULL) MEM_free(export_bom); export_bom = NULL;
	WRITE_LOG("%s\n", "7");
	if (type_obj_key!=NULL) MEM_free(type_obj_key); type_obj_key = NULL;
	WRITE_LOG("%s\n", "8");
	if (key_values!=NULL) MEM_free(key_values); key_values = NULL;
	WRITE_LOG("%s\n", "9");
	if (key!=NULL) MEM_free(key); key = NULL;
	WRITE_LOG("%s\n", "10");
	if (rev!=NULL) MEM_free(rev); rev = NULL;

	WRITE_LOG("%s\n", "11");
	if (obj_uid!=NULL) MEM_free(obj_uid); obj_uid = NULL;
	WRITE_LOG("%s\n", "12");
	if (obj_export_uid!=NULL) MEM_free(obj_export_uid); obj_export_uid = NULL;
	WRITE_LOG("%s\n", "13");
	if (task_uid!=NULL) MEM_free(task_uid); task_uid = NULL;
	WRITE_LOG("%s\n", "14");
	if (site_uid!=NULL) MEM_free(site_uid); site_uid = NULL;
	WRITE_LOG("%s\n", "15");
	if (attachments!=NULL) MEM_free(attachments); attachments = NULL;
	WRITE_LOG("%s\n", "16");
	if (attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;
	WRITE_LOG("%s\n", "17");

	return ifail;
}
