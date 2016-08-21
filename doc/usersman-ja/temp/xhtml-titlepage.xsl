<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exsl="http://exslt.org/common" version="1.0" exclude-result-prefixes="exsl">

<!-- This stylesheet was created by template/titlepage.xsl-->

<xsl:template name="book.titlepage.recto">
  <xsl:choose>
    <xsl:when test="bookinfo/title">
      <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="bookinfo/title"/>
    </xsl:when>
    <xsl:when test="info/title">
      <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="info/title"/>
    </xsl:when>
    <xsl:when test="title">
      <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="title"/>
    </xsl:when>
  </xsl:choose>

  <xsl:choose>
    <xsl:when test="bookinfo/subtitle">
      <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="bookinfo/subtitle"/>
    </xsl:when>
    <xsl:when test="info/subtitle">
      <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="info/subtitle"/>
    </xsl:when>
    <xsl:when test="subtitle">
      <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="subtitle"/>
    </xsl:when>
  </xsl:choose>

  <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="bookinfo/edition"/>
  <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="info/edition"/>
  <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="bookinfo/abstract"/>
  <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="info/abstract"/>
  <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="bookinfo/mediaobject"/>
  <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="info/mediaobject"/>
  <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="bookinfo/copyright"/>
  <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="info/copyright"/>
</xsl:template>

<xsl:template name="book.titlepage.verso">
</xsl:template>

<xsl:template name="book.titlepage.separator"><hr/>
</xsl:template>

<xsl:template name="book.titlepage.before.recto">
</xsl:template>

<xsl:template name="book.titlepage.before.verso">
</xsl:template>

<xsl:template name="book.titlepage">
  <div class="titlepage">
    <xsl:variable name="recto.content">
      <xsl:call-template name="book.titlepage.before.recto"/>
      <xsl:call-template name="book.titlepage.recto"/>
    </xsl:variable>
    <xsl:variable name="recto.elements.count">
      <xsl:choose>
        <xsl:when test="function-available('exsl:node-set')"><xsl:value-of select="count(exsl:node-set($recto.content)/*)"/></xsl:when>
        <xsl:when test="contains(system-property('xsl:vendor'), 'Apache Software Foundation')">
          <!--Xalan quirk--><xsl:value-of select="count(exsl:node-set($recto.content)/*)"/></xsl:when>
        <xsl:otherwise>1</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="(normalize-space($recto.content) != '') or ($recto.elements.count &gt; 0)">
      <div><xsl:copy-of select="$recto.content"/></div>
    </xsl:if>
    <xsl:variable name="verso.content">
      <xsl:call-template name="book.titlepage.before.verso"/>
      <xsl:call-template name="book.titlepage.verso"/>
    </xsl:variable>
    <xsl:variable name="verso.elements.count">
      <xsl:choose>
        <xsl:when test="function-available('exsl:node-set')"><xsl:value-of select="count(exsl:node-set($verso.content)/*)"/></xsl:when>
        <xsl:when test="contains(system-property('xsl:vendor'), 'Apache Software Foundation')">
          <!--Xalan quirk--><xsl:value-of select="count(exsl:node-set($verso.content)/*)"/></xsl:when>
        <xsl:otherwise>1</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="(normalize-space($verso.content) != '') or ($verso.elements.count &gt; 0)">
      <div><xsl:copy-of select="$verso.content"/></div>
    </xsl:if>
    <xsl:call-template name="book.titlepage.separator"/>
  </div>
</xsl:template>

