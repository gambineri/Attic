/*
 * @(#)testjtx.java 1.0 01/11/26
 *
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.test.testjtx;

public class testjtx
{
  testVector      m_testVector;
  testResultset   m_testResultset;
  testSocket      m_testSocket;
  testNaryTree    m_testNaryTree;

  public testjtx()
  {
    m_testVector    = new testVector();
    m_testResultset = new testResultset();
    m_testSocket    = new testSocket();
    m_testNaryTree  = new testNaryTree();
  }

/***************************************************************
 * test for PostGreSQL
 ***************************************************************/
//  public static void main(String args[])
//  {
//    testjtx test = new testjtx();
//    System.out.println("Starting testjtx...\n");
//
//    JTXConnection conn = null;
//    RecordSet jtxrs = null;
//
//    try
//    {
//      conn = new JTXConnection("sun.jdbc.odbc.JdbcOdbcDriver",
//                               "jdbc:odbc:testpgsql");
//      jtxrs = new RecordSet(conn);
//
//      String q = "";
//      q = "SELECT UNIKEY, JOBNUMBER, ISBN, DESCRIPTION, QTY " +
//          "FROM STOCK WHERE UNIKEY LIKE '%T5%' " +
//          "ORDER BY UNIKEY ASC";
//      jtxrs.select(q, 0);
//
//      if (jtxrs != null)
//      {
//        int i = 0;
//        String unikey = "";
//        while (jtxrs.next())
//        {
//          System.out.println(jtxrs.getField("UNIKEY"));
//        }
//      }
//    }
//    catch(Exception sqle) {sqle.printStackTrace();}
//  }

/***************************************************************
 * Regular expressions tests
 ***************************************************************/
//  public static void main(String args[])
//  {
//    testjtx test = new testjtx();
//    System.out.println("Starting testjtx...\n");
//
//    String aux = "a'";    
//    aux = aux.replaceAll("[^\\\\][\\']", "\\\'");
//    System.out.println(aux);
//    if (aux.equals("acca"))
//      System.out.println("sono veramente uquali");
//  }

/***************************************************************
 * N-ary tree tests
 ***************************************************************/
//  public static void main(String args[])
//  {
//    testjtx test = new testjtx();
//    System.out.println("Starting testjtx...\n");
//    test.m_testNaryTree.runTest();
//    test.m_testNaryTree.runMassiveTest();
//  }

/***************************************************************
 * standard tests
 ***************************************************************/
//  public static void main(String args[])
//  {
//    testjtx test = new testjtx();
//    System.out.println("Starting testjtx...\n");
//    test.m_testVector.runTest();
//    test.m_testSocket.runTest(args);
//    test.m_testResultset.runTest();
//  }

/***************************************************************
 * test JTXTools
 ***************************************************************/
  public static void main(String args[])
  {
    testjtx test = new testjtx();
    System.out.println("Starting testjtx...\n");

//    System.out.println(JTXTools.stringToLong("956893732456"));
//    System.out.println((int)JTXTools.stringToLong("956893732456"));
//    System.out.println(JTXTools.stringToLong("150"));
//    System.out.println(JTXTools.stringToDouble("956.893732"));
//    System.out.println((int)JTXTools.stringToDouble("956.893732"));
    
//    System.out.println(JTXTools.SQLQuotes("L'Aquila"));
//    System.out.println(JTXTools.SQLQuotesEnclose("L'Aquila"));

    String appo = "aaa\\bbb\\ccc/ddd/eee";
    appo = appo.replace("\\", "-");
    appo = appo.replace("/", ".");
    System.out.println(appo);

    String z = "cahhhzhhho";
    System.out.println(z.replaceAll("hhh", "z"));
  }

/***************************************************************
 * test for CallableStatement
 ***************************************************************/
//  public static void main(String args[])
//  {
//    Connection conn = null;
//    Statement  stat;
//    ResultSet data;
//
//    try
//    {
//      Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
//      conn = DriverManager.getConnection("jdbc:odbc:poucheval");
//    }
//    catch(SQLException sqle){}
//    catch(ClassNotFoundException e){}
//
//    try
//    {
//      CallableStatement cstmt = conn.prepareCall("{? = GetDHLCost(4.5, 12)}");
//      cstmt.registerOutParameter(1, java.sql.Types.VARCHAR);
//      cstmt.execute();
//      System.out.println(cstmt.getString(1));
//    }
//    catch(SQLException sqle){sqle.printStackTrace();}
//    catch(Exception e){e.printStackTrace();}
//  }

/***************************************************************
 * test for jdbc drivers
 ***************************************************************/
//  public static void main(String args[])
//  {
//    try
//    {
//      Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
////      Class.forName("oracle.jdbc.driver.OracleDriver");
//
//      Enumeration drivers = DriverManager.getDrivers();
//
//      while (drivers.hasMoreElements())
//      {
//         System.out.println(drivers.nextElement());
//      }
//    }
//    catch(ClassNotFoundException cnfe)
//    {
//      System.out.println("??? ClassNotFoundException\n");
//    }
//  }
}

