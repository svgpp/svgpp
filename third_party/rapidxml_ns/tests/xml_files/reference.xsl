<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<!-- This stylesheet converts Doxygen XML output into a neat HTML reference file -->

	<xsl:template match="/doxygenindex">
		<html>
			<body>

				<h2>RapidXml Reference</h2>

				<!-- Index -->

				<dl>
					<xsl:for-each select="compound">
						<xsl:if test="document(concat(@refid, '.xml'))/doxygen/compounddef/@prot='public'">
							<xsl:variable name="compoundName" select="document(concat(@refid, '.xml'))/doxygen/compounddef/compoundname"></xsl:variable>
							<dt>
								<xsl:value-of select="@kind"/>
								<xsl:if test="document(concat(@refid, '.xml'))/doxygen/compounddef/templateparamlist!=''">
									template
								</xsl:if>
								<xsl:text> </xsl:text>
								<a href="#{@refid}">
									<xsl:value-of select="substring-after(name, '::')"/>
								</a>
							</dt>
							<xsl:for-each select="member">
								<dt>
									<xsl:variable name="memberId" select="@refid"></xsl:variable>
									<xsl:for-each select="document(concat(../@refid, '.xml'))/doxygen/compounddef//memberdef">
										<xsl:if test="@prot='public' and @id=$memberId">
											<xsl:value-of select="@kind"/>
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

				<!-- Descriptions -->

				<xsl:for-each select="compound">
					<xsl:if test="@kind='class'">

						<!-- Compound -->

						<xsl:variable name="compoundId" select="@refid"></xsl:variable>
						<xsl:for-each select="document(concat(@refid, '.xml'))/doxygen//compounddef">
							<xsl:if test="@prot='public' and @id=$compoundId">

								<hr></hr>

								<!-- Compound name -->

								<h3 id="{@id}">
									<xsl:value-of select="@kind"/>
									<xsl:if test="templateparamlist!=''">
										template
									</xsl:if>
									<xsl:text> </xsl:text>
									<xsl:value-of select="compoundname"/>
								</h3>

								<!-- Compound include -->

								<h4>Include</h4>
								<![CDATA[#include "]]><a href="../../{normalize-space(includes)}">
									<xsl:value-of select="includes"/>
								</a>
								<![CDATA["]]>

								<!-- Compound parameters -->

								<xsl:if test="templateparamlist!=''">
									<xsl:if test="detaileddescription/para/parameterlist!=''">
										<h4>Parameters</h4>
										<xsl:for-each select="detaileddescription/para/parameterlist/parameteritem">
											<dl>
												<dt>
													<xsl:value-of select="parameternamelist/parametername"/>
												</dt>
												<dd>
													<xsl:value-of select="parameterdescription/para"/>
												</dd>
											</dl>
										</xsl:for-each>
									</xsl:if>
								</xsl:if>

								<!-- Compound description -->

								<h4>Description</h4>
								<xsl:value-of select="briefdescription"/>
								<xsl:value-of select="detaileddescription/para/text()"/>

								<p></p>

							</xsl:if>
						</xsl:for-each>

						<!-- Member -->

						<xsl:for-each select="member">
							<xsl:variable name="memberId" select="@refid"></xsl:variable>
							<xsl:for-each select="document(concat(../@refid, '.xml'))/doxygen/compounddef//memberdef">
								<xsl:if test="@prot='public' and @id=$memberId">

									<!-- Member name -->

									<hr></hr>
									<h3 id="{@id}">
										<xsl:value-of select="@kind"/>
										<xsl:text> </xsl:text>
										<xsl:value-of select="definition"/><xsl:value-of select="argsstring"/>
									</h3>

									<!-- Member synopsis -->

									<h4>Synopsis</h4>
									<code>
										<xsl:value-of select="definition"/><xsl:value-of select="argsstring"/>
									</code>

									<!-- Member description -->

									<xsl:if test="briefdescription/para!='' or detaileddescription/para/text()!=''">
										<h4>Description</h4>
										<xsl:value-of select="briefdescription"/>
										<xsl:value-of select="detaileddescription/para/text()"/>
									</xsl:if>

									<!-- Member parameters -->

									<xsl:if test="detaileddescription/para/parameterlist!=''">
										<h4>Parameters</h4>
										<xsl:for-each select="detaileddescription/para/parameterlist">
											<dl>
												<dt>
													<xsl:value-of select="parameteritem/parameternamelist/parametername"/>
												</dt>
												<dd>
													<xsl:value-of select="parameteritem/parameterdescription/para"/>
												</dd>
											</dl>
										</xsl:for-each>
									</xsl:if>

									<!-- Member returns -->

									<xsl:if test="detaileddescription/para/simplesect!=''">
										<h4>Returns</h4>
										<xsl:value-of select="detaileddescription/para/simplesect/para"/>
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

</xsl:stylesheet>

