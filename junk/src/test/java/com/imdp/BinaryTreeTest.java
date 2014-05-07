package com.imdp;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

/**
 * Created by massimo on 4/21/14.
 */
public class BinaryTreeTest {

  private BinaryTree m_BTObj = new BinaryTree();
  private Node m_BT01;
  private Node m_BT02;

  @Before
  public void setUp() throws Exception {
    m_BT01 = createBinaryTree01();
    m_BT02 = createBinaryTree02();
  }

  @After
  public void tearDown() throws Exception {
    /* nothing to do at mom */
  }

  @Test
  public void testDFInOrderIterativeNoStruct() throws Exception {
    // AKA Morris traversal
    Assert.assertEquals("Depth-First In-Order iterative no-structs (Morris traversal) is failing.",
      m_BTObj.DFInOrderIterativeNoStruct(m_BT01),
      "G M C A H D I root E B N L F ");

    Assert.assertEquals("Depth-First In-Order iterative no-structs (Morris traversal) is failing.",
      m_BTObj.DFInOrderIterativeNoStruct(m_BT02),
      "3 5 6 7 10 12 13 15 16 18 20 23 ");

//    System.out.println(m_BTObj.DFInOrderIterativeNoStruct(m_BT01));
//    System.out.println(m_BTObj.DFInOrderIterativeNoStruct(m_BT02));
  }

  @Test
  public void testBFIterative() throws Exception {
    Assert.assertEquals("Breadth-First iterative is failing.",
                        m_BTObj.BFIterative(m_BT01),
                        "root \n" +
                        "A B \n" +
                        "C D E F \n" +
                        "G H I L \n" +
                        "M N ");

    Assert.assertEquals("Breadth-First iterative is failing.",
                        m_BTObj.BFIterative(m_BT02),
                        "15 \n" +
                        "5 16 \n" +
                        "3 12 20 \n" +
                        "10 13 18 23 \n" +
                        "6 \n" +
                        "7 ");
  }

  @Test
  public void testDFInOrderRecursive() throws Exception {
    Assert.assertEquals("Depth-First In-Order recursive is failing.",
                        m_BTObj.DFInOrderRecursive(m_BT01),
                        "G M C A H D I root E B N L F ");

    Assert.assertEquals("Depth-First In-Order recursive is failing.",
                        m_BTObj.DFInOrderRecursive(m_BT02),
                        "3 5 6 7 10 12 13 15 16 18 20 23 ");
  }

  @Test
  public void testDFInOrderIterative() throws Exception {
    Assert.assertEquals("Depth-First In-Order iterative is failing.",
      m_BTObj.DFInOrderIterative(m_BT01),
      "G M C A H D I root E B N L F ");

    Assert.assertEquals("Depth-First In-Order iterative is failing.",
      m_BTObj.DFInOrderIterative(m_BT02),
      "3 5 6 7 10 12 13 15 16 18 20 23 ");
  }

  private Node createBinaryTree01() {
/*
 *                 root
 *               /      \
 *              A        B
 *             / \      / \
 *            C   D    E   F
 *           /   / \      /
 *          G   H   I    L
 *           \          /
 *            M        N
 */

    Node<Element> root  = new Node<Element>(new Element("root"));
    Node<Element> a     = new Node<Element>(new Element("A"));
    Node<Element> b     = new Node<Element>(new Element("B"));
    Node<Element> c     = new Node<Element>(new Element("C"));
    Node<Element> d     = new Node<Element>(new Element("D"));
    Node<Element> e     = new Node<Element>(new Element("E"));
    Node<Element> f     = new Node<Element>(new Element("F"));
    Node<Element> g     = new Node<Element>(new Element("G"));
    Node<Element> h     = new Node<Element>(new Element("H"));
    Node<Element> i     = new Node<Element>(new Element("I"));
    Node<Element> l     = new Node<Element>(new Element("L"));
    Node<Element> m     = new Node<Element>(new Element("M"));
    Node<Element> n     = new Node<Element>(new Element("N"));

    root.setLeft(a);
    root.setRight(b);
    a.setLeft(c);
    a.setRight(d);
    c.setLeft(g);
    d.setLeft(h);
    d.setRight(i);
    g.setRight(m);
    b.setLeft(e);
    b.setRight(f);
    f.setLeft(l);
    l.setLeft(n);

    return root;
  }

  private Node<Element> createBinaryTree02() {
/*
 *                  15
 *                /    \
 *              5       16
 *             / \        \
 *            3   12       20
 *               /  \     /  \
 *              10  13   18   23
 *             /
 *            6
 *             \
 *              7
 */
    Node<Element> e3  = new Node<Element>(new Element(3));
    Node<Element> e5  = new Node<Element>(new Element(5));
    Node<Element> e6  = new Node<Element>(new Element(6));
    Node<Element> e7  = new Node<Element>(new Element(7));
    Node<Element> e10 = new Node<Element>(new Element(10));
    Node<Element> e12 = new Node<Element>(new Element(12));
    Node<Element> e13 = new Node<Element>(new Element(13));
    Node<Element> e15 = new Node<Element>(new Element(15));
    Node<Element> e16 = new Node<Element>(new Element(16));
    Node<Element> e18 = new Node<Element>(new Element(18));
    Node<Element> e20 = new Node<Element>(new Element(20));
    Node<Element> e23 = new Node<Element>(new Element(23));

    e15.setLeft(e5);
    e15.setRight(e16);
    e5.setLeft(e3);
    e5.setRight(e12);
    e12.setLeft(e10);
    e12.setRight(e13);
    e10.setLeft(e6);
    e6.setRight(e7);
    e16.setRight(e20);
    e20.setLeft(e18);
    e20.setRight(e23);

    return e15;
  }
}
