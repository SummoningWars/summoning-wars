<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  
  
  
  <xsl:template match="/">
    <html>
      <head>
      </head>
      
      <body>
        <h1>Meshdocumentation - <xsl:value-of select="creature/name"/></h1>
        <br/>
        <table>
          <tr>
            <td>Clipname</td>
            <td>Time</td>
            
          </tr>
          <xsl:for-each select="creature/animations/clip">
            <tr>
              <td><xsl:value-of select="name"/></td>
              <td><xsl:value-of select="startframe"/> - <xsl:value-of select="endframe"/></td>
            </tr>
          </xsl:for-each>
        </table>
      </body>
    </html>
  
  
  </xsl:template>


</xsl:stylesheet>
