package com.imdp;

public class Singleton {
  private final static Singleton m_This = new Singleton();
  private Singleton(){}
  public static Singleton getSingleton() { return m_This; }
}