
CXX = g++
LIBS = -g `pkg-config --libs gtk+-2.0 gconf-2.0`
CFLAGS = -g `pkg-config --cflags gtk+-2.0 gconf-2.0` -O2 -Wall
OPTIONS = 

VERSION="0.4.666"

PREFIX=/usr

SRCS = main.cpp CreateStashWizard.cpp ImportStashWizard.cpp ConfigDialog.cpp \
	PasswordChangeDialog.cpp encfs_wrapper.cpp

OBJS = $(SRCS:.cpp=.o)

.cpp.o:
	$(CXX) $(CFLAGS) $(OPTIONS) -c $<

cryptkeeper: $(OBJS)
	$(CXX) -o cryptkeeper $(OBJS) $(LIBS)

clean:
	-rm *.o
	-rm cryptkeeper

depends: $(SRCS)
	$(CXX) -MM $(CFLAGS) $(SRCS) > Makefile.dep

srcball:
	mkdir cryptkeeper-$(VERSION)
	cp TODO cryptkeeper-$(VERSION)
	cp Makefile cryptkeeper-$(VERSION)
	cp cryptkeeper.desktop cryptkeeper-$(VERSION)
	cp COPYING cryptkeeper-$(VERSION)
	cp cryptkeeper.png cryptkeeper-$(VERSION)
	cp cryptkeeper_password cryptkeeper-$(VERSION)
	cp *.cpp *.h cryptkeeper-$(VERSION)
	tar cvzf cryptkeeper-$(VERSION).tar.gz cryptkeeper-$(VERSION)
	rm -rf cryptkeeper-$(VERSION)

install:
	install cryptkeeper $(PREFIX)/bin/
	install cryptkeeper_password $(PREFIX)/bin/
	install cryptkeeper.desktop $(PREFIX)/share/applications/
	install cryptkeeper.png $(PREFIX)/share/pixmaps/

ifneq (,$(wildcard Makefile.dep))
include Makefile.dep
endif

