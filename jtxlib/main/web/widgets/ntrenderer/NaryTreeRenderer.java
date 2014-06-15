/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.web.widgets.ntrenderer;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import jtxlib.main.aux.Tools;
import jtxlib.main.datastruct.narytree.NaryTree;
import jtxlib.main.datastruct.narytree.NaryTree.Node;

/**
 *
 * @author gambineri
 */
public class NaryTreeRenderer
{
  private final byte   FIXED_CTRL_COLS              = 6;
  private final byte   ACTION_CTRL_COLS             = 2;
  private final String CRLF = "\r\n";
  
  // CSS names
  private final String CSS_FILENAME                 = "ntrenderer.css";
  private final String CSS_CLASS_MAIN_TABLE         = "NaryTreeTable";
  private final String CSS_CLASS_CTRL_COL           = "ControlColumn";
  private final String CSS_CLASS_TD_EXPCOLL         = "ExpColl";
  private final String CSS_CLASS_NODELABEL          = "NodeLabel";
  private final String CSS_CLASS_LEAFLABEL          = "NodeLeafLabel";

  // DOM names
  private final String DOM_MAIN_TABLE_ID            = "NTTID";
  private final String DOM_EXPCOLL_TD_ID            = "tdExpColl"; 
  private final String DOM_FOLDERIMG_TD_ID          = "tdFolderImg"; 
  private final String DOM_LABEL_TD_ID              = "tdLabel"; 
  private final String DOM_EXPCOLL_IMG_NAME         = "imgExpColl";
  private final String DOM_FOLDER_IMG_NAME          = "imgFolder";
  private final String DOM_LEAF_IMG_NAME            = "imgLeaf";

  // Javascript names
  private final String JS_FILENAME                  = "ntrenderer.js";
  private final String JS_EXPCOLL_FUNC_CALL         = "nt_expand_collapse(this);";

  // Error messages
  private final String ERR_NARYTREE_NULL            = "Cannot render: JTXNaryTree instance is null.";
  
  //Load methods
  public enum LoadMethod
  {
    LM_TOTAL,       // the whole tree is rendered
    LM_INCREMENTAL  // rendering is limited to the immediate underlying generation of child nodes
  }
  
  //Management variables
  private NaryTree m_NT          = null;
  private int         m_MaxDepth    = 0;
  private LoadMethod  m_LoadMethod  = LoadMethod.LM_TOTAL; //default
  private String      m_SessionName = ""; // the name of this object when used as a session obj for incremental loading
  private String      m_ChildNodesStreamerPath = ""; // the path for the ChildNodesStreamer servlet
    
  // Node images source paths
  private String      m_ImgCollapsedSrc = "";
  private String      m_ImgExpandedSrc  = "";
  private String      m_ImgFolderCollapsedSrc  = "";
  private String      m_ImgFolderExpandedSrc  = "";
  private String      m_ImgLeafSrc  = "";
  
  //DHTML event handlers
  private String      m_NodeLabelClick = "";

  //----------------------------------------------------------------------------
  public NaryTreeRenderer() {}

  //----------------------------------------------------------------------------
  public NaryTreeRenderer(NaryTree nt, String session_name, String child_nodes_streamer_path,
                             String img_collapsed, String img_expanded,
                             String img_folder_collapsed, String img_folder_expanded,
                             String img_leaf)
  {
    m_NT = nt;
    m_MaxDepth = m_NT.getMaxDepth();
    m_ImgCollapsedSrc = img_collapsed;
    m_ImgExpandedSrc = img_expanded;
    m_ImgFolderCollapsedSrc = img_folder_collapsed;
    m_ImgFolderExpandedSrc = img_folder_expanded;
    m_ImgLeafSrc = img_leaf;
    m_SessionName = session_name;
    m_ChildNodesStreamerPath = child_nodes_streamer_path;
  }

