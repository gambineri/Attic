package jtxlib.test.testjtx;

import jtxlib.main.datastruct.Vector;
import jtxlib.main.sql.DBConnection;
import jtxlib.main.sql.RecordSet;

class testResultset
{
  public void show1(DBConnection conn)
  {
    RecordSet rs = new RecordSet(conn);
    rs.select("SELECT * FROM Tab1", 2000);

    long curmilli = System.currentTimeMillis();

    for (int j=0; j<100; j++)
    {
      int rcount = rs.getRecordCount();
      for (int i=0; i<rcount; i++)
      {
        rs.getField(i, 0);
        rs.getField(i, 1);
        rs.getField(i, 2);
      }
    }
    System.out.println("show1: queries and fetches 2000 records, cycles 100 times and 'gets' all records and fields by using field indexes.");
    System.out.println("elapsed: " + (System.currentTimeMillis() - curmilli) + " millis.");
  }

//-----------------------------------------------
  public void show2(DBConnection conn)
  {
    RecordSet rs = new RecordSet(conn);
    rs.select("SELECT * FROM Tab1", 2000);

    long curmilli = System.currentTimeMillis();

    for (int j=0; j<100; j++)
    {
      int rcount = rs.getRecordCount();
      for (int i=0; i<rcount; i++)
      {
        rs.getField(i, "ID");
        rs.getField(i, "text1");
        rs.getField(i, "number1");
      }
    }

    System.out.println("show2: queries and fetches 2000 records, cycles 100 times and 'gets' all records and fields by using field names.");
    System.out.println("show2: " + (System.currentTimeMillis() - curmilli) + " millis.");
  }

//-----------------------------------------------
  public void show3(DBConnection conn)
  {
    RecordSet rs = new RecordSet(conn);
    rs.select("SELECT * FROM Tab1", 10);

    System.out.println("show3: selects 10 records and prints them out.");
    System.out.println(rs);
  }

//-----------------------------------------------
  public void show4(DBConnection conn)
  {
    RecordSet rs = new RecordSet(conn);
    rs.select("SELECT * FROM Tab1", 2000);

    long curmilli = System.currentTimeMillis();

    while(rs.next())
    {
      rs.getField("ID");
      rs.getField("text1");
      rs.getField("number1");
    }

    System.out.println("show4: queries and fetches 2000 records, 'gets' all records and fields by using field names.");
    System.out.println("show4: " + (System.currentTimeMillis() - curmilli) + " millis.");
  }

//-----------------------------------------------
  public void fetchAll(DBConnection conn)
  {
    RecordSet rs = new RecordSet(conn);
    rs.select("SELECT * FROM Tab1", 10);

    System.out.println("fetchAll: queries and fetches 10 records.");
    System.out.println("rs.fetchAll() returns " + rs.fetchAll());
    System.out.println("rs.getRecordCount() returns " + rs.getRecordCount());
  }

//-----------------------------------------------
  public void manualFill()
  {
    Vector v = new Vector(10);
    v.addElement("campo1");
    v.addElement("campo2");
    v.addElement("campo3");
    v.addElement("campo di carne");
    v.addElement("campi flegrei");
    v.addElement("campo santo");

    RecordSet rs = new RecordSet(v);

    System.out.println("Adding four times the same row:");
    rs.addRow(v);
    rs.addRow(v);
    rs.addRow(v);
    rs.addRow(v);
    System.out.println(rs);

    System.out.println("Deleting two times row 2:");
    rs.removeRow(2);
    rs.removeRow(2);
    System.out.println(rs);
      
//    testDataPanel tdp = new testDataPanel();
//    tdp.add(rs);
  }

//-----------------------------------------------
  public void first_next_last(DBConnection conn)
  {
    RecordSet rs = new RecordSet(conn);

    rs.select("SELECT number1 FROM Tab1 WHERE number1 <= 10 ORDER BY number1 ASC", 1);

    System.out.println("cursor subito dopo select = " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.next();
    System.out.println("cursor dopo next " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.next();
    System.out.println("cursor dopo next " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.last();
    System.out.println("cursor dopo last " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.first();
    System.out.println("cursor dopo first " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.next();
    System.out.println("cursor dopo next " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));

    //Requery and re-assign resultset
    System.out.println("Requery and re-assign......");

    rs.select("SELECT number1 FROM Tab1 WHERE number1 <= 10 ORDER BY number1 ASC", 0);
    System.out.println("cursor subito dopo select = " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.next();
    System.out.println("cursor dopo next " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.next();
    System.out.println("cursor dopo next " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.last();
    System.out.println("cursor dopo last " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.first();
    System.out.println("cursor dopo first " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    rs.next();
    System.out.println("cursor dopo next " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
  }

//-----------------------------------------------
  public void empty_resultset(DBConnection conn)
  {
    RecordSet rs = new RecordSet(conn);
    boolean ret = false;

    rs.select("SELECT number1 FROM Tab1 WHERE number1 < 0");
    System.out.println("cursor subito dopo select = " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
    ret = rs.first();
    System.out.println("first torna " + ret);
    ret = rs.next();
    System.out.println("next torna " + ret);
    System.out.println("cursor subito dopo next = " + rs.getCursorPos() + " number1 = " + rs.getField("number1"));
  }

//-----------------------------------------------
/*  
  public void manuallyAddedColumn(DBConnection conn)
  {
    RecordSet rs = new RecordSet(conn);
    boolean ret = false;

    System.out.println("manuallyAddedColumn: shows 3 records from a resultset " +
                       "before and after adding and removing columns manually.");

    rs.select("SELECT * FROM Tab1", 3);
    System.out.println(rs);

    int manaddcolidx = rs.appendColumn("manualColumn1", "manualValue1");
    System.out.println("Manually added a column with colidx = " + manaddcolidx);
    System.out.println(rs);
    
    manaddcolidx = rs.appendColumn("manualColumn2", "manualValue2");
    System.out.println("Manually added a column with colidx = " + manaddcolidx);
    System.out.println(rs);
    
    rs.removeColumn("manualColumn1");
    System.out.println("Manually removed column 'manualColumn1'");
    System.out.println(rs);
    
    rs.removeColumn("text1");
    System.out.println("Manually removed column 'text1'");
    System.out.println(rs);
  }
*/  
//-----------------------------------------------
  public void runTest()
  {
    DBConnection conn = new DBConnection("testjtx", "", "");
    show1(conn);
    show2(conn);
    show3(conn);
    show4(conn);
    fetchAll(conn);
    manualFill();
    first_next_last(conn);
    empty_resultset(conn);
    
    conn.close();
  }
}
