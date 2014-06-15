/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: tqr.cpp,v $
 * $Revision: 1.44 $  
 * $Author: massimo $
 * $Date: 2002-06-26 11:25:18+02 $
 *
 * Tannit Query Resultset Object and Manager Implementation
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */


#include "itxtypes.h"
#include "auxfile.h"
#include "tqr.h"


#define TNT_MAX_RECORD_LENGTH  8192
#define TNT_MAX_FIELD_LENGTH   256

//------------------------------------------------------------------------
//-------------------------- TQRCollection Section --------------------------
//------------------------------------------------------------------------

TQR* TQRCollection::CreateTQR(char* queryname, int numfields)
{
	TQR* qres = NULL;

  try
  {
	  qres = (TQR*) new TQR(queryname, numfields);
  	Store(NULL, qres);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRCollection::CreateTQR\n");
  }

	return qres;
}


// ritorna il puntatore al TQR di nome name
TQR* TQRCollection::Retrieve(char* name)
{
	TQR* qres = NULL;
	int icount = 0, i = 0;
	int qcount = m_qcount;

  try
  {
	  if (name != NULL && qcount > 0)
	  {
		  while (icount < qcount)
		  {
        if (m_qres[i] != NULL)
        {
			    if ((m_qres[i]->GetName()).Compare(name) == 0)
				    break;
			    icount++;
        }
        i++;
		  }

		  if (icount < qcount)
			  qres = m_qres[i];
	  }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRCollection::Retrieve\n");
    throw;
  }
	return qres;
}

// ritorna l'indice al TQR di nome name
int TQRCollection::Index(char* name)
{
	int icount = 0, i = 0;
	int qcount = m_qcount;

  try
  {
	  if (name != NULL && qcount > 0)
	  {
		  while (icount < qcount)
		  {
        if (m_qres[i] != NULL)
        {
			    if ((m_qres[i]->GetName()).Compare(name) == 0)
				    break;
			    icount++;
        }
        i++;
		  }
	  }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRCollection::Index\n");
    throw;
  }

	if (icount >= qcount)
		icount = ITXFAILED;

	return icount;
}

// torna TQR_NOT_EXIST oppure il numero di record presenti
int TQRCollection::Exist(char* name)
{
	TQR* qres = NULL;
	int icount = 0, i = 0;
	int qcount = m_qcount;

  try
  {
    if ((qres = Retrieve(name)) != NULL)
      return qres->totalRows;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRCollection::Exist\n");
    throw;
  }

	return TQR_NOT_EXIST;
}

// memorizza qres nella lista dei TQR gestiti e torna l'indice
int TQRCollection::Store(PTQRMng mng, TQR* qres)
{
  int i = 0;

  try
  {
	  while (m_qres[i] != NULL && i < QUERY_NUMBER)
      i++;

    if (m_qres[i] == NULL)
    {
      m_qres[i] = qres;
      m_mng[i] = mng;
      m_qcount++; 
      return i;
    }
    else
      return ITXFAILED;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRCollection::Store\n");
    return ITXFAILED;
  }
}

