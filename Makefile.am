## Process this file with automake to produce Makefile.in

dist_bin_SCRIPTS = cryptkeeper_password

install-data-local:
	$(mkinstalldirs) $(DESTDIR)/share/applications
	$(mkinstalldirs) $(DESTDIR)/share/pixmaps
	$(INSTALL_DATA) cryptkeeper.desktop $(DESTDIR)/share/applications/
	$(INSTALL_DATA) cryptkeeper.png $(DESTDIR)/share/pixmaps/

SUBDIRS = src/