  //----------------------------------------------------------------------------
  public NaryTreeRenderer(NaryTree nt,
                             String img_collapsed, String img_expanded,
                             String img_folder_collapsed, String img_folder_expanded,
                             String img_leaf)
  {
    m_NT = nt;
    m_MaxDepth = m_NT.getMaxDepth();
    m_ImgCollapsedSrc = img_collapsed;
    m_ImgExpandedSrc = img_expanded;
    m_ImgFolderCollapsedSrc = img_folder_collapsed;
    m_ImgFolderExpandedSrc = img_folder_expanded;
    m_ImgLeafSrc = img_leaf;
  }
  
  //----------------------------------------------------------------------------
  public NaryTreeRenderer(NaryTree nt)
  {
    m_NT = nt;
    m_MaxDepth = m_NT.getMaxDepth();
  }
  
  //----------------------------------------------------------------------------
  public void setProperties(String img_collapsed, String img_expanded,
                            String img_folder_collapsed, String img_folder_expanded,
                            String img_leaf)
  {
    m_ImgCollapsedSrc = img_collapsed;
    m_ImgExpandedSrc = img_expanded;
    m_ImgFolderCollapsedSrc = img_folder_collapsed;
    m_ImgFolderExpandedSrc = img_folder_expanded;
    m_ImgLeafSrc = img_leaf;
  }
  
  //----------------------------------------------------------------------------
  public void setSessionName(String session_name)
  {
    m_SessionName = session_name;
  }

  //----------------------------------------------------------------------------
  public void setChildNodesStreamerServlet(String child_nodes_streamer_path)
  {
    m_ChildNodesStreamerPath = child_nodes_streamer_path;
  }

  //----------------------------------------------------------------------------
  public void setNaryTree(NaryTree nt)
  {
    m_NT = nt;
    m_MaxDepth = m_NT.getMaxDepth();
  }
  
  //----------------------------------------------------------------------------
  public NaryTree getNaryTree()
  {
    return m_NT;
  }
  
  //----------------------------------------------------------------------------
  public String toTable()
  {
    CheckNaryTree();
    
    String ret    = "";
    Node[] roots  = m_NT.getRoots();
    int i = 0, j = 0;
    
    ret = "<table id=\"" + DOM_MAIN_TABLE_ID + "\" class=\"" + CSS_CLASS_MAIN_TABLE + "\">" + CRLF;
    ret += "<thead><tr>";

    //Fixed control columns
    ret += "<td class=\"" + CSS_CLASS_CTRL_COL + "\">Depth</td>";
    ret += "<td class=\"" + CSS_CLASS_CTRL_COL + "\">NodeId</td>";
    ret += "<td class=\"" + CSS_CLASS_CTRL_COL + "\">NodePid</td>";
    ret += "<td class=\"" + CSS_CLASS_CTRL_COL + "\">ECstat</td>";
    ret += "<td class=\"" + CSS_CLASS_CTRL_COL + "\">Visible</td>";
    ret += "<td class=\"" + CSS_CLASS_CTRL_COL + "\">CLoaded</td>";

    for (j=0; j<(m_MaxDepth + ACTION_CTRL_COLS - 1); j++)
      ret += "<td></td>";

    ret += "</tr></thead>" + CRLF;
    ret += "<tbody>" + CRLF;
    
    for (i=0; i<roots.length; i++)
      ret += outNodeToTR(roots[i]);
    
    ret += "</tbody>" + CRLF +
           "</table>" + CRLF;
    
    return ret;
  }

