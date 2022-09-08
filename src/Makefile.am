AM_CPPFLAGS=\
	$(PPARAM_CFLAGS) \
	$(PUTIL_CFLAGS) \
	$(PLOGGER_CFLAGS)\
	$(IPC_CFLAGS)\
	-I../include

actrepoincludedir = $(includedir)/pvm/actrepo
actrepoinclude_HEADERS=\
		../include/plogger.hpp \
		../include/actrepo.hpp \
		../include/fireloop.hpp

lib_LTLIBRARIES= libpactrepo.la
libpactrepo_la_SOURCES=\
		plogger.cpp \
		actrepo.cpp \
		fireloop.cpp

libpactrepo_la_LDFLAGS= -version-info $(LIBPACTREPO_SO_VERSION)
libpactrepo_la_LIBADD=\
		$(PPARAM_LIBS) \
		$(PLOGGER_LIBS) \
		$(PUTIL_LIBS) \
		$(IPC_LIBS)

