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
#include <dirent.h>
#include <sys/stat.h>

#include "utils.h"
#include "config.h"
#include "template.h"
char *get_formdata_filename(const char *data, int *len)
{
    // get file name from data, return the address of beginning of filenamr and store length in len
    char *begin = strstr(data, "filename=\"") + 10;
    char *end = strstr(begin, "\"");
    *len = end - begin;
    char file_name[*len + 1];
    memcpy(file_name, begin, *len);
    file_name[*len] = 0;
    printf("Filename: %s\n", file_name);
    fflush(stdout);
    return begin;
}

char *get_formdata_path(const char *data, int *len, size_t data_length)
{
    // get request path from data, return the address of beginning of path and store length in len
    char *begin = strstr2(data, "name=\"path\"", data_length) + 11; // 11 for name="path"
    begin = strstr(begin, "/");                                     // begin = '/'
    char *end = strstr(begin, "\r\n");                              //end = '\n'
    *len = end - begin;
    char path[*len + 1];
    memcpy(path, begin, *len);
    printf("Len:%d", *len);
    path[*len] = 0;
    printf("Path:%s", path);
    fflush(stdout);
    return begin;
}

char *get_formdata_content(const char *data, size_t *len, size_t data_length)
{
    int boundary_size = strstr(data, "\n") - data;
    char boundary[boundary_size + 1];
    memcpy(boundary, data, boundary_size);
    boundary[boundary_size] = '\0';
    // printf("Bound:%s\n", boundary);
    // char *begin = data + boundary_size;
    // fflush(stdout);

    // printf("%s", end);
    char *begin = strstr(data, "Content-Type");
    begin = strstr(begin, "\n");

    begin = strstr(begin + 1, "\n") + 1;
    size_t bd = data_length - (begin - data);
    char *end = strstr2(begin, boundary, bd);
    *len = end - begin;
    // char path[*len + 1];
    // memcpy(path, begin, *len);
    // path[*len] = 0;
    // printf("Content:%s", path);
    printf("Content-Len:%ld", *len);
    fflush(stdout);

    return begin;
}

