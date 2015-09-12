<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:param name="userVal"/>
<xsl:template match="/">

<html>
<body>
  
  <h2>GRMS Category Code Set</h2>
  
  <table border="1">
    <tr bgcolor="#9acd32">
      <th>segment</th>
      <th>family</th>
      <th>class</th>      
      <th>commodity</th>
      <th>category</th>
      <th>description</th>
    </tr>
    
    <xsl:for-each select="GRMSCategoryCodes/GRMSCategory[./description[contains(., $userVal)]]">
    
    <tr>
      <td><xsl:value-of select="segment"/></td>
      <td><xsl:value-of select="family"/></td>
      <td><xsl:value-of select="class"/></td>
      <td><xsl:value-of select="commodity"/></td>
      <td><xsl:value-of select="category"/></td>
      <td><xsl:value-of select="description"/></td>
    </tr>
    </xsl:for-each>
  </table>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>
