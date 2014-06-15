/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

// test.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <sys\timeb.h>

#include "..\itxlib.h"
#include "..\itxwraplib.h"

#include "..\itxbuffer.h"


#define NALPHABET       256
#define NTEST           1000
#define GRANULARITY     10


void Time_Trace()
{
#ifdef WIN32
  struct _timeb timebuffer;
  _ftime(&timebuffer);
#endif

#ifdef LINUX
  struct timeb timebuffer;   
  ftime(&timebuffer);
#endif

  char*         timeline;
  timeline = ctime(&timebuffer.time);

  printf("TIME IS: %.19s.%hu %s", timeline, timebuffer.millitm, &timeline[20]);
}


void SetArray(itxString* istrarr)
{
  srand(time(NULL));
  for (int cicli=0; cicli<NTEST; cicli++)
  {
    int clearlen;
    while((clearlen = rand() % 40) == 0);
 
    char* clear = (char*)malloc(clearlen + 1);
    clear[clearlen] = '\0';
    
    int c;
    for(int i=0; i<clearlen; i++)
    {
      c = rand() % NALPHABET;
      if (c != '\0') 
        clear[i] = c;
      else
        clear[i] = '?';
    }
    
    istrarr[cicli] = clear;

    free(clear);
  }
}

int TestMassivo()
{
  itxString istr("abcd");
  itxString istr2;

//  istr2 = istr;  
//  istr2.Copy(istr);

  istr2 = "";
  printf("%d\n", istr2.IsNull());
  printf("%d\n", istr2.IsEmpty());
  printf("%d\n", istr2.Len());

  istr2 = "";
  printf("%d\n", istr2.IsNull());
  printf("%d\n", istr2.IsEmpty());
  printf("%d\n", istr2.Len());

  istr2 = "pippo";
  printf("%d\n", istr2.IsNull());
  printf("%d\n", istr2.IsEmpty());
  printf("%d\n", istr2.Len());

  istr2 = "La fregna regna";
  printf("%d\n", istr2.IsNull());
  printf("%d\n", istr2.IsEmpty());
  printf("%d\n", istr2.Len());

  printf("%s\n", istr2);

  itxString* pstr = new itxString(istr2); 
  delete pstr;

  pstr = new itxString("zs;fc jaes[rva "); 
  delete pstr;

//Test massivo  
  itxString istrarr[NTEST];
  
  for (int z=0; z<NTEST; z++)
    SetArray(istrarr);
/*
  for (cicli=0; cicli<NTEST; cicli++)
    printf("%s\n", istrarr[cicli].GetBuffer());
*/
	return 0;
}

int Aux1()
{
  itxString a("L'anima dell'aquila e' immor(t)ale");
  a.AdjustStr();
  printf("%s\n", a.GetBuffer());

  itxString b("");
  char* base = "a   a   a   a   a   ";
  char* what = "   ";
  char* with = "BBBBB";

  b = base;
  int n = b.SubstituteSubString(what, with);
  
  b = "Ma dico veramente veramente?";
  n = b.SubstituteSubString(" ", "\0");
  
  b = "0000pippo";
	n = b.RemoveLeadingZeros(2); //deve tornare la stringa '00pippo'
  
  b = "pippo";
  b.Capitalize();
  
  b = "123456789\\lib";
  n = b.RightInstr("\\");
  
  b = "123 12345 12345678  1 1234567 123445 123 ";
  n = b.MaxTokenLen("\\");
  
  b = "pippo";
  b.UCase();

  b = "#porco il frio#";
  b.InBlinder('#');

  b = "aaa�bbb";
  b.EscapeChars();

  a = "cico";
  b = "cicoria";
  if (a.CompareNoCase(&b))
    printf("iquali\n");
  else
    printf("difers\n");

  a = "123456789";
  a.Left(6);

  a = "123456789";
  a.Left(0);

  a = "123456789";
  a.Left(-1);

  a = "123456789";
  a.Left(15);

  a = "123456789";
  a.Right(6);

  a = "123456789";
  a.Right(0);

  a = "123456789";
  a.Right(-1);

  a = "123456789";
  a.Right(15);
 
  return 0;
}

int Aux2()
{
  char a[50];

  strcpy(a, "   123   ");
  itxString_RTrim(a);
  printf(">%s<\n", a);

  itxString istr("   123   ");
  istr.LTrim();

  istr = "   123   ";
  istr.RTrim();

  istr = "   123   ";
  istr.Trim();

  return 0;
}


int Aux3()
{
  itxString a = "aite";
  itxString b = "csa";
  a += b;
  a += " la benemerita";

  printf(">%s<\n", a.GetBuffer());


  a = "";
  char z[10];
  memset(z, 'A', 10);
  a.Strncpy(z, 5);

  return 0;
}

