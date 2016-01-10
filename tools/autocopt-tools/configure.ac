#Copyright (c) 2013 Andreas Werner
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.
AC_INIT([quato-tools], [1.0], [webmaster@andy89.org, torben.gruenewald@web.de])
AC_PREREQ([2.67])
AC_CONFIG_MACRO_DIR([m4])
AC_REVISION([Version 1.0 17.07.2015])
AC_COPYRIGHT([Copyright (c) by Andreas Werner <webmaster@andy89.org> and Torben Grünewald <torben.gruenewald@web.de> Version 1.0 (17.07.2015)])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])
AS_VAR_APPEND(CFLAGS, " -Wall -Werror -Wno-long-long ")
AS_VAR_APPEND(CXXFLAGS, " -Wall -Werror -Wno-long-long -fno-exceptions ")
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)

AC_CONFIG_SRCDIR([src])

AC_CANONICAL_HOST

AC_CHECK_DECL([usleep], AC_DEFINE(_BSD_SOURCE), [
	#remove Variable in cache
	AS_UNSET(ac_cv_have_decl_usleep)
	AC_CHECK_DECL([usleep], AC_DEFINE(_DEFAULT_SOURCE), [AC_MSG_ERROR([can not include usleep])], [[
#define _DEFAULT_SOURCE
#include <unistd.h>
	]]
	)
],[[
#define _BSD_SOURCE
#include <unistd.h>
]])
AC_CHECK_DECL([sigaction],, [
	AS_UNSET(ac_cv_have_decl_sigaction)
	AC_CHECK_DECL([sigaction], AC_DEFINE(_POSIX_C_SOURCE, [199506L]), [AC_MSG_ERROR([can not include sigaction])],[[
#define _POSIX_C_SOURCE 1
#include <signal.h>
	]])
])

AC_CHECK_HEADER([stdint.h],,AC_MSG_ERROR([stdint.h not found]))
AC_CHECK_HEADER([string.h],,AC_MSG_ERROR([string.h not found]))
AC_CHECK_HEADER([stdio.h],,AC_MSG_ERROR([stdio.h not found]))
AC_CHECK_HEADER([stdlib.h],,AC_MSG_ERROR([stdlib.h not found]))
AC_CHECK_HEADER([stdarg.h],,AC_MSG_ERROR([stdarg.h not found]))

AC_PROG_CXX

#Check Libs
#Example: (That is only an example. It didn't work because sin is a build-in Funktion of gcc). 
#For -lm add "progname_ldadd = -lm" in Makefile.am
#AC_CHECK_LIB([m],[sin],,AC_MSG_ERROR([libm not found]))

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [Activiert Debugin]), [
	AS_VAR_APPEND(CFLAGS, [ -g ])
	AS_VAR_APPEND(CXXFLAGS, [ -g ])
],[
])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	include/Makefile
])

AC_OUTPUT
