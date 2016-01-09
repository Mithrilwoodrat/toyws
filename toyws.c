/*
 *  TOYWebServer.c - 一个简单直接的 HTTP/1.0 Web server, 阅读csapp中出于学习目的编写 
 *
 */
#include "toyws.h"


void serve(int fd);
void read_request_headers(int fd);
void send_error(int fd, char *cause, char *errnum, char *errmsg);
void serve_static(int fd, char *filename, int filesise);
int parse_uri(char *uri, char *filename, char *cgiargs);
void do_get(int fd, char *uri);
void do_head(int fd, char *uri);
int read_post_data(int fd);
void do_post(int fd, char *uri);

int main(int argc, char* argv[])
{
    int listenfd, connfd, port;
    socklen_t c_len;
    struct sockaddr_in s_addr, c_addr;
    pthread_t new_thread;

    
    /* 检查参数个数是否正确 */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n",argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    /* 初始化listen_fd*/
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons((unsigned short)port);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(listenfd, (SA *)&s_addr, sizeof(s_addr));
    
    listen(listenfd, 3);

    /*run forever,不返回*/
    while(1) {
        c_len = sizeof(c_addr);
        connfd = accept(listenfd, (SA*)&c_addr, &c_len);
        if (connfd == -1) {
            printf("Accept Error\n");
            exit(-1);
        }
        printf("connected from %s <%d>\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
        if (pthread_create(&new_thread, NULL, serve, connfd) != 0){    
            perror("pthread_creat"); /* thread safety */
        }
    }
    close(listenfd);
    return 0;
}

void serve(int fd)
{
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];

    /*读取 Rquest Line 和Headers*/
    rio_readline(fd, buf, MAXLINE);
    //printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);
    printf("uri: %s\t method: %s\n",uri, method);
    if (!strncasecmp(method,"GET",MAXLINE)) {
        /* get不区分大小写 */
        do_get(fd, uri);
        return;
    }
    else if (!strncasecmp(method,"HEAD",MAXLINE)) {
        do_head(fd, uri);
        return;
    }
    else if (!strncasecmp(method,"POST",MAXLINE)) {
        do_post(fd, uri);
        return;
    }
    send_error(fd, method, "501","Not Implemented");
}

void do_get(int fd, char * uri)
{
    int is_static;
    struct stat sbuf;
    char filename[MAXLINE], cgiargs[MAXLINE];
    read_request_headers(fd); /* 读取并忽略headers */

    /* 从GET request 中解析 URI  */
    is_static = parse_uri(uri, filename, cgiargs);
    printf("request filename: %s\n",filename);
    if (stat(filename, &sbuf) < 0) {
        send_error(fd, filename, "404", "Not Found");
        return;
    }
    if (is_static) {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            send_error(fd, filename, "403", "Forbidden");
            return;
        }
        serve_static(fd, filename, sbuf.st_size);
    }
    close(fd);
    return;
}

void do_head(int fd, char *uri)
{

    struct stat sbuf;
    char filename[MAXLINE], cgiargs[MAXLINE];
    read_request_headers(fd); /* 读取并忽略headers */

    /* 从HEAD request 中解析 URI  */
    parse_uri(uri, filename, cgiargs);
    printf("request filename: %s\n",filename);
    if (stat(filename, &sbuf) < 0) {
        send_error(fd, filename, "404", "Not Found");
        return;
    }

    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
        send_error(fd, filename, "403", "Forbidden");
        return;
    }
    int filesize = sbuf.st_size;
    char filetype[MAXLINE], buf[MAXLINE], *body;
    /* 发送 Response headers 到客户端 */
    get_file_type(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK \r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: toyws\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: %s\r\n", filetype);
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", filesize);
    rio_writen(fd, buf, strlen(buf));
    close(fd);
    return;
}

int read_post_data(int fd)
{
    char buf[MAXLINE];
    char key[MAXLINE], value[MAXLINE];
    int i,split;
    u_int64_t content_length = 0;

    
    
    while(strcmp(buf,"\r\n")) {
        split = -1;
        bzero(key, sizeof(key));
        bzero(value, sizeof(value));
        rio_readline(fd, buf, MAXLINE);

        if (!strcmp(buf,"\r\n")) {
            break;
        }
        
        for (i=0; i < strlen(buf); i++) {
            if (buf[i] == ' '){
                continue;    
            }
            else if (buf[i] == ':'){
                split = i;
                break;
            }            
        }
        if (split) {
            strncpy(key, buf, split);
            strncpy(value, buf+split+1, strlen(buf) - split-1);
            printf("Key : %s \t Value : %s \n", key, value);
            /* Content-Type: application/x-www-form-urlencoded */
            if (!strncasecmp(key,"Content-Type",MAXLINE)){
                printf("Content-Type in header\n");
            }
            /* Content-Length */
            else if (!strncasecmp(key,"Content-Length",MAXLINE)){
                printf("Content-Length in header\n");
                content_length = atol(value);
                printf("Content-Length:%d\n");
            }
            
        }
    }
    //rio_readline(fd, buf, MAXLINE);
    //printf("request body: %s\n", buf);

    return 1;
}

void do_post(int fd, char *uri)
{
    int is_static;
    struct stat sbuf;
    char filename[MAXLINE], cgiargs[MAXLINE];
    
    read_post_data(fd);

    /* 从POST request 中解析 URI  */
    is_static = parse_uri(uri, filename, cgiargs);
    if (stat(filename, &sbuf) < 0) {
        send_error(fd, filename, "404", "Not Found");
        return;
    }
    send_error(fd, filename, "403", "Forbidden");
    close(fd);
    return;
}

void send_error(int fd, char *cause, char *errnum, char *errmsg)
{
    char buf[MAXLINE], body[MAXLINE];

    /* 构建 HTTP response body */
    sprintf(body, "<html><title>toyws error</title>");
    sprintf(body, "%s<p>%s:%s</p></body></html>", body, errnum, errmsg);

    /* 构建 HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s \r\n", errnum, errmsg);
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    rio_writen(fd, buf, strlen(buf));
    rio_writen(fd, body, strlen(body));
    close(fd);
    return;
}

void read_request_headers(int fd)
{
    char buf[MAXLINE];

    while(strcmp(buf,"\r\n")) {
        rio_readline(fd, buf, MAXLINE);
        //printf("%s", buf);
    }
    return;
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
    char *ptr;

    /* Static */
    if (!strstr(uri, "cgi-bin")) {
        strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri) -1] == '/')
            strcat(filename, "index.html");
        return 1;
    }
    else {
        return 0;
    }
}

void get_file_type(char *filename, char *filetype)
{
    if (strstr(filename, ".html")) {
        strcpy(filetype, "text/html");
    }
    else {
        strcpy(filetype, "text/plain");
    }
}


void serve_static(int fd, char *filename, int filesize)
{
    int srcfd;
    char filetype[MAXLINE], buf[MAXLINE], *body;


    /* 发送 Response headers 到客户端 */
    get_file_type(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK \r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: toyws\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: %s\r\n", filetype);
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", filesize);
    rio_writen(fd, buf, strlen(buf));

    /* 发送Response body */
    srcfd = open(filename, 'r');

    body = (char *)malloc(filesize);
    /* 读取文件内容到body */
    while (rio_readline(srcfd, buf, MAXLINE)) {
        sprintf(body, "%s%s",body, buf);
    }    
    close(srcfd);
    rio_writen(fd, body, filesize);
}
