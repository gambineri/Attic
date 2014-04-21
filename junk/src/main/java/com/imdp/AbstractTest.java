package com.imdp;

public class AbstractTest {
  public String absString = "ciao";

  public static void main(final String... args) {

    final Printer p = new Printer() {
      void printSomethingOther() {
        System.out.println("other");
      }

      void absMethod() {
        return;
      }

      @Override
      public void print() {
        System.out.println("world" + (++abs));
        printSomethingOther(); // works fine
        super.print();
      }
    };
    p.print();
    //p.printSomethingOther(); // does not work
  }
}

abstract class Printer {
  int abs = 5;
  public void print() {
    System.out.println("hello" + abs);
  }

  abstract void absMethod();
}