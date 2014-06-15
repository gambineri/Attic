/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.tcp;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.NoRouteToHostException;
import java.net.Socket;

import jtxlib.main.aux.Debug;

public class SocketClient
{
  Debug         m_Dbg = new Debug();

  private Socket            connection;
  private DataOutputStream  outputStream;
  private BufferedReader    inputStream;
  private DataInputStream   dataInputStream;

  private int               m_ConnectionTimeout = 0;


  public SocketClient()
  {
  }

//----------------------------------------------------------
/**
 * Creates a new connection.
 * <P>
 */
  public boolean connect(String connAddress, int connPort)
  {
    boolean connavail = false;

    try
    {
      connection      = new Socket(connAddress, connPort);
      outputStream    = new DataOutputStream(connection.getOutputStream());
      inputStream     = new BufferedReader(new InputStreamReader(connection.getInputStream()));
      dataInputStream = new DataInputStream(connection.getInputStream());

      connavail       = true;
    }
    catch(NoRouteToHostException ex)
    {
      Debug.Dbg("JTXSocket.connect: " + ex);
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXSocket.connect:  " + ex);
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
      Debug.Dbg("JTXSocket.send: " + ex);
      return false;
    }

    return true;
  }

//----------------------------------------------------------
  public boolean sendNoCRLF(String message)
  {
    try
    {
      Debug.Dbg("sendNoCRLF: " + message);

      outputStream.write(message.getBytes(), 0, message.length());
      outputStream.flush();
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXSocket.sendNoCRLF: " + ex);
      return false;
    }

    return true;
  }

//----------------------------------------------------------
  public boolean sendBytes(byte[] message, int len)
  {
    try
    {
      outputStream.write(message, 0, len);
      outputStream.flush();
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXSocket.sendBytes: " + ex);
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
      Debug.Dbg("JTXSocket.receive: " + ex);
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
  public int receiveBytes(byte[] message)
  {
    int readBytes = 0;
    try
    {
      readBytes = dataInputStream.read(message);
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXSocket.receiveBytes: " + ex);
    }

    return readBytes;
  }

//----------------------------------------------------------
/**
 *Server closes connection
 * <P>
 */
  public void close()
  {
    try
    {
      connection.close();
      connection = null;
      inputStream.close();
      inputStream = null;
      outputStream.close();
      outputStream = null;
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXSocket.close: " + ex);
    }
  }

//----------------------------------------------------------
  public int getConnectionTimeout()
  {
    return m_ConnectionTimeout;
  }

//----------------------------------------------------------
  public void setConnectionTimeout(int timeout)
  {
    try
    {
      connection.setSoTimeout(timeout);
      m_ConnectionTimeout = timeout;
    }
    catch(IOException ex)
    {
      Debug.Dbg("JTXSocket.setConnectionTimeout: " + ex);
    }
  }
}