int Aux4()
{
  itxString a = "aaa";
  itxString b = "aa";

  printf("%d\n", a.Compare(&b));

  a.Space(10, 0);
  printf("<%s>\n", a.GetBuffer());
  
  a.Space(10);
  printf("<%s>\n", a.GetBuffer());

  itxString uno;

  uno = "aaaaaaa";
  printf("%s\n", uno.GetBuffer());
  uno.SetEmpty();
  printf("%s\n", uno.GetBuffer());

  char c = 'Y';
  a += c;
  printf("%s\n", a.GetBuffer());

  a = "";
  int s = 1234567899;
  a.SetInt(s);
  printf("<%s>\n", a.GetBuffer());
    
  return 0;
}


void Aux5()
{
  itxString a;

  char* p = "abc";
  char* aux = p;
  aux++;

  a += *aux;
}

int Aux6()
{
  itxStack  stak;
  void*     appo;

  char* a = "aaa\n";
  char* b = "bbb\n";
  char* c = "ccc\n";
  char* d = "ddd\n";

  stak.Push(a);
  stak.Push(b);
  stak.Push(c);
  stak.Push(d);

  printf("---------------------------------------------------------------\n");

  stak.SetCursorBottom();
  printf("Bottom is %s\n", (char*)stak.Bottom());
  
  while ((appo = stak.NextUp()) != NULL)
    printf("Current is %s\n", (char*)appo);

  printf("---------------------------------------------------------------\n");

  stak.SetCursorTop();
  printf("Top is %s\n", (char*)stak.Top());
  
  while ((appo = stak.NextDown()) != NULL)
    printf("Current is %s\n", (char*)appo);

  printf("---------------------------------------------------------------\n");

  printf("Top is %s\n", (char*)stak.Top());
  
  char* out;
  stak.Pop((void**)&out);
  printf("%s", out);
  stak.Pop((void**)&out);
  printf("%s", out);
  stak.Pop((void**)&out);
  printf("%s", out);
  stak.Pop((void**)&out);
  printf("%s", out);
  
  return 0;
}


void Aux7()
{
  DWORD         start, stop;
  int           G = 5000;
  int           l = 0;
  itxString     istr(G);
  itxString     appo((char*)NULL);
  itxString     appo2("");

  for (int j=10000; j<= 50000; j+=10000)
  {
    istr.SetEmpty();
    start = timeGetTime();

    for (int i=0; i<=j; i++)
      istr += "a";

    stop = timeGetTime();

    float f = (stop - start) / (float)1000;
    printf("Elapsed time to reach %d size : %f seconds.\n", j, f);
  }

//  printf("%s", istr.GetBuffer());

  printf("JOB DONE\n");
}


void Aux8()
{
  itxString a;

  a = "I nostri ragazzi imparano a uccidere...";

  int x = a.FindSubString(" I nostri");

  char* s = "3477";
  char  d[20];
  a.HexToAscii(s, d, 4);

  a.SetInt(5);
  printf("SetInt test: %s\n", a.GetBuffer());

  a.SetInt(1234567890);
  printf("SetInt test: %s\n", a.GetBuffer());
}

void Aux9()
{
  itxString a;

  a = "123456789";
  a = "1234567890";
  
  a.Space(20);

  a = "123456789\n0123456789";
  printf("BEFORE PurgeSubString: %s\n", a.GetBuffer());
  char c = 0xA;
  a.PurgeSubString(&c);
  printf("AFTER PurgeSubString test: %s\n", a.GetBuffer());

}

void Aux10()
{
  itxListPtr list;
  int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int i = 0;
  int* px;

  for (i=0; i<10; i++)
    list.Append(&a[i]);

  printf("------- HEAD - TAIL ---------------------\n");
  px = (int*)list.GetHead();
  while(px)
  {
    printf("~~~~~~~~~~~ %d\n", *px);
    px = (int*)list.GetNext();
  }

  printf("------- TAIL - HEAD ---------------------\n");
  px = (int*)list.GetTail();
  while(px)
  {
    printf("~~~~~~~~~~~ %d\n", *px);
    px = (int*)list.GetPrev();
  }

  printf("-----------------------------------------\n");

  for (i=0; i<list.GetCount(); i++)
  {
    px = (int*)list.GetElement(i);
    printf("valore %d = %d\n", i, *px);
  }

  printf("---------------------------------\n");

  int newint = 20;
  list.Insert(&newint, 5);
  list.Insert(&newint, 0);
  list.Insert(&newint, list.GetCount());
  list.Insert(&newint, 100);

  for (i=0; i<list.GetCount(); i++)
  {
    px = (int*)list.GetElement(i);
    printf("valore %d = %d\n", i, *px);
  }

  printf("---------------------------------\n");

  list.Remove(0);
  list.Remove(5);
  list.Remove(list.GetCount());
  list.Remove(100);

  for (i=0; i<list.GetCount(); i++)
  {
    px = (int*)list.GetElement(i);
    printf("valore %d = %d\n", i, *px);
  }
  
  list.Empty();

  itxString boh("rizla");
  printf("GetAt ---> %d -> %c\n", boh.GetAt(-1), boh.GetAt(-1));
  printf("GetAt ---> %d -> %c\n", boh.GetAt(0) , boh.GetAt(0) );
  printf("GetAt ---> %d -> %c\n", boh.GetAt(1) , boh.GetAt(1) );
  printf("GetAt ---> %d -> %c\n", boh.GetAt(2) , boh.GetAt(2) );
  printf("GetAt ---> %d -> %c\n", boh.GetAt(3) , boh.GetAt(3) );
  printf("GetAt ---> %d -> %c\n", boh.GetAt(4) , boh.GetAt(4) );
  printf("GetAt ---> %d -> %c\n", boh.GetAt(5) , boh.GetAt(5) );
  printf("GetAt ---> %d -> %c\n", boh.GetAt(6) , boh.GetAt(6) );
}