  //----------------------------------------------------------------------------
  private String outNodeToTR(Node n)
  {
    CheckNaryTree();
    
    String  ret = "<tr style=\"display: " + 
                  (n.m_Depth == 1 || (m_LoadMethod == LoadMethod.LM_INCREMENTAL) ? 
                  "block" : "none") + ";\">";
    
    boolean has_childs = (m_NT.getChildsCount(n.m_Id) > 0);
    
    //Fixed index control column 0: node depth
    ret += "<td>" + n.m_Depth + "</td>";

    //Fixed index control column 1: node id
    ret += "<td>" + n.m_Id + "</td>";
    
    //Fixed index control column 2: parent node id
    ret += "<td>" + n.m_Pid + "</td>";
    
    //Fixed index control column 3: expanded/collapsed status
    ret += "<td>" + (has_childs ? "+" : "") + "</td>";

    //Fixed index control column 4: visibility status
    ret += "<td>" + (n.m_Depth == 1 ? "1" : "0") + "</td>";
    
    //Fixed index control column 5: 1 if childs already loaded, 0 o/w (for LM_TOTAL always 1)
    ret += "<td>" + (m_LoadMethod == LoadMethod.LM_TOTAL ? "1" : "0") + "</td>";
    
    //variable num of cells depending on the depth of the node.
    for (int j=0; j<n.m_Depth-1; j++)
      ret += "<td></td>";

    //Variable index control column 1: Expand/Collapse features
    if (has_childs)
      ret += "<td id=\"" + DOM_EXPCOLL_TD_ID + "\" " +
             "class=\"" + CSS_CLASS_TD_EXPCOLL + "\" " +
             "onclick=\"" + JS_EXPCOLL_FUNC_CALL + "\">" +
             writeExpCollImg(has_childs) + 
             "</td>";
    else
      ret += "<td>" +
             writeExpCollImg(has_childs) + 
             "</td>";

    //Variable index control column 2: Folder image
    if (has_childs)
      ret += "<td id=\"" + DOM_FOLDERIMG_TD_ID + "\">" + writeFolderImg(has_childs) + "</td>";
    else
      ret += "<td>" + writeLeafImg()+ "</td>";

    // Node Label
    ret += "<td id=\"" + DOM_LABEL_TD_ID + "\" " +
           "class=\"" + (has_childs ? CSS_CLASS_NODELABEL : CSS_CLASS_LEAFLABEL) + "\" " +
           "colspan=\"" + (m_MaxDepth + ACTION_CTRL_COLS - n.m_Depth) + "\" " +
           "onclick=\"highlightNodeLabel(this);" + m_NodeLabelClick + "\" " + 
           ">" + n.m_Label + "</td>";
      
    //End of node
    ret += "</tr>" + CRLF;
    
    if (m_LoadMethod == LoadMethod.LM_TOTAL)
    {
      // Recurse on child nodes
      Node[] childs  = m_NT.getChilds(n.m_Id);
      for (int i=0; i<childs.length; i++)
        ret += outNodeToTR(childs[i]);    
    }
    
    return ret;
  }

  //----------------------------------------------------------------------------
/*  START CREATING JAVASACRIPT CODE TO ADD CHILD NARYTREE NODES
    --- REMINDER ----
    when creating new dhtml nodes, the createElement method must be invoked by 
    the document that will *actually* contain the node, otherwise the newly 
    created node could have a different, local 'ownerDocument' and 'appendChild' 
    would raise an 'Invalid argument' error.
 *
 *  -- and here we go:
 *
    function onload_handler()
    {
      var tbody = getTableInDocument(parent.document, 'NTTID').tBodies[0];
      var row = parent.document.createElement('<tr>');

      tbody.appendChild(row);
    }
*/
  protected String dynCreateChildsToHiddenFrame(String nodeid, String noderow)
  {
    String ret = "";
    Node[] childs = m_NT.getChilds(Tools.stringToInt(nodeid));
    int    newrowstart = Tools.stringToInt(noderow) + 1;
    
    ret += "var nttbl = (parent.document.all ? parent.document.all['NTTID'] : " +
           "(parent.document.getElementById ? parent.document.getElementById('NTTID') : null));" + CRLF;
    ret += "var tbody = nttbl.tBodies[0];" + CRLF;
    ret += "var row, cell, img;" + CRLF + CRLF;
   
    for (int i=0; i<childs.length; i++)
    {
      ret += dynCreateNode(childs[i]);
      ret += "tbody.appendChild(row);" + CRLF;
      ret += "tbody.moveRow(tbody.rows.length-1, " + (newrowstart + i) + ");" + CRLF + CRLF;
    }

    return ret;
  }
  
