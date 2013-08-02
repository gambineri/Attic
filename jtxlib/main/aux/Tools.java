/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.aux;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.net.URL;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.StringTokenizer;

import javax.swing.JOptionPane;

public class Tools
{
  private final static String DEFAULT_DATE_FORMAT = "dd/MM/yyyy";
  private final static String DEFAULT_TIME_FORMAT = "HH:mm:ss";

  public Tools()
  {
  }

  /*****************************************************************************
                                    Message boxes
   ****************************************************************************/
  
//---------------------------------------------------------------------------------------------
  public static void msgBox(String msg)
  {
    JOptionPane.showMessageDialog(null, msg, "JTXTools Message Box", JOptionPane.INFORMATION_MESSAGE);
  }

//---------------------------------------------------------------------------------------------
  public static void msgBox(String msg, String title)
  {
    JOptionPane.showMessageDialog(null, msg, title, JOptionPane.INFORMATION_MESSAGE);
  }

//---------------------------------------------------------------------------------------------
  public static int confirmBox(String msg, String title)
  {
    return JOptionPane.showConfirmDialog(null, msg, title, JOptionPane.YES_NO_CANCEL_OPTION);
  }

//---------------------------------------------------------------------------------------------
  public static int confirmBoxY_N(String msg, String title)
  {
    return JOptionPane.showConfirmDialog(null, msg, title, JOptionPane.YES_NO_OPTION);
  }

