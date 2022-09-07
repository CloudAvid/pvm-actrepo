# Process this file with autoconf to produce a configure script.

m4_define([package_name], [pvm-actrepo])
m4_define([major_version], [1])
m4_define([minor_version], [0])
m4_define([micro_version], [0])
m4_define([release_version], [1])
m4_define([package_version],
	[major_version.minor_version.micro_version])

AC_PREREQ([2.63])
AC_INIT([package_name], [package_version], [package_name@cloadavid.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/)
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

AC_LANG([C++])
AC_PROG_CXX
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
LT_INIT([disable-static], [enable-silent-rules])
AC_PROG_LIBTOOL
CXXFLAGS="-Wall -O0 -g -std=c++0x -fPIC"
CFLAGS="-Wall -O0 -g"

PKG_CHECK_MODULES([PPARAM], [pparam])
PKG_CHECK_MODULES([PUTIL], [putil])
PKG_CHECK_MODULES([IPC], [pvm-ipc])
PKG_CHECK_MODULES([PLOGGER], [plogger])

AC_CHECK_LIB([pparam], [loadXmlDoc])

AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/file.h sys/socket.h syslog.h unistd.h])

AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

AC_FUNC_FORK
AC_CHECK_FUNCS([dup2 memset pow socket strerror strtol strtoul sysinfo])

AC_ARG_ENABLE([debugging], AS_HELP_STRING([--enable-debugging],[Enable debugging mode]),
[if test x$enableval = xyes; then
    AC_DEFINE(__DEBUG__,[1],[Enable debugging mode.])
     fi])

PACKAGE_RPMNAME="package_name"
PACKAGE_RPMVERSION="major_version.minor_version.micro_version"
PACKAGE_TARNAME="package_name-major_version.minor_version.micro_version"
MAJOR_VERSION="major_version"
MINOR_VERSION="minor_version"
MICRO_VERSION="micro_version"
RELEASE_VERSION="release_version"
LIBPACTREPO_SO_VERSION="$((major_version + minor_version)):micro_version:minor_version"

AC_SUBST([PACKAGE_RPMNAME])
AC_SUBST([PACKAGE_RPMVERSION])
AC_SUBST([PACKAGE_TARNAME])
AC_SUBST([MAJOR_VERSION])
AC_SUBST([MINOR_VERSION])
AC_SUBST([MICRO_VERSION])
AC_SUBST([RELEASE_VERSION])
AC_SUBST([LIBPACTREPO_SO_VERSION])

RPM_PASSPHRASE="somePassword"
AC_SUBST([RPM_PASSPHRASE])

test "x$prefix" = xNONE && prefix=/usr
if test -e /usr/lib64; then
	libdir=${prefix}/lib64
else
	libdir=${prefix}/lib
fi
localstatedir=/var
bindir=${prefix}/sbin
sysconfdir=/etc
includedir=${prefix}/include

AC_CONFIG_FILES(Makefile
	src/Makefile
	package_name.pc
	package_name.spec
	package_name.info
	)
AC_OUTPUT
