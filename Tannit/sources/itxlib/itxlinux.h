/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxlinux.h,v $
 * $Revision: 1.6 $  
 * $Author: administrator $
 * $Date: 2002-07-23 17:44:46+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */



#ifndef __ITXLINUX_H__
#define __ITXLINUX_H__

#ifdef LINUX

/*  -------------------------------------------------------------
 *  This file is the main header containing all includes, defines
 *  and other definitions needed to implement the Linux version 
 *  of the itxSystem object.
 *  -------------------------------------------------------------
 */


#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <strstream.h>
#include <errno.h>
#include <ctype.h>

// At moment, we don't know if exists a version od ODBC 
// drivers for this platform which implements the standard 3.0, 
// so we must define the following to force code in itxsql 
// to run with 1.0 version.
#define USE_ODBC_10


// Dummy value here
#define _O_BINARY           1 


typedef int                 SOCKET;
typedef sockaddr_in         SOCKADDR_IN;
typedef sockaddr*           LPSOCKADDR;


#define ITXCDECL //this must be left blank because _cdecl does not exists.

#define PATH_SEPARATOR_C    '/'
#define PATH_SEPARATOR      "/"
#define INVALID_SOCKET      (-1)
#define SOCKET_ERROR        (-1)





#endif  //LINUX

#endif  //__ITXLINUX_H__


