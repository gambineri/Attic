/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITX_TYPES_H__
#define __ITX_TYPES_H__

#ifndef NULL
#define NULL    0
#endif
/*
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
*/
#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#define ITXFAILED  -1

#define ISEQUAL(a,b)		(strcmp(a,b) == 0)
#define ISNULL(a)				(ISEQUAL(a, ""))
#define FREE(a)					{ free(a); a = NULL; }
#define ISVALIDPTR(a)   ( (a) != (void*)0 && (a) != (void*)1 )

#endif //__ITX_TYPES_H__