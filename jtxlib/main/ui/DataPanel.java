/* $RCSfile: JTXDataPanel.java,v $
 * $Revision: 1.4 $
 * $Author: Gambineri $
 * $Date: 2006/05/26 15:08:42 $
 *
 * developed by Massimo Gambineri
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 */

package jtxlib.main.ui;


//import java.*;
//import java.awt.*;
//import java.lang.*;

import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JTextField;

import jtxlib.main.datastruct.Vector;
import jtxlib.main.sql.RecordSet;


/**
 * A class which automatically displays contents in a RecordSet.
 * <P>
 */

public class DataPanel extends JPanel
{
  private final static long serialVersionUID = 1L;

/**
 *
 */
  public interface ControlType
  {
    public final byte TEXTFIELD = 0;
    public final byte TEXTAREA  = 1;
  }

  private Vector   m_Labels        = new Vector(10);
  private Vector   m_Controls      = new Vector(10);
//  private JScrollPane m_ScrollPane    = new JScrollPane();


/**
 *
 */
  public DataPanel()
  {
  }


/**
 *
 */
  public DataPanel(RecordSet rs, byte[] controlTypes)
  {
    try
    {
      Object obj = null;

      for (int i=0; i<rs.getFieldCount(); i++)
      {
        m_Labels.addElement(new JLabel(rs.getFieldName(i)));
        switch (controlTypes[i])
        {
          case ControlType.TEXTFIELD:
            obj = new JTextField();
          break;
          case ControlType.TEXTAREA:
            obj = new JTextArea();
          break;
        }

        m_Controls.addElement(obj);
        addControls();
      }
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }

/**
 *
 */
  public void addControls()
  {
    try
    {
      for (int i=0; i<m_Controls.size(); i++)
      {
        add((JComponent)m_Labels.elementAt(i));
        add((JComponent)m_Controls.elementAt(i));
      }

      revalidate();
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }

  class DataViewer
  {
    Vector   m_Labels        = new Vector(5);
    Vector   m_Controls      = new Vector(5);


  }
}


