#ifndef REQUEST_H
#define REQUEST_H

#include "toyws.h"

typedef struct request_msg{
    request_line start_line;
    request_header header;
    request_body body;
}requst_msg;

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

#define MAXSIZE 100
typedef struct requst_header {
    header_field header_fields[MAXSIZE];
}
#endif

