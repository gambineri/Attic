/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.datastruct.narytree;

import java.sql.SQLException;

import jtxlib.main.aux.Tools;
import jtxlib.main.sql.DBConnection;
import jtxlib.main.sql.DataManager;
import jtxlib.main.sql.RecordSet;

/**
 * This class maps an n-ary tree data structure onto a relational database table, 
 * giving to its clients a safe, unique and numeric node identifier together 
 * with a name (label) and all the necessary functionalities to navigate the tree 
 * without having to know the inner design of the table and itself. 
 * It can be used as a lookup table to easily manage knowledge-tree entities or 
 * complex classification issues.
 * 
 * The way of working relies upon some basic facts fixed by design:
 * 
 * 1)  the class must be initialized through the init method, providing a valid
 *     database connection and the name of the main table.
 *    
 * 2)  the main table must contain (at least) the following fields:
 *
 *    *)  n_id    : numeric auto-sequence (unique node identifier)
 *    *)  n_label : text                  (name or description associated with the node)
 *    *)  n_depth : numeric               (depth of the node, 1-based, i.e. roots are depth 1)
 *    *)  n_pid   : numeric               (nodeid of the parent node)
 *
 * @author gambineri
 */

public class NaryTree 
{
  private final String  FN_NODEID     = "n_id";
  private final String  FN_NODELABEL  = "n_label";
  private final String  FN_NODEDEPTH  = "n_depth";
  private final String  FN_NODEPID    = "n_pid";
  
  // Error messages
  private final String ERR_NO_CONN         = "Cannot access database: no connection available.";
  private final String ERR_WRONG_DB_STRUCT = "Main table not found or fixed field names missing/mispelled.";
  
  private DBConnection    m_Conn  = null;
  private String          m_Tbl   = null;
  private RecordSet       m_TmpRs = null;
  
  /**
   * Creates a new instance of NaryTree
   * @param conn A DBConnection to the database.
   * @param narytree_table Name of the table in which the tree will be stored.
   */
  public NaryTree(DBConnection conn, String narytree_table)
  {
    m_Conn = conn;
    m_Tbl  = narytree_table;
      
    checkRDBMS();
  }
  
  /**
   * Checks if the database structure is consistent with the requirements (see general comment on NaryTree) 
   * and raises an exception with detailed error messages.
   * @return True if successful, false otherwise.
   */
  private void checkRDBMS()
  {
    CheckDBConn();

    m_TmpRs = new RecordSet(m_Conn);
    if (!m_TmpRs.select("SELECT COUNT(*) FROM " + m_Tbl + " WHERE " + 
                        FN_NODEID + " = 0 AND " + FN_NODELABEL + " = '' AND " + 
                        FN_NODEDEPTH  + " = 0 AND " + FN_NODEPID  + " = 0", 0))
      throw new Error(ERR_WRONG_DB_STRUCT);
  }

  /**
   * Adds a root element to the tree.
   * @param rootLabel Label for the root element.
   * @return returns the node id of the newly created root.
   */
  public int addRoot(String rootLabel)
  {
    int rootid = 0;
    
    try
    {
      m_TmpRs.executeStatement("BEGIN;", DataManager.StatementType.OTHER);
      
      m_TmpRs.executeStatement(
        "INSERT INTO " + m_Tbl + 
        " (" + FN_NODEDEPTH + ", " + FN_NODELABEL + ", " + FN_NODEPID + ") VALUES (" +
        "1, " + m_TmpRs.adjQuotes(rootLabel) + ", 0)", 
        DataManager.StatementType.INSERT);

      m_TmpRs.select("SELECT MAX(" + FN_NODEID + ") FROM " + m_Tbl);
      m_TmpRs.first();
      rootid = Tools.stringToInt(m_TmpRs.getField(0, 0));
      m_TmpRs.executeStatement("COMMIT;", DataManager.StatementType.OTHER);
    } 
    catch (SQLException ex)
    {
      try {m_TmpRs.executeStatement("ROLLBACK;", DataManager.StatementType.OTHER);} 
      catch(SQLException ex2) {ex2.printStackTrace();}
      ex.printStackTrace();
    }
    
    return rootid;
  }
  
