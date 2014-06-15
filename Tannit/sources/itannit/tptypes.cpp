/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

// AITECSA s.r.l.   1999
//    filename:     tptypes.cpp
//    description:  itannit AS400 interface library
//                  AS400 transaction program definition
//    project:      ASWEB

#ifndef __ITX_TP_TYPES_CPP__
#define __ITX_TP_TYPES_CPP__
#endif

#include <stdio.h>
#include <string.h>

#include "itxtypes.h"
#include "tptypes.h"

TPAS400 tpAS400[NUM_TPAS400] =
{
  { TPAS400_UNDEFINED, "ITX", 0 },
  { A01, "A01", 45 },
  { A02, "A02", 11 },
  { A03, "A03", 19 },
  { A04, "A04",  9 },
  { A05, "A05",  5 },
  { A06, "A06", 10 },
  { A07, "A07", 21 },
  { A08, "A08",  4 },
  { A09, "A09", 28 },
  { A10, "A10", 17 },
  { A11, "A11", 13 },
  { A12, "A12", 13 },
  { A20, "A20", 28 },
  { A23, "A23", 16 },
  { A24, "A24",  3 },
  { A25, "A25", 16 },
  { A26, "A26", 14 },
  { A27, "A27", 14 },
  { A30, "A30", 12 },
  { A31, "A31", 12 },
  { A32, "A32", 12 },
  { A33, "A33", 12 },
  { A34, "A34", 12 },
  { A35, "A35", 12 },
  { A36, "A36", 12 },
  { A37, "A37", 12 },
  { A38, "A38", 12 },
  { A40, "A40",  3 },
  { A50, "A50", 33 },
  { A52, "A52", 18 },
  { A53, "A53",  6 },
  { A54, "A54", 16 },
  { A60, "A60", 21 },
  { A70, "A70",  8 },
  { A75, "A75", 12 },
  { A80, "A80",  3 },
  { A82, "A82",  3 },
  { A90, "A90",  7 },
  { A94, "A94",  4 },
  { A95, "A95", 12 },
  { A96, "A96",  7 },
  { A97, "A97", 17 },
  { C00, "C00",  2 },
  { C01, "C01",  4 },
  { C02, "C02",  3 },
  { C03, "C03",  3 },
  { E00, "E00",  5 },
  { E11, "E11",  5 },
  { H00, "H00", 51 },
  { H01, "H01", 28 },
  { H04, "H04",  2 },
  { H06, "H06", 15 },
  { H08, "H08",  7 },
  { H14, "H14",  3 },
  { H15, "H15", 15 },
  { H16, "H16",  2 },
  { H20, "H20",  2 },
  { H23, "H23",  2 },
  { H25, "H25", 15 },
  { H27, "H27",  2 },
  { H33, "H33",  6 },
  { H61, "H61", 17 },
  { H62, "H62", 25 },
  { H63, "H63", 15 },
  { H70, "H70",  2 },
  { H71, "H71", 12 },
  { H72, "H72", 12 },
  { H7L, "H7L", 13 },
  { H82, "H82", 13 },
  { HAI, "HAI",  9 },
  { HBA, "HBA",  9 },
  { HBB, "HBB",  9 },
  { HCD, "HCD",  9 },
  { HCM, "HCM", 11 },
  { HCN, "HCN", 11 },
  { HG0, "HG0",  9 },
  { HG1, "HG1",  9 },
  { HG2, "HG2",  9 },
  { HG3, "HG3",  9 },
  { HG4, "HG4",  9 },
  { HG5, "HG5",  9 },
  { HG6, "HG6",  9 },
  { HG7, "HG7",  9 },
  { HG8, "HG8",  9 },
  { HG9, "HG9",  9 },
  { HL4, "HL4", 21 },
  { HLA, "HLA",  9 },
  { HLB, "HLB",  9 },
  { HOK, "HOK",  1 },
  { HPI, "HPI",  9 },
  { HPO, "HPO", 14 },
  { HPT, "HPT",  9 },
  { HR4, "HR4", 12 },
  { HR6, "HR6", 18 },
  { HR7, "HR7", 12 },
  { HR8, "HR8",  9 },
  { HRA, "HRA",  9 }, 
  { HRF, "HRF", 10 },
  { HRI, "HRI",  9 },
  { HSC, "HSC",  4 },
  { HSH, "HSH",  9 },
  { HVA, "HVA",  9 },
  { HVO, "HVO",  9 },
  { P00, "P00", 46 },
  { P03, "P03", 13 },
  { P04, "P04",  3 },
  { P06, "P06", 18 },
  { P07, "P07",  6 },
  { P11, "P11",  3 },
  { P12, "P12",  3 },
  { P14, "P14", 12 },
  { P15, "P15", 18 },
  { P16, "P16",  2 },
  { P20, "P20",  1 },
  { P21, "P21",  7 },
  { P25, "P25", 18 },
  { P27, "P27",  3 }, 
  { P30, "P30",  3 },
  { P60, "P60",  5 },
  { P70, "P70", 15 },
  { PR4, "PR4",  3 }
};
  
  
TPAS400_ID GetTPID(char* name)
{
  int id = TPAS400_UNDEFINED;

  if (name != NULL)
  {
    while (strcmp(tpAS400[id].name, name) != 0 && id < NUM_TPAS400)
      id++;           
  }
  return (TPAS400_ID)id;
}

