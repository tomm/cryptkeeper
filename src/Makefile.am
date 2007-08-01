## Process this file with automake to produce Makefile.in

#lib_LTLIBRARIES		= libhello.la
#libhello_la_SOURCES	= hello.c
#libhello_la_LDFLAGS	= -version-info 0:0:0

DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

bin_PROGRAMS = cryptkeeper
cryptkeeper_SOURCES = encfs_wrapper.cpp ConfigDialog.cpp ImportStashWizard.cpp CreateStashWizard.cpp \
PasswordChangeDialog.cpp main.cpp gtkstatusicon.c gtktrayicon-x11.c \
ConfigDialog.h cryptkeeper.h encfs_wrapper.h gtktrayicon.h PasswordChangeDialog.h \
CreateStashWizard.h defines.h gtkstatusicon.h ImportStashWizard.h \
PasswordEntryDialog.cpp PasswordEntryDialog.h

#hello_LDADD		= libhello.la
