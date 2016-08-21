@echo off
setlocal

rem   XSL Formmatter

XSLCmd -d temp\document.xml -s temp\axf.xsl -o usersmanual-ja.pdf

pause
endlocal