  /*****************************************************************************
                              Date and time management
   ****************************************************************************/
  
//---------------------------------------------------------------------------------------------
  public static String timeConvert(GregorianCalendar gcalendar)
  {
    Integer intcal = new Integer(3600 * gcalendar.get(Calendar.HOUR_OF_DAY) + 
                                 60 * gcalendar.get(Calendar.MINUTE) + 
                                 gcalendar.get(Calendar.SECOND));
    return intcal.toString();
  }

//---------------------------------------------------------------------------------------------
  public static String timeConvert(Date d)
  {
    GregorianCalendar gc = new GregorianCalendar();
    gc.setTime(d);

    return timeConvert(gc);
  }

//---------------------------------------------------------------------------------------------
  public static String timeConvert(String str)
  {
    SimpleDateFormat sdf = new SimpleDateFormat(DEFAULT_TIME_FORMAT);
    GregorianCalendar gc = new GregorianCalendar();
    sdf.setCalendar(gc);
    gc.setLenient(false);

    try
    {
      Date dayDate = sdf.parse(str);
      gc.setTime(dayDate);
    }

    catch(ParseException pe)
    {
      Debug.Dbg("ParseException in timeConvert: " + pe.getMessage());
    }

    return timeConvert(gc);
  }

//---------------------------------------------------------------------------------------------
  public static int minsecConvert(String str)
  {
    int ret = 0;
    int min = 0;

    for (int i = 0; i < str.length(); i++)
    {
      if (str.charAt(i) == '\'')
      {
        ret = ret + ((new Integer(str.substring(min, i))).intValue() * 60);
        min = i + 1;
      }
      else if (str.charAt(i) == '\"')
        return (ret + (new Integer(str.substring(min, i))).intValue());
    }

    return ret;
  }

//---------------------------------------------------------------------------------------------
  public static String dateToString(String begdate, int ndays)
  {
    String ret = "";
    SimpleDateFormat  sdf = new SimpleDateFormat(DEFAULT_DATE_FORMAT);
    GregorianCalendar gc = new GregorianCalendar();
    sdf.setCalendar(gc);
    gc.setLenient(false);

    try
    {
      if (ndays != 0)
      {
        java.util.Date auxdate = sdf.parse(begdate);
        gc.setTime(auxdate);

        ret = dateToString(gc, ndays);
      }
      else
        ret = begdate;
    }
    catch(ParseException pe)
    {
      Debug.Dbg("ParseException in dateToString: " + pe.getMessage());
      return "";
    }

    return ret;
  }

//--------------------------------------------------------------------
  public static String dateToString(GregorianCalendar begdate, int ndays)
  {
    String ret = "";
    GregorianCalendar gc = new GregorianCalendar();
    SimpleDateFormat sdf = new SimpleDateFormat(DEFAULT_DATE_FORMAT);
    sdf.setCalendar(gc);
    gc.setTime(begdate.getTime());
    gc.setLenient(false);

    if (ndays != 0)
      gc.add(Calendar.DATE, ndays);



    ret = sdf.format(gc.getTime());

    return ret;
  }

//--------------------------------------------------------------------
  public static String dateToString(Date begdate, int ndays)
  {
    return dateToString(dateToString(begdate), ndays);
  }

//--------------------------------------------------------------------
  public static String dateToString(Date d)
  {
    SimpleDateFormat sdf = new SimpleDateFormat(DEFAULT_DATE_FORMAT);
    return sdf.format(d);
  }

//--------------------------------------------------------------------
  public static Date stringToDate(String dateString)
  {
    SimpleDateFormat sdf = new SimpleDateFormat(DEFAULT_DATE_FORMAT);
    java.util.Date dt = new Date();

    try
    {
      dt = sdf.parse(dateString);
    }
    catch(ParseException pe)
    {
      Debug.Dbg("ParseException in stringToDate: " + pe.getMessage());
      return dt;
    }

    return dt;
  }

//--------------------------------------------------------------------
  public static int stringToInt(String num)
  {
    try{return Integer.parseInt(num);}
    catch(Exception e){return 0;}
  }
  
//--------------------------------------------------------------------
  public static int stringToInt(String num, int default_return)
  {
    try{return Integer.parseInt(num);}
    catch(Exception e){return default_return;}
  }
  
//--------------------------------------------------------------------
  public static long stringToLong(String num)
  {
    try{return Long.parseLong(num);}
    catch(Exception e){return 0L;}
  }

//--------------------------------------------------------------------
  public static double stringToDouble(String num)
  {
    try{return Double.parseDouble(num);}
    catch(Exception e){return 0.0;}
  }

//--------------------------------------------------------------------
  public static GregorianCalendar stringToCalendar(String dateString)
  {
    GregorianCalendar gc = new GregorianCalendar();
    gc.setTime(stringToDate(dateString));
    return gc;
  }

//--------------------------------------------------------------------
  public static String getWeekdayName(String yyyyMMdd)
  {
    String ret = "";
    SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");
    GregorianCalendar gc = new GregorianCalendar();
    
    try
    {
      gc.setTime(sdf.parse(yyyyMMdd));
      sdf.setCalendar(gc);
      ret = sdf.getDateFormatSymbols().getWeekdays()[gc.get(Calendar.DAY_OF_WEEK)];
    } 
    catch (ParseException ex) {ex.printStackTrace();}
    
    return ret;
  }
  
//--------------------------------------------------------------------
  public static int compareDay(GregorianCalendar day1, GregorianCalendar day2)
  {
    int month_1 = day1.get(Calendar.MONTH);
    int month_2 = day2.get(Calendar.MONTH);
    int day_1   = day1.get(Calendar.DAY_OF_MONTH);
    int day_2   = day2.get(Calendar.DAY_OF_MONTH);
    int ret = 0;

    if ((month_1 - month_2) == 0)
      ret = (day_1 - day_2);
    else if ((month_1 - month_2) > 0)
      ret = 1;
    else if ((month_1 - month_2) < 0)
      ret = -1;

    return ret;
  }


/*******************************************************************************
  Date manip. routines for database
*******************************************************************************/

//--------------------------------------------------------------------
  public static String SQLQuotes(String in)
  {
    if (in == null)
      return "";
    
    int     pos     = 0;
    int     oldpos  = 0;
    String  ret     = "";

    while ((pos = in.indexOf('\'', pos)) != -1)
    {
      pos++;
      ret += in.substring(oldpos, pos) + "'";
      oldpos = pos;
    }
    ret += in.substring(oldpos, in.length());

    return ret;
  }
  
//--------------------------------------------------------------------
  public static String SQLQuotesEnclose(String in)
  {
    return "'" + SQLQuotes(in) + "'";
  }
  
//--------------------------------------------------------------------
  public static boolean isValidDate(String dateString)
  {
    SimpleDateFormat  sdf = new SimpleDateFormat(DEFAULT_DATE_FORMAT);
    GregorianCalendar gc = new GregorianCalendar();
    sdf.setCalendar(gc);
    gc.setLenient(false);

    try
    {
      if ( (dateString.length() != 10)                  ||
           (!(dateString.substring(2, 3)).equals("/"))  ||
           (!(dateString.substring(5, 6)).equals("/")) )
        return false;

      gc.setTime(sdf.parse(dateString));
    }
    catch(Exception e)
    {
      return false;
    }

    return true;
  }

//--------------------------------------------------------------------
  public static boolean isValidTime(String timeString)
  {
    SimpleDateFormat sdf = new SimpleDateFormat(DEFAULT_TIME_FORMAT);
    GregorianCalendar gc = new GregorianCalendar();
    sdf.setCalendar(gc);
    gc.setLenient(false);

    try
    {
      if ( (timeString.length() != 8)                   ||
           (!(timeString.substring(2, 3)).equals(":"))  ||
           (!(timeString.substring(5, 6)).equals(":")) )
        return false;

      int first   = (new Integer(timeString.substring(0, 2))).intValue();
      int second  = (new Integer(timeString.substring(3, 5))).intValue();
      int third   = (new Integer(timeString.substring(6, 8))).intValue();

      if ( (first > 23) || (second > 59) || (third >59) )
        return false;

      gc.setTime(sdf.parse(timeString));
    }
    catch(Exception e)
    {
      return false;
    }

    return true;
  }

//--------------------------------------------------------------------
  public static boolean isNumber(String numberstr)
  {
    try{Integer.parseInt(numberstr);}
    catch(Exception e){return false;}
    return true;
  }

//--------------------------------------------------------------------
  public static boolean isDigit(char v) { return Character.isDigit(v); }

//--------------------------------------------------------------------
  public static boolean isAlpha(char v) { return Character.isLetter(v);}

//--------------------------------------------------------------------
  public static boolean isAlphaNumeric(char v) { return Character.isLetterOrDigit(v); }  
  
//--------------------------------------------------------------------
  public static boolean isAlpha(String str)
  {
    if (str == null)
      return false;

    int strlen = str.length();
    for(int i=0; i<strlen; i++)
      if (Character.isLetter(str.charAt(i)) == false)
       return false;

    return true;
  }

//--------------------------------------------------------------------
  // number or digit
  public static boolean isAlphaNumeric(String str)
  {
    if (str == null)
      return false;

    int strlen = str.length();
    for(int i=0; i<strlen; i++)
      if (Character.isLetterOrDigit(str.charAt(i)) == false)
        return false;

    return true;
  }  
//--------------------------------------------------------------------
  public static String date2DB(String date2conv)
  {
    String ret = "";

    if (date2conv.length() == 10) // input format: xx/xx/yyyy
      ret = date2conv.substring(6, 10) + date2conv.substring(3, 5) + date2conv.substring(0, 2);

    return ret;
  }

//--------------------------------------------------------------------
  public static String DB2Date(String date2conv)
  {
    String ret = "";

    if (date2conv == null)
      return "";

    if (date2conv.length() == 8)  // input format: yyyyMMdd
      ret = date2conv.substring(6, 8) + "/" + date2conv.substring(4, 6) + "/" + date2conv.substring(0, 4);

    return ret;
  }

//--------------------------------------------------------------------
  public static String date2DB(Date date2conv)
  {
    SimpleDateFormat  sdf = new SimpleDateFormat(DEFAULT_DATE_FORMAT);
    return date2DB(sdf.format(date2conv));
  }



