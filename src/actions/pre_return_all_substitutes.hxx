#ifndef PRE_RETURN_ALL_SUBSTITUTES_HXX
#define PRE_RETURN_ALL_SUBSTITUTES_HXX
#include <tccore/method.h>
#include <aom.h>
#include <item.h>
#include <preferences.h>
#include <tccore/aom_prop.h>
#include <tccore/tctype.h>
#include <iostream>
#ifdef __cplusplus
         extern "C"{
#endif

int get_substitute_base_data(tag_t replacement_form, tag_t* rev, tag_t* item);
int restore_all_substitutes(tag_t bvr, tag_t top_replacement_form, int child_bom_lines_count, tag_t* child_bom_lines);
int pre_return_all_substitutes(METHOD_message_t* msg, va_list args);

#ifdef __cplusplus
                   }
#endif

#endif
