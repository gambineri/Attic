/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxdefines.h,v $
 * $Revision: 1.5 $  
 * $Author: massimo $
 * $Date: 2002-06-24 13:38:56+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#ifndef __ITXDEFINES_H__
#define __ITXDEFINES_H__


#define GENERIC_CATCHALL    catch(...)            \
                            {                     \
                              throw;              \
                            }


#define CATCH_TO_NOTHING    catch(...){}


//  This macro sends generic exception message
#define CATCHALLMSG(msg) catch(...) {throw (msg);}

//  This macro returns the signed distance between 
//  the absolute value of two pointers
#define PTR_DISTANCE(p2, p1)  ((unsigned)(p2) - (unsigned)(p1))


//Memory
#define ITXALLOC(nbytes)    malloc((nbytes))
#define ITXFREE(ptr)        free((ptr))


//  This macro expands the internal buffer of one G. NO CHECKS PERFORMED!!!!
#define MALLOC(buf, endbuf, bufsize, G) { \
  try \
  { \
    char* temp; \
    if ((temp = (char*)ITXALLOC((bufsize) + (G))) != NULL) \
    { \
      if ((buf) != NULL) \
      { \
        memcpy(temp, (buf), (bufsize)); \
        (endbuf) = temp + ((unsigned)(endbuf) - (unsigned)(buf)); \
        ITXFREE(buf); \
      } \
      else \
        (endbuf) = temp; \
      (buf) = temp; \
      (bufsize) += (G); \
    } \
  } \
  catch(...){throw;}}


/* 
    The user of this macro MUST compare current m_Bufsize with needed new 
    dimension before calling this. The algorithm computes the number of 
    G packs needed (at least) to extend the actual buffer.

    XALLOC(buf, endbuf, bufsize, G, size)
    {      
      saveG = G;
      G = ((size - bufsize) / G) + 1) * (G);
      MALLOC(buf, endbuf, bufsize, G);
      G = saveG;
    }
*/

#define XALLOC(buf, endbuf, bufsize, G, size) \
{ \
  int saveG = (int)(G); \
  (G) = ((int)(((size) - (bufsize)) / (G)) + 1) * (G); \
  MALLOC((buf), (endbuf), (bufsize), (G)); \
  (G) = saveG; \
}

#endif  /* __ITXDEFINES_H__ */



