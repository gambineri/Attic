package com.imdp;

import java.util.Arrays;
import java.util.BitSet;
import java.util.Random;

public class Junk {

  public static void main(String[] args) {
    minWindow("-b--a-ac-b--a--b--c-a--c-b--".toCharArray(), "abc".toCharArray());


  }

  static int minWindow(char[] str, char[] set) {
    int min = -1;
    int start = 0;
    BitSet found = new BitSet(set.length);

    found.clear();
    
    for (int i=0; i<str.length; i++) {
      for (int j=0; j<set.length; j++) {
        if ((str[i] == set[j]) && (found.get(j) == false)) {
          min = i;
          found.set(j);
        }
      }
    }

    return min;
  }

  static void stringComparison() {
    String a  = "aone";
    String b = "aone";
    String c = new String("aone");
    String d = new String("aone");

    System.out.println("a==b is " + (a==b));
    System.out.println("a==c is " + (a==c));
    System.out.println("a==d is " + (a==d));
    System.out.println("b==c is " + (b==c));
    System.out.println("b==d is " + (b==d));
    System.out.println("c==d is " + (c==d));

    System.out.println("a.equals(b) is " + a.equals(b));
    System.out.println("a.equals(c) is " + a.equals(c));
    System.out.println("a.equals(d) is " + a.equals(d));
    System.out.println("b.equals(c) is " + b.equals(c));
    System.out.println("b.equals(d) is " + b.equals(d));
    System.out.println("c.equals(d) is " + c.equals(d));
  }

  static void randon() {
    for (int iB=0; iB<15; iB++)
      System.out.println((new Random().nextInt(3)) + "");
  }

  static void splitArray() {
    int[] A = new int[]{0, 1, 2, 3, 4, 5};
    int[] B = new int[]{2, 1, 3};

    int[][] iStep1 = new int[3][];

    int idx = 0;
    for (int iB=0; iB<3; iB++) {
      iStep1[iB] = Arrays.copyOfRange(A, idx, idx+B[iB]);
      idx += B[iB];
    }
  }
}