<xsl:template match="*" mode="book.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="book.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="title" mode="book.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="book.titlepage.recto.style">
<xsl:apply-templates select="." mode="book.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="subtitle" mode="book.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="book.titlepage.recto.style">
<xsl:apply-templates select="." mode="book.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="edition" mode="book.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="book.titlepage.recto.style">
<xsl:apply-templates select="." mode="book.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="abstract" mode="book.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="book.titlepage.recto.style">
<xsl:apply-templates select="." mode="book.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="mediaobject" mode="book.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="book.titlepage.recto.style">
<xsl:apply-templates select="." mode="book.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="copyright" mode="book.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="book.titlepage.recto.style">
<xsl:apply-templates select="." mode="book.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template name="chapter.titlepage.recto">
  <xsl:choose>
    <xsl:when test="chapterinfo/title">
      <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/title"/>
    </xsl:when>
    <xsl:when test="docinfo/title">
      <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/title"/>
    </xsl:when>
    <xsl:when test="info/title">
      <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/title"/>
    </xsl:when>
    <xsl:when test="title">
      <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="title"/>
    </xsl:when>
  </xsl:choose>

  <xsl:choose>
    <xsl:when test="chapterinfo/subtitle">
      <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/subtitle"/>
    </xsl:when>
    <xsl:when test="docinfo/subtitle">
      <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/subtitle"/>
    </xsl:when>
    <xsl:when test="info/subtitle">
      <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/subtitle"/>
    </xsl:when>
    <xsl:when test="subtitle">
      <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="subtitle"/>
    </xsl:when>
  </xsl:choose>

  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/corpauthor"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/corpauthor"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/corpauthor"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/authorgroup"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/authorgroup"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/authorgroup"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/author"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/author"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/author"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/othercredit"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/othercredit"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/othercredit"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/releaseinfo"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/releaseinfo"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/releaseinfo"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/copyright"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/copyright"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/copyright"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/legalnotice"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/legalnotice"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/legalnotice"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/pubdate"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/pubdate"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/pubdate"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/revision"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/revision"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/revision"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/revhistory"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/revhistory"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/revhistory"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="chapterinfo/abstract"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="docinfo/abstract"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.auto.mode" select="info/abstract"/>
</xsl:template>

<xsl:template name="chapter.titlepage.verso">
</xsl:template>

<xsl:template name="chapter.titlepage.separator">
</xsl:template>

<xsl:template name="chapter.titlepage.before.recto">
</xsl:template>

<xsl:template name="chapter.titlepage.before.verso">
</xsl:template>

<xsl:template name="chapter.titlepage">
  <div class="titlepage">
    <xsl:variable name="recto.content">
      <xsl:call-template name="chapter.titlepage.before.recto"/>
      <xsl:call-template name="chapter.titlepage.recto"/>
    </xsl:variable>
    <xsl:variable name="recto.elements.count">
      <xsl:choose>
        <xsl:when test="function-available('exsl:node-set')"><xsl:value-of select="count(exsl:node-set($recto.content)/*)"/></xsl:when>
        <xsl:when test="contains(system-property('xsl:vendor'), 'Apache Software Foundation')">
          <!--Xalan quirk--><xsl:value-of select="count(exsl:node-set($recto.content)/*)"/></xsl:when>
        <xsl:otherwise>1</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="(normalize-space($recto.content) != '') or ($recto.elements.count &gt; 0)">
      <div><xsl:copy-of select="$recto.content"/></div>
    </xsl:if>
    <xsl:variable name="verso.content">
      <xsl:call-template name="chapter.titlepage.before.verso"/>
      <xsl:call-template name="chapter.titlepage.verso"/>
    </xsl:variable>
    <xsl:variable name="verso.elements.count">
      <xsl:choose>
        <xsl:when test="function-available('exsl:node-set')"><xsl:value-of select="count(exsl:node-set($verso.content)/*)"/></xsl:when>
        <xsl:when test="contains(system-property('xsl:vendor'), 'Apache Software Foundation')">
          <!--Xalan quirk--><xsl:value-of select="count(exsl:node-set($verso.content)/*)"/></xsl:when>
        <xsl:otherwise>1</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="(normalize-space($verso.content) != '') or ($verso.elements.count &gt; 0)">
      <div><xsl:copy-of select="$verso.content"/></div>
    </xsl:if>
    <xsl:call-template name="chapter.titlepage.separator"/>
  </div>
</xsl:template>

