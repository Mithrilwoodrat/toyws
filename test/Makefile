CC= gcc

# Compiler flags
OPT=		-g
INCLUDES=	-I/usr/include/python2.7/
CFLAGS=		$(OPT)
CPPFLAGS=	$(INCLUDES)

# The Python library
LIBPYTHON=	/usr/lib/libpython2.7.so

LIBS=		-lev
SYSLIBS=	-lm

CCFLAGS= -std=gnu99 -O2 -Wall -g
ALLLIBS= $(LIBPYTHON) $(LIBS) $(SYSLIBS)

all: wsgi

wsgi: wsgi.c
	$(CC)  $(CCFLAGS) $^ $(ALLLIBS) -o $@

clean:
	rm -f wsgi