  //----------------------------------------------------------------------------
  private String dynCreateNode(Node n)
  {
    CheckNaryTree();
    
    String ret = "";
    boolean has_childs = (m_NT.getChildsCount(n.m_Id) > 0);
    
    ret += dynCreateElement(null, "<tr style=\"display: block;\">", "row", null);
    
    //Fixed index control column 0: node depth
    ret += dynCreateElement("row", "<td>", "cell", n.m_Depth + "");
    
    //Fixed index control column 1: node id
    ret += dynCreateElement("row", "<td>", "cell", n.m_Id + "");
    
    //Fixed index control column 2: parent node id
    ret += dynCreateElement("row", "<td>", "cell", n.m_Pid + "");
    
    //Fixed index control column 3: expanded/collapsed status
    ret += dynCreateElement("row", "<td>", "cell", (has_childs ? "+" : null));

    //Fixed index control column 4: visibility status
    ret += dynCreateElement("row", "<td>", "cell", "1");
  
    //Fixed index control column 5: 1 if childs already loaded, 0 o/w
    ret += dynCreateElement("row", "<td>", "cell", "0");
    
    //variable num of cells depending on the depth of the node.
    for (int j=0; j<n.m_Depth-1; j++)
      ret += dynCreateElement("row", "<td>", "cell", null);

    //Variable index control column 1: Expand/Collapse features
    if (has_childs)
      ret += dynCreateElement("row", "<td id=\"" + DOM_EXPCOLL_TD_ID + "\" " + 
                                  "class=\"" + CSS_CLASS_TD_EXPCOLL + "\" " + 
                                  "onclick=\"" + JS_EXPCOLL_FUNC_CALL + "\">", "cell", null);
    else
      ret += dynCreateElement("row", "<td>", "cell", null);
    
    ret += dynCreateElement("cell", writeExpCollImg(has_childs), "img", null);

    //Variable index control column 2: Folder image
    if (has_childs)
    {
      ret += dynCreateElement("row", "<td id=\"" + DOM_FOLDERIMG_TD_ID + "\">", "cell", null);
      ret += dynCreateElement("cell", writeFolderImg(has_childs), "img", null);
    }
    else
    {
      ret += dynCreateElement("row", "<td>", "cell", null);
      ret += dynCreateElement("cell", writeLeafImg(), "img", null);
    }

    // Node Label
    ret += dynCreateElement("row", "<td id=\"" + DOM_LABEL_TD_ID + "\" " +
                                "class=\"" + (has_childs ? CSS_CLASS_NODELABEL : CSS_CLASS_LEAFLABEL) + "\" " +
                                "colspan=\"" + (m_MaxDepth + ACTION_CTRL_COLS - n.m_Depth) + "\" " +
                                "onclick=\"highlightNodeLabel(this);" + m_NodeLabelClick + "\">", "cell", n.m_Label);
      
    return ret;
  }
  
  //----------------------------------------------------------------------------
  private String dynCreateElement(String par_var, String element, String aux_var, String innerText)
  {
    if (element == null || element.length() == 0 || !(element.substring(0, 1).equals("<")))
      return "";
    
    return (aux_var == null ? "" : 
            aux_var + " = parent.document.createElement('" + element.replace("'", "\\'") + "');" + CRLF +
            (par_var == null ? "" : par_var + ".appendChild(" + aux_var + ");" + CRLF) +
           (innerText == null ? "" : (aux_var + ".innerText = '" + innerText.replace("'", "\\'") + "';" + CRLF)));
  }
  
  //----------------------------------------------------------------------------
  private String writeExpCollImg(boolean has_childs)
  {
    String ret = "";

    if (has_childs)
    {
      if (m_ImgCollapsedSrc.length() > 0)
        ret = "<img name=\"" + DOM_EXPCOLL_IMG_NAME + "\" src=\"" + m_ImgCollapsedSrc + "\">";
      else
        ret = "+";
    }
    
    return ret;
  }
  
