
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/linker.h>

#include <machine/elf.h>
#include <machine/metadata.h>

#include <stand.h>
#include <bootstrap.h>

#include <fdt.h>
#include <libfdt.h>

/*
 * Copy module-related data into the load area, where it can be
 * used as a directory for loaded modules.
 *
 * Module data is presented in a self-describing format.  Each datum
 * is preceded by a 32-bit identifier and a 32-bit size field.
 *
 * Currently, the following data are saved:
 *
 * MOD_NAME	(variable)		module name (string)
 * MOD_TYPE	(variable)		module type (string)
 * MOD_ARGS	(variable)		module parameters (string)
 * MOD_ADDR	sizeof(vm_offset_t)	module load address
 * MOD_SIZE	sizeof(size_t)		module size
 * MOD_METADATA	(variable)		type-specific metadata
 */
#define COPY32(v, a, c) {				\
	uint32_t *x = (uint32_t *)(a);			\
	if (c)						\
		*x = (v);				\
	a += sizeof(*x);				\
}

#define MOD_STR(t, a, s, c) {				\
	COPY32(t, a, c);				\
	COPY32(strlen(s) + 1, a, c);			\
	if (c)						\
		memcpy((void *)a, s, strlen(s) + 1);	\
	a += roundup(strlen(s) + 1, sizeof(u_long));	\
}

#define MOD_NAME(a, s, c)	MOD_STR(MODINFO_NAME, a, s, c)
#define MOD_TYPE(a, s, c)	MOD_STR(MODINFO_TYPE, a, s, c)
#define MOD_ARGS(a, s, c)	MOD_STR(MODINFO_ARGS, a, s, c)

#define MOD_VAR(t, a, s, c) {				\
	COPY32(t, a, c);				\
	COPY32(sizeof(s), a, c);			\
	if (c)						\
		memcpy((void *)a, &s, sizeof(s));	\
	a += roundup(sizeof(s), sizeof(u_long));	\
}

#define MOD_ADDR(a, s, c)	MOD_VAR(MODINFO_ADDR, a, s, c)
#define MOD_SIZE(a, s, c)	MOD_VAR(MODINFO_SIZE, a, s, c)

#define MOD_METADATA(a, t, d, c) {			\
	COPY32(MODINFO_METADATA | t, a, c);		\
	COPY32(sizeof(d), a, c);			\
	if (c)						\
		memcpy((void *)a, &(d), sizeof(d));	\
	a += roundup(sizeof(d), sizeof(u_long));	\
}

#define MOD_END(a, c) {					\
	COPY32(MODINFO_END, a, c);			\
	COPY32(0, a, c);				\
}

static vm_offset_t
bi_copymodules(vm_offset_t addr, struct preloaded_file *fp, int howto,
    void *envp, vm_offset_t dtbp, vm_offset_t kernend)
{
	int copy;

	copy = addr != 0;

	MOD_NAME(addr, "kernel", copy);
	MOD_TYPE(addr, "elf kernel", copy);
	MOD_ADDR(addr, fp->f_addr, copy);
	MOD_SIZE(addr, fp->f_size, copy);

	MOD_METADATA(addr, MODINFOMD_HOWTO, howto, copy);
	MOD_METADATA(addr, MODINFOMD_ENVP, envp, copy);
	MOD_METADATA(addr, MODINFOMD_DTBP, dtbp, copy);
	MOD_METADATA(addr, MODINFOMD_KERNEND, kernend, copy);

	MOD_END(addr, copy);

	return (addr);
}

static void
exec(void *base, struct fdt_header *dtb)
{
	struct preloaded_file kfp;
	Elf_Ehdr *e;
	void (*entry)(void *);
	vm_offset_t addr, size;
	vm_offset_t dtbp, kernend;
	void *envp;
	int howto;

	bzero(&kfp, sizeof(kfp));
	kfp.f_addr = (vm_offset_t)base;
	/* Guess */
	kfp.f_size = 6 * 1024 * 1024;

	howto = 0;
	envp = NULL;

	addr = kfp.f_addr + kfp.f_size;
	memcpy((void *)addr, dtb, fdt_totalsize(dtb));
	dtbp = addr - (unsigned int)base + 0xc0100000;
	addr += roundup2(fdt_totalsize(dtb), PAGE_SIZE);

	size = bi_copymodules(0, &kfp, 0, NULL, dtbp, 0);
	kernend = roundup(addr + size, PAGE_SIZE);
	kernend = kernend - (unsigned int)base + 0xc0100000;
	(void)bi_copymodules(addr, &kfp, 0, NULL, dtbp, kernend);

	e = base;
	entry = (void *)((unsigned int)e->e_entry - 0xc0100000 +
	    (unsigned int)base);
	addr = addr - (unsigned int)base + 0xc0100000;

	printf("entry: %p %x\n", entry, *(uint32_t *)entry);
	entry((void *)addr);
}

int
main(void *dtb, unsigned int end)
{
	volatile uint32_t *gpio = (volatile uint32_t *)0x3f200000;
	struct fdt_header *header;
	uint32_t tmp;

	tmp = gpio[1];
	tmp &= ~(0x7 << 15 | 0x7 << 12);
	tmp |= (0x4 << 15 | 0x4 << 12);
	gpio[1] = tmp;

	/*
	 * Initialise the heap as early as possible.  Once this is done,
	 * alloc() is usable. The stack is buried inside us, so this is safe.
	 */
	setheap((void *)end, (void *)(end + 512 * 1024));

	/*
	 * Set up console.
	 */
	cons_probe();

	header = dtb;
	exec((void *)0xa00000, header);

	return (1);
}

void _exit(void);
void
exit(int code)
{

	_exit();
}
