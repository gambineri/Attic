/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __TNT_CRYPT_H__
#define __TNT_CRYPT_H__

int  BuildCryptUserLabel(unsigned char* usercode, unsigned char* userlabel);
long VerifyCryptUserLabel(unsigned char* usercode, unsigned char* userlabel);
void tannitEncrypt(char* source, char* destination);
void tannitDecrypt(char* source, char* destination);

#endif //__TNT_CRYPT_H__
