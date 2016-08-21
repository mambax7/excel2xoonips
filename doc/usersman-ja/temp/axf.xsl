<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:fo="http://www.w3.org/1999/XSL/Format"
		version='1.0'>

<xsl:import  href="m:/local/share/sgml/docbook/xsl/fo/docbook.xsl"/>
<!--  <xsl:import  href="unfill.xsl"/> -->
<xsl:include href="fo-titlepage.xsl"/>

<!-- draft mode -->
<xsl:param name="draft.mode" select="'no'"/>

<!-- Font Setting -->
<xsl:param name="title.font.family" select="'sans-serif,Gothic'"/>
<xsl:param name="body.font.family" select="'serif,Mincho'"/>
<xsl:param name="monospace.font.family" select="'monospace,Mincho'"/>
<xsl:param name="body.font.master" select="10"/>
<!-- font size of section -->
<xsl:attribute-set name="section.title.level1.properties">
  <xsl:attribute name="font-size">
    <xsl:value-of select="$body.font.master * 1.7490062"/>
    <xsl:text>pt</xsl:text>
  </xsl:attribute>
</xsl:attribute-set>
<xsl:attribute-set name="section.title.level2.properties">
  <xsl:attribute name="font-size">
    <xsl:value-of select="$body.font.master * 1.520875"/>
    <xsl:text>pt</xsl:text>
 </xsl:attribute>
</xsl:attribute-set>
<xsl:attribute-set name="section.title.level3.properties">
  <xsl:attribute name="font-size">
    <xsl:value-of select="$body.font.master * 1.3225"/>
    <xsl:text>pt</xsl:text>
  </xsl:attribute>
</xsl:attribute-set>
<xsl:attribute-set name="section.title.level4.properties">
  <xsl:attribute name="font-size">
    <xsl:value-of select="$body.font.master * 1.15"/>
    <xsl:text>pt</xsl:text>
  </xsl:attribute>
</xsl:attribute-set>
<xsl:attribute-set name="section.title.level6.properties">
  <xsl:attribute name="font-size">
    <xsl:value-of select="$body.font.master"/>
    <xsl:text>pt</xsl:text>
  </xsl:attribute>
</xsl:attribute-set>

<!-- Page Layout -->
<xsl:param name="paper.type" select="'A4'"/>
<xsl:param name="double.sided">0</xsl:param>
<xsl:param name="page.margin.top" select="'15mm'"/>
<xsl:param name="region.before.extent" select="'8mm'"/>
<xsl:param name="body.margin.top" select="'10mm'"/>
<xsl:param name="region.after.extent" select="'8mm'"/>
<xsl:param name="body.margin.bottom" select="'15mm'"/>
<xsl:param name="page.margin.inner">
  <xsl:choose>
    <xsl:when test="$double.sided != 0">22mm</xsl:when>
    <xsl:otherwise>18mm</xsl:otherwise>
  </xsl:choose>
</xsl:param>
<xsl:param name="page.margin.outer">
  <xsl:choose>
    <xsl:when test="$double.sided != 0">14mm</xsl:when>
    <xsl:otherwise>18mm</xsl:otherwise>
  </xsl:choose>
</xsl:param>
<xsl:param name="title.margin.left">0in</xsl:param>

<!-- ordered list -->
<xsl:param name="orderedlist.label.width" select="'1.5em'"/>

<!-- Hyphenation -->
<xsl:param name="hyphenate">true</xsl:param>

<!-- Extensions -->
<xsl:param name="use.extensions" select="1"/>
<xsl:param name="fop.extensions" select="0"/>
<xsl:param name="axf.extensions" select="1"/>
<xsl:param name="tablecolumns.extension" select="0"/>
<xsl:param name="tex.math.in.alt" select="'latex'"/>

<!-- Toc -->
<xsl:param name="generate.toc">
  book       toc,title
  chapter    title
</xsl:param>
<xsl:param name="refentry.generate.title" select="1"/>
<xsl:param name="refentry.generate.name" select="0"/>
<xsl:param name="toc.section.depth" select="1"/>

<!-- Edition -->
<xsl:template match="edition" mode="titlepage.mode">
  <xsl:text>RELEASE</xsl:text>
  <xsl:call-template name="gentext.space"/>
  <xsl:apply-templates mode="titlepage.mode"/>
</xsl:template>

<!-- Year -->
<xsl:param name="make.year.ranges" select="1"/>
<xsl:param name="make.single.year.ranges" select="1"/>

