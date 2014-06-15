# Makefile for itxlib
# Copyright (C) 2000 aitecsa s.r.l.

CC=g++

CFLAGS=-O 
CPPFLAGS=-DLINUX

LDFLAGS=-L. -lz
LDSHARED=$(CC)

VER=1.0
LIBS=libitx.a
SHAREDLIB=libitx.so

AR=ar rc
RANLIB=ranlib
TAR=tar
SHELL=/bin/bash

prefix = $HOME
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include



OBJS = itxstring.o itxcoll.o itxsocket.o itxsmtp.o itxhttp.o itxbuffer.o \
       itxfileini.o itxthread.o itxtime.o itxwin32.o itxlinux.o itxsql.o itxlib.o

TEST_OBJS = test.o


#all: test

#test: all
#	@LD_LIBRARY_PATH=.:$(LD_LIBRARY_PATH) ; export LD_LIBRARY_PATH; \
#	if ./test; then \
#	  echo '		*** itxlib test OK ***'; \
#	else \
#	  echo '		*** itxlib test FAILED ***'; \
#	fi

libitx.a: $(OBJS)
	$(AR) $@ $(OBJS)
	-@ ($(RANLIB) $@ || true) >/dev/null 2>&1

$(SHAREDLIB).$(VER): $(OBJS)
	$(LDSHARED) -o $@ $(OBJS)
	rm -f $(SHAREDLIB) $(SHAREDLIB).1
	ln -s $@ $(SHAREDLIB)
	ln -s $@ $(SHAREDLIB).1

test: test.o $(LIBS)
	$(CC) $(CFLAGS) -o $@ test.o $(LDFLAGS)


install: $(LIBS)
	-@if [ ! -d $(includedir)  ]; then mkdir $(includedir); fi
	-@if [ ! -d $(libdir) ]; then mkdir $(libdir); fi
	cp itxstring.h itxcoll.h itxlib.h $(includedir)
	chmod 644 $(includedir)/itxlib.h $(includedir)/itxstring.h \
            $(includedir)/itxcoll.h
	cp $(LIBS) $(libdir)
	cd $(libdir); chmod 755 $(LIBS)
	-@(cd $(libdir); $(RANLIB) libitx.a || true) >/dev/null 2>&1
	cd $(libdir); if test -f $(SHAREDLIB).$(VER); then \
	  rm -f $(SHAREDLIB) $(SHAREDLIB).1; \
	  ln -s $(SHAREDLIB).$(VER) $(SHAREDLIB); \
	  ln -s $(SHAREDLIB).$(VER) $(SHAREDLIB).1; \
	  (ldconfig || true)  >/dev/null 2>&1; \
	fi
# The ranlib in install is needed on NeXTSTEP which checks file times
# ldconfig is for Linux

uninstall:
	cd $(includedir); \
	v=$(VER); \
	if test -f itxlib.h; then \
	  v=`sed -n '/VERSION "/s/.*"\(.*\)".*/\1/p' < itxlib.h`; \
          rm -f itxlib.h itxcoll.h itxstring.h; \
	fi; \
	cd $(libdir); rm -f libitx.a; \
	if test -f $(SHAREDLIB).$$v; then \
	  rm -f $(SHAREDLIB).$$v $(SHAREDLIB) $(SHAREDLIB).1; \
	fi

clean:
	rm -f *.o *~ test libitx.a libitx.so*

distclean:	clean

depend:
	makedepend -- $(CFLAGS)  -- *.[ch]

# DO NOT DELETE THIS LINE -- make depend depends on it.





