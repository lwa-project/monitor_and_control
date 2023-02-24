AC_INIT([lwa_mcs], [0.1.0], [], [], [https://github.com/lwa-project/monitor_and_control/])
AC_LANG(C)
AC_CONFIG_SRCDIR([sch/msei.c])

AC_PREFIX_DEFAULT([/home/op1/MCS/])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config])

m4_ifdef([AM_SILENT_RULES], [], [AC_SUBST([AM_DEFAULT_VERBOSITY], [1])])

: ${CXXFLAGS="-O3 -Wall -pedantic"}

#
# Programs
#

LT_INIT
AC_PROG_CC
AC_PROG_AWK
AC_PROG_SED
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_SUBST(SO_EXT, $shrext_cmds)

#
# Features
#

AC_CHECK_FUNCS([socket])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([arpa/inet.h])

AC_CHECK_LIB([m], [cos],
             [AC_SUBST([HAVE_MATH], [1])
              LIBS="$LIBS -lm"],
             [AC_MSG_ERROR([no])])

AC_CHECK_LIB([gdbm], [gdbm_open],
             [AC_SUBST([HAVE_GDBM], [1])
              LIBS="$LIBS -lgdbm"],
             [AC_MSG_ERROR([no])])

AC_CHECK_LIB([ncurses], [initscr],
             [AC_SUBST([HAVE_NCURSES], [1])
              LIBS="$LIBS -lncurses"],
             [AC_MSG_ERROR([no])])

#
# Station Options
#

AC_SUBST([HAVE_DP], [1])

AC_ARG_ENABLE([adp],
              [AS_HELP_STRING([--enable-adp],
                              [enable support for ADP at LWA-SV (default=no)])],
              [enable_adp=yes],
              [enable_adp=no])
AC_SUBST([HAVE_ADP], [0])
AS_IF([test x$enable_adp != xno],
      [AC_SUBST([HAVE_ADP], [1])
       AC_SUBST([HAVE_DP], [0])])

AC_ARG_ENABLE([ndp],
              [AS_HELP_STRING([--enable-ndp],
                              [enable support for NDP at LWA-NA (default=no)])],
              [enable_ndp=yes],
              [enable_ndp=no])
AC_SUBST([HAVE_NDP], [0])
AS_IF([test x$enable_ndp != xno],
      [AC_SUBST([HAVE_NDP], [1])
       AC_SUBST([HAVE_DP], [0])])

AC_ARG_ENABLE([full-ndp],
              [AS_HELP_STRING([--enable-ndp-as-full],
                              [enable support for a full station with NDP at LWA-NA (default=no)])],
              [enable_full_ndp=yes],
              [enable_full_ndp=no])
AC_SUBST([HAVE_FULL_NDP], [0])
AS_IF([test x$enable_full_ndp != xno],
      [AC_SUBST([HAVE_FULL_NDP], [1])])

AC_ARG_WITH([repoint-check],
            [AS_HELP_STRING([--with-repoint-check=N],
                            [default interval in seconds for repointing check (default=1)])],
            [],
            [with_repoint_check=1])
AC_SUBST([BEAM_REPOINT_INTERVAL], [$with_repoint_check])

AC_ARG_WITH([repoint-res],
            [AS_HELP_STRING([--with-beam-res=N],
                            [default step size in degrees for repointing check (default=0.2)])],
            [],
            [with_repoint_res=0.2])
AC_SUBST([BEAM_REPOINT_RES], [$with_repoint_res])

AC_ARG_WITH([sch-ip],
            [AS_HELP_STRING([--with-sch-ip=N.N.N.N],
                            [default scheduler IP address (default=172.16.1.103)])],
            [],
            [with_sch_ip=172.16.1.103])
AC_SUBST([MSE_IP], [$with_sch_ip])

AC_ARG_WITH([sch-username],
            [AS_HELP_STRING([--with-sch-username=A],
                            [default scheduler username (default=op1)])],
            [],
            [with_sch_username=op1])
AC_SUBST([MSE_USERNAME], [$with_sch_username])

