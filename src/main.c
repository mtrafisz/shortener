#include <stdlib.h>
#include <time.h>

#include <sqlite3.h>
#include <comet.h>
#include <json.h>

#include "../include/db.h"

CometRouter* router;
Database* db;

char* get_rand_id(size_t len) {
    const char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* id = malloc(len + 1);

    for (size_t i = 0; i < len; i++) {
        id[i] = charset[rand() % strlen(charset)];
    }

    id[len] = '\0';
    return id;
}

HttpcResponse* post_short(HttpcRequest* req, UrlParams* _) {
    HttpcResponse* res;
    HttpcHeader* content_type = httpc_header_new("Content-Type", "text/plain");

    if (!req->body || req->body_size == 0) {
        res = httpc_response_new("Bad Request", 400);
        httpc_response_set_body(res, "Missing body", 13);
        goto send;
    }

    json_object* root = json_tokener_parse(req->body);
    if (root == NULL) {
        res = httpc_response_new("Bad Request", 400);
        httpc_response_set_body(res, "Invalid JSON", 13);
        goto send;
    }

    json_object* url;
    if (!json_object_object_get_ex(root, "url", &url)) {
        res = httpc_response_new("Bad Request", 400);
        httpc_response_set_body(res, "Missing 'url' field", 20);
        goto send;
    }

    const char* url_str = json_object_get_string(url);
    if (url_str == NULL) {
        res = httpc_response_new("Bad Request", 400);
        httpc_response_set_body(res, "Invalid 'url' field", 20);
        goto send;
    }

    char* id = get_rand_id(8);
    if (!db_set(db, id, url_str)) {
        res = httpc_response_new("Internal Server Error", 500);
        httpc_response_set_body(res, "Failed to save URL", 19);
        goto send;
    }

    json_object* id_json = json_object_new_string(id);
    const char* id_str = json_object_to_json_string(id_json);

    res = httpc_response_new("Created", 201);
    httpc_response_set_body(res, id_str, strlen(id_str));
    
    json_object_put(root);
    json_object_put(id_json);

    httpc_header_free(content_type);
    content_type = httpc_header_new("Content-Type", "application/json");

send:
    httpc_add_header_h(res->headers, content_type);
    return res;
}

HttpcRequest* log_request(HttpcRequest* req, UrlParams* _) {
    log_message(LOG_INFO, "%s request for %s", httpc_method_to_string(req->method), req->url);
    return req;
}

int main(void) {
    srand(time(NULL));
    comet_init(false, true);

    router = router_init(8080);
    if (router == NULL) {
        return 1;
    }

    if (!db_open(&db, "test.db")) {
        return 1;
    }

    router_add_route(router, "/short", HTTPC_POST, post_short);

    for (size_t i = 0; i < router->num_routes; i++) {
        router_add_middleware(router, i, log_request);
    }

    router_start(router);

    return 0;
}
