#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include "epm/epm_task_template_itk.h"
#include <tccore/grm.h>
#include <tccore/aom_prop.h>
#include <tccore/aom.h>
#include <tccore/tctype.h>
#include <stdlib.h>
#include "../misc.hxx"
#include "export_plmxml.hxx"

int ep_read_arguments(TC_argument_list_t* arguments, char** transfer_mode, char** rev_rule, char** export_dir)
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
		if (arguments_count != 3)
		{
			throw EPM_wrong_number_of_arguments;
		}
		*transfer_mode = "";
		*rev_rule = "";
		*export_dir = "";

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
			    	WRITE_LOG("%s\n", "Its except");
			        if (!STR_EMPTY(normal_value)) {
			        	*transfer_mode = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			        	strcpy(*transfer_mode, normal_value);
			        	WRITE_LOG("%s\n", *transfer_mode);
			        } else {
			        	*transfer_mode = "";
			        }
			    }
			    if (strcmp("REV_RULE", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its except");
			    	if (!STR_EMPTY(normal_value)) {
			    		*rev_rule = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*rev_rule, normal_value);
			    		WRITE_LOG("%s\n", *rev_rule);
			    	} else {
			    		*rev_rule = "";
			    	}
			    }
			    if (strcmp("EXPORT_DIR", Flag)==0)
			    {
			    	WRITE_LOG("%s\n", "Its except");
			    	if (!STR_EMPTY(normal_value)) {
			    		*export_dir = (char *) MEM_alloc(sizeof(char)*(strlen(normal_value)+1));
			    		strcpy(*export_dir, normal_value);
			    		WRITE_LOG("%s\n", *export_dir);
			    	} else {
			    		*export_dir = "";
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

int get_related_sap_form(tag_t primary_object, tag_t relation, tag_t* sap_form)
{
	int ifail = ITK_ok;

	tag_t* related_objects = NULL;
	int related_objects_count = 0;

	try
	{
		WRITE_LOG("%s\n", "Getting related sap form");
		IFERR_THROW( GRM_list_secondary_objects_only(primary_object, relation, &related_objects_count, &related_objects) );
		if(related_objects_count!=1)
		{
			throw GRM_invalid_secondary_object;
		}

		*sap_form = related_objects[0];
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(related_objects!=NULL) MEM_free(related_objects); related_objects=NULL;

	return ifail;
}

int export_plmxml(EPM_action_message_t msg)
{
	int ifail = ITK_ok;
	char
	 	*task_uid = NULL,
	 	*form_uid = NULL,
	 	*revision_uid = NULL,
	 	//util[] = "%TC_BIN%\\plmxml_export", // FOR WINDOWS
	 	util[] = "plmxml_export", //FOR SOLARIS
	    login[] = "-u=infodba -p=infodba -g=dba",
	    cmd[256] = " ",
		*transfer_mode = NULL,
		*export_dir = NULL,
		*rev_rule = NULL,
		*site_uid,
		*sap_form_type_name = "SL4_SAPForm",
		*sap_form_relation_name = "SL4_SAPFormRel";
	tag_t
		*attachments = NULL,
		root_task = NULLTAG,
		sap_form = NULLTAG,
		item_revision_type = NULLTAG,
		temp_type_t = NULLTAG,
		sap_form_type,
		sap_form_relation_type;
	int
		*attachments_types = NULL,
		attachments_count = 0,
		num_of_args = 0;
	bool
		is_item_revision_type;

	try
	{
		WRITE_LOG("%s\n", "Into export plmxml");
		IFERR_THROW( ep_read_arguments(msg.arguments, &transfer_mode, &rev_rule, &export_dir) );

		if(STR_EMPTY(transfer_mode)){
			transfer_mode = (char *) MEM_alloc(sizeof(char)*(strlen("ConfiguredDataExportDefault")+1));
			strcpy(transfer_mode, "ConfiguredDataExportDefault");
		}
		if(STR_EMPTY(rev_rule)){
			rev_rule = (char *) MEM_alloc(sizeof(char)*(strlen("Latest Working")+1));
			strcpy(rev_rule, "Latest Working");
		}
		if(STR_EMPTY(export_dir)){
			export_dir = (char *) MEM_alloc(sizeof(char)*(strlen("C:/Temp")+1));
			strcpy(export_dir, "C:/Temp");
		}

		IFERR_THROW( TCTYPE_find_type("ItemRevision", NULL, &item_revision_type) );
		IFNULLTAG_THROW( item_revision_type );
		IFERR_THROW( TCTYPE_find_type(sap_form_type_name, NULL, &sap_form_type) );
		IFNULLTAG_THROW( sap_form_type );
		IFERR_THROW( TCTYPE_find_type(sap_form_relation_name, NULL, &sap_form_relation_type) );
		IFNULLTAG_THROW( sap_form_relation_type );

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
					IFERR_THROW( TCTYPE_ask_object_type(attachments[i], &temp_type_t) );
					IFERR_THROW( TCTYPE_is_type_of(temp_type_t, item_revision_type, &is_item_revision_type) );
					if(is_item_revision_type)
					{
						WRITE_LOG("%s\n", "Is item revision");
						IFERR_THROW( get_related_sap_form(attachments[i], sap_form_relation_type, &sap_form) );
						IFNULLTAG_THROW( sap_form );
						ITK__convert_tag_to_uid(attachments[i], &revision_uid);
						ITK__convert_tag_to_uid(sap_form, &form_uid);
						tag_t owning_site = NULL;
						IFERR_THROW( AOM_ask_value_tag(sap_form, "owning_site", &owning_site) );
						if(owning_site!=NULL){
							ITK__convert_tag_to_uid(owning_site, &site_uid);
						} else {
							site_uid = (char *) MEM_alloc(sizeof(char)*(strlen("AAAAAAAAAAAAAA")+1));
							strcpy(site_uid, "AAAAAAAAAAAAAA");
						}
						WRITE_LOG("%s\n", "Forming cmd line");
						//sprintf(cmd, "%s %s -xml_file=C:/Temp/%s-%s.xml -uid=", util, login, form_uid, task_uid);
						sprintf(cmd, "%s %s -xml_file=\"%s/%s-%s%s.xml\" -transfermode=\"%s\" -rev_rule=\"%s\" -uid=", util, login, export_dir, form_uid, task_uid, site_uid, transfer_mode, rev_rule);
						strcat(cmd, revision_uid);
						//printf("Result command: \n%s", cmd);
						WRITE_LOG("%s\n", cmd);
						WRITE_LOG("%s\n", "Executing");
						system(cmd);
						break;
					}
				}
			}
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

	if (transfer_mode) MEM_free(transfer_mode);
	if (export_dir) MEM_free(export_dir);
	if (rev_rule) MEM_free(rev_rule);
	if (form_uid) MEM_free(form_uid);
	if (task_uid) MEM_free(task_uid);
	if (site_uid) MEM_free(site_uid);
	if(attachments!=NULL) MEM_free(attachments); attachments = NULL;
	if(attachments_types!=NULL) MEM_free(attachments_types); attachments_types = NULL;

	return ifail;
}