<xsl:template match="*" mode="chapter.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="chapter.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="title" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="subtitle" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="corpauthor" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="authorgroup" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="author" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="othercredit" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="releaseinfo" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="copyright" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="legalnotice" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="pubdate" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="revision" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="revhistory" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="abstract" mode="chapter.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="chapter.titlepage.recto.style">
<xsl:apply-templates select="." mode="chapter.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template name="section.titlepage.recto">
  <xsl:choose>
    <xsl:when test="sectioninfo/title">
      <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/title"/>
    </xsl:when>
    <xsl:when test="info/title">
      <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/title"/>
    </xsl:when>
    <xsl:when test="title">
      <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="title"/>
    </xsl:when>
  </xsl:choose>

  <xsl:choose>
    <xsl:when test="sectioninfo/subtitle">
      <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/subtitle"/>
    </xsl:when>
    <xsl:when test="info/subtitle">
      <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/subtitle"/>
    </xsl:when>
    <xsl:when test="subtitle">
      <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="subtitle"/>
    </xsl:when>
  </xsl:choose>

  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/corpauthor"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/corpauthor"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/authorgroup"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/authorgroup"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/author"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/author"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/othercredit"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/othercredit"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/releaseinfo"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/releaseinfo"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/copyright"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/copyright"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/legalnotice"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/legalnotice"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/pubdate"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/pubdate"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/revision"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/revision"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/revhistory"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/revhistory"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="sectioninfo/abstract"/>
  <xsl:apply-templates mode="section.titlepage.recto.auto.mode" select="info/abstract"/>
</xsl:template>

<xsl:template name="section.titlepage.verso">
</xsl:template>

<xsl:template name="section.titlepage.separator"><xsl:if test="count(parent::*)='0'"><hr/></xsl:if>
</xsl:template>

<xsl:template name="section.titlepage.before.recto">
</xsl:template>

<xsl:template name="section.titlepage.before.verso">
</xsl:template>

<xsl:template name="section.titlepage">
  <div class="titlepage">
    <xsl:variable name="recto.content">
      <xsl:call-template name="section.titlepage.before.recto"/>
      <xsl:call-template name="section.titlepage.recto"/>
    </xsl:variable>
    <xsl:variable name="recto.elements.count">
      <xsl:choose>
        <xsl:when test="function-available('exsl:node-set')"><xsl:value-of select="count(exsl:node-set($recto.content)/*)"/></xsl:when>
        <xsl:when test="contains(system-property('xsl:vendor'), 'Apache Software Foundation')">
          <!--Xalan quirk--><xsl:value-of select="count(exsl:node-set($recto.content)/*)"/></xsl:when>
        <xsl:otherwise>1</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="(normalize-space($recto.content) != '') or ($recto.elements.count &gt; 0)">
      <div><xsl:copy-of select="$recto.content"/></div>
    </xsl:if>
    <xsl:variable name="verso.content">
      <xsl:call-template name="section.titlepage.before.verso"/>
      <xsl:call-template name="section.titlepage.verso"/>
    </xsl:variable>
    <xsl:variable name="verso.elements.count">
      <xsl:choose>
        <xsl:when test="function-available('exsl:node-set')"><xsl:value-of select="count(exsl:node-set($verso.content)/*)"/></xsl:when>
        <xsl:when test="contains(system-property('xsl:vendor'), 'Apache Software Foundation')">
          <!--Xalan quirk--><xsl:value-of select="count(exsl:node-set($verso.content)/*)"/></xsl:when>
        <xsl:otherwise>1</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="(normalize-space($verso.content) != '') or ($verso.elements.count &gt; 0)">
      <div><xsl:copy-of select="$verso.content"/></div>
    </xsl:if>
    <xsl:call-template name="section.titlepage.separator"/>
  </div>
</xsl:template>

<xsl:template match="*" mode="section.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="section.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="title" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="subtitle" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="corpauthor" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="authorgroup" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="author" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="othercredit" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="releaseinfo" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="copyright" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="legalnotice" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="pubdate" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="revision" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="revhistory" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

<xsl:template match="abstract" mode="section.titlepage.recto.auto.mode">
<div xsl:use-attribute-sets="section.titlepage.recto.style">
<xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</div>
</xsl:template>

</xsl:stylesheet>

