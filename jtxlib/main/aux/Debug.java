/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.aux;

import java.io.File;
import java.io.FileOutputStream;

public class Debug
{
  public static String     m_Filename;
  public static boolean    m_DebugActive = true;

  public Debug(String filename)
  {
    m_Filename = new String(filename);
    if (m_Filename.trim().length() == 0)
      m_DebugActive = false;
  }

  public Debug()
  {
  }

  private static void writeOutput(String msg)
  {
    System.out.println(msg + "\n");
//    send2file(msg);
  }

  public static void Dbg(String msg)
  {
    if (m_DebugActive)
      writeOutput(msg);
  }

  public static void Dbg(int num)
  {
    if (m_DebugActive)
    {
      Integer a = new Integer(num);
      writeOutput(a.toString());
    }
  }

  public static void send2file(String msg)
  {
    if (m_DebugActive)
    {
      try
      {
  /*
    	  Date d = new Date();
        m_Filename = new String(d.toString());
        m_Filename = m_Filename.replace(':', '.');
        m_Filename = m_Filename.substring(0, 14);
  */
    	  FileOutputStream fos = new FileOutputStream(m_Filename, true);
    	  fos.write(msg.getBytes());
    	  fos.close();
    	}
      catch(Exception e)
      {
    		System.out.println("JTXDebug.send2file function: " + e.getMessage());
    		return;
  		}
  	}
  }

  public static void ClearDebugFile()
  {
    if (m_DebugActive)
    {
      File f = new File(m_Filename);

      if (f.exists())
        f.delete();
    }
  }
  
  //----------------------------------------------------------------------------
  public static void LogException(String func_name, Exception e, String q)
  {
    System.out.println("****************** LogException ********************"); 
    System.out.println("When: " + new java.util.Date()); 
    System.out.println("Exception raised in function: " + func_name); 
    System.out.println("Query is: " + q); 
    e.printStackTrace();
    System.out.println("*************** End of LogException ****************"); 
  }

  //----------------------------------------------------------------------------
  public static void LogMessage(String func_name, String msg)
  {
    System.out.println("******************* LogMessage *********************"); 
    System.out.println("When: " + new java.util.Date()); 
    System.out.println("Raised in: " + func_name); 
    System.out.println(msg); 
    System.out.println("**************** End of LogMessage *****************"); 
  }
}