  /**
   * Adds a child node to the given node.
   * @param parentNodeId the id of the node that will be the parent of the newly created node.
   * @param nodeLabel the label of the newly created node.
   * @return the id of the newly created node.
   */
  public int addChild(int parentNodeId, String nodeLabel)
  {
    int nodeid = 0;
    
    try
    {
      m_TmpRs.executeStatement("BEGIN;", DataManager.StatementType.OTHER);
      
      m_TmpRs.executeStatement(
        "INSERT INTO " + m_Tbl + 
        "(" + FN_NODEDEPTH + ", " + FN_NODELABEL + ", " + FN_NODEPID + ") VALUES (" +
        "(SELECT " + FN_NODEDEPTH + " + 1 FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + parentNodeId + "), " +
        m_TmpRs.adjQuotes(nodeLabel) + ", " + parentNodeId + ")", 
        DataManager.StatementType.INSERT);
      
      m_TmpRs.select("SELECT MAX(" + FN_NODEID + ") FROM " + m_Tbl);
      m_TmpRs.first();
      nodeid = Tools.stringToInt(m_TmpRs.getField(0, 0));
      m_TmpRs.executeStatement("COMMIT;", DataManager.StatementType.OTHER);
    } 
    catch (SQLException ex)
    {
      try {m_TmpRs.executeStatement("ROLLBACK;", DataManager.StatementType.OTHER);} 
      catch(SQLException ex2) {ex2.printStackTrace();}
      ex.printStackTrace();
    }
    
