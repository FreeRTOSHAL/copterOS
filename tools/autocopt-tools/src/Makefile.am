#Copyright (c) 2016 Andreas Werner
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

ACLOCAL_AMFLAGS = -I $(top_srcdir)/m4
lib_LTLIBRARIES = libcopt.la

libcopt_la_SOURCES = autocopt.c emergency.c
libcopt_la_CFLAGS = -I$(top_srcdir)/include
libcopt_la_LDFLAGS = -version-info 1:0:0

bin_PROGRAMS=shutdown reset ping clear pid control
shutdown_SOURCES=shutdown.c
shutdown_CFLAGS=-I$(top_srcdir)/include
reset_SOURCES=reset.c
reset_CFLAGS=-I$(top_srcdir)/include
reset_LDADD=libcopt.la
ping_SOURCES=ping.c
ping_CFLAGS=-I$(top_srcdir)/include
ping_LDADD=libcopt.la
clear_SOURCES=clear.c
clear_CFLAGS=-I$(top_srcdir)/include
clear_LDADD=libcopt.la
pid_SOURCES=pid.c
pid_CFLAGS=-I$(top_srcdir)/include
pid_LDADD=libcopt.la
control_SOURCES=control.c
control_CFLAGS=-I$(top_srcdir)/include
control_LDADD=libcopt.la
