#include <inc/x86.h>
#include <inc/elf.h>

/**********************************************************************
 * This a dirt simple boot loader, whose sole job is to boot
 * an ELF kernel image from the first IDE hard disk.
 *
 * DISK LAYOUT
 *  * This program(boot.S and main.c) is the bootloader.  It should
 *    be stored in the first sector of the disk.
 *
 *  * The 2nd sector onward holds the kernel image.
 *
 *  * The kernel image must be in ELF format.
 *
 * BOOT UP STEPS
 *  * when the CPU boots it loads the BIOS into memory and executes it
 *
 *  * the BIOS intializes devices, sets of the interrupt routines, and
 *    reads the first sector of the boot device(e.g., hard-drive)
 *    into memory and jumps to it.
 *
 *  * Assuming this boot loader is stored in the first sector of the
 *    hard-drive, this code takes over...
 *
 *  * control starts in boot.S -- which sets up protected mode,
 *    and a stack so C code then run, then calls bootmain()
 *
 *  * bootmain() in this file takes over, reads in the kernel and jumps to it.
 **********************************************************************/

#define SECTSIZE	512
// 一个扇区的大小
#define ELFHDR		((struct Elf *) 0x10000) // scratch space
// 定义了一个 Elf头部 类型的指针, 这个指针的地址为 0x10000, 

void readsect(void*, uint32_t);
void readseg(uint32_t, uint32_t, uint32_t);

void
bootmain(void)
{
	struct Proghdr *ph, *eph;
	int i;

	// read 1st page off disk, 一页的大小是 4KB
	// 从内核的开头中读取一页到 0x10000 地址处
	readseg((uint32_t) ELFHDR, SECTSIZE*8, 0);
	// 这一页就是内核文件的 ELF头部, 描述了内核文件的结构, 因为内核是可执行文件

	// is this a valid ELF?
	// ELF 头部的魔数不正确
	if (ELFHDR->e_magic != ELF_MAGIC)
		goto bad;

	// load each program segment (ignores ph flags)
	ph = (struct Proghdr *) ((uint8_t *) ELFHDR + ELFHDR->e_phoff);
	// ph 是程序头部表(segment 描述表, 包含多个 Segment 头部信息)的位置
	eph = ph + ELFHDR->e_phnum;
	// ELFHDR->e_phnum 表示程序头部表表项的个数, 即程序 segment 的个数 
	// 所以 eph 就是程序头部表的结尾
	for (; ph < eph; ph++) {
		// 所以每次表头往后移动一位, 表示下一个程序段
		// p_pa is the load address of this segment (as well as the physical address)
		readseg(ph->p_pa, ph->p_memsz, ph->p_offset);
		// 从外存中将这一数据段读进内存地址, 这个地址就是 p_pa, 
		for (i = 0; i < ph->p_memsz - ph->p_filesz; i++) {
			*((char *) ph->p_pa + ph->p_filesz + i) = 0;
		}
	}
	// call the entry point from the ELF header
	// note: does not return!
	((void (*)(void)) (ELFHDR->e_entry))();

bad:
	outw(0x8A00, 0x8A00);
	outw(0x8A00, 0x8E00);
	while (1)
		/* do nothing */;
}

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked
void
readseg(uint32_t pa, uint32_t count, uint32_t offset)
{
	uint32_t end_pa;

	end_pa = pa + count;

	// round down to sector boundary
	pa &= ~(SECTSIZE - 1);
	// 这个向下舍入就是将低 9 位全部变成0, 这样才能使地址扇区对齐
	// translate from bytes to sectors, and kernel starts at sector 1
	offset = (offset / SECTSIZE) + 1;
	// 这一步就是将页大小变成扇区大小, 一页是 4 个扇区
	// 因为从硬盘每次读的是一个扇区的单位, 内核起始是第一个扇区, 而不是第 0 个
	// If this is too slow, we could read lots of sectors at a time.
	// We'd write more to memory than asked, but it doesn't matter --
	// we load in increasing order.
	while (pa < end_pa) {
		// Since we haven't enabled paging yet and we're using
		// an identity segment mapping (see boot.S), we can
		// use physical addresses directly.  This won't be the
		// case once JOS enables the MMU.
		readsect((uint8_t*) pa, offset);
		// 从硬盘读一个扇区
		pa += SECTSIZE;
		// 物理地址到一个扇区, 扇区位置加 1
		offset++;
	}
}

void waitdisk(void)
{
	// wait for disk reaady
	while ((inb(0x1F7) & 0xC0) != 0x40)
		/* do nothing */;
}

void
readsect(void *dst, uint32_t offset)
{
	// wait for disk to be ready
	waitdisk();
	// 把数据写入端口
	outb(0x1F2, 1);		// count = 1
	// offset 太长了, 所以分段
	// 注意,  0x1F 是端口, 后面是数据
	outb(0x1F3, offset);
	outb(0x1F4, offset >> 8);
	outb(0x1F5, offset >> 16);
	outb(0x1F6, (offset >> 24) | 0xE0);
	outb(0x1F7, 0x20);	// cmd 0x20 - read sectors
	// 注意secno是通过不同的端口，分四次发给磁盘的。
	// 然后向0x1f7端口发出0x20，说明我要开始读扇区了。

	// wait for disk to be ready
	waitdisk();
	// 这里的实现方式和 boot.S 中的对端口的操作类似
	// waitdisk（）函数就是向磁盘0x1f7端口读状态字，如果没有处于ready状态就一直等待：
	// read a sector
	insl(0x1F0, dst, SECTSIZE/4);
	// 注意 insl 函数中的 repne 就是重复读, 因为 inl 每次读 4 字节, 所以要读 SECTSIZE/4 次
}