void TQRCollection::Remove(char* name)
{
  int i;

  TQR* qres;

  try
  {
    if ((i = Index(name)) != ITXFAILED)
    {
      m_qcount -= 1;
      
      qres = m_qres[i];
      delete qres;

      m_qres[i] = NULL;
      m_mng[i] = NULL;
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRCollection::Remove\n");
  }
}

char* TQRCollection::GetCurrentRecordField(char* name, char* colname)
{
  char* value = "";
  TQR* qres;

  try
  {
	  qres = Retrieve(name);
	  value = qres->GetCurrentRecordField(colname);
  }
  catch(...)
  {
    value = "";
//    if (qres == NULL)
//      DebugTrace2(IN_WARNING, "Cannot find a tqr named '%s'.\n", name);
  }

  return value;
}

void TQRCollection::SetCurrentRecordField(char* name, char* colname, char* colvalue)
{
  try
  {
	  TQR* qres = Retrieve(name);
	  qres->SetCurrentRecordField(colname, colvalue);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRCollection::SetCurrentRecordField\n");
  }
}

//------------------------------------------------------------------------
//-------------------------- TQRManager Section --------------------------
//------------------------------------------------------------------------

TQR* TQRManager::CreateTQR(char* queryname)
{
	TQR* qres = NULL;

  try
  {
	  qres = (TQR*) new TQR(queryname);
  	Store(qres);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRManager::CreateTQR\n");
  }

	return qres;
}

TQR* TQRManager::CreateTQR(char* queryname, int numfields)
{
	TQR* qres = NULL;

  try
  {
	  qres = (TQR*) new TQR(queryname, numfields);
  	Store(qres);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRManager::CreateTQR\n");
  }

	return qres;
}

void TQRManager::SetColumnAttributes(char* queryname, int colindex, char* colname, short coltype, long colsize)
{
	TQR* qres = NULL;

  try
  {
    qres = Get(queryname);
    qres->SetColumnAttributes(colindex, colname, coltype, colsize);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRManager::SetColumnAttributes\n");
  }
}

void TQRManager::SetColumnAttributes(TQR* qres, int colindex, char* colname, short coltype, long colsize)
{
  try
  {
    qres->SetColumnAttributes(colindex, colname, coltype, colsize);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRManager::SetColumnAttributes\n");
  }
}

void TQRManager::SetName(TQR* qres, char* name)
{
  try
  {
    qres->SetName(name);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRManager::SetName\n");
  }
}

TQR* TQRManager::Filter(char* source, char* field, char* value, char* destination)
{
  TQR* qres = NULL;
  TQR* qsrc = NULL;
  
  itxString filter;
  int irow = 0;

  try
  {
	  if ((qsrc = Get(source)) == NULL)
      return qres;

	  if ((qres = qsrc->Clone(destination)) != NULL)
	  {
  	  Store(qres);
      qsrc->MoveFirst();
      for (int irec = 0; irec < qsrc->totalRows; irec++)
      {
        filter = qsrc->GetCurrentRecordField(field);
        if (filter.Compare(value) == 0)
        {
          qres->AddTail();
          *(qres->current_record) = *(qsrc->current_record);
        }
        qsrc->MoveNext(); 
      }
      qres->MoveFirst();
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRManager::Filter\n");
  }
  return qres;
}

TQR* TQRManager::Sample(char* source, char* destination, int destMaxRecs, int seed)
{
  TQR* qres = NULL;
  TQR* qsrc = NULL;
  int * myArray;

  itxString filter;
  int irow = 0;
  int extracted = 0;
  int ii=0;
  int irec = 0;
  int buffer = 0;
  int qsrcStartingRow;

  try
  {
	  if ((qsrc = Get(source)) == NULL)
      return qres;

	  if (qsrc->totalRows <= destMaxRecs)
    {
      destMaxRecs = qsrc->totalRows;
//    *qres = *qsrc;
//    qres->SetName(destination);
//    return qres;
    }

    if ((qres = qsrc->Clone(destination)) != NULL)
	  {

      Store(qres);
      qsrcStartingRow=qsrc->GetActualRow();

      myArray = new int[qsrc->totalRows];
      for (int i = 0; i < qsrc->totalRows; i++)
      {
        myArray[i] = i+1;
      }
      srand((unsigned)seed);

      for (irec = 0; irec < destMaxRecs; irec++)
      {
        extracted = irec + rand() % (qsrc->totalRows - irec);
        DebugTrace2(DEFAULT, "myArray[0]-a----------------:%d\n", myArray[0]);
        DebugTrace2(DEFAULT, "myArray[%d]-----------------:%d\n", irec, myArray[irec]);
        DebugTrace2(DEFAULT, "extracted-----------------:%d\n", extracted);
        DebugTrace2(DEFAULT, "destMaxRecs-----------------:%d\n", destMaxRecs);
        buffer = myArray[irec];
        myArray[irec] = myArray[extracted];
        myArray[extracted] = buffer;

        DebugTrace2(DEFAULT, "myArray[0]-b----------------:%d\n", myArray[0]);
        ii = irec;
        while(ii > 0)
        {
          if (myArray[ii] < myArray[ii-1]) {
            buffer = myArray[ii-1];
            myArray[ii-1] = myArray[ii];
            myArray[ii] = buffer;
          }
          else
          {
            DebugTrace2(DEFAULT, "myArray[0]-c----------------:%d\n<br>", myArray[0]);
            break;
          }
          ii--;
        }
        DebugTrace2(DEFAULT, "myArray[0]-c1---------------:%d\n<br>", myArray[0]);
      }

      for (irec = 0; irec < destMaxRecs; irec++)
      {
        qsrc->MoveTo(myArray[irec]);
        qres->AddTail();
        *(qres->current_record) = *(qsrc->current_record);
        DebugTrace2(DEFAULT, "myArray[0]-----------------:%d\n", myArray[0]);
        DebugTrace2(DEFAULT, "______:%d-%s\n", myArray[irec], qres->GetCurrentRecordField("dscr"));
      }
//      DebugTrace2(DEFAULT, "______:totalrows  %d\n", qres->GetTotalRows());

      qsrc->MoveTo(qsrcStartingRow);
      qres->MoveFirst();
      delete [] myArray;

    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRManager::Sample\n");
  }

  return qres;
}

void TQRManager::LoadDataBuffer(char* tqrname, int tqrcols, char recsep, char fieldsep, char* buffer)
{
  istrstream str_buffer(buffer);
  char record[TNT_MAX_RECORD_LENGTH];
  TQR* qres = NULL;

	if ((qres = Get(tqrname)) == NULL)
    qres = CreateTQR(tqrname, tqrcols);

  qres->MoveLast();
  while(!str_buffer.eof())
  {    
    str_buffer.getline(record, TNT_MAX_RECORD_LENGTH, recsep);
		if (!ISNULL(record))
			qres->LoadRecordBuffer(record, fieldsep);
  }
  qres->MoveFirst();
}


//-----------------------------------------------------------------
//-------------------------- TQR Section --------------------------
//-----------------------------------------------------------------

TQR::TQR(char* queryname)
{
  try
  {
    id = queryname;
		colsNumber = 0;
		actualRow = -1;
	  startingRow = STARTING_ROW;
    rowsToStore = ROWS_TO_STORE;
	  totalRows = 0;
	  current_record = NULL;
	  recordshead = NULL;
	  recordstail = NULL;
    m_MoreDBRows = false;
    m_SourceRecordCount = 0;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::TQR\n");
  }
}

TQR::TQR(char* queryname, int numfields)
{
  int ifields = 0;
  char appo[10];

  try
  {
    id = queryname;
		colsNumber = numfields;
		actualRow = -1;
	  startingRow = STARTING_ROW;
    rowsToStore = ROWS_TO_STORE;
	  totalRows = 0;
	  current_record = NULL;
	  recordshead = NULL;
	  recordstail = NULL;
    m_MoreDBRows = false;
    m_SourceRecordCount = 0;

    queryHeader = (TQRColumnHeader*) new TQRColumnHeader[numfields]();

    ifields = 0;
    while (ifields < numfields)
    {
      sprintf(appo, "f%d\0", ifields + 1);
      queryHeader[ifields].name = appo;
      ifields++;
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::TQR %s\n", queryHeader[ifields].name.GetBuffer());
  }
}

TQR::~TQR()
{
  TQRRecord* delrecord;

  try
  {
    delete [] queryHeader;

    current_record = recordshead;
    while (current_record != NULL)
    {
      delrecord = current_record;
      current_record = current_record->m_next;
      delete delrecord;
    }    
  }
  catch(...)
  {
// fabio-->patch 30/05/2001
//    DebugTrace2(IN_ERROR, "TQR::~TQR\n");
  }
}

void TQR::operator=(TQR* src)
{
  try
  {
    id = src->id;
		colsNumber = src->colsNumber;
		actualRow = src->actualRow;
	  startingRow = src->startingRow;
    rowsToStore = src->rowsToStore;
	  totalRows = src->totalRows;

    queryHeader = (TQRColumnHeader*) new TQRColumnHeader[colsNumber]();
    for (int icol = 0; icol < colsNumber; icol ++)
      SetColumnAttributes(icol, src->queryHeader->name.GetBuffer(), 
                          src->queryHeader->sqlDataType, src->queryHeader->colummnSize);
    
    src->MoveFirst();
    for (int irec = 0; irec < totalRows; irec++)
    {
      AddTail();
      *(current_record) = *(src->current_record);
      src->MoveNext(); 
    }

  } 
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::operator=\n");
  }
}

TQR* TQR::Clone(char* clonename)
{
  TQR* qres = NULL;

  try
  {
	  qres = (TQR*) new TQR(clonename, colsNumber);

    qres->queryHeader = (TQRColumnHeader*) new TQRColumnHeader[colsNumber]();
    for (int icol = 0; icol < colsNumber; icol ++)
      qres->SetColumnAttributes(icol, queryHeader[icol].name.GetBuffer(), 
                          queryHeader[icol].sqlDataType, queryHeader[icol].colummnSize);    
  } 
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::Clone\n");
  }

  return qres;
}

void TQR::SetColumnAttributes(int colindex, char* colname, short coltype, long colsize)
{
  try
  {
    this->queryHeader[colindex].SetAttributes(colname, coltype, colsize);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::SetColumnAttributes %s\n", colname);
  }
}

// alloca spazio per un nuovo record e lo aggiunge in coda
bool TQR::AddTail()
{
	TQRRecord* new_record;

	try
  {
    if (totalRows == 0)
    {
      new_record = (TQRRecord*) new TQRRecord(colsNumber);
    
		  recordshead = new_record;
      recordstail = new_record;
      actualRow = 1;
    }
	  else
	  {
      new_record = (TQRRecord*) new TQRRecord(recordstail, colsNumber, NULL);

		  recordstail->m_next = new_record;
      recordstail = new_record;
      actualRow++;
	  }

    current_record = new_record;
    totalRows++;
    return true;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::AddTail\n");
    return false;
  }
}

void TQR::RemoveTail()
{
	try
  {
    if (recordstail == NULL) 
    {
      actualRow = -1;
      return;
    }

    current_record = recordstail->m_previous;
    delete recordstail;
    recordstail = current_record;
    if (recordstail != NULL)
      recordstail->m_next = NULL;
    else  
      recordshead = NULL;
  
    actualRow--;
    totalRows--;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::RemoveTail\n");
  }
}

// determina la posizione di una colonna di nome fieldname in base 0.
// il valore di ritorno ITXFAILED indica che non � stata trovata.
int TQR::GetColPos(char* colname)
{
  int icol = 0;

	try
  {
    while (icol < colsNumber)
    {
      if (queryHeader[icol].name.Compare(colname) == 0)
        break;
      icol++;
    }
    if (icol == colsNumber)
    {
  	  icol = ITXFAILED;
      DebugTrace2(IN_WARNING, "Cannot find a field named '%s'\n", colname);
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::GetColPos\n");
  }

  return icol;
}

void TQR::MoveTo(int rowindex)
{
  if (rowindex < 0)
    return;

  try
  {
    int irow = 1;
    MoveFirst();
    while (irow < rowindex && current_record != NULL)
    {
      MoveNext();
      irow++;
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::MoveTo\n");
  }
}

void TQR::LoadRecordBuffer(char* record, char fieldsep)
{
  istrstream str_REC(record);
  
  char stoken[TNT_MAX_FIELD_LENGTH];
  itxString token;

  bool record_head = TRUE;
	int field_count = 0;

  try
  {
    if (record != NULL)
      AddTail();

    // stores values presented in buffer
    while(!str_REC.eof())
    {
      str_REC.getline(stoken, TNT_MAX_FIELD_LENGTH, fieldsep);
    
      token = stoken;
      token.Trim();

		  if (field_count < colsNumber)
		  {
        SetCurrentRecordField(field_count, token.GetBuffer());
        field_count++;
		  }
	  }

    // stores values not presented in buffer
	  while (field_count < colsNumber)
	  {
		  {
        SetCurrentRecordField(field_count, token.GetBuffer());
        field_count++;
		  }
	  }

	  if (field_count != colsNumber)
	  {
		  RemoveTail();
		  DebugTrace2(DEFAULT, "TQR::LoadRecordBuffer %s\n", record);
      return;
	  }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQR::LoadRecordBuffer\n");
  }
}
  

//-----------------------------------------------------------------------------
//-------------------------- TQRColumnHeader Section --------------------------
//-----------------------------------------------------------------------------

// inizializza i valori della colonna 
void TQRColumnHeader::SetAttributes(char* colname, short coltype, long colsize)
{
  try
  {
    name = colname;
    sqlDataType = coltype;
    this->colummnSize = colsize;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRColumnHeader::SetAttributes\n");
  }
}

//-----------------------------------------------------------------------------
//-------------------------- TQRRecord Section --------------------------
//-----------------------------------------------------------------------------

TQRRecord::TQRRecord(int colnum)
{
  try
  {
    m_previous = NULL;
	  m_row = (itxString*) new itxString[colnum]();
    for (int i = 0; i < colnum; i++)
      m_row[i].SetEmpty();
    colsNumber = colnum;
    m_next = NULL;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRRecord::TQRRecord\n");
  }
}

TQRRecord::TQRRecord(TQRRecord* previous, int colnum, TQRRecord* next)
{
  try
  {
    m_previous = previous;
	  m_row = (itxString*) new itxString[colnum]();
    for (int i = 0; i < colnum; i++)
      m_row[i].SetEmpty();
    colsNumber = colnum;
    m_next = next;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRRecord::TQRRecord\n");
  }
}

TQRRecord::~TQRRecord()
{
  try
  {
    delete [] m_row;
  }
  catch(char* e)
  {
    char* now_e_is_referenced = e;
// fabio-->patch 30/05/2001
//    DebugTrace2(IN_ERROR, "%s\n", e);
  }
  catch(...)
  {
// fabio-->patch 30/05/2001
//    DebugTrace2(IN_ERROR, "TQRRecord::~TQRRecord\n");
  }
}

void TQRRecord::operator=(TQRRecord& src)
{
  try
  {
    for (int icol = 0; icol < colsNumber; icol++)
      m_row[icol] = (src.m_row[icol]).GetBuffer();
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRRecord::operator=\n");
  }
}


// predispone lo spazio size per il campo in posizione colindex
void TQRRecord::ExpandCol(int colindex, int size)
{
  try
  {
	  m_row[colindex].Space(size, '\0');
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRRecord::ExpandCol\n");
  }
}