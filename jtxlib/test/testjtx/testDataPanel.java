package jtxlib.test.testjtx;

import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.border.TitledBorder;

import jtxlib.main.sql.RecordSet;
import jtxlib.main.ui.DataPanel;
import jtxlib.main.ui.VerticalLayout;

class testDataPanel extends JFrame
{
  private static final long serialVersionUID = -6036504064490871052L;

  public testDataPanel()
  {
    setSize(new Dimension(800, 600));
    setVisible(true);
  }
  
  public void add(RecordSet rs)
  {
    byte[] ctltypes = new byte[rs.getFieldCount()];
    
    for (int i=0; i<rs.getFieldCount(); i++) 
      ctltypes[i] = DataPanel.ControlType.TEXTFIELD;
      
    ctltypes[1] = DataPanel.ControlType.TEXTAREA;
    ctltypes[3] = DataPanel.ControlType.TEXTAREA;

    DataPanel pan = new DataPanel(rs, ctltypes);
    pan.setLayout(new VerticalLayout());
    pan.setBorder(new TitledBorder("Io senta..."));
    this.getContentPane().add(pan);
  }
}
	