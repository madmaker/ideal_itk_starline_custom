#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include "../misc.hxx"
#include "post_rename_item.hxx"

int post_rename_item(METHOD_message_t *msg, va_list args)
{
	int ifail = ITK_ok;
	tag_t item = NULLTAG;
	tag_t *revisions = NULL;
	int revisions_count = 0;
	char* name = NULL;

	try
	{
		WRITE_LOG("%s\n", "-POST rename item called");

		IFERR_THROW( ITEM_ask_item_of_rev(msg->object_tag, &item) );
		IFERR_THROW( AOM_ask_value_tags(item, "revision_list", &revisions_count, &revisions) );
		if(revisions_count == 1)
		{
			WRITE_LOG("%s\n", "one revision");
			IFERR_THROW( ITEM_ask_rev_name2(msg->object_tag, &name) );
			IFERR_THROW( AOM_refresh(item, TRUE) );
			IFERR_THROW( ITEM_set_name(item, name) );
			IFERR_THROW( AOM_save(item) );
			IFERR_THROW( AOM_refresh(item, FALSE) );
		}

		WRITE_LOG("%s\n", "-POST done");
	}
	catch (int exfail)
	{
		ifail = exfail;
	}

	if(name!=NULL) MEM_free(name); name = NULL;

	return ifail;
}
