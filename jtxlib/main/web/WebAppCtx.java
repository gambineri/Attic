/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.web;

/**
 *
 * @author gambineri
 */
  

import java.util.*;
import java.io.FileInputStream;
import javax.servlet.ServletContext;


public class WebAppCtx
{
  private final String APP_CTX_LOADED_ATTR_NAME  = "APPLICATION_CONTEXT_LOADED";
  private final String APP_CTX_LOADED_ATTR_VALUE = "YES";

  /** Creates a new instance of JTXWebAppCtx */
  public WebAppCtx()
  {
  }

  public void Init(ServletContext app, String fileini) throws Exception
  {
    if (IsLoaded(app))
      return;

    FileInputStream fileIn = new FileInputStream(fileini);
    Properties props = new Properties();

    props.load(fileIn);
    Enumeration<?> keynames = props.propertyNames();

    String curkey = "";
    for (; keynames.hasMoreElements();)
    {
      curkey = (String)keynames.nextElement();
      app.setAttribute(curkey, props.getProperty(curkey, ""));
      System.out.println(curkey + "=" + props.getProperty(curkey, ""));
    }

    app.setAttribute(APP_CTX_LOADED_ATTR_NAME, APP_CTX_LOADED_ATTR_VALUE);
  }

  public boolean IsLoaded(ServletContext app)
  {
    return (app.getAttribute(APP_CTX_LOADED_ATTR_NAME) == APP_CTX_LOADED_ATTR_VALUE);
  }
}
