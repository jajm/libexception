AC_PREREQ([2.67])
LT_PREREQ([2.2])

AC_INIT([libexception], [1.1.0], [jajm@jajm.org])
LT_INIT()

AC_SUBST(LIBTOOL_CURRENT, 0)
AC_SUBST(LIBTOOL_REVISION, 0)
AC_SUBST(LIBTOOL_AGE, 0)

AC_PROG_CC
AC_CHECK_HEADERS([stdio.h setjmp.h stdarg.h stdlib.h string.h stddef.h])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	libexception.pc
	config.mk
])

AC_OUTPUT
