package com.imdp;

import java.util.ArrayDeque;
import java.util.Deque;

/**
 * Created by massimo on 4/17/14.
 */
public class BinaryTree {

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

  /* Depth-First In-Order iterative without aux structures.
   * This algo traverses the binary tree without recursion and without stack.
   * It is also known as Morris Traversal.
   ******************************************************************/
  /*  */
  String DFInOrderIterativeNoStruct(Node root)
  {
    String ret = "";
    Node current, pre;

    if(root == null)
      return ret;

    current = root;
    while(current != null) {
      if(current.mLeft == null) {
        ret += current.print() + " ";
        current = current.mRight;
      }
      else {
        /* Find the inorder predecessor of current */
        pre = current.mLeft;
        while(pre.mRight != null && pre.mRight != current)
          pre = pre.mRight;
   
        /* Make current as right child of its inorder predecessor */
        if(pre.mRight == null)
        {
          pre.mRight = current;
          current = current.mLeft;
        }
               
        /* Revert the changes made in if part to restore the original 
          tree i.e., fix the right child of predecssor */
        else
        {
          pre.mRight = null;
          ret += current.print() + " ";
          current = current.mRight;
        } /* End of if condition pre.mRight == NULL */
      } /* End of if condition current->left == NULL*/
    } /* End of while */

    return ret;
  }
} // *** End of public class BinaryTree

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
