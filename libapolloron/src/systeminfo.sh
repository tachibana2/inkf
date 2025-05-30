#!/bin/sh

CXX=$1
BUILD_OS=`${CXX} -dumpmachine`
if [ "${BUILD_OS}" = "" ];
then
    BUILD_OS=`uname -sm`
fi
if [ "${BUILD_OS}" = "" ];
then
    BUILD_OS=`uname -s`
fi
BUILD_OS_DISTRO=""
if [ -f /etc/issue.net ];
then
    BUILD_OS_DISTRO=`cat /etc/issue.net | grep release`
fi
if [ "${BUILD_OS_DISTRO}" = "" ];
then
    BUILD_OS_DISTRO="generic"
fi

SYSTEMINFO_H="systeminfo.h"
echo '/* '${SYSTEMINFO_H}' : auto-generated */' > ${SYSTEMINFO_H}
echo '' >> ${SYSTEMINFO_H}
echo '#ifndef _SYSTEMINFO_H_' >> ${SYSTEMINFO_H}
echo '#define _SYSTEMINFO_H_' >> ${SYSTEMINFO_H}
echo '' >> ${SYSTEMINFO_H}
echo 'namespace apolloron {' >> ${SYSTEMINFO_H}
echo '' >> ${SYSTEMINFO_H}

echo '#define _BUILD_DATE      "'`date "+%Y-%m-%d %H:%M:%S %z (%Z)"`'"' >> ${SYSTEMINFO_H}
echo '#define _BUILD_HOST      "'`hostname`'"' >> ${SYSTEMINFO_H}
echo '#define _BUILD_OS        "'${BUILD_OS}'"' >> ${SYSTEMINFO_H}
echo '#define _BUILD_OS_DISTRO "'${BUILD_OS_DISTRO}'"' >> ${SYSTEMINFO_H}
echo '#define _BUILD_OS_KERNEL "'`uname -r`'"' >> ${SYSTEMINFO_H}

echo '' >> ${SYSTEMINFO_H}
echo '} // apolloron' >> ${SYSTEMINFO_H}
echo '' >> ${SYSTEMINFO_H}
echo '#endif' >> ${SYSTEMINFO_H}