  //----------------------------------------------------------------------------
  private String writeFolderImg(boolean has_childs)
  {
    String ret = "";

    if (has_childs)
    {
      if (m_ImgCollapsedSrc.length() > 0)
        ret = "<img name=\"" + DOM_FOLDER_IMG_NAME + "\" src=\"" + m_ImgFolderCollapsedSrc + "\">";
      else
        ret = "";
    }
    
    return ret;
  }
  
  //----------------------------------------------------------------------------
  private String writeLeafImg()
  {
    return (m_ImgLeafSrc.length() > 0 ?
            "<img name=\"" + DOM_LEAF_IMG_NAME + "\" src=\"" + m_ImgLeafSrc + "\">" : 
            "");
  }

  //----------------------------------------------------------------------------
  public String writeJSEngine()
  {
    String default_js_file = Tools.convertPackageToPath(this.getClass().getPackage(), "/") + "/" + JS_FILENAME;
    InputStream is = this.getClass().getClassLoader().getResourceAsStream(default_js_file);

    StringBuffer content = new StringBuffer();

    try 
    {
      int b = 0;
      while ((b = is.read()) != -1) 
        content.append((char)b);
    } 
    catch (FileNotFoundException e) {e.printStackTrace();} 
    catch (IOException e) {e.printStackTrace();}

    //Build the dynamic part of javascript (the one depending on the narytree client settings)
    String dyn_part = ""; 
    dyn_part += "var imgCollapsed = document.createElement('<img src=\"" + m_ImgCollapsedSrc + "\">');" + CRLF;
    dyn_part += "var imgExpanded  = document.createElement('<img src=\"" + m_ImgExpandedSrc + "\">');" + CRLF;
    dyn_part += "var imgFolderCollapsed = document.createElement('<img src=\"" + m_ImgFolderCollapsedSrc + "\">');" + CRLF;
    dyn_part += "var imgFolderExpanded  = document.createElement('<img src=\"" + m_ImgFolderExpandedSrc + "\">');" + CRLF;
    dyn_part += "var ChildNodesStreamer = '" + m_ChildNodesStreamerPath + "?renderer=" + m_SessionName + 
                "&dsn=" + m_NT.GetDBConnDSN() + "&nt_table=" + m_NT.GetDBTableName() + "';" + CRLF;
        
    return "<script>" + CRLF + dyn_part + content + CRLF + "</script>" + CRLF;
  }

  //----------------------------------------------------------------------------
  public String writeCSS()
  {
    String default_css_file = Tools.convertPackageToPath(this.getClass().getPackage(), "/") + "/" + CSS_FILENAME;
    InputStream is = this.getClass().getClassLoader().getResourceAsStream(default_css_file);

    StringBuffer content = new StringBuffer();

    try 
    {
      int b = 0;
      while ((b = is.read()) != -1) 
        content.append((char)b);      
    } 
    catch (FileNotFoundException e) {e.printStackTrace();} 
    catch (IOException e) {e.printStackTrace();}

    return "<style>" + CRLF + content + CRLF + "</style>" + CRLF;
  }

  //----------------------------------------------------------------------------
  /**
   * This function, like all other *setJS_* functions, assigns your javascript code 
   * to the OnClick event handler associated with ALL the nodes in the NaryTree.
   * It must be used only if behaviour in this respect is the same for all nodes.
   * @param on_node_label_click Any valid (in the context) piece of javascript code.
   */
  public void setJS_OnNodeLabelClick(String on_node_label_click)
  {
    m_NodeLabelClick = on_node_label_click;
  }
  
  //----------------------------------------------------------------------------
  public void setLoadMethod(LoadMethod lm)
  {
    m_LoadMethod = lm;
  }
  
  //----------------------------------------------------------------------------
  private void CheckNaryTree()
  {
    if (m_NT == null)
      throw new Error(ERR_NARYTREE_NULL);
  }
}


