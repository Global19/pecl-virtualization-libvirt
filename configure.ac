AC_INIT([libvirt-php], [0.4], [http://libvirt.org])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_FILES([Makefile src/Makefile docs/Makefile])
AC_CHECK_LIB([virt], [virConnectOpen], [], AC_MSG_ERROR([You need libvirt to compile this module]))
AM_INIT_AUTOMAKE([-Wno-portability])
AM_MAINTAINER_MODE([enable])

dnl Specific XML catalog file for validation of generated html
AC_ARG_WITH([xml-catalog-file],
            [AC_HELP_STRING([--with-xml-catalog-file=path],
                [path to XML catalog file for validating
                 generated html, default /etc/xml/catalog])],
            [XML_CATALOG_FILE=$withval],
            [XML_CATALOG_FILE='/etc/xml/catalog'])
AC_SUBST([XML_CATALOG_FILE])

# External programs to generate documentation
AC_PATH_PROG([XSLTPROC], [xsltproc], [/usr/bin/xsltproc])
AC_PATH_PROG([XMLLINT], [xmllint], [/usr/bin/xmllint])
AC_PATH_PROG([XMLCATALOG], [xmlcatalog], [/usr/bin/xmlcatalog])

AC_PATH_PROG([PHPIZE], [phpize], [no])
if test "x$PHPIZE" = "xno"; then
    AC_MSG_ERROR([phpize not found; please install the PHP SDK])
fi

AC_PATH_PROG([PHPCONFIG], [php-config], [no])
if test "x$PHPCONFIG" = "xno"; then
    AC_MSG_ERROR([php-config not found; please install the PHP SDK])
fi

AC_SUBST([PHPIZE])
AC_SUBST([PHPCONFIG])
AC_OUTPUT