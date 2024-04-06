<!-- <?xml version="1.0" encoding="windows-1251" ?> -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- Template rule for log element(s): -->
   <xsl:template match="report">
   <html>
   <head>
   <!-- <meta http-equiv="Content-Type" content="text/html; charset=utf-16"></meta> --> 
   <style>
   </style>
   </head>
   <body>
      <table cellpadding="0" cellspacing="0">
         <tr>
            <TH >Дата</TH>
            <TH >Время</TH>
            <TH >Зал</TH>
            <TH >Кол-во мест</TH>
            <TH >Кол-во зрителей</TH>
         </tr>
         <!-- Template rule for server element(s): -->
         <xsl:for-each select="record">
            <tr>
           		<xsl:apply-templates select="@date"/>     		
           		<xsl:for-each select="@*[position() > 1]">
           		<td>
           			<xsl:value-of select="."/>
           		</td>	
           		</xsl:for-each>	
			 </tr>  
         </xsl:for-each>
      </table>
      </body>
     </html>
   </xsl:template>
   
   	<xsl:template match="@date">
		<td>
			<xsl:value-of select="@date"/>
		</td>
	</xsl:template>	  
	
	<!-- assume date/time is divided by -, if it will be changed, change ReportDocument too -->
	<xsl:template match="@date">
        <td>
        	<xsl:value-of select="substring-before(., '-')"/>
        </td>	
        <td>
        	<xsl:value-of select="substring-after(., '-')"/>
        </td>	        
	</xsl:template>
	
</xsl:stylesheet>
