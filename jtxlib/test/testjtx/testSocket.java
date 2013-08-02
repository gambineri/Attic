package jtxlib.test.testjtx;

import jtxlib.main.aux.Debug;
import jtxlib.main.tcp.SocketClient;
import jtxlib.main.tcp.SocketServer;

public class testSocket
{
  Debug        m_Dbg = new Debug();
  SocketServer m_Server;
  SocketClient m_Client;
  
  public void runTest(String args[])
  {
    if (args[0].equals("server"))
    {
      m_Server = new SocketServer();
      m_Server.init(1234);
      m_Server.accept();
      Debug.Dbg(m_Server.receiveLine());
      m_Server.send("damme un sett'ottocentomila lire");
      try{Thread.sleep(1000);}catch(InterruptedException ie){}
    }
    else if (args[0].equals("client"))
    {
      m_Client = new SocketClient();
      m_Client.connect("localhost", 1234);
      m_Client.send("che te debbo da`?");
      Debug.Dbg(m_Client.receiveLine());
    }
    else if (args[0].equals("tannit"))
    {
      m_Client = new SocketClient();

      //Tannit connection
      m_Client.connect("cannonau.aitecsa.com", 80);

      //cosi` arriva pure l'header http      
      m_Client.send("GET /cgi-bin/tannit.exe?tpl=cico HTTP/1.0\r\nHost: cannonau.aitecsa.com \r\nUser-Agent: testjtx\r\n\r\n");

      //cosi` arriva solo il contenuto
//      m_Client.send("GET /cgi-bin/tannit.exe?tpl=cico");
      
//      m_Client.send("GET /cgi-bin/tannit.exe?mime=image%2Fgif");
//    
//      byte  rets[]  = new byte[5000];
//      byte  appo[]  = new byte[5000];
//      int   read    = 0;
//      int   curs    = 0;
//      
//      while ((read = m_Client.receiveBytes(appo)) != -1)
//      {
//        for(int i=0; i<read; i++)
//          rets[curs + i] = appo[i];
//        curs += read;
//      }
//      
//      byte  imma[]  = new byte[curs];
//      for(int i=0; i<curs; i++)
//        imma[i] = rets[i];
//      
//      m_Dbg.Dbg("->> " + curs);
//  
//      Debug fd = new Debug("U:\\massimo\\images\\aaa.gif");    
//      fd.send2file(new String(imma));
    }
  }  
}
