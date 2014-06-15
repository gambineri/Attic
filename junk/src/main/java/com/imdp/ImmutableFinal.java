package com.imdp;

/**
 * Created by massimo on 4/24/14.
 */
public class ImmutableFinal {

  public String a = "pippo";
  public final String b = "cico";

  public void test() {

    System.out.println("a: " + a.hashCode() + " - b: " + b.hashCode());

    a = "zzz";

    System.out.println("a = " + a + " - a: " + a.hashCode() + " - b: " + b.hashCode());

  }
}