  //*********************************
  // IPC Message manip. routines
  //*********************************

//--------------------------------------------------------------------
  public static String IPCgetID(String message, String token)
  {
    return (new StringTokenizer(message, token)).nextToken();
  }

//--------------------------------------------------------------------
//index = 0 for the first token
  public static String IPCgetToken(String message, int index, String token)
  {
    String          strtoken = "";
    StringTokenizer tokenizer = new StringTokenizer(message, token, true);

    if (index < 0)
      return message;

//    if (index == 0)
//    {
//      strtoken = tokenizer.nextToken();
//      return (strtoken.equals(token) ? "" : strtoken);
//    }

    //Here, index is strictly positive
    int count = 0;
    while (count < index && tokenizer.hasMoreTokens())
    {
      strtoken = tokenizer.nextToken();
      if (strtoken.equals(token))
        count++;
    }

    if (count < index)
      return message;

    strtoken = tokenizer.nextToken();
    return (strtoken.equals(token) ? "" : strtoken);
  }



  //*********************************
  // Time & duration manip. routines
  //*********************************

//---------------------------------------------------------------------
  public static String duration2String(int min, int sec)
  {
    if (min < 0)
     return "";
    else if(sec < 0 || sec > 59)
     return "";
    else
       return min + "'" + sec + "\"";
  }

//---------------------------------------------------------------------
  public static String duration2String(int totalSecs)
  {
    if (totalSecs < 0)
     return "";
    else
      return (totalSecs / 60) + "'" + (totalSecs % 60) + "\"";
  }

//---------------------------------------------------------------------
  public static int minFromDuration(String duration)
  {
    int index = duration.indexOf('\'');

    return (index < 0 ? 0 : Integer.parseInt(duration.substring(0, index).trim()));
  }

//---------------------------------------------------------------------
  public static int secFromDuration(String duration)
  {
    int indexStart = duration.indexOf('\'');
    int indexEnd   = duration.indexOf('"');

    return (indexEnd < 0 ? 0 : Integer.parseInt(duration.substring(indexStart + 1, indexEnd).trim()));
  }

//---------------------------------------------------------------------
  public static int totalFromDuration(String duration)
  {
    return (minFromDuration(duration)*60)+ secFromDuration(duration);
  }

//---------------------------------------------------------------------
  public static boolean isValidDuration(String duration)
  {
    try
    {
      int apex = duration.indexOf('\'');
      int quote = duration.indexOf('\"');

      if (apex < 0 && quote < 0)
        return false;

      if (apex >= 0)
        Integer.parseInt(duration.substring(0, apex).trim());

      if (quote >= 0)
        Integer.parseInt(duration.substring(apex + 1, quote).trim());
    }
    catch(NumberFormatException nfe)
    {
      Debug.Dbg("NumberFormatException Exception in isValidDuration");
      return false;
    }
    catch(Exception e)
    {
      Debug.Dbg("Exception in isValidDuration");
      return false;
    }

    return true;
  }
//---------------------------------------------------------------------------------------------
  public static String formatSecsFromMidnight(String  strTime, String sum)
  {
    int timeInt = (new Integer(strTime)).intValue();
    int sumInt  = (new Integer(sum)).intValue();

    return formatSecsFromMidnight(timeInt + sumInt);
  }

//---------------------------------------------------------------------
  public static String formatSecsFromMidnight(int intTime)
  {
    String strHours;
    String strMins;
    String strSecs;

    if (intTime >= 86400)
      intTime = intTime - 86400;

    int mins          = intTime / 60;
    int hours         = mins / 60;

    int secs          = intTime - mins * 60;
    mins              = mins - hours * 60;

    if ( hours < 10)
      strHours = "0" + hours;
    else
      strHours = "" + hours;
    if ( mins < 10)
      strMins = "0" + mins;
    else
      strMins = "" + mins;
    if ( secs < 10)
      strSecs = "0" + secs;
    else
      strSecs = "" + secs;

    return strHours + ":" + strMins + ":" + strSecs;
  }
  
//---------------------------------------------------------------------
  public static String monthNameEN(int month_num)
  {
    String ret = "";
    switch (month_num)
    {
      case 1:  ret = "January"; break;
      case 2:  ret = "February"; break;
      case 3:  ret = "March"; break;
      case 4:  ret = "April"; break;
      case 5:  ret = "May"; break;
      case 6:  ret = "June"; break;
      case 7:  ret = "July"; break;
      case 8:  ret = "August"; break;
      case 9:  ret = "September"; break;
      case 10: ret = "October"; break;
      case 11: ret = "November"; break;
      case 12: ret = "December"; break;
    }
    
    return ret;
  }

//---------------------------------------------------------------------
  public static String monthNameEN(String month_num)
  {
    int monthnum = 0;
    
    try{monthnum = Integer.parseInt(month_num);}
    catch(NumberFormatException nfe){nfe.printStackTrace();}
    
    return monthNameEN(monthnum);
  }
  
