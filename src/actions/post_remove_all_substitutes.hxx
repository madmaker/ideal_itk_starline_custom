#ifndef POST_REMOVE_ALL_SUBSTITUTES_HXX
#define POST_REMOVE_ALL_SUBSTITUTES_HXX
#include <tc/preferences.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include <tccore/method.h>
#include <tccore/tctype.h>
#include <iostream>

int set_substitute_base_data(tag_t replacement_form, tag_t revision);
int relate(tag_t bvr, tag_t form);
int list_substitutes(tag_t bvr, tag_t occurence, tag_t top_replacement_form);
int get_bom_window_and_top_line_from_bvr(tag_t bvr, logical packed, tag_t* bom_window, tag_t* top_line);
int post_remove_all_substitutes(METHOD_message_t* msg, va_list args);
int create_form(tag_t* form_r);

#endif
