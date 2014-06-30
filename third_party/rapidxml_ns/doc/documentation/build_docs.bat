echo Step 1: Running doxygen on RapidXml sources...
bin\doxygen.exe

echo Step 2: Running xltproc on doxygen output...
cd xml
copy ..\reference.xsl reference.xsl /Y >nul
..\bin\xsltproc.exe -o ..\tmp.html reference.xsl index.xml
del reference.xsl
cd ..

echo Step 3: Running postprocessor on xltproc output...
bin\postprocessor.exe <tmp.html >..\..\manual.html
rem del tmp.html

echo Done.
