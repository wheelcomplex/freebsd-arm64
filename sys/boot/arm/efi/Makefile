# $FreeBSD$

MAN=

.include <src.opts.mk>

MK_SSP=		no

PROG=		loader.sym
INTERNALPROG=

.PATH: ${.CURDIR}/../../efi/loader
# architecture-specific loader code
SRCS=	start.S \
	autoload.c \
	bootinfo.c \
	conf.c \
	copy.c \
	devicename.c \
	exec.c \
	main.c \
	reloc.c \
	vers.c

#	elf64_freebsd.c \
	framebuffer.c \
	reloc.c \

CFLAGS+=	-ffreestanding -fno-builtin -msoft-float
#CFLAGS+=	-fPIC
CFLAGS+=	-I${.CURDIR}/../../efi/loader
CFLAGS+=	-I${.CURDIR}/../../efi/include
CFLAGS+=	-I${.CURDIR}/../../efi/include/${MACHINE_CPUARCH}
CFLAGS+=	-I${.CURDIR}/../../../contrib/dev/acpica/include
CFLAGS+=	-I${.CURDIR}/../../..

.if ${MK_FORTH} != "no"
BOOT_FORTH=	yes
CFLAGS+=	-DBOOT_FORTH
CFLAGS+=	-I${.CURDIR}/../../ficl
CFLAGS+=	-I${.CURDIR}/../../ficl/${MACHINE_CPUARCH}
LIBFICL=	${.OBJDIR}/../../ficl/libficl.a
.endif
.if ${MK_FDT} != "no"
CFLAGS+=	-I${.CURDIR}/../../fdt
CFLAGS+=	-I${.OBJDIR}/../../fdt
CFLAGS+=	-DLOADER_FDT_SUPPORT
LIBEFI_FDT=	${.OBJDIR}/../../efi/fdt/libefi_fdt.a
LIBFDT=		${.OBJDIR}/../../fdt/libfdt.a
.endif

# Always add MI sources 
.PATH:		${.CURDIR}/../../common
.include	"${.CURDIR}/../../common/Makefile.inc"
CFLAGS+=	-I${.CURDIR}/../../common

FILES=	loader.efi
FILESMODE_loader.efi=	${BINMODE}

LDSCRIPT=	${.CURDIR}/ldscript.${MACHINE_CPUARCH}
LDFLAGS=	-Wl,-T${LDSCRIPT}
LDFLAGS+=	-Wl,-Bsymbolic
LDFLAGS+=	-shared
#LDFLAGS+=	-static
LDFLAGS+=	-nostdlib

${PROG}:	${LDSCRIPT}

CLEANFILES=	vers.c loader.efi

NEWVERSWHAT=	"EFI loader" ${MACHINE_CPUARCH}

vers.c:	${.CURDIR}/../../common/newvers.sh ${.CURDIR}/../../efi/loader/version
	sh ${.CURDIR}/../../common/newvers.sh ${.CURDIR}/version ${NEWVERSWHAT}

OBJCOPY=	objcopy
OBJDUMP?=	objdump

loader.efi: loader.sym
	if [ `${OBJDUMP} -t ${.ALLSRC} | fgrep '*UND*' | wc -l` != 0 ]; then \
		${OBJDUMP} -t ${.ALLSRC} | fgrep '*UND*'; \
		exit 1; \
	fi
	${OBJCOPY} -j .data -j .dynamic -j .dynstr -j .dynsym -j .hash \
	    -j .rel.dyn -j .reloc -j .sdata -j .text -j set_Xcommand_set \
	    -j .sbss -j .bss -O binary ${.ALLSRC} ${.TARGET}

LIBEFI=		${.OBJDIR}/../../efi/libefi/libefi.a
LIBSTAND=       ${.OBJDIR}/../../../../lib/libstand/libstand.a

DPADD=		${LIBFICL} ${LIBEFI} ${LIBFDT} ${LIBEFI_FDT} ${LIBSTAND}
LDADD=		${LIBFICL} ${LIBEFI} ${LIBFDT} ${LIBEFI_FDT} ${LIBSTAND}

.include <bsd.prog.mk>

