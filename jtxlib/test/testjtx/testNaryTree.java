/*
 * testNaryTree.java
 *
 * Created on 15 May 2006, 15:17
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package jtxlib.test.testjtx;

import jtxlib.main.datastruct.narytree.NaryTree;
import jtxlib.main.sql.DBConnection;
import jtxlib.main.sql.DataManager;
import jtxlib.main.web.widgets.ntrenderer.NaryTreeRenderer;

/**
 *
 * @author gambineri
 */
public class testNaryTree
{
  
  /** Creates a new instance of testNaryTree */
  public testNaryTree()
  {
  }
  
//-----------------------------------------------
  public void runTest()
  {
    try
    {
      DBConnection conn = new DBConnection("pg_test", "", "");
      DataManager dm = new DataManager(conn);
      NaryTree nt = new NaryTree(conn, "narytree");
      
      int rid = 0;
      int cid = 0;
      int cid2 = 0;
      int cid3 = 0;
      int leaf = 0;
      int i   = 0;
      
      //cleanup tree table
      dm.executeStatement("DELETE FROM narytree", DataManager.StatementType.DELETE);
      
      rid = nt.addRoot("root01");
      cid = nt.addChild(rid, "child01");
      nt.removeNode(cid);
      cid = nt.addChild(rid, "child01");
      nt.addChild(rid, "child02");
      cid3 = nt.addChild(rid, "child03");
      
      cid2 = nt.addChild(cid, "nephew02");
      nt.addChild(cid, "nephew03");
      nt.addChild(cid2, "leaf01");
      leaf = nt.addChild(cid2, "leaf02");
      nt.addChild(cid2, "leaf03");

      cid = nt.addChild(cid3, "nephew01");
      nt.addChild(cid, "leaf04");
      nt.addChild(cid, "leaf05");
      
      System.out.println(nt);
      
      System.out.println("Count of sibling of child01 = " + nt.getSiblingsCount(cid));
      System.out.println("Siblings of child01 : ");
      NaryTree.Node[] sib = nt.getSiblings(cid);
      for (i=0; i<sib.length; i++)
        System.out.println(nt.hierarchyToString(sib[i].m_Id));

      System.out.println("Siblings of child01 with getSiblingsId: ");
      int sibid[] = nt.getSiblingsId(cid);
      for (i=0; i<sibid.length; i++)
        System.out.println(nt.hierarchyToString(sibid[i]));
      
      System.out.println("Count of childs of child01 = " + nt.getChildsCount(cid));
      System.out.println("Childs of child01 : ");
      NaryTree.Node[] ch = nt.getChilds(cid);
      for (i=0; i<ch.length; i++)
        System.out.println(nt.hierarchyToString(ch[i].m_Id));
      
      System.out.println("Childs of child01 with getChildsId: ");
      int chid[] = nt.getChildsId(cid);
      for (i=0; i<chid.length; i++)
        System.out.println(nt.hierarchyToString(chid[i]));
      
      System.out.println("getLabel(" + cid +") = " + nt.getNodeLabel(cid));      
      System.out.println("getMaxDepth() = " + nt.getMaxDepth());
      
      System.out.println("root for leaf " + nt.getNodeLabel(leaf) + " is " + nt.getRootNode(leaf));
      
      System.out.println("Renderer:");
      NaryTreeRenderer ntr = new NaryTreeRenderer(nt);      
      System.out.println(ntr.toTable());
      
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }    
  }

//-----------------------------------------------
  public void runMassiveTest()
  {
    try
    {
      DBConnection conn = new DBConnection("pg_test", "", "");
      DataManager dm = new DataManager(conn);
      NaryTree nt = new NaryTree(conn, "narytree");
      
      int i = 0;
      int j = 0;
      int k = 0;
      int l = 0;
      
      int rid = 0;
      int cid = 0;
      int cid2 = 0;
      
      //cleanup tree table
      dm.executeStatement("DELETE FROM narytree", DataManager.StatementType.DELETE);

      for (i=0; i<5; i++)
      {
        rid = nt.addRoot("root"+i);
        for (j=0; j<5; j++)
        {
          cid = nt.addChild(rid, "child"+j);
          for (k=0; k<5; k++)
          {
            cid2 = nt.addChild(cid, "nephew"+k);
            for (l=0; l<5; l++)
            {
              nt.addChild(cid2, "leaf"+l);
            }
          }          
        }
      }
      
      dm.close();
      conn.close();
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }    
  }
}