  //*********************************
  // Path manip. routines
  //*********************************

  public static String prefixRelativePath(String root_part, String relative_part)
  {
    String sep = System.getProperty("file.separator");

    if (root_part.endsWith(sep))
    {
      if (relative_part.startsWith(sep))
        relative_part = relative_part.substring(1, relative_part.length());
    }
    else
    {
      if (!relative_part.startsWith(sep))
        relative_part = sep + relative_part;
    }

    return relative_part;
  }

//---------------------------------------------------------------------------------------------
  public static String addEndSlash(String path)
  {
    String sep = System.getProperty("file.separator");
    if (path.endsWith(sep))
      return path;
    else
      return (path + sep);
  }

//---------------------------------------------------------------------------------------------
  public static boolean isValidPath(String str)
  {
    boolean valid = true;
    for (int i = 0; i < str.length(); i++)
    {
      if (str.charAt(i) == ':'  || str.charAt(i) == '*' ||
          str.charAt(i) == '?'  || str.charAt(i) == '"' ||
          str.charAt(i) == '<'  || str.charAt(i) == '>' ||
          str.charAt(i) == '|')
        valid = false;
     }
     return valid;
  }

  //*********************************
  //    IO routines
  //*********************************
  
//---------------------------------------------------------------------------------------------
  /**
   * Read a file from disk and place the content in a String.
   *
   * @param fileName The name of the file (should be in the classpath)
   * @return a String with the content of the file.
   * @throws RuntimeException In case there is an error loading the file.
   */
  public static String readFile(String fileName) 
  {
    URL url = Thread.currentThread().getContextClassLoader().getResource(fileName);
    if (url == null)
      throw new RuntimeException("JTXTools: File '" + fileName + "' not found in classpath.");

    return readFile(url);
  }

//---------------------------------------------------------------------------------------------
  /**
   * Read a file from disk and place the content in a String.
   *
   * @param url The name of the resource (should be in the classpath)
   * @return a String with the content of the file or an 
   *         empty string if parameter 'url' is null.
   * @throws RuntimeException In case there is an error loading the file.
   */
  public static String readFile(URL url) 
  {
    if (url == null)
      return "";

    BufferedReader in = null;
    StringBuffer message = new StringBuffer();
    try 
    {
      in = new BufferedReader(new FileReader(url.getFile()));
      String line = null;
      while ((line = in.readLine()) != null) 
        message.append(line);
      in.close();
    } 
    catch (FileNotFoundException e) 
    {
      throw new RuntimeException("JTXTools: File '" + url.getFile() + "' not found.");
    } 
    catch (IOException e) 
    {
      throw new RuntimeException("JTXTools: Error reading file '" + url.getFile() + "'.");
    }
    
    return message.toString();
  }
  
//---------------------------------------------------------------------------------------------
  /**
   * Create one or more directories.
   *
   * Either '/' or '\' should be used as a path seperator.<BR>
   * <BR>
   * Example: foo/test/bar
   *
   * @param path The name of the path to be created
   * @return true in case of success
   */
  public static boolean mkdirs(String path) 
  {
    File dir = new File(path);
    return dir.mkdirs();
  }

//---------------------------------------------------------------------------------------------
  /**
   * Remove one or more directories
   *
   * Either '/' or '\' should be used as a path seperator.<BR>
   * <BR>
   * Example: foo/test/bar
   *
   * @param path The name of the path to be created
   * @return true in case of success
   */
  public static boolean removeDirectory(String path) 
  {
    File dir = new File(path);
    return dir.delete();
  }

//---------------------------------------------------------------------------------------------
  /**
   * Check if a path exists
   *
   * Either '/' or '\' should be used as a path seperator.<BR>
   * <BR>
   * Example: foo/test/bar
   *
   * @param path The name of the path to be created
   * @return true in case of success
   */
  public static boolean directoryExists(String path) 
  {
    File dir = new File(path);
    return dir.isDirectory() && dir.exists();
  }

//---------------------------------------------------------------------------------------------
  /**
   * Convert a Java package name to a full path name with the given path separator
   * or the native file system one if sep == "".
   *
   * <BR>
   * Example: packageName=net.sourceforge.anttestsetgen<BR>
   * The result will then be net/sourceforge/anttestsetgen
   *
   * @param packageName The Java package name
   * @param sep The requested path separator (can be null or "")
   * @return the full path
   */
  public static String convertPackageToPath(String packageName, String sep)
  {
    if (packageName != null && !packageName.equals(""))
    {
      if (sep == null || sep.equals(""))
        return packageName.replace('.', System.getProperty("file.separator").charAt(0));
      else
        return packageName.replace('.', sep.charAt(0));
    }
    else
      return "";
  }

//---------------------------------------------------------------------------------------------
  /**
   * Convert a Java package name to a full path name with the given path separator
   * or the native file system one if sep == "".
   *
   * <BR>
   * Example: packageName=net.sourceforge.anttestsetgen<BR>
   * The result will then be net/sourceforge/anttestsetgen
   *
   * @param package The Java package object
   * @param sep The requested path separator (can be null or "")
   * @return the full path
   */
  public static String convertPackageToPath(Package _package, String sep) 
  {
    return convertPackageToPath(_package.getName(), sep);
  }
  
//---------------------------------------------------------------------------------------------
  /**
   * Remove a file from the file system.
   *
   * No status info is given about the success (or failure) of the operation.
   *
   * @param fileName The name of the file to be removed.
   */
  public static void removeFile(String fileName) 
  {
    File file = new File(fileName);
    file.delete();
  }

//---------------------------------------------------------------------------------------------
  /**
   * Check if a file exists in the file system.
   *
   * @param fileName The name of the file to be removed.
   * @return True in case the file exists.
   */
  public static boolean fileExists(String fileName) 
  {
    File file = new File(fileName);
    return file.exists();
  }

} // End class
