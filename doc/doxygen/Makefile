#Statt diesem Makefile koennen Sie auch das im uebergeordneten Verzeichnis benutzen. Es bietet neben diesen noch weitere Optionen...
#Browser um die Doku anzuzeigen, bei Verwendung des uebergeordneten Makefiles wird diese Einstellung uebergangen
BROWSER = firefox
doc_html:
	doxygen Doxyfile_html > "doxyoutput.log"
doc_pdf:
	doxygen Doxyfile_latex > "doxyoutput.log"
	$(MAKE) -C latex >> "doxywarn.log"
view:
	doxygen
	$(BROWSER) file://`pwd`/html/index.html &
clean :
	rm -f -r html latex man
	rm -f doxyoutput.log doxywarn.log
