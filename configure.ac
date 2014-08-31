AC_PREREQ([2.69])
AC_INIT([mkmnfst], [0.3], [support@mnfst.io])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AM_PATH_GPGME(,,
   [AC_MSG_ERROR([Could not find GPGME. Please define GPGME_CFLAGS and
                  GPGME_LIBS if it is installed.])])
PKG_CHECK_MODULES([CURL], [libcurl])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([realpath strstr strlcpy reallocarray])

AC_CONFIG_FILES([Makefile src/Makefile man/Makefile])
AC_OUTPUT
