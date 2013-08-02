/* $RCSfile: JTXResultSet.java,v $
 * $Revision: 1.10 $
 * $Author: Gambineri $
 * $Date: 2007/01/24 09:56:11 $
 *
 * developed by Massimo Gambineri
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 */

package jtxlib.main.sql;

import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;

import jtxlib.main.datastruct.Vector;


/**
 * A class representing in a matrix-like style a population of records,
 * filled via an ODBC query or a manual construction.
 * The first record (row) has index 0 and the last one has index <code>getRecordCount() - 1</code>.
 * The first field (column) has index 0 and the last one has index <code>getFieldCount() - 1</code>.
 * <P>
 * <B>Example</B>
 * <P>
 * You use this class to get data from an ODBC source in this way:
 * <PRE>
 *         JTXConnection conn = new JTXConnection("dsn_name", "dsn_user", "dsn_password");
 *         JTXResultSet  rs   = new JTXResultSet(conn);
 *
 *         rs.select("SELECT * FROM Tab1", 2000);
 *
 *         int record_count = rs.getRecordCount();
 *         for (int i=0; i < record_count; i++)
 *         {
 *           System.out.println(rs.getField(i, 2));
 *           System.out.println(rs.getField(i, "Xyz"));
 *           System.out.println(rs.getField(i, 0));
 *         }
 * </PRE>
 * or, alternatively:
 * <PRE>
 *         JTXConnection conn = new JTXConnection("dsn_name", "dsn_user", "dsn_password");
 *         JTXResultSet  rs   = new JTXResultSet(conn);
 *
 *         rs.select("SELECT * FROM Tab1", 2000);
 *
 *         while(rs.next())
 *         {
 *           System.out.println(rs.getField("ID"));
 *           System.out.println(rs.getField("text1"));
 *           System.out.println(rs.getField("number1"));
 *         }
 *</PRE>
 * <P>
 * The examples above execute a SELECT query to retrieve 2000 records,
 * then they print out, for each row, the values of the requested fields.
 */

public class RecordSet extends DataManager
{
  private int               m_RowsToFetch         = 100;
  private String            m_Statement           = "";
  private ResultSet         m_RS                  = null;
  private DBColumns         m_Cols                = null;

  // Size of this vector is the total number of fetched rows
  private Vector         m_Rows                = null;

