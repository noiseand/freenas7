PORTNAME=			bsnmp-ucd
PORTVERSION=	0.3.3
CATEGORIES=		net-mgmt
COMMENT=			A bsnmpd module that implements parts of UCD-SNMP-MIB
MAINTAINER=		michael.zoon@freenas.org

MASTER_SITES=	${MASTER_SITE_GOOGLE_CODE}

USE_MAKE=				yes
USE_AUTOTOOLS=	libtool:env

post-patch:
	@${REINPLACE_CMD} -e '32,32 s/^.*/#include <sys\/queue.h>/' ${WRKSRC}/snmp_ucd.h
	@${REINPLACE_CMD} -e "s/LOCALBASE/PREFIX/g" ${WRKSRC}/Makefile

do-install:
	@${INSTALL_PROGRAM} -v ${WRKSRC}/.libs/snmp_ucd.so.* ${FREENAS_ROOTFS}/usr/lib
	${LN} -sfv /usr/lib/snmp_ucd.so.1 ${FREENAS_ROOTFS}/usr/lib/snmp_ucd.so

	@${INSTALL_DATA} ${WRKSRC}/UCD-SNMP-MIB.txt ${FREENAS_ROOTFS}/usr/share/snmp/mibs

.include <bsd.port.mk>
