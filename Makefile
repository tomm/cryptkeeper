
CXX = g++
LIBS = -g `pkg-config --libs gtk+-2.0 gconf-2.0`
CFLAGS = -g `pkg-config --cflags gtk+-2.0 gconf-2.0` -O2 -Wall
OPTIONS = 

VERSION="0.2.666"

SRCS = main.cpp CreateStashWizard.cpp ImportStashWizard.cpp ConfigDialog.cpp

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
	cp install.sh cryptkeeper-$(VERSION)
	cp TODO cryptkeeper-$(VERSION)
	cp Makefile cryptkeeper-$(VERSION)
	cp *.cpp *.h cryptkeeper-$(VERSION)
	tar cvzf cryptkeeper-$(VERSION).tar.gz cryptkeeper-$(VERSION)
	rm -rf cryptkeeper-$(VERSION)

binball: cryptkeeper
	mkdir cryptkeeper-$(VERSION)
	cp install.sh cryptkeeper-$(VERSION)
	cp cryptkeeper cryptkeeper-$(VERSION)
	cp cryptkeeper_password cryptkeeper-$(VERSION)
	tar cvzf cryptkeeper-$(VERSION).tar.gz cryptkeeper-$(VERSION)
	rm -rf cryptkeeper-$(VERSION)

ifneq (,$(wildcard Makefile.dep))
include Makefile.dep
endif

