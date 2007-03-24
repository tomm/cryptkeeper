
CXX = g++
LIBS = -g `pkg-config gtk+-2.0 --libs`
CFLAGS = -g `pkg-config gtk+-2.0 --cflags` -O2 -Wall
OPTIONS = 

VERSION="0.1.666"

SRCS = main.cpp CreateStashWizard.cpp ImportStashWizard.cpp

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