<!-- header footer -->
<xsl:param name="headers.on.blank.pages" select="0"></xsl:param>
<xsl:param name="header.column.widths" select="'3 1 3'"/>
<xsl:param name="header.rule" select="0"/>
<xsl:param name="footer.rule" select="0"/>
<!-- label -->
<xsl:param name="section.autolabel" select="1"/>
<xsl:param name="section.label.includes.component.label" select="1"/>
<xsl:param name="section.autolabel.max.depth" select="2"/>
<!--
    <xsl:param name="admon.graphics" select="1"/>
-->

<!-- Localization -->
<xsl:param name="l10n.gentext.default.language" select="'ja'"/>
<xsl:param name="l10n.gentext.language" select="'ja'"/>
<xsl:param name="local.l10n.xml" select="document('')"/>
<l:i18n xmlns:l="http://docbook.sourceforge.net/xmlns/l10n/1.0">
  <l:l10n language="ja" english-language-name="Japanese">
    <l:gentext key="Copyright" text="Copyright" lang="en"/>
    <l:gentext key="copyright" text="Copyright" lang="en"/>
    <l:gentext key="RefName" text="&#27231;&#33021;"/>
    <l:gentext key="refname" text="&#27231;&#33021;"/>
    <l:gentext key="RefSynopsisDiv" text="&#24418;&#24335;"/>
    <l:gentext key="refsynopsisdiv" text="&#24418;&#24335;"/>
  </l:l10n>
</l:i18n>

<!-- Attributes Settings -->
<!-- normal para -->
<xsl:attribute-set name="normal.para.spacing">
  <xsl:attribute name="space-before.optimum">0.8em</xsl:attribute>
  <xsl:attribute name="space-before.minimum">0.6em</xsl:attribute>
  <xsl:attribute name="space-before.maximum">1.0em</xsl:attribute>
</xsl:attribute-set>

<!-- section -->
<xsl:attribute-set name="section.title.properties">
  <xsl:attribute name="font-family">
    <xsl:value-of select="$title.font.family"/>
  </xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
  <!-- font size is calculated dynamically by section.heading template -->
  <xsl:attribute name="keep-with-next.within-column">always</xsl:attribute>
  <xsl:attribute name="space-before.minimum">1.0em</xsl:attribute>
  <xsl:attribute name="space-before.optimum">1.2em</xsl:attribute>
  <xsl:attribute name="space-before.maximum">1.4em</xsl:attribute>
</xsl:attribute-set>

<!-- compact list item : simplelist -->
<xsl:attribute-set name="compact.list.item.spacing">
  <xsl:attribute name="space-before.optimum">0em</xsl:attribute>
  <xsl:attribute name="space-before.minimum">0em</xsl:attribute>
  <xsl:attribute name="space-before.maximum">0em</xsl:attribute>
</xsl:attribute-set>


<!-- Template custmizing -->
<!-- Text indent for para tag -->
<xsl:template match="chapter/para|section/para">
  <fo:block xsl:use-attribute-sets="normal.para.spacing" text-indent="1em">
    <xsl:call-template name="anchor"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!-- Fixed table cell padding -->
<xsl:attribute-set name="table.cell.padding">
  <xsl:attribute name="start-indent">0pt</xsl:attribute>
  <xsl:attribute name="end-indent">0pt</xsl:attribute>
  <xsl:attribute name="padding-left">2pt</xsl:attribute>
  <xsl:attribute name="padding-right">2pt</xsl:attribute>
  <xsl:attribute name="padding-top">2pt</xsl:attribute>
  <xsl:attribute name="padding-bottom">2pt</xsl:attribute>
</xsl:attribute-set>

<!-- Header layout -->
<xsl:template name="header.content">
  <xsl:param name="pageclass" select="''"/>
  <xsl:param name="sequence" select="''"/>
  <xsl:param name="position" select="''"/>
  <xsl:param name="gentext-key" select="''"/>
  <xsl:variable name="candidate">
    <!-- sequence can be odd, even, first, blank -->
    <!-- position can be left, center, right -->
    <xsl:choose>
      <xsl:when test="$sequence = 'odd' and $position = 'right'">
        <xsl:apply-templates select="." mode="object.title.markup"/>
      </xsl:when>
      <xsl:when test="$sequence = 'even' and $position = 'left'">
        <xsl:apply-templates select="." mode="object.title.markup"/>
      </xsl:when>
    </xsl:choose>
  </xsl:variable>
  <!-- Does runtime parameter turn off blank page headers? -->
  <xsl:choose>
    <xsl:when test="$sequence='blank' and $headers.on.blank.pages=0">
      <!-- no output -->
    </xsl:when>
    <!-- titlepages have no headers -->
    <xsl:when test="$pageclass = 'titlepage'">
      <!-- no output -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy-of select="$candidate"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Abstract -->
<xsl:template match="abstract" mode="titlepage.mode">
  <fo:block>
    <xsl:apply-templates mode="titlepage.mode"/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>