void do_get_request(struct evhttp_request *req, void *args)
{
    // Callback for GET request
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
    // Callback for POST request
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

void do_download_file(struct evhttp_request *req, void *args)
{
    // Download Format: http://localhost:8888/download/index.html
    struct evhttp_uri *uri = evhttp_request_get_evhttp_uri(req);
    printf("%s\n", evhttp_uri_get_path(uri));
    struct evkeyvalq param;
    // 提取文件路径
    char *uri_path = evhttp_uri_get_path(uri);
    char decoded_uri_path[100];
    urldecode(decoded_uri_path, uri_path);
    printf("%s\n", decoded_uri_path);
    char *p = strstr(decoded_uri_path, "/download");
    // "/"或者""
    char path[100];
    strcpy(path, FILE_WORKING_DIR);
    strcat(path, p + 9);
    if (strcmp(path, "/download") == 0)
    {
        strcat(path, "/");
    }
    printf("Path:%s\n", path);
    // char * path = realpath(path, );
    fflush(stdout);

    //path = "./files/"
    if (is_dir(path))
    {
        // list all the files;
        struct evbuffer *buff = evbuffer_new();
        struct evkeyvalq *output_header_kvq = evhttp_request_get_output_headers(req);
        evhttp_add_header(output_header_kvq, "Content-Type", "text/html");
        evbuffer_add_printf(buff, DOWNLOAD_HTML_HEAD_TEMPLATE);
        evbuffer_add_printf(buff, "<h1>Index of %s</h1>", p + 9);
        DIR *dir = opendir(path);
        struct dirent *entry;
        if (dir == NULL)
        {
            printf("%s\n", "Opendir error！");
        }
        struct dirent **namelist;
        int n = scandir(path, &namelist, 0, alphasort);
        for (int i = 0; i < n; i++)
        {
            entry = namelist[i];
            char url_path[100];
            strcpy(url_path, evhttp_request_get_uri(req));
            int len = strlen(evhttp_request_get_uri(req));
            if (evhttp_request_get_uri(req)[len - 1] != '/')
            {
                strcat(url_path, "/");
            }
            strcat(url_path, entry->d_name);
            if (entry->d_type == DT_DIR)
            {
                evbuffer_add_printf(buff, "<p><a href=\"%s\">%s/</p>", url_path, entry->d_name);
            }
            else
            {
                evbuffer_add_printf(buff, "<p><a href=\"%s\">%s</p>", url_path, entry->d_name);
            }
        }
        evbuffer_add_printf(buff, DOWNLOAD_HTML_TAIL_TEMPLATE);
        evhttp_send_reply(req, HTTP_OK, "OK", buff);
        free(namelist);
    }

    else
    {
        if (has_file(path))
        {
            // send reply to download the file
            struct evbuffer *buff = evbuffer_new();
            FILE *fp = fopen(path, "r");
            printf("%s\n", path);
            int fd = fileno(fp);
            fseek(fp, 0, SEEK_END);
            size_t file_size = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            // To judge if header has 'keep-alive'
            struct evkeyvalq *input_header_kvq = evhttp_request_get_input_headers(req);
            const char *connection_type = evhttp_find_header(input_header_kvq, "Connection");
            if (strcasecmp(connection_type, "keep-alive") != 0 || DOWNLOAD_FILE_CHUNK_OPTION == 0)
            {
                // No keep-alive or not to use chunk
                // normal transfer
                evbuffer_add_file(buff, fd, 0, file_size);
                struct evkeyvalq *output_header_kvq = evhttp_request_get_output_headers(req);
                evhttp_add_header(output_header_kvq, "Content-Type", "application/octet-stream");
                evhttp_send_reply(req, HTTP_OK, "OK", buff);
                evbuffer_free(buff);
            }

            else
            {
                // Chunk transfer
                struct evkeyvalq *output_header_kvq = evhttp_request_get_output_headers(req);
                evhttp_add_header(output_header_kvq, "Content-Type", "application/octet-stream");
                size_t num_chunk = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE; // To ensure that at least one chunk

                evhttp_send_reply_start(req, HTTP_OK, "OK");
                // 前 n - 1 个chunk是完整的
                for (int i = 0; i < num_chunk - 1; i++)
                {
                    struct evbuffer *buff = evbuffer_new();
                    struct evbuffer_file_segment *this_seg = evbuffer_file_segment_new(fd, i * CHUNK_SIZE, CHUNK_SIZE, NULL);
                    evbuffer_add_file_segment(buff, this_seg, 0, CHUNK_SIZE);
                    evhttp_send_reply_chunk(req, buff);
                    evbuffer_free(buff);
                }

                // 最后一个chunk是不完整的
                struct evbuffer *buff = evbuffer_new();
                struct evbuffer_file_segment *this_seg = evbuffer_file_segment_new(fd, (num_chunk - 1) * CHUNK_SIZE, file_size - (num_chunk - 1) * CHUNK_SIZE, NULL);
                evbuffer_add_file_segment(buff, this_seg, 0, CHUNK_SIZE);
                evhttp_send_reply_chunk(req, buff);
                evbuffer_free(buff);
                evhttp_send_reply_end(req);
            }
        }
        else
        {
            // send error reply
            evhttp_send_reply(req, HTTP_NOTFOUND, "File not found!", NULL);
        }
    }

    return;
}

void do_upload_file(struct evhttp_request *req, void *args)
{

    enum evhttp_cmd_type method = evhttp_request_get_command(req);
    if (method == EVHTTP_REQ_GET)
    {
        // Show upload interface
        struct evbuffer *buff = evbuffer_new();
        struct evkeyvalq *output_header_kvq = evhttp_request_get_output_headers(req);
        evbuffer_add_printf(buff, UPLOAD_HTML_TEMPLATE);
        evhttp_add_header(output_header_kvq, "Content-Type", "text/html");
        evhttp_send_reply(req, HTTP_OK, "OK", buff);
    }

    else if (method == EVHTTP_REQ_POST)
    {
        // Actual upload process
        size_t buffer_length = EVBUFFER_LENGTH(evhttp_request_get_input_buffer(req));
        printf("Buffer-Length: %ld", buffer_length);
        char input_buffer[buffer_length];

        memcpy(input_buffer, EVBUFFER_DATA(evhttp_request_get_input_buffer(req)), buffer_length);
        for (int i = 0; i < buffer_length; i++)
        {
            printf("%c", input_buffer[i]);
        }
        struct evbuffer *buff = evbuffer_new();
        struct evkeyvalq *output_header_kvq = evhttp_request_get_output_headers(req);
        // File name
        int file_name_len;
        char *file_name_begin = get_formdata_filename(input_buffer, &file_name_len);
        char file_name[file_name_len + 1];
        memcpy(file_name, file_name_begin, file_name_len);
        file_name[file_name_len] = '\0';
        // File content
        size_t file_content_len = 0;
        char *file_content_begin = get_formdata_content(input_buffer, &file_content_len, buffer_length);
        printf("Content-Length:%ld", file_content_len);
        fflush(stdout);
        char *file_content = malloc(file_content_len + 1);
        memcpy(file_content, file_content_begin, file_content_len);
        file_content[file_content_len] = '\0';
        fflush(stdout);
        // Path
        int file_path_len;

        char *file_path_begin = get_formdata_path(input_buffer, &file_path_len, buffer_length);
        char file_path[file_path_len + 1];
        memcpy(file_path, file_path_begin, file_path_len);
        file_path[file_path_len] = '\0';
        printf("%s\n", "start to save");
        fflush(stdout);
        int res = save_file(file_path, file_name, file_content, file_content_len);
        if (res)
        {
            evhttp_send_reply(req, HTTP_OK, "OK", buff);
        }
        else
        {
            evhttp_send_reply(req, HTTP_BADREQUEST, "Fail", buff);
        }
    }
    else
    {
        // Not supported yet!
    }
    return;
}
void normal_dispatch_callback(struct evhttp_request *req, void *args)
{
    char *uri = evhttp_request_get_uri(req);
    char *found = strstr(uri, "/download");
    if (found == uri)
    {
        do_download_file(req, args);
        return;
    }

    found = strstr(uri, "/upload");
    if (found == uri)
    {
        do_upload_file(req, args);
        return;
    }
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
}

int main()
{

    struct event_base *base = event_base_new();
    struct evhttp *http_server = evhttp_new(base);
    if (!http_server)
    {
        return -1;
    }

    int res = evhttp_bind_socket(http_server, "0.0.0.0", 8888);
    if (res == -1)
    {
        return -1;
    }
    // Normal requst

    evhttp_set_gencb(http_server, normal_dispatch_callback, NULL);

    // File Download and Upload Request
    event_base_dispatch(base);
    evhttp_free(http_server);
    event_base_free(base);

    // DIR *dir;
    // dir = opendir("./files/..");
    // struct dirent *entry;
    // if (dir == NULL)
    // {
    //     printf("%s\n", "Opendir error！");
    // }

    // while ((entry = readdir(dir)) != NULL)
    // {
    //     printf("%s\n", entry->d_name);
    // }
    if (has_file("/Users/yangchen/Desktop/Computer Networking/libevent-test/files/ken/test.go"))
    {
        printf("YES");
    }
    else
    {
        printf("NO");
    }
}