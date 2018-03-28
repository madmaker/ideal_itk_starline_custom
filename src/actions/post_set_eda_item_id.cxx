#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include <tccore/grm.h>
#include <tccore/tctype.h>
#include "../misc.hxx"
#include "post_set_eda_item_id.hxx"

int post_set_eda_item_id(METHOD_message_t *msg, va_list args)
{
	int ifail = ITK_ok;
	tag_t item = NULLTAG;
	tag_t *revisions = NULL;
	tag_t relation_type = NULLTAG;
	int revisions_count = 0;
	char* relation_name = "EDAHasVariant";
	char* item_id = NULL;

	try
	{
		WRITE_LOG("%s\n", "-POST change eda item id");

		IFERR_THROW( TCTYPE_find_type(relation_name, NULL, &relation_type) );
		IFNULLTAG_THROW( relation_type );

		item = msg->object_tag;
		IFERR_THROW( AOM_ask_value_tags(item, "revision_list", &revisions_count, &revisions) );
		IFERR_THROW( AOM_ask_value_string(item, "item_id", &item_id) );
		WRITE_LOG("%s%s\n", "current item_id is ", item_id);
		for(int i = 0; i < revisions_count; i++)
		{
			list_related_edas_to_rev_and_set_variant_name(revisions[i], relation_type, item_id);
		}
		WRITE_LOG("%s\n", "-POST done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(revisions!=NULL) MEM_free(revisions); revisions = NULL;
	if(item_id!=NULL) MEM_free(item_id); item_id = NULL;

	return ifail;
}

int list_related_edas_to_rev_and_set_variant_name(tag_t revision, tag_t relation_type, char* item_id)
{
	int ifail = ITK_ok;
	int relations_count = 0;
	tag_t* relations = NULL;
	tag_t relation = NULLTAG;

	try
	{
		IFERR_THROW( GRM_list_primary_objects_only(revision, relation_type, &relations_count, &relations) );
		for(int i = 0; i < relations_count; i++)
		{
			WRITE_LOG("%s\n", "working with relation");
			IFERR_THROW( GRM_find_relation(relations[i], revision, relation_type, &relation) );
			IFNULLTAG_THROW( relation );
			IFERR_THROW( AOM_refresh(relation, TRUE) );
			IFERR_THROW( AOM_set_value_string(relation, "variant_name", item_id) );
			IFERR_THROW( AOM_save(relation) );
			IFERR_THROW( AOM_refresh(relation, FALSE) );
		}
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(relations!=NULL) MEM_free(relations); relations = NULL;

	return ifail;
}
