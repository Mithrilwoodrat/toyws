#include "rio.h"

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while(nleft > 0)
    {
        if ((nread = read(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0; /* 被中断后重启*/
            else
                return -1;
        }
        else if (nread == 0)
            break;
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while(nleft > 0)
    {
        if ((nwritten = write(fd, bufp, nleft)) <= 0)
        {
            if (errno == EINTR)
                nwritten = 0; /* 被中断后重启*/
            else
                return -1;
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}

ssize_t rio_readline(int fd, void *usrbuf, size_t maxlen)
{
    int i, rc;
    char c, *bufp = usrbuf;

    for (i = 1; i < maxlen; i++)
    {
        if ((rc = rio_readn(fd, &c, 1)) == 1)
        {
            *bufp++ = c;
            if (c == '\n')
                break;
        }
        else if (rc == 0)
        {
            if (i == 1)
                return 0; /* EOF 没有读到数据*/
            else
                break; /*EOF 读到一些数据*/
        }
        else
            return -1;
    }
    *bufp = 0;
    return i;
}

