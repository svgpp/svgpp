<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="xml" encoding="utf-8"/>

	<!-- This stylesheet converts Doxygen XML output into a HTML manual file -->

	<xsl:template match="/doxygenindex">
		<html>

			<!-- ************************** -->
			<!-- Style -->
			<!-- ************************** -->

			<head>
				<style type="text/css">

          body
          {
          font-family: sans-serif;
          font-size: 90%;
          margin: 8pt 8pt 8pt 8pt;
          text-align: justify;
          background-color: White;
          }

          h1 { font-weight: bold; text-align: left;  }
          h2 { font: 140% sans-serif; font-weight: bold; text-align: left;  }
          h3 { font: 120% sans-serif; font-weight: bold; text-align: left;  }
          h4 { font: bold 100% sans-serif; font-weight: bold; text-align: left;  }
          h5 { font: italic 100% sans-serif; font-weight: bold; text-align: left;  }
          h6 { font: small-caps 100% sans-serif; font-weight: bold; text-align: left;  }

          code
          {
          font-family: "Courier New", Courier, mono;
          }

          pre
          {
          border-top: gray 0.5pt solid;
          border-right: gray 0.5pt solid;
          border-left: gray 0.5pt solid;
          border-bottom: gray 0.5pt solid;
          padding-top: 2pt;
          padding-right: 2pt;
          padding-left: 2pt;
          padding-bottom: 2pt;
          display: block;
          font-family: "courier new", courier, mono;
          background-color: #eeeeee;
          }

          a
          {
          color: #000080;
          text-decoration: none;
          }

          a:hover
          {
          text-decoration: underline;
          }

          .reference-header
          {
          border-top: gray 0.5pt solid;
          border-right: gray 0.5pt solid;
          border-left: gray 0.5pt solid;
          border-bottom: gray 0.5pt solid;
          padding-top: 2pt;
          padding-right: 2pt;
          padding-left: 2pt;
          padding-bottom: 2pt;
          background-color: #dedede;
          }

          .parameter-name
          {
          font-style: italic;
          }

          .indented
          {
          margin-left: 0.5cm;
          }

          a.toc1
          {
          margin-left: 0.0cm;
          }

          a.toc2
          {
          margin-left: 0.75cm;
          }

          a.toc3
          {
          margin-left: 1.5cm;
          }

        </style>
			</head>

			<body>

				<!-- ************************** -->
				<!-- Manual  -->
				<!-- ************************** -->

        <h1>RAPIDXML NS Manual</h1>

        <h3>Version 1.13</h3>

			  <!-- rapidxml_ns namespace -->

			  <xsl:for-each select="compound">
				  <xsl:if test="@kind='namespace' and name='rapidxml_ns'">
					  <xsl:variable name="compoundId" select="@refid"></xsl:variable>
					  <xsl:for-each select="document(concat(@refid, '.xml'))/doxygen//compounddef">
						  <xsl:if test="@id=$compoundId">
							  <xsl:copy-of select="detaileddescription"/>
						  </xsl:if>
					  </xsl:for-each>
				  </xsl:if>
			  </xsl:for-each>

			  <!-- ************************** -->
			  <!-- Reference Index -->
			  <!-- ************************** -->

			  <dl>
				  <xsl:for-each select="compound">
					  <xsl:if test="document(concat(@refid, '.xml'))/doxygen/compounddef/@prot='public' 
								    or document(concat(@refid, '.xml'))/doxygen/compounddef/@kind='namespace'">

						  <!-- Compound -->

						  <xsl:variable name="compoundName" select="document(concat(@refid, '.xml'))/doxygen/compounddef/compoundname"></xsl:variable>
						  <dt>
							  <xsl:value-of select="@kind"/>
							  <xsl:if test="document(concat(@refid, '.xml'))/doxygen/compounddef/templateparamlist!=''">
								  template
							  </xsl:if>
							  <xsl:text> </xsl:text>
							  <a href="#{@refid}">
								  <xsl:value-of select="name"/>
							  </a>
						  </dt>

						  <xsl:for-each select="member">

							  <!-- Member -->

							  <dt class="indented">
								  <xsl:variable name="memberId" select="@refid"></xsl:variable>
								  <xsl:for-each select="document(concat(../@refid, '.xml'))/doxygen/compounddef//memberdef">
									  <xsl:if test="@prot='public' and @id=$memberId">
										  <xsl:call-template name="MemberKind">
											  <xsl:with-param name="compoundName" select="$compoundName"/>
										  </xsl:call-template>
										  <xsl:text> </xsl:text>
										  <a href="#{@id}">
											  <xsl:value-of select="name"/>
											  <xsl:value-of select="argsstring"/>
										  </a>
									  </xsl:if>
								  </xsl:for-each>
							  </dt>

						  </xsl:for-each>

					  </xsl:if>
					  <p></p>
				  </xsl:for-each>
			  </dl>

			  <hr/>

			  <!-- ************************** -->
			  <!-- Reference Descriptions -->
			  <!-- ************************** -->

			  <xsl:for-each select="compound">
				  <xsl:if test="@kind='class' or @kind='namespace'">

					  <!-- Compound -->

					  <xsl:variable name="compoundId" select="@refid"></xsl:variable>
					  <xsl:variable name="compoundName" select="document(concat(@refid, '.xml'))/doxygen//compounddef/compoundname"></xsl:variable>

					  <xsl:for-each select="document(concat(@refid, '.xml'))/doxygen//compounddef">
						  <xsl:if test="@prot='public' and @id=$compoundId">

							  <!-- Compound name -->

							  <h3 class="reference-header" id="{@id}">
								  <xsl:value-of select="@kind"/>
								  <xsl:if test="templateparamlist!=''">
									  template
								  </xsl:if>
								  <xsl:text> </xsl:text>
								  <xsl:value-of select="compoundname"/>
							  </h3>

							  <!-- Compound include -->

							  Defined in <a href="{includes}">
								  <xsl:value-of select="includes"/>
							  </a>

							  <!-- Compound base and derived classes -->

							  <xsl:if test="basecompoundref">
								  <br></br>
								  Inherits from
								  <xsl:for-each select="basecompoundref">
									  <a href="#{@refid}">
										  <xsl:value-of select="substring-after(document(concat(@refid, '.xml'))/doxygen/compounddef/compoundname, '::')"></xsl:value-of>
									  </a>
									  <space></space>
								  </xsl:for-each>
							  </xsl:if>
							  <br></br>
							  <xsl:if test="derivedcompoundref">
								  Base class for
								  <xsl:for-each select="derivedcompoundref">
									  <a href="#{@refid}">
										  <xsl:value-of select="substring-after(document(concat(@refid, '.xml'))/doxygen/compounddef/compoundname, '::')"></xsl:value-of>
									  </a>
									  <space></space>
								  </xsl:for-each>
							  </xsl:if>

							  <!-- Compound description -->

							  <h4>Description</h4>
							  <xsl:copy-of select="briefdescription/para"/>
							  <xsl:copy-of select="detaileddescription/para"/>

							  <!-- Compound parameters -->

							  <xsl:if test="templateparamlist!=''">
								  <xsl:if test="detaileddescription/para/parameterlist!=''">
									  <h4>Parameters</h4>
									  <xsl:for-each select="detaileddescription/para/parameterlist/parameteritem">
										  <dl>
											  <dt class="parameter-name">
												  <xsl:value-of select="parameternamelist/parametername"/>
											  </dt>
											  <dd>
												  <xsl:value-of select="parameterdescription/para"/>
											  </dd>
										  </dl>
									  </xsl:for-each>
								  </xsl:if>
							  </xsl:if>

							  <p></p>

						  </xsl:if>
					  </xsl:for-each>

					  <!-- Member -->

					  <xsl:for-each select="member">
						  <xsl:variable name="memberId" select="@refid"></xsl:variable>
						  <xsl:for-each select="document(concat(../@refid, '.xml'))/doxygen/compounddef//memberdef">
							  <xsl:if test="@prot='public' and @id=$memberId">

								  <!-- Member name -->

								  <h3 class="reference-header" id="{@id}">
									  <xsl:call-template name="MemberKind">
										  <xsl:with-param name="compoundName" select="$compoundName"/>
									  </xsl:call-template>
									  <xsl:text> </xsl:text>
									  <xsl:if test="substring-after($compoundName, '::')!=''"><xsl:value-of select="substring-after($compoundName, '::')"/>::</xsl:if><xsl:value-of select="name"/>
                  </h3>

								  <!-- Member synopsis -->

								  <xsl:if test="definition!=''">
									  <h4>Synopsis</h4>
									  <code class="synopsis">
										  <xsl:value-of select="definition"/><xsl:value-of select="normalize-space(argsstring)"/><xsl:if test="initializer!=''">
											  =<xsl:value-of select="initializer"/>
										  </xsl:if>;
									  </code>
								  </xsl:if>

                  <!-- Member description -->

								  <xsl:if test="briefdescription/para/text()!='' or detaileddescription/para/text()!=''">
									  <h4>Description</h4>
									  <xsl:copy-of select="briefdescription/para"/>
									  <xsl:copy-of select="detaileddescription/para"/>
								  </xsl:if>

								  <!-- Member parameters -->

								  <xsl:if test="detaileddescription/para/parameterlist!=''">
									  <h4>Parameters</h4>
									  <xsl:for-each select="detaileddescription/para/parameterlist/parameteritem">
										  <dl>
											  <dt class="parameter-name">
												  <xsl:value-of select="parameternamelist/parametername"/>
											  </dt>
											  <dd class="parameter-def">
												  <xsl:value-of select="parameterdescription/para"/>
											  </dd>
										  </dl>
									  </xsl:for-each>
								  </xsl:if>

								  <!-- Member returns -->

								  <xsl:if test="detaileddescription/para/simplesect!=''">
									  <h4>Returns</h4>
									  <xsl:value-of select="detaileddescription/para/simplesect/para"/>
								  </xsl:if>

								  <!-- Member enum values -->

								  <xsl:if test="@kind='enum'">
									  <h4>Values</h4>
									  <xsl:for-each select="enumvalue">
										  <dl>
											  <dt class="parameter-name">
												  <xsl:value-of select="name"/>
											  </dt>
											  <dd class="parameter-def">
												  <xsl:value-of select="briefdescription/para"/>
												  <xsl:value-of select="detaileddescription/para"/>
											  </dd>
										  </dl>
									  </xsl:for-each>
								  </xsl:if>

								  <p></p>

							  </xsl:if>
						  </xsl:for-each>
					  </xsl:for-each>

				  </xsl:if>
			  </xsl:for-each>
        
			</body>
		</html>
	</xsl:template>

	<xsl:template name="MemberKind">
		<xsl:param name="compoundName"/>
		<xsl:choose>
			<xsl:when test="@kind='variable' and starts-with(type, 'const')">
				constant
			</xsl:when>
			<xsl:when test="@kind='function' and substring-after($compoundName, '::')!='' and starts-with(name, substring-after($compoundName, '::'))">
				constructor
			</xsl:when>
			<xsl:when test="@kind='function' and starts-with(name, '~')">
				destructor
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="@kind"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>

