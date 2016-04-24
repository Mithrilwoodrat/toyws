#ifndef TOYWS_H
#define TOYWS_H

#include <python2.7/Python.h>
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
#include <signal.h>
#include "rio.h"
#include "request.h"

#include <ev.h>
typedef struct sockaddr SA;


/*
 *  根据 nginx 文档 http://nginx.org/en/docs/http/ngx_http_core_module.html#large_client_header_buffers
 * 
 */
#define	MAXLINE	 8192  /* Max text line length */
#define MAXBUF   8192  /* Max I/O buffer size */
#define LISTENQ  1024  /* Second argument to listen() */

void on_request(struct ev_loop *loop, struct ev_io *watcher, int revents);
void on_read(struct ev_loop *loop, struct ev_io *watcher, int revents);
void on_write(struct ev_loop *loop, struct ev_io *watcher, int revents);

void serve(int fd);
void read_request_headers(int fd);
void send_error(int fd, char *cause, char *errnum, char *errmsg);
void serve_static(int fd, char *filename, int filesise);
void serve_wsgi(int fd, char *uri);
int parse_uri(char *uri, char *filename, char *cgiargs);
void do_get(int fd, char *uri);
void do_head(int fd, char *uri);
int read_post_data(int fd);
void do_post(int fd, char *uri);
void get_file_type(char *filename, char *filetype);
void signal_handler(int sig);

#define DEBUG 1 /* is debug mode open */

#ifdef DEBUG
#include <sys/param.h>
static void
call_realpath (char * path)
{
    char resolved_path[PATH_MAX];

    if (realpath (path, resolved_path) == 0) { 
        fprintf (stderr, "realpath failed: %s\n", strerror (errno));
    } else {
        printf ("Program's full path is '%s'\n", resolved_path);
    }
}

static void
set_unblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if(fcntl(fd, F_SETFL, (flags < 0 ? 0 : flags) | O_NONBLOCK) == -1) {
        perror("Could not set nonblock");
        return;
    }
}
#endif

#define WSGI 1

typedef struct
{
    PyObject* status; /* string */
    PyObject* headers; /* list */
    PyObject* body; /* string */
} Response;

static char *py_app = "app";
static char *py_module = "app";

#endif
