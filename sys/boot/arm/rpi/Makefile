# $FreeBSD$

.include <src.opts.mk>

PROG=		loader.sym
INTERNALPROG=

NEWVERSWHAT=	"Raspberry Pi loader" ${MACHINE_ARCH}
BINDIR?=	/boot
INSTALLFLAGS=	-b
WARNS?=		1

# Architecture-specific loader code
SRCS=	start.S \
	main.c \
	reloc.c \
	conf.c \
	vers.c \
	pl011_console.c

# Always add MI sources
.PATH:		${.CURDIR}/../../common
#.include	"${.CURDIR}/../../common/Makefile.inc"
SRCS+=		console.c \
		panic.c
CFLAGS+=	-I${.CURDIR}/../../common
CFLAGS+=	-I.
CFLAGS+=	-I${.CURDIR}/../../../contrib/libfdt

FILES=		loader.bin
FILESMODE_loader.bin= ${BINMODE}
CLEANFILES+=	vers.c loader.sym

CFLAGS+=	-ffreestanding -msoft-float

LDFLAGS=	-nostdlib
LDFLAGS+=	-Wl,-Bsymbolic -shared
#LDFLAGS+=	-static
LDFLAGS+=	-Wl,-T ${.CURDIR}/ldscript.${MACHINE_CPUARCH}

# where to get libstand from
CFLAGS+=	-I${.CURDIR}/../../../../lib/libstand/

# clang doesn't understand %D as a specifier to printf
NO_WERROR.clang=

#DPADD=		${LIBSTAND}
DDADD=		${.OBJDIR}/../../../../lib/libstand/libstand.a
#LDADD=		-lstand
LDADD=		${.OBJDIR}/../../../../lib/libstand/libstand.a

vers.c:	${.CURDIR}/../../common/newvers.sh ${.CURDIR}/version
	sh ${.CURDIR}/../../common/newvers.sh ${.CURDIR}/version ${NEWVERSWHAT}

loader.sym: ${.CURDIR}/ldscript.${MACHINE_CPUARCH}

loader.bin: loader.sym
	${OBJCOPY} -S -O binary loader.sym ${.TARGET}

MAN=

.include <bsd.prog.mk>
