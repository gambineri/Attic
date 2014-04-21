package com.imdp;

import java.util.ArrayDeque;
import java.util.Deque;

/**
 * Created by massimo on 4/17/14.
 */
public class BinaryTree {

  interface Printable {
    public String print();  
  }

  /* Internal class Element to contain information to be associated to the Node */
  class Element implements Printable {
    private String mDescription = "n/a";
    @Override
    public String print() {
      return mDescription;
    }

    Element(String descr) {mDescription = descr;}
    Element(int descr) {mDescription = Integer.toString(descr);}
  }
  
  /* Internal class Node  */
  class Node<T extends Printable> {
    private T mData;
    Node mLeft;
    Node mRight;

    Node(T element) {mData = element;}
    Node(T element, Node nl, Node nr){mData=element; mLeft=nl; mRight=nr;}

    void setLeft(Node nl) {mLeft=nl;}
    void setRight(Node nr) {mRight=nr;}

    String print() {
      return mData.print();
    }
  }


/* Breadth-First iterative (non recursive) with structures
 ******************************************************************/
  String BFIterative(Node<Element> root) {
    int l = 1;
    int n = 0;
    String ret = "";
    Deque<Node<Element>> queue = new ArrayDeque<Node<Element>>();

    queue.offer(root);
    while (!queue.isEmpty()) {
      Node<Element> node = queue.poll();
      ret += node.print() + " ";
      l--;

      if (node.mLeft != null) {
        queue.offer(node.mLeft);
        n++;
      }
      if (node.mRight != null) {
        queue.offer(node.mRight);
        n++;
      }
      if (l==0 && n>0) {
        l = n;
        n = 0;
        ret += "\n";
      }
    }
    return ret;
  }

 /* Depth-First recursive
 ******************************************************************/
  String DFInOrderRecursive(Node root) {
    String ret = "";

    if (root == null) return ret;

    ret += DFInOrderRecursive(root.mLeft);
    ret += root.print() + " ";
    ret += DFInOrderRecursive(root.mRight);

    return ret;
  }

  /* Depth-First In-Order iterative with structures
  ******************************************************************/
  String DFInOrderIterative(Node root) {
    String ret = "";
    Deque<Node<Element>> stack = new ArrayDeque<Node<Element>>();

    Node n = root;
    while(n != null){
      stack.push(n);
      n=n.mLeft;
    }

    while(!stack.isEmpty()){
      Node t = stack.pop();
      ret += t.print() + " ";
      t=t.mRight;

      while(t != null){
        stack.push(t);
        t = t.mLeft;
      }
    }

    return ret;
  }

/*===================================================================
 * Testing algos
 */
  Node createBinaryTree() {
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

  Node<Element> createBinaryTree2() {
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

  public static void main(String[] args) {
    BinaryTree bt = new BinaryTree();
    Node<Element> tree = bt.createBinaryTree2();

    System.out.println("Tree level by level (Breadth-First):\n" + bt.BFIterative(tree) + "\n");
    System.out.println("Depth-First In-Order with recursion:\n" + bt.DFInOrderRecursive(tree) + "\n");
    System.out.println("Depth-First In-Order iterative:\n" + bt.DFInOrderIterative(tree) + "\n");

  }
}
