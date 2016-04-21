#ifndef REQUEST_H
#define REQUEST_H

#include "toyws.h"


#define MAXLEN 100
#define URLLEN 1000

typedef struct request_line {
    char method[MAXLEN];
    char request_uri[URLLEN];
    char http_version[MAXLEN];
}request_line;

typedef struct header_field {
    char key[MAXLEN];
    char value[MAXLEN];
    struct header_field *next;
}header_field;

typedef struct requst_header {
    ssize_t linenums;
    header_field head;
}request_header;

#define MAXBODYLEN 1000
typedef struct request_body {
    ssize_t content_length;
    char _body[MAXBODYLEN];
}request_body;



typedef struct request_msg{
    request_line start_line;
    request_header header;
    request_body body;
}requst_msg;

#endif



















