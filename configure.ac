AC_PREREQ([2.69])
AC_INIT([mkmnfst], [0.1], [support@mnfst.io])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AM_PATH_GPGME
PKG_CHECK_MODULES([CURL], [libcurl])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([realpath])

AC_CONFIG_FILES([Makefile src/Makefile man/Makefile])
AC_OUTPUT
