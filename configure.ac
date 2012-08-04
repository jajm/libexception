AC_PREREQ([2.67])
AC_INIT([libexception], [0.1], [jajm@jajm.org])
AM_INIT_AUTOMAKE([-Wall -Werror silent-rules subdir-objects])
AC_CONFIG_SRCDIR([src/exception.c])
AC_CONFIG_HEADERS([config.h])

# Libtool version info
LIBEXCEPTION_CUR=0
LIBEXCEPTION_REV=0
LIBEXCEPTION_AGE=0
AC_SUBST(LIBEXCEPTION_CUR)
AC_SUBST(LIBEXCEPTION_REV)
AC_SUBST(LIBEXCEPTION_AGE)

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([setjmp.h stdarg.h stdio.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	t/Makefile
	libexception.pc
])

AM_SILENT_RULES

AC_OUTPUT
