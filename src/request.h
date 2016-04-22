#ifndef REQUEST_H
#define REQUEST_H

#include <ev.h>
#include <stdlib.h>

#define	MAXLINE	 8192  /* Max text line length */

typedef struct {
    char method[MAXLINE];
    char uri[MAXLINE];
    char version[MAXLINE];
}RequestLine;

#define MAXLEN 100
typedef struct header_field {
    char key[MAXLEN];
    char value[MAXLEN];
    struct header_field *next;
}HeaderField;

typedef struct {
    ssize_t linenums;
    HeaderField head;
}RequestHeader;

#define MAXBUF 8192
typedef struct {
    ssize_t content_length;
    char _body[MAXBUF];
}RequestBody;



typedef struct {
    RequestLine start_line;
    RequestHeader header;
    RequestBody body;
}request_msg;



typedef struct request {
    ev_io io;
    RequestLine request_line;
}Request;

#endif



















