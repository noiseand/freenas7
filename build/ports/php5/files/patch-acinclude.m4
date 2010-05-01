--- acinclude.m4.orig	2009-11-29 06:13:22.000000000 +0100
+++ acinclude.m4	2010-04-30 20:59:11.000000000 +0200
@@ -194,7 +194,7 @@
 dnl
 dnl which array to append to?
 AC_DEFUN([PHP_ADD_SOURCES],[
-  PHP_ADD_SOURCES_X($1, $2, $3, ifelse($4,cli,PHP_CLI_OBJS,ifelse($4,sapi,PHP_SAPI_OBJS,PHP_GLOBAL_OBJS)))
+  PHP_ADD_SOURCES_X($1, $2, $3, ifelse($4,cli,PHP_CLI_OBJS,ifelse($4,sapi,PHP_SAPI_OBJS,ifelse($4,cgi,PHP_CGI_OBJS,PHP_GLOBAL_OBJS))))
 ])
 
 dnl
@@ -968,15 +968,10 @@
   if test "$3" != "shared" && test "$3" != "yes" && test "$4" = "cli"; then
 dnl ---------------------------------------------- CLI static module
     [PHP_]translit($1,a-z_-,A-Z__)[_SHARED]=no
-    case "$PHP_SAPI" in
-      cgi|embed[)]
-        PHP_ADD_SOURCES(PHP_EXT_DIR($1),$2,$ac_extra,)
-        EXT_STATIC="$EXT_STATIC $1"
-        ;;
-      *[)]
         PHP_ADD_SOURCES(PHP_EXT_DIR($1),$2,$ac_extra,cli)
         ;;
     esac
+    PHP_ADD_SOURCES(PHP_EXT_DIR($1),$2,$ac_extra,cgi)
     EXT_CLI_STATIC="$EXT_CLI_STATIC $1"
   fi
   PHP_ADD_BUILD_DIR($ext_builddir)
@@ -1026,12 +1021,6 @@
 build to be successful.
 ])
   fi
-  if test "x$is_it_enabled" = "xno" && test "x$3" != "xtrue"; then
-    AC_MSG_ERROR([
-You've configured extension $1, which depends on extension $2,
-but you've either not enabled $2, or have disabled it.
-])
-  fi
   dnl Some systems require that we link $2 to $1 when building
 ])
 
@@ -2303,9 +2292,9 @@
   test -z "$PHP_IMAP_SSL" && PHP_IMAP_SSL=no
 
   dnl Fallbacks for different configure options
-  if test "$PHP_OPENSSL" != "no"; then
+  if test -n "$PHP_OPENSSL" && test "$PHP_OPENSSL" != "no"; then
     PHP_OPENSSL_DIR=$PHP_OPENSSL
-  elif test "$PHP_IMAP_SSL" != "no"; then
+  elif test -n "$PHP_IMAP_SSL" && test "$PHP_IMAP_SSL" != "no"; then
     PHP_OPENSSL_DIR=$PHP_IMAP_SSL
   fi
 
