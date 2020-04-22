#ifndef JOS_INC_ELF_H
#define JOS_INC_ELF_H

#define ELF_MAGIC 0x464C457FU	/* "\x7FELF" in little endian */

struct Elf {
	uint32_t e_magic;	// must equal ELF_MAGIC
	/*
	在最前面的一项magic中的16个字节用来规定ELF文件的平台，最开始的4个字节是所有ELF文件都相同的标示码，
	分别为7f、45、4c、46。它们分别对应ascii码表中的delete、E、L、F字符，如图参考表10。
	接下来的一个字节标示ELF文件类型，01表示32位的，02表示64位的，从图8中可以知道test.o为64位ELF文件类型。
	之后的一个字节表示是大端(big-endian)还是小端(little-endian)，第7字节表示ELF文件主版本号，再后面几个字节无实际意义，默认填0。
	*/
	uint8_t e_elf[12];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;		// 程序的入口
	uint32_t e_phoff;   	// program header table起始位置相对于ELF 头部表的偏移
	uint32_t e_shoff;		// section header起始位置, 段表头部在文件中的偏移位置
	uint32_t e_flags;		// Elf标志位
	uint16_t e_ehsize;		// ELF文件头本身大小	
	uint16_t e_phentsize;
	uint16_t e_phnum;		// 对于可执行文件(Program)而言, 段的个数
	uint16_t e_shentsize;	// 段表描述符的大小，即为
	uint16_t e_shnum;		// 段表描述符的数量，值为12表示test目标文件中有12个段
	uint16_t e_shstrndx;	// 段表字符串表所在段表数组的下标，值为9表示段表字符串表位于第9个段
};

// 对于可执行文件而言, 注意与下面的 Secthdr 的区别, 描述的都是段,
struct Proghdr {			// 描述一个段的结构
	uint32_t p_type;
	uint32_t p_offset;		// 段相对于ELF文件开头的偏移
	uint32_t p_va;			// 虚拟地址
	uint32_t p_pa;			// 载入内存中的物理地址
	uint32_t p_filesz;		// 这个段实际的大小
	uint32_t p_memsz;		// 这一段内存中最大的大小
	uint32_t p_flags;		// 读，写，执行权限
	uint32_t p_align;		// 对齐方式
};


// 对于链接文件而言
struct Secthdr {
	uint32_t sh_name;		// 段的名字
	uint32_t sh_type;		// 段的类型
	uint32_t sh_flags;		// 段表的标志
	uint32_t sh_addr;		// 可执行文件中段表的虚拟地址
	uint32_t sh_offset;		// sh_offset表示段在文件中的偏移位置
	uint32_t sh_size;		// 一段的大小, 以字节为单位
	uint32_t sh_link;		// Link to another sector
	uint32_t sh_info;		// Additional section information
	uint32_t sh_addralign;	// 段对齐方式
	uint32_t sh_entsize;	// 
};

// Values for Proghdr::p_type
#define ELF_PROG_LOAD		1

// Flag bits for Proghdr::p_flags
#define ELF_PROG_FLAG_EXEC	1
#define ELF_PROG_FLAG_WRITE	2
#define ELF_PROG_FLAG_READ	4

// Values for Secthdr::sh_type
#define ELF_SHT_NULL		0
#define ELF_SHT_PROGBITS	1
#define ELF_SHT_SYMTAB		2
#define ELF_SHT_STRTAB		3

// Values for Secthdr::sh_name
#define ELF_SHN_UNDEF		0

#endif /* !JOS_INC_ELF_H */
