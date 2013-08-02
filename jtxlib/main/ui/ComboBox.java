/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.ui;

//import java.io.*;
//import java.lang.*;

import java.awt.Color;

import javax.swing.JComboBox;

import jtxlib.main.aux.Debug;
import jtxlib.main.datastruct.Vector;



  //*******************
  //--- ComboBox ---
  //*******************

public class ComboBox extends JComboBox<Object>
{
  private final static long serialVersionUID = 1L;
  private Vector     m_ID      = new Vector(10);

  public ComboBox(int halign, int valign, Color bg, Color fg, Vector id, Vector descr)
  {
    setAlignmentX(halign);
    setAlignmentY(valign);
    setForeground(fg);
    setBackground(bg);
    setValues(id, descr);
  }

  public ComboBox(Vector id, Vector descr)
  {
    this((int)LEFT_ALIGNMENT, (int)TOP_ALIGNMENT, Color.white, Color.black, id, descr);
  }

  public ComboBox()
  {
    this((int)LEFT_ALIGNMENT, (int)TOP_ALIGNMENT, Color.white, Color.black, null, null);
  }

  public void setValues(Vector id, Vector descr)
  {
    if (id == null || descr == null)
      return;

    if (getItemCount() > 0)
      removeAllItems();
    m_ID.removeAllElements();

    int i;

    for(i=0; i<descr.size(); i++)
    {
      Debug.Dbg((String)descr.elementAt(i));
      addItem(descr.elementAt(i));
    }

    for(i=0; i<id.size(); i++)
      m_ID.addElement(id.elementAt(i));
  }

  public String getSelected()
  {
    int appo = -1;

    try
    {
      return (String)(((appo = getSelectedIndex()) > -1) ? m_ID.elementAt(appo) : null);
    }
    catch(Exception e)
    {
    }

    return null;
  }

  public void setSelected(String id)
  {
    int curidx = m_ID.indexOf(id);

    if (curidx >= 0)
      setSelectedIndex(curidx);
  }

  public void appendItem(String descr, String id)
  {
    addItem(descr);
    m_ID.addElement(id);
  }

  public void removeAll()
  {
    removeAllItems();
    m_ID.removeAllElements();
  }
}