  // Cursor: -1 means BOF. BOF state is possible ONLY before the first fetch!!!
  private int               m_RowCursor           = -1;


//------------------------------------------------------------------------------
/**
 * Creates an instance of <code>this</code> object.
 * <P>
 * @param conn A JTXConnection to an ODBC data source.
 */
  public RecordSet(DBConnection conn)
  {
    super(conn);
  }

//------------------------------------------------------------------------------
/**
 * Creates an instance of <code>this</code> object.
 * <P>
 * @param coldata A JTXVector of strings containing the names of the columns to assign
 *                to the newly created instance.
 */
  public RecordSet(Vector coldata)
  {
    try
    {
      m_Cols = new DBColumns(coldata);
      m_Rows = new Vector(1);
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }


//------------------------------------------------------------------------------
/**
 * Returns the ResultSet object.
 */
  public ResultSet getResultSet()
  {
    return m_RS;
  }

//------------------------------------------------------------------------------
/**
 * Executes a SELECT statement and fills <code>this</code> JTXResultSet with the specified
 * number of records, if possible.
 * <p>
 * <b>Note</b>: apart from the required number of rows to be fetched, the cursor position
 * on the returned resultset is <b>always BOF</b>. This means that you always have to
 * perform a {@link #first first()} or a {@link #next next()} to begin accessing data.
 * The only way to skip this approach is to use the "positional" version of
 * {@link #getField(int, String) getField(int rowIndex, String fieldName)}: in this case,
 * you are explicitly indicating the row you want to access data into, thus bypassing the
 * current cursor position.
 * @param query the SELECT statement.
 * @param nrows the number of rows initially fetched inside the current object.
 * @see #select(String query)
 */
  public boolean select(String query, int nrows)
  {
    if (query != null && query.trim().length() > 0) 
    {
      try
      {
        m_Statement = query;

        if ((m_RS = executeStatement(m_Statement, StatementType.SELECT)) == null)
          return false;

        m_Rows = null;
        m_RowCursor = -1;
        m_Cols = new DBColumns(m_RS.getMetaData());
        if (fetch(nrows) == 0)
          m_Rows = null;  // query returns an empty resultset
      }
      catch (Exception e)
      {
        e.printStackTrace();
        return false;
      }
    }
    
    return true;
  }

//------------------------------------------------------------------------------
/**
 * Executes a SELECT statement and fills <code>this</code> JTXResultSet with the default
 *  number (100) of records, if possible.
 * @param query the SELECT statement.
 * @see #select(String query, int nrows)
 */
  public boolean select(String query)
  {
    return select(query, m_RowsToFetch);
  }

//------------------------------------------------------------------------------
/**
 * Fetches the desired number of records (rows) into <code>this</code> JTXResultSet, appending them
 * to the end of its internal vector.
 * <P>
 * <B>Note: </B>This function is equivalent to a <i>nrows</i> cycle calling the <i>next()</i>
 * method in the java.sql.ResultSet class, if this is possible.
 * @param nrows the number of rows to be fetched.
 * @see #fetch()
 * @see #fetchAll()
 */
  public int fetch(int nrows)
  {
    if (nrows <= 0)
      return 0;

    if (m_Rows == null)
      m_Rows = new Vector(nrows);

    if (m_Cols == null)
      return 0;

    int      colNum   = m_Cols.getColumnCount();
    int      nFetched = 0;
    DBRecord appo     = null;
    
    try
    {
      while(m_RS.next())
      {
        appo = new DBRecord(colNum);

        for (int i=0; i<colNum; i++)
          appo.addFieldValue(m_RS.getString(i+1));

        m_Rows.addElement(appo);
        nFetched++;

        if (nFetched == nrows)
          break;
      }
    }
    catch (SQLException e)
    {
      e.printStackTrace();
    }

    return nFetched;
  }

//------------------------------------------------------------------------------
/**
 * Fetches all the records (rows) into <code>this</code> JTXResultSet, appending them
 * to the end of its internal vector.
 * <P>
 * <B>Note: </B>
 * This function can be very expensive in terms of memory usage and CPU time:
 * use but not abuse...
 * <P>
 * @see #fetch(int nrows)
 * @see #fetch()
 */
  public int fetchAll()
  {
    int ret = (m_Rows == null ? 0 : m_Rows.size());
    while (fetch() != 0);
    
    return (m_Rows == null ? 0 : m_Rows.size()) - ret;
  }

//------------------------------------------------------------------------------
/**
 * Fetches the default number of records (rows) into <code>this</code> JTXResultSet, appending them
 * to the end of its internal vector.
 * @see #fetch(int nrows)
 * @see #fetchAll()
 */
  public int fetch()
  {
    return fetch(m_RowsToFetch);
  }

//------------------------------------------------------------------------------
/**
 * Sets the cursor to the first record, fetching it if necessary and
 * if it exists, returns true.
 * If the resultset is empty or if it fails, returns false.
 */
  public boolean first()
  {
    if (m_Rows == null)
    {
      if (fetch(1) > 0)
      {
        m_RowCursor = 0;
        return true;
      }
      return false;
    }

    m_RowCursor = 0;
    return true;
  }

//------------------------------------------------------------------------------
/**
 * Fetches the next record and sets the cursor to the next record, if it exists,
 * and returns true.
 * If the next record does not exist or the resultset is empty or if it fails,
 * returns false.
 */
  public boolean next()
  {
    if (m_Rows == null || m_Rows.size() == (m_RowCursor + 1))
    {
      if (fetch(1) > 0)
      {
        m_RowCursor++;
        return true;
      }
      return false;
    }

    m_RowCursor++;
    return true;
  }

//------------------------------------------------------------------------------
/**
 * Fetches all remaining records and sets the cursor to the last record,
 * if it exists, and returns true.
 * If the resultset is empty or if it fails, returns false.
 */
  public boolean last()
  {
    fetchAll();

    if (m_Rows != null && m_Rows.size() > 0)
      m_RowCursor = m_Rows.size() - 1;

    return (m_RowCursor >= 0 ? true : false);
  }

//------------------------------------------------------------------------------
/**
 * Returns the current cursor position (-1 means BOF or EOF).
 */
  public int getCursorPos()
  {
    return m_RowCursor;
  }

//------------------------------------------------------------------------------
/**
 * Returns the number of records (rows) currently contained in <code>this</code> object.
 */
  public int getRecordCount()
  {
    return (m_Rows == null ? 0 : m_Rows.size());
  }

//------------------------------------------------------------------------------
/**
 * Returns the number of fields (columns) currently contained int <code>this</code> object.
 */
  public int getFieldCount()
  {
    return (m_Cols == null ? 0 : m_Cols.getColumnCount());
  }

//------------------------------------------------------------------------------
/**
 * Returns a string containing the value of the requested field.
 * <P>
 * @param rowIndex    the 0-based index of the row from which the field must be retrieved.
 * @param fieldIndex  the 0-based index of the column from which the field must be retrieved.
 */
  public String getField(int rowIndex, int fieldIndex)
  {
    try
    {
      String aux = (String)(((DBRecord)m_Rows.elementAt(rowIndex)).m_Row.elementAt(fieldIndex));
      return (aux == null ? "" : aux);
    }
    catch (Exception e)
    {
      return "";
    }
  }

//------------------------------------------------------------------------------
/**
 * Returns a string containing the value of the requested field.
 * <P>
 * @param rowIndex    the 0-based index of the row from which the field must be retrieved.
 * @param fieldName   the name of the field to be retrieved.
 */
  public String getField(int rowIndex, String fieldName)
  {
    return (m_Cols == null ? "" : getField(rowIndex, m_Cols.getColumnIndex(fieldName)));
  }

//------------------------------------------------------------------------------
/**
 * Returns a string containing the value of the requested field at the current cursor position.
 * <P>
 * @param fieldName   the name of the field to be retrieved.
 */
  public String getField(String fieldName)
  {
    if (m_RowCursor >= 0 && m_Cols != null)
      return getField(m_RowCursor, m_Cols.getColumnIndex(fieldName));
    else
      return "";
  }

//------------------------------------------------------------------------------
/**
 * Returns a string containing the name of the requested field.
 * <P>
 * @param fieldIndex  the 0-based index of the column from which the field name must be retrieved.
 */
  public String getFieldName(int fieldIndex)
  {
    try
    {
      if (m_Cols == null)
        return "";
      return (String)(((DBColumns)m_Cols).m_Name.elementAt(fieldIndex));
    }
    catch (Exception e)
    {
      return "";
    }
  }

//------------------------------------------------------------------------------
/**
 * Sets a string value in the requested record-field.
 * <P>
 * @param rowIndex    the 0-based index of the row.
 * @param fieldIndex  the 0-based index of the column.
 * @param fieldVal    the value to boolean assigned to the column.
 */
  public void setField(int rowIndex, int fieldIndex, String fieldVal)
  {
    try
    {
      ((DBRecord)m_Rows.elementAt(rowIndex)).setElementAt(fieldVal, fieldIndex);
    }
    catch (Exception e) {e.printStackTrace();}
  }
  
//------------------------------------------------------------------------------
/**
 * Sets a string value in the requested record-field.
 * <P>
 * @param rowIndex    the 0-based index of the row.
 * @param fieldName   the name of the column to be re-assigned.
 * @param fieldVal    the value to boolean assigned to the column.
 */
  public void setField(int rowIndex, String fieldName, String fieldVal)
  {
    try
    {
      ((DBRecord)m_Rows.elementAt(rowIndex)).setElementAt(fieldVal, m_Cols.getColumnIndex(fieldName));
    }
    catch (Exception e) {e.printStackTrace();}
  }  
  
//------------------------------------------------------------------------------
/**
 * Adds a record to <code>this</code> object.
 * <P>
 * @param newrow A JTXVector of strings.
 */
  public int addRow(Vector newrow)
  {
    if (newrow == null)
      return -1;

    m_Rows.addElement(new DBRecord(newrow));

    return m_Rows.size();
  }

//------------------------------------------------------------------------------
/**
   * Removes a record from <code>this</code> object.
   * <P>
   * @param rowidx The zero-based index of the row to be removed.
   * @return void
   */
  public void removeRow(int rowidx)
  {
    if (rowidx >=0 && rowidx < m_Rows.size())
      m_Rows.removeElement(rowidx);
  }

//------------------------------------------------------------------------------
  public String toString()
  {
    String s = "";
    String d = "";
    
    try
    {
      if (m_Rows != null)
        for (int i=0; i<m_Rows.size(); i++)
          d += "[RECORD " + i + "] " + m_Rows.elementAt(i) + "\n";

      s = m_Cols.toString() +
          "---------------------------------- DATA -------------------------------------\n" +
          "             ";

      for(int k=0; k<m_Cols.getColumnCount(); k++)
        s += m_Cols.getName(k) + "\t\t";
      
      s += "\n" + d;
    }
    catch(Exception e) {e.printStackTrace();}

    return s;
  }

//***********************************************
//          DBColumns
//***********************************************
  class DBColumns
  {
    private int               m_ColNum      = 0;

    private Vector         m_CatalogName = null;
    private Vector         m_Name        = null;
    private Vector         m_Label       = null;
    private Vector         m_Table       = null;

//------------------------------------------------------------------------------
    private DBColumns(Vector coldata)
    {
      try
      {
        m_ColNum  = coldata.size();
        m_Name    = coldata;
      }
      catch (Exception e)
      {
        e.printStackTrace();
      }
    }

//------------------------------------------------------------------------------
    private DBColumns(ResultSetMetaData rsmd)
    {
      try
      {
        m_ColNum  = rsmd.getColumnCount();

        m_CatalogName = new Vector(m_ColNum);
        m_Name        = new Vector(m_ColNum);
        m_Label       = new Vector(m_ColNum);
        m_Table       = new Vector(m_ColNum);

        for (int i=1; i<=m_ColNum; i++)
        {
          m_CatalogName.addElement(rsmd.getCatalogName(i));
          m_Name.addElement(rsmd.getColumnName(i));
          m_Label.addElement(rsmd.getColumnLabel(i));
          m_Table.addElement(rsmd.getTableName(i));
        }
      }
      catch (SQLException e)
      {
        e.printStackTrace();
      }
    }

//------------------------------------------------------------------------------
    private int getColumnIndex(String fieldName)
    {
      int siz = m_Name.size();
      int ret = 0;

      while (ret < siz)
      {
        /* Retrieve the column index if exists:
           jtxlib is CASE INSENSITIVE ! */
        if (((String)m_Name.elementAt(ret)).equalsIgnoreCase(fieldName))
          return ret;
        else
          ret++;
      }

      return -1;
    }

//------------------------------------------------------------------------------
    private int getColumnCount()
    {
      return m_ColNum;
    }

//------------------------------------------------------------------------------
    private String getCatalogName(int idx)
    {
      try
      {
        return (String)m_CatalogName.elementAt(idx);
      }
      catch (Exception e)
      {
        return "";
      }
    }

//------------------------------------------------------------------------------
    private String getLabel(int idx)
    {
      try
      {
        return (String)m_Label.elementAt(idx);
      }
      catch (Exception e)
      {
        return "";
      }
    }

//------------------------------------------------------------------------------
    private String getName(int idx)
    {
      try
      {
        return (String)m_Name.elementAt(idx);
      }
      catch (Exception e)
      {
        return "";
      }
    }

//------------------------------------------------------------------------------
    private String getTable(int idx)
    {
      try
      {
        return (String)m_Table.elementAt(idx);
      }
      catch (Exception e)
      {
        return "";
      }
    }

//------------------------------------------------------------------------------
    public String toString()
    {
      StringBuffer buf = new StringBuffer();

      try
      {
        buf.append("CATALOG NAME              LABEL                     NAME                     \n");
        buf.append("-----------------------------------------------------------------------------\n");

        for (int i=0; i<m_ColNum; i++)
        {
          StringBuffer app = new StringBuffer();
          app.append("                                                                             \n");

          String tablename = (String)(m_Table == null ? "NO_TABLE" : m_Table.elementAt(i));
          if (tablename.length() > 0)
           tablename += ".";

          app.insert(0,  tablename + (String)(m_CatalogName == null || m_CatalogName.elementAt(i).equals("") ? 
                                              "NO_CATALOG" : m_CatalogName.elementAt(i)));
          app.insert(26, tablename + (String)(m_Label == null || m_Label.elementAt(i).equals("") ? 
                                              "NO_LABEL" : m_Label.elementAt(i)));
          app.insert(52, tablename + (String)(m_Name == null || m_Name.elementAt(i).equals("") ? 
                                              "NO_NAME" : m_Name.elementAt(i)));

          buf.append(app);
        }
      }
      catch(Exception e)
      {
        e.printStackTrace();
      }

      return buf.toString();
    }
  } // class DBColumn

//***********************************************
//          DBRecord
//***********************************************
  class DBRecord
  {
    private Vector m_Row;

//------------------------------------------------------------------------------
    private DBRecord(){}

//------------------------------------------------------------------------------
    private DBRecord(Vector newrow)
    {
      m_Row = newrow;
    }

//------------------------------------------------------------------------------
    private DBRecord(int colnum)
    {
      m_Row = new Vector(colnum);
    }

//------------------------------------------------------------------------------
    private void addFieldValue(String fvalue)
    {
      m_Row.addElement(fvalue);
    }

//------------------------------------------------------------------------------
    private void removeField(int idx)
    {
      m_Row.removeElement(idx);
    }

//------------------------------------------------------------------------------
    private void setElementAt(Object obj, int index) throws java.lang.ArrayIndexOutOfBoundsException
    {
      m_Row.setElementAt(obj, index);
    }
    
//------------------------------------------------------------------------------
    public String toString()
    {
      StringBuffer buf = new StringBuffer();

      for (int i=0; i<m_Row.size(); i++)
        buf.append(m_Row.elementAt(i) + "\t\t");

      return buf.toString();
    }
  } // class DBRecord
}


