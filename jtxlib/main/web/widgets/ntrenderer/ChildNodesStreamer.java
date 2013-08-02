/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.web.widgets.ntrenderer;

import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import jtxlib.main.datastruct.narytree.NaryTree;
import jtxlib.main.sql.DBConnection;

/**
 * This servlet returns the html needed to dynamically add (child) nodes
 * to the narytree html table: it is used in conjunction with the JTXNaryTreeRenderer
 * mechanism of dynamic rendering, alternative to the 'toTABLE' method, which 
 * loads the whole tree structure at once (not that efficient if the tree is *very* big).<br>
 * Basically, it returns all the javascript code needed to render the new nodes.<br>
 * <p>
 * Parameters of the request:<br>
 * -------------------------<br>
 * <dl> 
 *  <dt>nodeid   :<dd>the id of the node for which the child nodes are requested.
 *  <dt>noderow  :<dd>the row index of the html table.
 *  <dt>dsn      :<dd>the ODBC dsn used for the connection to the database
 *  <dt>nt_table :<dd>the narytree table name (see {@link jtxlib.datastruct.narytree.NaryTree})
 *  <dt>renderer :<dd>the name of the JTXNaryTreeRenderer instance at session scope to properly script table rows.
 * <dl>
 * @author gambineri
 * @version 
 */
public class ChildNodesStreamer extends HttpServlet
{
  private static final long serialVersionUID = 8248471741770886684L;
  private final String CRLF = "\r\n";
  
  /** Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
   * @param request servlet request
   * @param response servlet response
   */
  protected void processRequest(HttpServletRequest request, HttpServletResponse response)
  throws ServletException, IOException
  {
    String nodeid   = request.getParameter("nodeid");
    String noderow  = request.getParameter("noderow");
    String dsn      = request.getParameter("dsn");
    String nt_table = request.getParameter("nt_table");
    String renderer = request.getParameter("renderer");
    
    if (nodeid    == null || nodeid.length()    == 0 ||
        noderow   == null || noderow.length()   == 0 ||
        dsn       == null || dsn.length()       == 0 ||
        nt_table  == null || nt_table.length()  == 0 ||
        renderer  == null || renderer.length()  == 0)
      return;
    
    NaryTreeRenderer ntr = (NaryTreeRenderer)request.getSession().getAttribute(renderer);
    
    DBConnection conn = null;
    conn = new DBConnection(dsn, "", "");
    
    NaryTree nt = new NaryTree(conn, nt_table);
    ntr.setNaryTree(nt);
    
    PrintWriter out = response.getWriter();
    response.setContentType("text/html; charset=UTF-8");
    out.println(scriptFirstGeneration(ntr, nodeid, noderow));

    out.close();
    conn.close();    
  }
  
  private String scriptFirstGeneration(NaryTreeRenderer ntr, String nodeid, String noderow)
  {
    String  ret           = "<html><head><script>" + CRLF + CRLF;
//    Node[]  childs        = ntr.getNaryTree().getChilds(Tools.stringToInt(nodeid));
//    int     noderowstart  = Tools.stringToInt(noderow);
    
    ntr.setLoadMethod(NaryTreeRenderer.LoadMethod.LM_INCREMENTAL);
    
    ret += ntr.dynCreateChildsToHiddenFrame(nodeid, noderow);  
    ret += "</script></head></html>";
    
    return ret;
  }
    
  // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
  /** Handles the HTTP <code>GET</code> method.
   * @param request servlet request
   * @param response servlet response
   */
  protected void doGet(HttpServletRequest request, HttpServletResponse response)
  throws ServletException, IOException
  {
    processRequest(request, response);
  }
  
  /** Handles the HTTP <code>POST</code> method.
   * @param request servlet request
   * @param response servlet response
   */
  protected void doPost(HttpServletRequest request, HttpServletResponse response)
  throws ServletException, IOException
  {
    processRequest(request, response);
  }
  
  /** Returns a short description of the servlet.
   */
  public String getServletInfo()
  {
    return "Short description";
  }
  // </editor-fold>
}