void Aux11()
{
  char a[5]  = {'a', 'b', 'c', 'd', 'e'};
  char b[10] = {'f', 'g', 'h', 'i', 'l', 'm', 'n', 'o', 'p', 'q'};
  char c[3] = {'1', '2', '3'};

  itxBuffer buf;
  buf.Append(a, 5);
  buf.Append(b, 10);

  itxBuffer buf2;
  buf2.SetGranularity(20);
  buf2.Append(a, 5);
  buf2.Append(b, 10);

  itxBuffer buf3;
  buf3 = buf2;

  int len = buf3.Len();
  buf3.SetBuffer(a, 5);
  len = buf3.Len();

  buf3 += buf;
  len = buf3.Len();

  bool isemp = buf.IsEmpty();
  buf.SetEmpty();
  isemp = buf.IsEmpty();

  buf3.Space(30, 'T');
  buf3.Space(50);
  sprintf(buf3.GetBuffer(), "fruzzica o no fruzzica? 3x2=%d", 6);

  itxBuffer buf4;
  buf4.SetBuffer(b, 10);
  buf4.InsAt(c, 3, 5);

  itxBuffer buf5;
  buf5.Space(10);
  buf5.Append("abcd", 4);
  char* p = buf5.GetBuffer() + buf5.Len();
  strncpy(p, "efg", 3);
  buf5.UpdateCursor(buf5.Len() + 3);
}

void Aux12()
{
  itxSQLConnection m_Conn;

  if (!m_Conn.Create("itxscratch", "sa", ""))
  { 
    printf("create failed\n");
    return;
  }

  if (!m_Conn.Execute("SELECT blob, descr FROM TABLE2"))
  {
    printf("execute failed\n");
    return;
  }

  itxBuffer out;
  m_Conn.Fetch();
  m_Conn.GetBinData(1, out);

  FILE* f = fopen("outimage.jpg", "wb");
  fwrite(out.GetBuffer(), out.Len(), 1, f);
  fclose(f);
}

//---------------------- test threads and http ------------
class TT : public itxThread
{
public:
  itxString m_URL;
  itxString m_DATA;
  int       m_Idx;

  void Set(char* url, char* data, int idx){m_URL = url; m_DATA = data; m_Idx = idx;}
  TT(){}
  ~TT(){}

  void Run()
  {
    try
    {
      itxHttp http;
      http.POST(m_URL.GetBuffer(), m_DATA.GetBuffer(), "text/plain", 4000, true);
      printf("\n>%d===%s===\n", m_Idx, http.GetDataReceived());
    }
    catch(itxException* e)
    { 
      if (e != NULL)
        printf("Catched itxException '%s' while requesting %s\n", e->m_procedure, m_URL);
    }
    catch(...)
    {
      printf("Catched error while requesting %s\n", m_URL);
    }
  }
};

#define N 50

void Aux13()
{
  TT pT[N];

/*
  itxHttp http;
  http.POST("www.Services.ort.fr/Bignet/xmledi-1/access", "UNB+UNOA:1+0171+0114+020626:1026+017110262001++TEST'UNH+017110262001+01:001'RQF+MAX:50:01'NAD+BSN+PO460579++DANIELA RIGHI++PRATO+++IT'UNT+4+017110262001'UNZ+1+017110262001'", "text/plain", 4000, true);
  printf("===%s===\n", http.GetDataReceived());
  return;
//*/
  for (int i=0; i<N; i++)  
  {
    pT[i].Set("www.Services.ort.fr/Bignet/xmledi-1/access", "UNB+UNOA:1+0114+0171+020626:1026+017110262001++TEST'UNH+017110262001+01:001'RQF+MAX:50:01'NAD+BSN+PO460579++DANIELA RIGHI++PRATO+++IT'UNT+4+017110262001'UNZ+1+017110262001'", i);
//    pT[i].Set("cannonau/cgi-bin/tannit.exe", "", i); //"prm=dbg&tpl=now");
    pT[i].Start();
  }

  printf("Launched %d threads.\n", N);
  _getch();
}


//---------------------------------------------------------
//------------------- MAIN --------------------------------
//---------------------------------------------------------


int main()
{
/*
  TestMassivo();

  Aux1();
  Aux2();
  Aux3();
  Aux4();
  Aux5();
  Aux6();
  Aux7();
  Aux8();
  Aux9();
  Aux10();
  Aux11();
  Aux12();
*/

  Aux13();
    
  return 0;
}






