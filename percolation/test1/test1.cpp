#include <windows.h>
#include <stdio.h>

#define n   6
#define k   3


int GetFirstFreeBit(int v)
{
  int i = 0;
  
  while (!(v&(1<<i)))
    i++;

  while (v&(1<<i))
    i++;

  return i;
}


void main()
{
  int i = 0;
  int j = 0;
  int min = 0;
  int max = 0;
  int next = 0;
  int freebit = 0;

  DWORD beg = GetTickCount();

  for (i=0; i<k; i++)
    min |= (1<<i);

  max = min<<(n-k);
  
  printf("min = %d, max = %d\n", min, max);

  next = min;
  while (next<max)
  {
    freebit = GetFirstFreeBit(next);
    next |= (1<<freebit);
    next &= (~(1<<(freebit-1)));
    
    printf("next = %d\n", next);
  }

  DWORD end = GetTickCount();
  printf("milli = %d\n", end - beg);
}
