#include "../include/handlers.h"
#include "../include/db.h"
#include "../include/context.h"

#include <json.h>
#include <comet.h>

char* get_rand_id(size_t len) {
    const char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* id = malloc(len + 1);

    for (size_t i = 0; i < len; i++) {
        id[i] = charset[rand() % strlen(charset)];
    }

    id[len] = '\0';
    return id;
}


HttpcResponse* post_short(void* ctx, HttpcRequest* req, UrlParams* _) {
    Context* context = (Context*)ctx;

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
    if (!db_set(context->db, id, url_str)) {
        res = httpc_response_new("Internal Server Error", 500);
        httpc_response_set_body(res, "Failed to save URL", 20);
        goto send;
    }

    json_object* id_json = json_object_new_string(id);
    const char* id_str = json_object_to_json_string(id_json);

    json_object* created = json_object_new_object();
    json_object_object_add(created, "id", id_json);

    const char* created_str = json_object_to_json_string(created);

    res = httpc_response_new("Created", 201);
    httpc_response_set_body(res, created_str, strlen(created_str));

    json_object_put(root);
    json_object_put(created);
    json_object_put(id_json);

    httpc_header_free(content_type);
    content_type = httpc_header_new("Content-Type", "application/json");

send:
    httpc_add_header_h(&res->headers, content_type);
    return res;
}

HttpcResponse* get_short(void* ctx, HttpcRequest* req, UrlParams* params) {
    Context* context = (Context*)ctx;

    HttpcResponse* res;
    HttpcHeader* content_type = httpc_header_new("Content-Type", "text/plain");

    const char* id = NULL;

    for (size_t i = 0; i < params->num_params; i++) {
        if (strcmp(params->params[i].key, "id") == 0) {
            id = params->params[i].value;
            break;
        }
    }

    if (id == NULL) {
        res = httpc_response_new("Bad Request", 400);
        httpc_response_set_body(res, "Missing 'id' parameter", 21);
        goto send;
    }

    char* url = db_get(context->db, id);

    if (url == NULL) {
        res = httpc_response_new("Not Found", 404);
        httpc_response_set_body(res, "Short URL not found", 19);
        goto send;
    }

    res = httpc_response_new("Found", 302);
    httpc_response_set_body(res, url, strlen(url));
    httpc_add_header_h(&res->headers, httpc_header_new("Location", url));

send:
    httpc_add_header_h(&res->headers, content_type);
    return res;
}

HttpcResponse* delete_short(void* ctx, HttpcRequest* req, UrlParams* params) {
    Context* context = (Context*)ctx;

    HttpcResponse* res;
    HttpcHeader* content_type = httpc_header_new("Content-Type", "text/plain");

    const char* id = NULL;

    for (size_t i = 0; i < params->num_params; i++) {
        if (strcmp(params->params[i].key, "id") == 0) {
            id = params->params[i].value;
            break;
        }
    }

    if (id == NULL) {
        res = httpc_response_new("Bad Request", 400);
        httpc_response_set_body(res, "Missing 'id' parameter", 21);
        goto send;
    }

    if (!db_del(context->db, id)) {
        res = httpc_response_new("Not Found", 404);
        httpc_response_set_body(res, "Short URL not found", 19);
        goto send;
    }

    res = httpc_response_new("No Content", 204);

send:
    httpc_add_header_h(&res->headers, content_type);
    return res;
}