    return nodeid;
  }
 
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public boolean removeNode(int nodeid)
  {
    try
    {
      m_TmpRs.executeStatement("DELETE FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + nodeid, 
                               DataManager.StatementType.DELETE);
    } 
    catch (SQLException ex)
    {
      ex.printStackTrace();
      return false;
    }
    
    return true;
  }
  
  /**
   * 
   * @param node
   * @return 
   */
  public boolean removeNode(Node node)
  {
    try
    {
      m_TmpRs.executeStatement("DELETE FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + node.m_Id, 
                               DataManager.StatementType.DELETE);
    } 
    catch (SQLException ex)
    {
      ex.printStackTrace();
      return false;
    }
    
    return true;
  }
  
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public Node getNode(int nodeid)
  {
    return new Node(nodeid);
  }
  
  //----------------------------------------------------------------------------
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public String getNodeLabel(int nodeid)
  {
    String ret = "";
    
    try
    {
      m_TmpRs.select("SELECT " + FN_NODELABEL + " FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + nodeid, 1);
      ret = m_TmpRs.getField(0, 0);
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }
  
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public int getParentId(int nodeid)
  {
    int ret = 0;
    
    try
    {
      m_TmpRs.select("SELECT " + FN_NODEPID + " FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + nodeid, 1);
      ret = Tools.stringToInt(m_TmpRs.getField(0, 0));
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }

  /**
   * 
   * @param node
   * @return 
   */
  public Node getParentNode(Node node)
  {
    Node ret = null;
    
    try
    {
      m_TmpRs.select("SELECT " + FN_NODEPID + " FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + node.m_Id, 1);
      ret = new Node(Tools.stringToInt(m_TmpRs.getField(0, 0)));
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }
  
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public Node getRootNode(int nodeid)
  {
    int parentid = 0;
    int aux = 0;
    
    aux = nodeid;
    while ((aux = getParentId(aux)) != 0)
      parentid = aux;
      
    return new Node(parentid);
  }

  /**
   * 
   * @return 
   */
  public int getRootsCount()
  {
    int ret = 0;
    
    try
    {
      m_TmpRs.select("SELECT COUNT(" + FN_NODEID + ") FROM " + m_Tbl + 
                     " WHERE " + FN_NODEDEPTH + " = 1", 1);
      
      ret = Tools.stringToInt(m_TmpRs.getField(0, 0));
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
  return ret;
  }
  
  /**
   * 
   * @return 
   */
  public Node[] getRoots()
  {
    Node[] ret = new Node[getRootsCount()];

    try
    {
      m_TmpRs.select("SELECT " + FN_NODEID + ", "  + FN_NODELABEL +
                     " FROM " + m_Tbl + " WHERE " + FN_NODEDEPTH + " = 1");
      
      int c = 0;
      while (m_TmpRs.next())
      {
        ret[c]          = new Node();
        ret[c].m_Id     = Tools.stringToInt(m_TmpRs.getField(FN_NODEID));
        ret[c].m_Pid    = 0;
        ret[c].m_Depth  = 1;
        ret[c].m_Label  = m_TmpRs.getField(FN_NODELABEL);
        c++;
      }
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }

  /**
   * 
   * @param nodeid 
   * @return 
   */
  public int getSiblingsCount(int nodeid)
  {
    int ret = 0;
    
    try
    {
      m_TmpRs.select("SELECT " + FN_NODEPID + " FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + nodeid, 1);
      m_TmpRs.select("SELECT COUNT(" + FN_NODEID + ") FROM " + m_Tbl + 
                     " WHERE " + FN_NODEPID + " = " + m_TmpRs.getField(0, 0), 1);
      
      ret = Tools.stringToInt(m_TmpRs.getField(0, 0));
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret - 1;
  }
  
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public int[] getSiblingsId(int nodeid)
  {
    int[] ret = new int[getSiblingsCount(nodeid)];

    try
    {
      m_TmpRs.select("SELECT " + FN_NODEPID + " FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + nodeid, 1);
      m_TmpRs.select("SELECT " + FN_NODEID + " FROM " + m_Tbl + 
                     " WHERE " + FN_NODEPID + " = " + m_TmpRs.getField(0, 0) + " AND " +
                     FN_NODEID + " <> " + nodeid);
      
      try 
      {
        int c = 0;
        while (m_TmpRs.next())
        {  
          ret[c] = Integer.parseInt(m_TmpRs.getField(c, 0));
          c++;
        }
        
      } 
      catch(NumberFormatException ex) {ex.printStackTrace();}
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }

  /**
   * 
   * @param nodeid 
   * @return 
   */
  public Node[] getSiblings(int nodeid)
  {
    Node[] ret = new Node[getSiblingsCount(nodeid)];

    try
    {
      m_TmpRs.select("SELECT " + FN_NODEPID + " FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + nodeid, 1);
      m_TmpRs.select("SELECT " + FN_NODEID + ", "  + FN_NODELABEL + ", "  + FN_NODEDEPTH + 
                     " FROM " + m_Tbl + 
                     " WHERE " + FN_NODEPID + " = " + m_TmpRs.getField(0, 0) + " AND " +
                     FN_NODEID + " <> " + nodeid);
      
      int c = 0;
      while (m_TmpRs.next())
      {
        ret[c]          = new Node();
        ret[c].m_Id     = Tools.stringToInt(m_TmpRs.getField(FN_NODEID));
        ret[c].m_Pid    = nodeid;
        ret[c].m_Depth  = Tools.stringToInt(m_TmpRs.getField(FN_NODEDEPTH));
        ret[c].m_Label  = m_TmpRs.getField(FN_NODELABEL);
        c++;
      }
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }
  
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public int getChildsCount(int nodeid)
  {
    int ret = 0;
    
    try
    {
      m_TmpRs.select("SELECT COUNT(" + FN_NODEID + ") FROM " + m_Tbl + 
                     " WHERE " + FN_NODEPID + " = " + nodeid, 1);
      
      ret = Tools.stringToInt(m_TmpRs.getField(0, 0));
    } 
    catch (Exception ex) 
    {
      ex.printStackTrace();
    }
    
    return ret;
  }
  
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public int[] getChildsId(int nodeid)
  {
    int[] ret = new int[getChildsCount(nodeid)];

    try
    {
      m_TmpRs.select("SELECT " + FN_NODEID + " FROM " + m_Tbl + " WHERE " + FN_NODEPID + " = " + nodeid);
      
      try 
      {
        int c = 0;
        while (m_TmpRs.next())
        {
          ret[c] = Integer.parseInt(m_TmpRs.getField(c, 0));
          c++;
        }
      } 
      catch(NumberFormatException ex) {ex.printStackTrace();}
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }
  
  /**
   * 
   * @return the greates value for a node depth.
   */
  public int getMaxDepth()
  {
    int ret = 0;

    try
    {
      m_TmpRs.select("SELECT MAX(" + FN_NODEDEPTH + ") FROM " + m_Tbl, 1);
      ret = Tools.stringToInt(m_TmpRs.getField(0, 0));
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }
  
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public Node[] getChilds(int nodeid)
  {
    Node[] ret = new Node[getChildsCount(nodeid)];

    try
    {
      m_TmpRs.select("SELECT " + FN_NODEID + ", " + FN_NODEDEPTH + ", " + FN_NODELABEL + 
                     " FROM " + m_Tbl + " WHERE " + FN_NODEPID + " = " + nodeid);
      
      int c = 0;
      while (m_TmpRs.next())
      {
        ret[c]          = new Node();
        ret[c].m_Id     = Tools.stringToInt(m_TmpRs.getField(FN_NODEID));
        ret[c].m_Pid    = nodeid;
        ret[c].m_Depth  = Tools.stringToInt(m_TmpRs.getField(FN_NODEDEPTH));
        ret[c].m_Label  = m_TmpRs.getField(FN_NODELABEL);
        
        c++;
      }
    } 
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
    
    return ret;
  }
  
  public void CheckDBConn()
  {
    if (!m_Conn.isValid())
      throw new Error(ERR_NO_CONN);
  }
  
  //GETTER ---------------------------------------------------------------------
  public String GetDBTableName() {return m_Tbl;}
  public String GetDBConnDSN() {return m_Conn.getDSN();}
  
  //Inner class Node ***********************************************************
  /**
   * An utility class to wrap the concept of NaryTree node.
   * Four members are available to easily get node properties.
   */
  public class Node
  {
    public int    m_Id    = 0;
    public int    m_Pid   = 0;
    public int    m_Depth = 0;
    public String m_Label = "";
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~
    public Node() {}
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * 
     * @param nodeid 
     */
    public Node(int nodeid)
    {
      try
      {
        m_TmpRs.select("SELECT " + FN_NODEPID + ", " + FN_NODEDEPTH + ", " + FN_NODELABEL + 
                       " FROM " + m_Tbl + " WHERE " + FN_NODEID + " = " + nodeid);
        m_TmpRs.first();

        m_Id    = nodeid;
        m_Pid   = Tools.stringToInt(m_TmpRs.getField(FN_NODEPID));
        m_Depth = Tools.stringToInt(m_TmpRs.getField(FN_NODEDEPTH));
        m_Label = m_TmpRs.getField(FN_NODELABEL);
      } 
      catch (Exception ex) {ex.printStackTrace();}
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Getter/Setter
/*    
    public String getId   () {return m_Id;}
    public String getPid  () {return m_Pid;}
    public String getLabel() {return m_Label;}
    public String getDepth() {return m_Depth;}
//    public int getId      () {return m_Id;}
//    public int getPid     () {return m_Pid;}
//    public int getDepth   () {return m_Depth;}
    public void setId   (String id)     {m_Id = id;}
    public void setPid  (String pid)    {m_Pid = pid;}
    public void setLabel(String label)  {m_Label = label;}
    public void setDepth(String depth)  {m_Depth = depth;}
*/    
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * 
     * @return 
     */
    public String toString()
    {
      String ret = "";
      
      for(int j=0; j<m_Depth; j++)
        ret += "-";

      return ret + "[" + m_Id + "-" + m_Label + "]\n";
    }
  }
  //End of class Node **********************************************************
  
  /**
   * 
   * @param nodeid 
   * @return 
   */
  public String hierarchyToString(int nodeid)
  {
    String ret = (new Node(nodeid)).toString();
    Node[] childs = getChilds(nodeid);
    
    if (childs.length == 0)
      return ret;
    
    for(int i=0; i < childs.length; i++)
      ret += hierarchyToString(childs[i].m_Id);
    
    return ret;
  }

  /**
   * 
   * @return 
   */
  public String toString()
  {
    String ret = "";
    Node[] roots = getRoots();
    
    for(int i=0; i<roots.length; i++)
      ret += hierarchyToString(roots[i].m_Id);
    
    return ret;
  }
} // End of class NaryTree 
