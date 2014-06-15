DEVUSERHOME = /Amarone/Progetti/itxSoftware/Web
ITXLIB = /Amarone/Progetti/itxSoftware/Librerie/itxlib

TANNITINCLUDE = $(DEVUSERHOME)/Tannit/include
ITXLIBINCLUDE = $(ITXLIB)
TANNITSOURCE = $(DEVUSERHOME)/Tannit/sources/Ocore
ODBCINCLUDE = /usr/local/include
ODBCLIB = /usr/local/lib

CC=g++
CFLAGS=-O

sources=auxfile.cpp cgiresolver.cpp commands.cpp commands2.cpp desx.cpp parser.cpp \
        tannitobj.cpp templatefile.cpp tntapiimpl.cpp tqr.cpp tqrodbc.cpp

objects=auxfile.o cgiresolver.o commands.o commands2.o desx.o parser.o tannitobj.o \
        templatefile.o tntapiimpl.o tqr.o tqrodbc.o

tannit: $(objects) 
	$(CC) $(CFLAGS) -o $@ $(objects) -litx -lodbc -L$(ITXLIB) -L$(ODBCLIB)

$(objects): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -DLINUX -DMAINT_1 -I$(TANNITINCLUDE) -I$(ITXLIBINCLUDE) 

clean : 
	rm $(TANNITSOURCE)/*.o; \
	touch *.h *.cpp

xw:
	chmod -x *.h; \
	chmod -x *.cpp; \
	chmod +w *.h; \
	chmod +w *.cpp

debug:
	$(TANNITINCLUDE) \
	$(ITXLIB) \
	$(ITXLIBINCLUDE) \
	$(TANNITSOURCE) \
	$(ODBCINCLUDE) \
	$(ODBCLIB)

depend:
	makedepend -I$(TANNITINCLUDE) -I$(ODBCINCLUDE) \
               -I$(ITXLIBINCLUDE) $(TANNITSOURCE)  -- $(CFLAGS) -- $(TANNITSOURCE)/*.[cpph]

# DO NOT DELETE THIS LINE -- make depend depends on it.

