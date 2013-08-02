/* $RCSfile: JTXIniFile.java,v $
 * $Revision: 1.3 $
 * $Author: Gambineri $
 * $Date: 2006/05/11 07:46:51 $
 *
 * developed by Massimo Gambineri
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 */

package jtxlib.main.aux;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.NoSuchElementException;
import java.util.StringTokenizer;
import java.util.Vector;

import jtxlib.main.ui.ComboBox;


public class IniFile
{
  public final String       COMMENT  = "#";
  public final String       ITEM_SEP = ";";

  public String             m_Filename;
  public RandomAccessFile   m_RAF;

  public Vector<String>     m_VarNames  = new Vector<String>(100, 1);
  public Vector<String>     m_VarValues = new Vector<String>(100, 1);

  static Debug           m_Dbg   = new Debug();

  private boolean           m_IsEmpty = true;

//---------------------------------------------------------
  public IniFile()
  {
  }

//---------------------------------------------------------
  public IniFile(String filename)
  {
    SetIniFile(filename);
  }

//---------------------------------------------------------
  public void SetIniFile(String filename)
  {
    String  fline;
    int     poseq;

    try
    {
      m_IsEmpty  = true;
      m_Filename = new String(filename);

      if (m_Filename.trim().length() > 0)
        m_RAF = new RandomAccessFile(m_Filename, "r");

      if (!m_RAF.equals(null))
      {
        m_IsEmpty = false;

        m_RAF.seek(0);
        int idx = 0;
        while ((fline = m_RAF.readLine()) != null)
        {
          fline.trim();

          if (fline.startsWith(COMMENT) == false)
          {
            if ((poseq = fline.indexOf("=")) != -1)
            {
              m_VarNames.addElement((new String(fline.substring(0, poseq))).trim());
              m_VarValues.addElement((new String(fline.substring(poseq + 1, fline.length()))).trim());
              Debug.Dbg(m_VarNames.elementAt(idx) + "=" + m_VarValues.elementAt(idx));
              idx++;
            }
          }
        }
      }
    }
    catch(IOException e)
    {
     	Debug.Dbg("Exception during JTXIniFile(String filename): " + e.getMessage());
    }
  }


//---------------------------------------------------------
  public String GetVar(String varname)
  {
    int       idx = 0;

    try
    {
      while(idx < m_VarNames.size())
      {
        if (((String)m_VarNames.elementAt(idx)).equals(varname))
          return (String)m_VarValues.elementAt(idx);

        idx++;
      }
    }
    catch(Exception e)
    {
     	Debug.Dbg("Exception during JTXIniFile.GetVar(String varname): " + e.getMessage());
    }

    return "JTXINIFILE_VAR_NOT_FOUND";
  }

//---------------------------------------------------------
  public void FillCombo(ComboBox jcb, String varname)
  {
    String    varval = "";

    if ((varval = GetVar(varname)).length() == 0)
      return;

    StringTokenizer  st = new StringTokenizer(varval, ITEM_SEP);

    String    parz = "";


    try
    {
     // while((parz = (String)st.nextToken()) != null)
     while (st.hasMoreTokens())
      {
        parz = st.nextToken();
        parz = parz.trim();

        if (parz.length() > 0)
          jcb.addItem(parz);
      }
    }
    catch(NoSuchElementException e)
    {
     	Debug.Dbg("Exception during JTXIniFile.FillCombo(JTXJComboBox jcb, String varname): " + e.getMessage());
    }
  }

//---------------------------------------------------------
  public boolean IsEmpty()
  {
    return m_IsEmpty;
  }
}