AC_ARG_WITH([exec-ip],
            [AS_HELP_STRING([--with-exec-ip=N.N.N.N],
                            [default executive IP address (default=172.16.1.103)])],
            [],
            [with_exec_ip=172.16.1.103])
AC_SUBST([MEE_IP], [$with_exec_ip])

AC_ARG_WITH([tp-ip],
            [AS_HELP_STRING([--with-tp-ip=N.N.N.N],
                            [default task processor IP address (default=172.16.1.103)])],
            [],
            [with_tp_ip=172.16.1.103])
AC_SUBST([MTE_IP], [$with_tp_ip])

AC_ARG_WITH([tp-username],
            [AS_HELP_STRING([--with-tp-username=A],
                            [default task processor username (default=op1)])],
            [],
            [with_tp_username=op1])
AC_SUBST([MTE_USERNAME], [$with_tp_username])

#
# Python
#

AC_ARG_ENABLE([python],
              [AS_HELP_STRING([--disable-python],
                              [disable building the Python bindings (default=no)])],
              [enable_python=no],
              [enable_python=yes])
AC_SUBST([HAVE_PYTHON], [0])
AS_IF([test x$enable_python != xno],
      [AX_WITH_PROG(PYTHON, python3, no, $PATH)
       AS_IF([test x${PYTHON} != xno],
             [AC_MSG_CHECKING([whether $PYTHON has pip])
              AS_IF([! ${PYTHON} -m pip > /dev/null],
                    [AC_MSG_RESULT([no])
                     AC_MSG_WARN([python module will not be built])],
                    [AC_MSG_RESULT([yes])
                     AC_SUBST(HAVE_PYTHON, 1)])])])
AC_ARG_WITH([pybuild_flags],
            [AS_HELP_STRING([--with-pybuild-flags],
                            [build flags for python (default='')])],
            [],
            [])
AC_SUBST(PYBUILDFLAGS, $with_pybuild_flags)

AC_ARG_WITH([pyinstall_flags],
            [AS_HELP_STRING([--with-pyinstall-flags],
                            [install flags for python (default='')])],
            [],
            [])
AC_SUBST(PYINSTALLFLAGS, $with_pyinstall_flags)

#
# Version splitting
#

AC_SUBST([PACKAGE_VERSION_MAJOR], [`echo $PACKAGE_VERSION | $AWK -F. '{print $1}
'`])
AC_SUBST([PACKAGE_VERSION_MINOR], [`echo $PACKAGE_VERSION | $AWK -F. '{print $2}
'`])
AC_SUBST([PACKAGE_VERSION_MICRO], [`echo $PACKAGE_VERSION | $AWK -F. '{print $3}
'`])

#
# Generate files
#

AC_CONFIG_FILES([common/config.h Makefile sch/Makefile exec/Makefile tp/Makefile utils/Makefile python/Makefile share/lwa_mcs.pc])

AC_OUTPUT

#
# User notes
#

echo ""

AC_MSG_NOTICE(install path: $prefix)

AC_MSG_NOTICE(network setup:)
AC_MSG_NOTICE([  scheduler - $MSE_IP])
AC_MSG_NOTICE([  executive - $MEE_IP])
AC_MSG_NOTICE([  task processor - $MTE_IP])

AS_IF([test x$HAVE_NDP = x1],
      [AC_MSG_NOTICE(digital backend: NDP)],
      [AS_IF([test x$HAVE_ADP = x1],
             [AC_MSG_NOTICE(digital backend: ADP)],
             [AC_MSG_NOTICE(digital backend: DP)])])

AC_MSG_NOTICE(beam repointing control:)
AC_MSG_NOTICE([  check interval - $BEAM_REPOINT_INTERVAL s])
AC_MSG_NOTICE([  step size - $BEAM_REPOINT_RES deg])
      
AS_IF([test x$HAVE_PYTHON = x1],
      [AC_MSG_NOTICE(python bindings: yes)],
      [AC_MSG_NOTICE(python bindings: no)])

echo ""
echo "lwa_mcs is now ready to be compiled.  Please run 'make'"
echo ""
