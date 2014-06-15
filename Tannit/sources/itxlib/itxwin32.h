/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxwin32.h,v $
 * $Revision: 1.3 $  
 * $Author: massimo $
 * $Date: 2002-06-26 11:25:18+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */


#ifndef __ITXWIN32_H__
#define __ITXWIN32_H__

#ifdef WIN32

/*  -------------------------------------------------------------
 *  This file is the main header containing all includes, defines
 *  and other definitions needed to implement the WIN32 version 
 *  of the itxSystem object.
 *  -------------------------------------------------------------
 */

#include <windows.h>
#include <sys\timeb.h>

#include <stdio.h>     //This for _setmode
#include <mmsystem.h>
#include <fcntl.h>  //This for stream modes
#include <io.h>     //This for _setmode

#include <strstrea.h>

#ifndef _WINDOWS_
  #include <winsock2.h>
#endif


// C calling convention
#define ITXCDECL          _cdecl

#define PATH_SEPARATOR    "\\"
#define PATH_SEPARATOR_C  '\\'



#endif  //WIN32

#endif  //__ITXWIN32_H__