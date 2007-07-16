## Process this file with automake to produce Makefile.in

dist_bin_SCRIPTS = cryptkeeper_password

install-data-local:
	$(mkinstalldirs) $(DESTDIR)/$(prefix)/share/applications
	$(mkinstalldirs) $(DESTDIR)/$(prefix)/share/pixmaps
	$(INSTALL_DATA) cryptkeeper.desktop $(DESTDIR)/$(prefix)/share/applications/
	$(INSTALL_DATA) cryptkeeper.png $(DESTDIR)/$(prefix)/share/pixmaps/

uninstall-local:
	rm -f $(DESTDIR)/$(prefix)/share/applications/cryptkeeper.desktop
	rm -f $(DESTDIR)/$(prefix)/share/pixmaps/cryptkeeper.png

SUBDIRS = po src/

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath m4/ChangeLog lib/gettext.h
