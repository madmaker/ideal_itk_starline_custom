#ifndef POST_REMOVE_ALL_SUBSTITUTES_HXX
#define POST_REMOVE_ALL_SUBSTITUTES_HXX
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

int list_substitutes(tag_t bvr, tag_t occurence);
int post_remove_all_substitutes(METHOD_message_t* msg, va_list args);

#ifdef __cplusplus
                   }
#endif

#endif
