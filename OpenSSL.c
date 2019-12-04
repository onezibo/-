#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;

    // return a pointer to the X509 certificate that the peer presented
    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates found.\n");
}

int main()
{
    SSL_CTX *ctx;
    SSL_library_init();
    ctx = InitServerCTX();
    int port;
    int server;

    SSL_METHOD *method;
    method = SSLv23_server_method();
    ctx = SSL_CTX_new(method); //设置OpenSSL隧道加密的算法。Server与Client使用的算法必须一致，否则SSL/TLS握手会失败，连接会被切断。
                               //     SSL_CTX_set_verify (ctx,
                               //                         int mode,
                               //                         int (*verify_callback),
                               //                         int (X509_STROE_CTX *));//设置CTX的属性
    // SSL_CTX_load_varify_location (SSL_CTX *ctx, const char *Cafile, const char *Capath);//加载CA证书
    // SSL_CTX_use_Private_file (SSL_CTX *ctx, const char *file, int type);//加载用户私钥
    // SSL_CTX_use_certificate_file (SSL_CTX *ctx, const char *file, int type);//加载用户证书
    // SSL_CTX_check_private_key (SSL_CTX *ctx);//验证私钥和证书是否相等

    int sd;
    struct sockaddr_in addr;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error in binding port");
        abort();
    }
    if (listen(sd, 5) < 0)
    {
        perror("Error in listening port");
        abort();
    }

    server = sd;

    while (1)
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;
        int client = accept(server, (struct sockaddr *)&addr, &len);
        printf("Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client); //使用socket绑定SSL套接字

        char buf[1024];
        char reply[1024];
        int sd, bytes;
        const char *HTMLecho = "<html><body><pre>%s</pre></body></html>\n\n";

        if (SSL_accept(ssl) == -1)
            ERR_print_errors_fp(stderr);
        else
        {
            ShowCerts(ssl);
            bytes = SSL_read(ssl, buf, sizeof(buf));
            if (bytes > 0)
            {
                buf[bytes] = 0;
                printf("Client msg: \"%s\"\n", buf);
                sprintf(reply, HTMLecho, buf);
                SSL_write(ssl, reply, strlen(reply));
            }
            else
                ERR_print_errors_fp(stderr);
        }

        sd = SSL_get_fd(ssl); 
        SSL_free(ssl);       // 释放SSL套接字
        close(sd);          
    }

    close(server);      
    SSL_CTX_free(ctx);      //释放SSL会话
}