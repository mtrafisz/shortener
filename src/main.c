#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include <comet.h>

#include "../include/db.h"
#include "../include/handlers.h"
#include "../include/context.h"

CometRouter* router;

void sigint_handler(int _) {
    router->running = false;
    puts("");
    log_message(LOG_INFO, "SIGINT received, shutting down...");
}

HttpcRequest* log_request(void* ctx, HttpcRequest* req, UrlParams* _) {
    log_message(LOG_INFO, "%s request for %s", httpc_method_to_string(req->method), req->url);
    return req;
}

int main(void) {
    srand(time(NULL));
    comet_init(false, true);
    signal(SIGINT, sigint_handler);

    Context ctx;

    router = router_init(8080, &ctx);
    if (router == NULL) {
        return 1;
    }

    if (!db_open(&ctx.db, "shortener.db")) {
        return 1;
    }

    router_add_route(router, "/short", HTTPC_POST, post_short);
    router_add_route(router, "/short/{id}", HTTPC_GET, get_short);
    router_add_route(router, "/short/{id}", HTTPC_DELETE, delete_short);
    router_add_route(router, "/", HTTPC_GET, redirect_home);
    router_add_route(router, "/*", HTTPC_GET, get_static);

    for (size_t i = 0; i < router->num_routes; i++) {
        router_add_middleware(router, i, log_request);
    }

    router_start(router);

    return 0;
}
