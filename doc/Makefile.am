AUTOMAKE_OPTIONS = 1.9

datadir = @datadir@/doc/@PACKAGE@

dist_data_DATA = \
  BUGS_ja.txt \
  EXCELFILE_ja.txt \
  INIFILE_ja.txt \
  STATUS_ja.txt \
  TMPLFILE_ja.txt

dist-hook:
	cp -r $(srcdir)/usersman-ja $(distdir)
	rm -rf `find $(distdir)/usersman-ja -name 'CVS'`
