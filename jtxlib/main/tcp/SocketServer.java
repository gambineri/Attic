/* $RCSfile: JTXServerSocket.java,v $
 * $Revision: 1.3 $
 * $Author: Gambineri $
 * $Date: 2006/05/11 07:46:51 $
 *
 * developed by Massimo Gambineri
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 */

package jtxlib.main.tcp;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.NoRouteToHostException;
import java.net.ServerSocket;
import java.net.Socket;

import jtxlib.main.aux.Debug;

public class SocketServer
{

  Debug                  m_Dbg = new Debug();

  private ServerSocket      server;
  private Socket            client;

  private DataOutputStream  outputStream;
  private BufferedReader    inputStream;

  private int               m_serverPort    = 0;
  private int               m_ServerTimeout = 0;
  private int               m_ClientTimeout = 0;


  public SocketServer()
  {
  }

//----------------------------------------------------------
/**
 *Creates a new ServerSocket on serverPort
 * <P>
 */

  public boolean init(int serverPort)
  {
    try
    {
      m_serverPort = serverPort;
      server = new ServerSocket(m_serverPort);
    }
    catch(IOException ex)
    {
    	Debug.Dbg("JTXServerSocket.init: " + ex);
      return false;
    }

    return true;
  }

//----------------------------------------------------------
/**
 *Server accepts client connections (blocking)
 * <P>
 */

  public boolean accept()
  {
    try
    {
      client       = server.accept();
      outputStream = new DataOutputStream(client.getOutputStream());
      inputStream  = new BufferedReader(new InputStreamReader(client.getInputStream()));
    }
    catch(IOException ex)
    {
    	Debug.Dbg("JTXServerSocket.accept: " + ex);
      return false;
    }

    return true;
  }

//----------------------------------------------------------
/**
 *Server connects to the client
 * <P>
 */
  public boolean connect(String clientAddress, int clientPort)
  {
    boolean connavail = false;

    try
    {
      client = new Socket(clientAddress, clientPort);
      outputStream = new DataOutputStream(client.getOutputStream());
      inputStream  = new BufferedReader(new InputStreamReader(client.getInputStream()));
      connavail = true;
    }
    catch(NoRouteToHostException ex)
    {
      Debug.Dbg("JTXServerSocket.connect: NoRouteToHostException. " + ex);
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXServerSocket.connect: IOException. " + ex);
    }

    return connavail;
  }

//----------------------------------------------------------
  public boolean send(String message)
  {
    try
    {
      outputStream.writeBytes(message);
      outputStream.write(13);
      outputStream.write(10);
      outputStream.flush();
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXServerSocket.send: unable to send message." + ex);
      return false;
    }

    return true;
  }

//----------------------------------------------------------
  public String receiveLine()
  {
    String received = "";

    try
    {
      received = inputStream.readLine();
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXServerSocket.receive: " + ex);
      received = "";
    }

    return received;
  }

//----------------------------------------------------------
  public String receive()
  {
    String received = "";
    String appo = "";

    try
    {
      while ((appo = inputStream.readLine()) != null)
        received += appo;
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXSocket.receive: " + ex);
      received = "";
    }

    return received;
  }

//----------------------------------------------------------
/**
 *Server closes client connection
 * <P>
 */
  public void close()
  {
    try
    {
      client.close();
      client = null;
      inputStream.close();
      inputStream = null;
      outputStream.close();
      outputStream = null;
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXServerSocket.close: " + ex);
    }
  }

//----------------------------------------------------------
  public void shutdown()
  {
    try
    {
      server.close();
      server = null;
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXServerSocket.shutdown: " + ex);
    }

  }

//----------------------------------------------------------
  public int getServerTimeout()
  {
    return m_ServerTimeout;
  }

//----------------------------------------------------------
  public int getClientTimeout()
  {
    return m_ClientTimeout;
  }

//----------------------------------------------------------
  public void setServerTimeout(int timeout)
  {
    try
    {
      server.setSoTimeout(timeout);
      m_ServerTimeout = timeout;
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXServerSocket.setServerTimeout: " + ex);
    }
  }

//----------------------------------------------------------
  public void setClientTimeout(int timeout)
  {
    try
    {
      client.setSoTimeout(timeout);
      m_ClientTimeout = timeout;
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXServerSocket.setClientTimeout: " + ex);
    }
  }
}
