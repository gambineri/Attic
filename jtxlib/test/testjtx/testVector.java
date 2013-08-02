package jtxlib.test.testjtx;

import jtxlib.main.datastruct.Vector;

class testVector
{
  final int NELEMS = 10000;
  final int REMOVE_NELEMS = 1000;
  final int REMOVEIDX = 500;
  final int TOTREMOVES = 300;

  //----------------------------------------------------------------------------
  public void showMethods()
  {
    Vector v = new Vector(10);

    for (int i=0; i<10; i++)
      v.addElement((char)('a' + i));

    System.out.println("showMethods - initial vector:");
    System.out.println(v);
    
    v.removeElement(4);
    System.out.println("showMethods - removed object at idx 4 - new vector:");
    System.out.println(v);

    v.removeElement(v.size());
    System.out.println("showMethods - removed object at idx size() - new vector:");
    System.out.println(v);

    v.removeElement(0);
    System.out.println("showMethods - removed object at idx 0 - new vector:");
    System.out.println(v);

    v.removeElement(v.size()-1);
    System.out.println("showMethods - removed object at idx 'size()-1' - new vector:");
    System.out.println(v);
    
    v.removeTail();
    System.out.println("showMethods - removed tail - new vector:");
    System.out.println(v);

    v.removeAllElements();
    System.out.println("showMethods - removed all - new vector:");
    System.out.println(v);
  }

  //----------------------------------------------------------------------------
  public void add_JTXVector()
  {
    String    s = "cico";
    Vector v = null;

    long curmilli = System.currentTimeMillis();

    for (int j=0; j<NELEMS; j++)
    {
      v = new Vector(NELEMS);
      for (int i=0; i<NELEMS; i++)
        v.addElement(s);
    }

    long curmilliend = System.currentTimeMillis();
    System.out.println("add_JTXVector: " + (curmilliend - curmilli) + " millis.");
  }

  //----------------------------------------------------------------------------
  public void add_Vector()
  {
    String s = "cico";
    java.util.Vector<String> v = null;

    long curmilli = System.currentTimeMillis();

    for (int j=0; j<NELEMS; j++)
    {
      v = new java.util.Vector<String>(NELEMS);
      for (int i=0; i<NELEMS; i++)
        v.addElement(s);
    }

    long curmilliend = System.currentTimeMillis();
    System.out.println("add_Vector: " + (curmilliend - curmilli) + " millis.");
  }

  //----------------------------------------------------------------------------
  public void removeTail_JTXVector()
  {
    String    s = "cico";
    Vector v = null;

    long curmilli = System.currentTimeMillis();

    for (int j=0; j<NELEMS; j++)
    {
      v = new Vector(NELEMS);
      for (int i=0; i<NELEMS; i++)
        v.addElement(s);

      for (int i=0; i<NELEMS; i++)
        v.removeTail();
    }

    long curmilliend = System.currentTimeMillis();
    System.out.println("removeTail_JTXVector: " + (curmilliend - curmilli) + " millis.");
  }

  //----------------------------------------------------------------------------
  public void removeTail_Vector()
  {
    String s = "cico";
    java.util.Vector<String> v = null;
    

    long curmilli = System.currentTimeMillis();

    for (int j=0; j<NELEMS; j++)
    {
      v = new java.util.Vector<String>(NELEMS);
      for (int i=0; i<NELEMS; i++)
        v.addElement(s);

      for (int i=NELEMS-1; i>=0; i--)
        v.remove(i);
    }

    long curmilliend = System.currentTimeMillis();
    System.out.println("removeTail_Vector: " + (curmilliend - curmilli) + " millis.");
  }

  //----------------------------------------------------------------------------
  public void remove_JTXVector()
  {
    String    s = "cico";
    Vector v = null;
    
    long curmilli = System.currentTimeMillis();

    for (int j=0; j<REMOVE_NELEMS; j++)
    {
      v = new Vector(REMOVE_NELEMS);
      for (int i=0; i<REMOVE_NELEMS; i++)
        v.addElement(s);

      for (int i=0; i<TOTREMOVES; i++)
        v.removeElement(REMOVEIDX);
    }

    long curmilliend = System.currentTimeMillis();
    System.out.println("remove_JTXVector: " + (curmilliend - curmilli) + " millis.");
  }

  //----------------------------------------------------------------------------
  public void remove_Vector()
  {
    String          s = "cico";
    java.util.Vector<String>  v = null;
    
    long curmilli = System.currentTimeMillis();

    for (int j=0; j<REMOVE_NELEMS; j++)
    {
      v = new java.util.Vector<String>(REMOVE_NELEMS);
      for (int i=0; i<REMOVE_NELEMS; i++)
        v.addElement(s);

      for (int i=0; i<TOTREMOVES; i++)
        v.removeElement(REMOVEIDX);
    }

    long curmilliend = System.currentTimeMillis();
    System.out.println("remove_Vector: " + (curmilliend - curmilli) + " millis.");
  }

  //----------------------------------------------------------------------------
  public void runTest()
  {
    showMethods();
//    add_JTXVector();
//    add_Vector();
//    removeTail_JTXVector();
//    removeTail_Vector();
    remove_JTXVector();
    remove_Vector();
  }
}
