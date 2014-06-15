package com.imdp;

/**
 * Created by massimo on 4/21/14.
 */
public class StaticMeth {

  int a = 5;

//  StaticMeth(){}

  String getA(){return "SaticMeth " + a;}

  static String  buf = new String("Who's creating this object?");

  StaticMeth(){
    System.out.println(this.toString() + " - " + this.hashCode());
  }

  public static String theStaticMethod() {

    return buf;
  }
}

class B extends StaticMeth {

  @Override
  String getA(){a = 7; return "B " + a;}

}

