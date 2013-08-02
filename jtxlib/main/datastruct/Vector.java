/* $RCSfile: Vector.java,v $
 * $Revision: 1.1 $
 * $Author: Gambineri $
 * $Date: 2006/05/26 15:07:20 $
 *
 * developed by Massimo Gambineri
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 */

package jtxlib.main.datastruct;


/**
 * A vector class implementation which offers less utilities than classical
 * <code>Vector</code> one but really increases speed of all fundamental operations.
 * <P>
 * Please use it.
 */


public class Vector
{
  private Object[]    m_ElementData;
  private int         m_ElementCount;

//---------------------------------------------------------------
/**
 * Creates an instance of the object with the specified capacity.
 * <P>
 */
  public Vector(int initialCapacity)
  {
    m_ElementData   = new Object[initialCapacity];
    m_ElementCount  = 0;
  }

//---------------------------------------------------------------
  private void ensureCapacity(int minCapacity)
  {
    int oldCap = m_ElementData.length;

    if (minCapacity > oldCap)
    {
      Object oldData[] = m_ElementData;
      int    newCap    = oldCap * 2;

      if (newCap < minCapacity)
        newCap = minCapacity;

      m_ElementData = new Object[newCap];
      System.arraycopy(oldData, 0, m_ElementData, 0, m_ElementCount);
    }
  }

//---------------------------------------------------------------
/**
 * Returns the number of elements contained in the vector.
 */
  public int size()
  {
    return m_ElementCount;
  }

//---------------------------------------------------------------
/**
 * Returns the <code>Object</code> element contained in the vector
 * at the specified position.
 * <P>
 * @param index the 0-based index of the element to retrieve.
 * *exception ArrayIndexOutOfBoundsException
 */
  public Object elementAt(int index) throws java.lang.ArrayIndexOutOfBoundsException
  {
    return m_ElementData[index];
  }

//---------------------------------------------------------------
/**
 * Sets the <code>Object</code> at the specified position in the vector.
 * BE AWARE that <b>this function does not increment the element count</b>, 
 * but simply replaces any reference to the existing/non-existing element 
 * at the given position with the one referenced by the parameter <code>obj</code>.
 * <P>
 * @param obj the <code>Object</code> to set.
 * @param index the 0-based position where to set the element.
 * *exception ArrayIndexOutOfBoundsException
 */
  public void setElementAt(Object obj, int index)  throws java.lang.ArrayIndexOutOfBoundsException
  {
    m_ElementData[index] = obj;
  }

//---------------------------------------------------------------
/**
 * Appends the <code>Object</code> to the vector increasing the capacity if needed.
 * <P>
 * @param obj the <code>Object</code> to append.
 */
  public void addElement(Object obj)
  {
    ensureCapacity(m_ElementCount + 1);
    m_ElementData[m_ElementCount++] = obj;
  }

//---------------------------------------------------------------
/**
 * Removes an <code>Object</code> at given index from this vector.
 * <P>
 * @param idx the index of the <code>Object</code> to be removed.
 */
  public void removeElement(int idx)
  {
    if (idx < 0 || idx >= m_ElementCount)
      return;
    
    System.arraycopy(m_ElementData, idx+1, m_ElementData, idx, m_ElementCount-idx-1);
    m_ElementCount--;
  }

//---------------------------------------------------------------
/**
 * Makes the clean up of all objects and removes them from <code>this</code> vector.
 */
  public void removeAllElements()
  {
    for (int i=0; i<m_ElementCount; i++)
      m_ElementData[i] = null;

    m_ElementCount = 0;
  }

//---------------------------------------------------------------
/**
 * Makes the clean up of the last object and removes it from <code>this</code> vector.
 */
  public Object removeTail()
  {
    Object oldVal = m_ElementData[--m_ElementCount];
    m_ElementData[m_ElementCount] = null;

    return oldVal;
  }

//---------------------------------------------------------------
/**
 * Looks for the first occurence of the given argument, beginning
 * the search at <code>index</code>, and testing for equality using
 * the <code>equals</code> method.
 *
 * @param   elem    an object.
 * @param   index   the index to start searching from.
 * @return  the index of the first occurrence of the object argument in
 *          this vector at position <code>index</code> or later in the
 *          vector, that is, the smallest value <tt>k</tt> such that
 *          <tt>elem.equals(elementData[k]) && (k &gt;= index)</tt> is
 *          <tt>true</tt>; returns <code>-1</code> if the object is not
 *           found.
 * @see     Object#equals(Object)
 */
  public int indexOf(Object elem, int index)
  {
    if (elem == null)
    {
      for (int i = index ; i < m_ElementCount; i++)
        if (m_ElementData[i] == null)
          return i;
    }
    else
    {
      for (int i = index ; i < m_ElementCount; i++)
        if (elem.equals(m_ElementData[i]))
          return i;
    }

    return -1;
  }

//---------------------------------------------------------------
/**
 * Searches for the first occurence of the given argument, testing
 * for equality using the <code>equals</code> method.
 *
 * @param   elem   an object.
 * @return  the index of the first occurrence of the argument in this
 *          vector, that is, the smallest value <tt>k</tt> such that
 *          <tt>elem.equals(elementData[k])</tt> is <tt>true</tt>;
 *          returns <code>-1</code> if the object is not found.
 * @see     Object#equals(Object)
 */
  public int indexOf(Object elem)
  {
    return indexOf(elem, 0);
  }

//---------------------------------------------------------------
  public String toString()
  {
    int           max = m_ElementCount - 1;
    StringBuffer  buf = new StringBuffer();

    buf.append("[");

    for (int i=0; i<m_ElementCount; i++)
    {
      String s = m_ElementData[i].toString();
      buf.append(s);

      if (i < max)
        buf.append(", ");
    }

    buf.append("]");

    return buf.toString();
  }
}
