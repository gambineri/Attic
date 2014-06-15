/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxlib.cpp,v $
 * $Revision: 1.3 $  
 * $Author: massimo $
 * $Date: 2002-06-07 11:08:26+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include "itxlib.h"

char* itxlibVer()
{
  static char a[128];
  sprintf(a, "%s%s%s", "AITECSA S.r.l. - All rights reserved.\nThis message from itxlib.lib, version ~ ", CURRENT_VERSION, " ~.");

  return a;
}
