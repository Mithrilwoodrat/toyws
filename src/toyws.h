#ifndef TOYWS_H
#define TOYWS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "rio.h"

typedef struct sockaddr SA;


/*
 *  根据 nginx 文档 http://nginx.org/en/docs/http/ngx_http_core_module.html#large_client_header_buffers
 * 
 */
#define	MAXLINE	 8192  /* Max text line length */
#define MAXBUF   8192  /* Max I/O buffer size */
#define LISTENQ  1024  /* Second argument to listen() */


void serve(int fd);
void read_request_headers(int fd);
void send_error(int fd, char *cause, char *errnum, char *errmsg);
void serve_static(int fd, char *filename, int filesise);
int parse_uri(char *uri, char *filename, char *cgiargs);
void do_get(int fd, char *uri);
void do_head(int fd, char *uri);
int read_post_data(int fd);
void do_post(int fd, char *uri);
void get_file_type(char *filename, char *filetype);
void signal_handler(int sig);

#define DEBUG 1 /* is debug mode open */

#endif
