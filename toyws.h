#ifndef TOYWS_H
#define TOYWS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "rio.h"

typedef struct sockaddr SA;


/*
 *  根据 nginx 文档 http://nginx.org/en/docs/http/ngx_http_core_module.html#large_client_header_buffers
 * 
 */
#define	MAXLINE	 8192  /* Max text line length */
#define MAXBUF   8192  /* Max I/O buffer size */
#define LISTENQ  1024  /* Second argument to listen() */

#endif
