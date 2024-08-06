#ifndef _HANDLERS_H
#define _HANDLERS_H

#include <comet.h>

extern HttpcResponse* post_short(void*, HttpcRequest*, UrlParams*);
extern HttpcResponse* get_short(void*, HttpcRequest*, UrlParams*);
extern HttpcResponse* delete_short(void*, HttpcRequest*, UrlParams*);
extern HttpcResponse* get_static(void*, HttpcRequest*, UrlParams*);
extern HttpcResponse* redirect_home(void*, HttpcRequest*, UrlParams*);

#endif
