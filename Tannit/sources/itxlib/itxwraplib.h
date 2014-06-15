/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxwraplib.h,v $
 * $Revision: 1.1 $  
 * $Author: massimo $
 * $Date: 2002-06-10 15:04:34+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#ifndef __ITXWRAPLIB_H__
#define __ITXWRAPLIB_H__

//---------------
//itxString
//---------------

int   itxString_SubstituteSubString(char* base, char* what, char* with, char* retstr);
int   itxString_AdjustStr(char* base, char* retstr);
int   itxString_RemoveLeadingZeros(char* base, int zleft, char* retstr);
int   itxString_Capitalize(char* base, char* retstr);
int   itxString_RightInstr(char* base, char* search_arg, int start_from);
int   itxString_MaxTokenLen(char* base, char* sep);
int   itxString_UCase(char* str);
void  itxString_InBlinder(char** source, char blinder);
void  itxString_EscapeChars(char* source, char** destination);
int   itxString_CompareNoCase(char* s1, char* s2);
char* itxString_LTrim(char* str);
char* itxString_RTrim(char* str);
char* itxString_Trim(char* str);


#endif /* __ITXWRAPLIB_H__ */

