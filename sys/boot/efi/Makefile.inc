# $FreeBSD$

BINDIR?=	/boot

.if ${MACHINE_CPUARCH} == "i386"
CFLAGS+=        -march=i386
.endif

# Options used when building app-specific efi components
CFLAGS+=	-ffreestanding -Wformat
LDFLAGS+=	-nostdlib

.include "../Makefile.inc"
