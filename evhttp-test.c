#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/buffer.h>
#include <sys/queue.h>
#include <event.h>

void do_get_request(struct evhttp_request *req, void *args)
{
    // Easily return a message and reqeust uri and arguments list
    // Example:
    // You have sent a GET request to the server
    // Request URI: /test?a=1&b=2
    // a = 1
    // b = 2
    struct evbuffer *buff = evbuffer_new();

    const char *uri = evhttp_request_get_uri(req);

    struct evkeyvalq kvs;
    evhttp_parse_query(uri, &kvs);
    evbuffer_add_printf(buff, "You have sent a GET request to the server\r\n");
    evbuffer_add_printf(buff, "Request URI: %s\r\n", uri);
    for (struct evkeyval *head = kvs.tqh_first; head != NULL; head = head->next.tqe_next)
    {
        evbuffer_add_printf(buff, "%s = %s\n", head->key, head->value);
    }

    evhttp_send_reply(req, HTTP_OK, "OK", buff);

    evbuffer_free(buff);
}

void do_post_request(struct evhttp_request *req, void *args)
{
    // Easily return a message, request uri and argument list
    // WARNING: FOR TEST USE, post requests' message should be hidden
    // "You have sent a POST request to the server"
    // Request URI: /test
    // a = 1
    // b = 2
    struct evbuffer *buff = evbuffer_new();

    size_t buffer_length = EVBUFFER_LENGTH(evhttp_request_get_input_buffer(req));
    size_t uri_length = strlen(evhttp_request_get_uri(req)) + buffer_length + 2; // 2 for '?' and \0

    char post_data[buffer_length];

    char post_uri[uri_length];

    memcpy(post_data, EVBUFFER_DATA(evhttp_request_get_input_buffer(req)), buffer_length);
    sprintf(post_uri, "%s?%s", evhttp_request_get_uri(req), post_data);
    post_uri[uri_length - 1] = '\0';

    struct evkeyvalq kvs;
    evhttp_parse_query(post_uri, &kvs);
    evbuffer_add_printf(buff, "You have sent a POST request to the server\r\n");
    evbuffer_add_printf(buff, "Request URI: %s\r\n", evhttp_request_get_uri(req));
    for (struct evkeyval *head = kvs.tqh_first; head != NULL; head = head->next.tqe_next)
    {
        evbuffer_add_printf(buff, "%s = %s\n", head->key, head->value);
    }
    evhttp_send_reply(req, HTTP_OK, "OK", buff);
    evbuffer_free(buff);
}

int fileExist()
{
}
void download_file_callback(struct evhttp_request *req, void *args)
{

    // Download Format: http://localhost:8888/download/index.html
}

void upload_file_callback(struct evhttp_request *req, void *args)
{
}
void normal_dispatch_callback(struct evhttp_request *req, void *args)
{
    struct evbuffer *buff = evbuffer_new();
    struct evkeyvalq kvs;
    evhttp_parse_query(evhttp_request_get_uri(req), &kvs);
    for (struct evkeyval *head = kvs.tqh_first; head != NULL; head = head->next.tqe_next)
    {
        printf("key:%s, value: %s\n", head->key, head->value);
    }
    enum evhttp_cmd_type method = evhttp_request_get_command(req);
    switch (method)
    {
    case EVHTTP_REQ_GET:
        do_get_request(req, args);
        break;
    case EVHTTP_REQ_POST:
        do_post_request(req, args);
        break;
    default:
        break;
    }

    // evbuffer_add_printf(buff, "Server responsed, %s", evhttp_request_get_uri(req));
    // evhttp_send_reply(req, HTTP_OK, "OK", buff);
    // evbuffer_free(buff);
}

int main()
{
    struct event_base *base = event_base_new();
    struct evhttp *http_server = evhttp_new(base);
    if (!http_server)
    {
        return -1;
    }

    int res = evhttp_bind_socket(http_server, "127.0.0.1", 8888);
    if (res == -1)
    {
        return -1;
    }
    // Normal requst

    evhttp_set_gencb(http_server, normal_dispatch_callback, NULL);

    // File Download and Upload Request
    evhttp_set_cb(http_server, "/download", download_file_callback, NULL);
    evhttp_set_cb(http_server, "/upload", upload_file_callback, NULL);
    event_base_dispatch(base);
    evhttp_free(http_server);
    event_base_free(base);
    return 0;
}