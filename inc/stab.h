#ifndef JOS_STAB_H
#define JOS_STAB_H
#include <inc/types.h>

// <inc/stab.h>
// STABS debugging info

// The JOS kernel debugger can understand some debugging information
// in the STABS format.  For more information on this format, see
// http://sourceware.org/gdb/onlinedocs/stabs.html

// The constants below define some symbol types used by various debuggers
// and compilers.  JOS uses the N_SO, N_SOL, N_FUN, and N_SLINE types.

#define	N_GSYM		0x20	// global symbol
#define	N_FNAME		0x22	// F77 function name
#define	N_FUN		0x24	// procedure name
#define	N_STSYM		0x26	// data segment variable
#define	N_LCSYM		0x28	// bss segment variable
#define	N_MAIN		0x2a	// main function name
#define	N_PC		0x30	// global Pascal symbol
#define	N_RSYM		0x40	// register variable
#define	N_SLINE		0x44	// text segment line number
#define	N_DSLINE	0x46	// data segment line number
#define	N_BSLINE	0x48	// bss segment line number
#define	N_SSYM		0x60	// structure/union element
#define	N_SO		0x64	// main source file name
#define	N_LSYM		0x80	// stack variable
#define	N_BINCL		0x82	// include file beginning
#define	N_SOL		0x84	// included source file name
#define	N_PSYM		0xa0	// parameter variable
#define	N_EINCL		0xa2	// include file end
#define	N_ENTRY		0xa4	// alternate entry point
#define	N_LBRAC		0xc0	// left bracket
#define	N_EXCL		0xc2	// deleted include file
#define	N_RBRAC		0xe0	// right bracket
#define	N_BCOMM		0xe2	// begin common
#define	N_ECOMM		0xe4	// end common
#define	N_ECOML		0xe8	// end common (local name)
#define	N_LENG		0xfe	// length of preceding entry

// Entries in the STABS table are formatted as follows.
struct Stab {
	uint32_t n_strx;	// index into string table of name
	uint8_t n_type;         // type of symbol
	uint8_t n_other;        // misc info (usually empty)
	uint16_t n_desc;        // description field
	uintptr_t n_value;	// value of symbol
};
/*
Symnum是符号索引，换句话说，整个符号表看作一个数组，Symnum是当前符号在数组中的下标
n_type是符号类型，FUN指函数名，SLINE指在text段中的行号
n_othr目前没被使用，其值固定为0
n_desc表示在文件中的行号
n_value表示地址。特别要注意的是，这里只有FUN类型的符号的地址是绝对地址，SLINE符号的地址是偏移量，
其实际地址为函数入口地址加上偏移量。比如第3行的含义是地址f01000b8(=0xf01000a6+0x00000012)对应文件第34行。
*/
#endif /* !JOS_STAB_H */
