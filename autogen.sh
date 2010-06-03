#!/bin/sh
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.
ORIGDIR=`pwd`
cd $srcdir
PROJECT=Log4g
TEST_TYPE=-f
FILE=log4g/log4g.h
DIE=0
have_libtool=false
if libtoolize --version < /dev/null > /dev/null 2>&1 ; then
    libtool_version=`libtoolize --version |
        head -1 |
        sed -e 's/^\(.*\)([^)]*)\(.*\)$/\1\2/g' \
            -e 's/^[^0-9]*\([0-9.][0-9.]*\).*/\1/'`
        case $libtool_version in
        2.2*)
        have_libtool=true
        ;;
        esac
fi
if $have_libtool ; then : ; else
    echo
    echo "Libtool must be installed to compile $PROJECT."
    echo "Install the appropriate package for your distribution,"
    echo "or get the source tarball at http://ftp.gnu.org/gnu/libtool/"
    DIE=1
fi
(gtkdocize --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "Gtk-doc must be installed to compile $PROJECT."
    echo "Install the appropriate package for your distribution,"
    echo "or get the source tarball at ftp://ftp.gnome.org/pub/GNOME/sources/gtk-doc/"
    DIE=1
}
(autoconf --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "Autoconf must be installed to compile $PROJECT."
    echo "Install the appropriate package for your distribution,"
    echo "or get the source tarball at http://ftp.gnu.org/gnu/autoconf/"
    DIE=1
}
if automake-1.11 --version < /dev/null > /dev/null 2>&1 ; then
    AUTOMAKE=automake-1.11
    ACLOCAL=aclocal-1.11
else
    echo
    echo "Automake 1.11.x must be installed to compile $PROJECT."
    echo "Install the appropriate package for your distribution,"
    echo "or get the source tarball at http://ftp.gnu.org/gnu/automake/"
    DIE=1
fi
if test "$DIE" -eq 1; then
    exit 1
fi
test $TEST_TYPE $FILE || {
    echo
    echo "You must run this script in the top-level $PROJECT directory"
    exit 1
}
if test -z "$AUTOGEN_SUBDIR_MODE"; then
    if test -z "$*"; then
        echo
        echo "Running ./configure with no arguments. Arguments may be "
        echo "specified on the $0 command line."
    fi
fi
rm -rf autom4te.cache
$ACLOCAL -I m4 $ACLOCAL_FLAGS || exit $?
libtoolize --force || exit $?
gtkdocize || exit $?
autoheader || exit $?
$AUTOMAKE --add-missing || exit $?
autoconf || exit $?
cd $ORIGDIR || exit $?
if test -z "$AUTOGEN_SUBDIR_MODE"; then
    $srcdir/configure $AUTOGEN_CONFIGURE_ARGS "$@" || exit $?
    echo
    echo "Now type 'make' to compile $PROJECT."
fi
