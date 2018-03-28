#ifndef POST_SET_EDA_ITEM_ID_HXX
#define POST_SET_EDA_ITEM_ID_HXX
#include <tccore/method.h>

int list_related_edas_to_rev_and_set_variant_name(tag_t revision, tag_t relation_type, char* item_id);
int post_set_eda_item_id(METHOD_message_t* msg, va_list args);

#endif
