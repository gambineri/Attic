/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.web;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.MalformedURLException;
import java.net.URL;

import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * This servlet returns the requested resource file as a stream.
 * It works on either a locally or remotely stored resource file, either on a 
 * resource contained within the jtxlib itself. In the first case only the res_*
 * parameters are used while the jtx_* ones are ignored;
 * vice-versa, in the second case, only the jtx_* parameters are used.
 * <p>
 *
 * <u>Parameters of the request:</u><br>
 * <dl>
 * <dt><b>res_path_var</b>:</dt>
 *    <dd>
 *      identifies the name of the variable in the application context containing  
 *      the local or remote location of the requested resource (usually configured in <app>.ini).
 *    </dd>
 *
 * <dt><b>res_filename</b>:</dt>
 *    <dd>
 *      this is the name of the requested resource.
 *    </dd>
 * 
 * <dt><b>jtx_resname</b>:</dt>
 *    <dd>
 *      the requested resource path and filename within the jtxlib. <br>
 *      For example, if the res.xxx file is in the package jtxlib.web.yyy, 
 *      the value to be passed in this parameter is <i>jtxlib/web/yyy/res.xxx</i>.
 *    </dd>
 *
 * <dt><b>mime_type</b>:</dt>
 *    <dd>
 *      optional, the mime to be set for the response.
 *    </dd>
 *
 * <dt><b>disposition</b>:</dt>
 *    <dd>
 *      optional, the content-disposition to be set for the response.
 *    </dd>
 *
 * <dt><b>r_tag</b>:</dt>
 *    <dd>
 *      optional, a tag to be replaced with <b>r_val</b> at streaming time.
 *      All the occurrences will be replaced.
 *      At moment (2009.04.30) this feature is available only for jtxlib resources.
 *    </dd>
 *
 * <dt><b>r_val</b>:</dt>
 *    <dd>
 *      optional, the value to be used in place of <b>r_tag</b> at streaming time.
 *      If a null string is passed (i.e. 'r_val=' in query string), the <b>r_tag</b> 
 *      will be removed everywhere.
 *
 *    </dd>
 * </dl>
 *
 * @author gambineri
 * @version 
 */
public class ResourceStreamer extends HttpServlet
{
  private static final long serialVersionUID = 7144360327773089172L;
  
  /** Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
   * @param request servlet request
   * @param response servlet response
   * @throws ServletException
   * @throws IOException 
   */
  protected void processRequest(HttpServletRequest request, HttpServletResponse response)
  throws ServletException, IOException
  {
    String jtx_resname  = request.getParameter("jtx_resname");
    String mime_type    = request.getParameter("mime_type");
    String disposition  = request.getParameter("disposition");

    if (mime_type != null && mime_type.length() > 0)
      response.setContentType(mime_type);
    
    if (disposition != null && disposition.length() > 0)
      response.setHeader("Content-disposition", disposition);   
    
    if (jtx_resname != null && jtx_resname.length() != 0)
      StreamJTXLIBResource(request, response);
    else
      StreamResource(request, response);
  }

  //----------------------------------------------------------------------------
  private void StreamResource(HttpServletRequest request, HttpServletResponse response) 
  throws IOException
  {
    String res_path_var = request.getParameter("res_path_var");
    String res_filename = request.getParameter("res_filename");

    //get real value from the application context
    String res_path_val = (String)this.getServletContext().getAttribute(res_path_var);

    if (res_path_var == null || res_path_var.length() == 0 ||
        res_filename == null || res_filename.length() == 0 ||
        res_path_val == null || res_path_val.length() == 0)
      return;
    
//    debug(response, res_path_val);

    BufferedInputStream  bis    = null;
    BufferedOutputStream bos    = null;
    ServletOutputStream  out    = response.getOutputStream();
    String               reqURL = "";
    
    try 
    {
      if (res_path_val.substring(1, 2).equals(":"))
        reqURL = "file://localhost/" + res_path_val + res_filename;
      else
        reqURL = res_path_val + res_filename;
          
      URL url = new URL(reqURL);

      // Use Buffered Stream for reading/writing.
      bis = new BufferedInputStream(url.openStream());
      bos = new BufferedOutputStream(out);

      byte[] buff = new byte[2048];
      int bytesRead;

      // Simple read/write loop.
      while((bytesRead = bis.read(buff, 0, buff.length)) != -1) 
        bos.write(buff, 0, bytesRead);
    } 
    catch(final MalformedURLException e) {throw e;} 
    catch(final IOException e) {throw e;} 
    finally 
    {
      if (bis != null)
        bis.close();
      if (bos != null)
        bos.close();
    }
  } 

  //----------------------------------------------------------------------------
  private void StreamJTXLIBResource(HttpServletRequest request, HttpServletResponse response) 
  {
    InputStream is = this.getClass().getClassLoader().getResourceAsStream(request.getParameter("jtx_resname"));
    String r_tag   = (request.getParameter("r_tag") == null ? "" : request.getParameter("r_tag"));
    String r_val   = (request.getParameter("r_val") == null ? "" : request.getParameter("r_val"));

    try 
    {
      ServletOutputStream out         = response.getOutputStream();
      StringBuffer        contentBuf  = new StringBuffer();
      
      int b = 0;      
      while ((b = is.read()) != -1) 
        contentBuf.append((char)b);

      // Do r_tag/r_val replacements
      String content = (contentBuf + "");
      if (r_tag.length() > 0)
        content = content.replaceAll(r_tag, r_val);

      //Send resource
      out.print(content);
    } 
    catch (FileNotFoundException e) {e.printStackTrace();} 
    catch (IOException e) {e.printStackTrace();}
  } 

  //----------------------------------------------------------------------------
  private void debug(HttpServletResponse response, String msg) throws IOException
  {
    PrintWriter out = response.getWriter();
    out.println(msg);
    out.close();
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
    return "Pushes server side resources with the requested content type and disposition.";
  }
  // </editor-fold>
}
