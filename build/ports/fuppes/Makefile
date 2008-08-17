PORTNAME=			fuppes
PORTVERSION=	0.7.2.3
CATEGORIES=		net
COMMENT=			Free UPnP Entertainment Service
MAINTAINER=		votdev@gmx.de

MASTER_SITES=				${MASTER_SITE_SOURCEFORGE}
MASTER_SITE_SUBDIR=	${PORTNAME}
DISTNAME=						${PORTNAME}-SVN-578

LIB_DEPENDS+=			sqlite3:${PORTSDIR}/databases/sqlite3 \
									tag:${PORTSDIR}/audio/taglib \
									pcre.0:${PORTSDIR}/devel/pcre \
									uuid.1:${PORTSDIR}/misc/e2fsprogs-libuuid \
									mp3lame.0:${PORTSDIR}/audio/lame \
									twolame.0:${PORTSDIR}/audio/twolame \
									vorbis.4:${PORTSDIR}/audio/libvorbis

USE_AUTOTOOLS=		autoconf:261
USE_GNOME=				libxml2
USE_ICONV=				yes
GNU_CONFIGURE=		yes
CONFIGURE_TARGET=	--build="${MACHINE_ARCH}-portbld-freebsd${OSREL}"
CONFIGURE_ARGS+=	--disable-libnotify \
									--disable-imagemagick \
									--enable-video-transcoding
CONFIGURE_ENV=		CPPFLAGS="-I${LOCALBASE}/include -I${LOCALBASE}/include/ffmpeg ${PTHREAD_CFLAGS}" \
									LIBS="-L${LOCALBASE}/lib ${PTHREAD_LIBS}"

post-patch:
	@${REINPLACE_CMD} -e 's|-ldl ||' ${WRKSRC}/configure.in

	# Copy options files
	@${MKDIR} -v /var/db/ports/sqlite3
	@${CP} -pv ${FILESDIR}/sqlite3-options.in /var/db/ports/sqlite3/options
	@${MKDIR} -v /var/db/ports/libsndfile
	@${CP} -pv ${FILESDIR}/libsndfile-options.in /var/db/ports/libsndfile/options
	@${MKDIR} -v /var/db/ports/ffmpeg
	@${CP} -pv ${FILESDIR}/ffmpeg-options.in /var/db/ports/ffmpeg/options

do-install:
	@${INSTALL_PROGRAM} -v ${WRKSRC}/src/.libs/${PORTNAME}d ${FREENAS_ROOTFS}/usr/local/bin
	@${INSTALL_PROGRAM} -v ${WRKSRC}/src/.libs/libfuppes.so.* ${FREENAS_ROOTFS}/usr/local/lib
	@${INSTALL_SCRIPT} -v ${FILESDIR}/${PORTNAME}.in ${FREENAS_ROOTFS}/etc/rc.d/${PORTNAME}
	@${MKDIR} -v ${FREENAS_ROOTFS}/usr/local/share/${PORTNAME}
	@${INSTALL_DATA} -v ${FILESDIR}/vfolder.cfg ${FREENAS_ROOTFS}/usr/local/share/${PORTNAME}

	# Copy additional libraries that are linked dynamically. These can not be
	# covered by ldd when building userland via make.sh.
	@${CP} -Rpv /usr/local/lib/libFLAC.so ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libFLAC.so.* ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libFLAC++.so ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libFLAC++.so.* ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libmp3lame.so ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libmp3lame.so.* ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libogg.so ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libogg.so.5 ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libsndfile.so ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libsndfile.so.* ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libtwolame.so ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libtwolame.so.* ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libvorbis.so ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libvorbis.so.* ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libvorbisfile.so ${FREENAS_ROOTFS}/usr/local/lib
	@${CP} -Rpv /usr/local/lib/libvorbisfile.so.* ${FREENAS_ROOTFS}/usr/local/lib

.include <bsd.port.mk>