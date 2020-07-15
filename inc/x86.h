#ifndef JOS_INC_X86_H
#define JOS_INC_X86_H

#include <inc/types.h>

static inline void
breakpoint(void)
{
	asm volatile("int3");
}

static inline uint8_t
inb(int port)
{
	// port 是端口
	// inb 从I/O端口读取一个字节
	uint8_t data;
	asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
	//  : "=a" (out_var) 格式是用于指明输出操作数 "=", 表示only_write
	// "d" (port) 表示指明输入操作数是 port, 其中的 'a' 'd' 等参数是用于指明中间寄存器
}

static inline void
insb(int port, void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\tinsb"
		     : "=D" (addr), "=c" (cnt)
		     : "d" (port), "0" (addr), "1" (cnt)
		     : "memory", "cc");
	// 这里是插入内联代码的一些语法, 具体可以参考: https://blog.csdn.net/slvher/article/details/8864996
    // 根据使用的约束条件, 指定了addr 放到 %edi， cnt 放到 %ecx，port放到 %edx ,这些就是 "=D", 这些约束参数指定的
    // cld指令是和repne指令配合使用的，repne指令是连续执行下一条指令，直到%ecx为0.cld指令初始化%ecx减小的步长为1
    // 该函数是将端口的数据读入到内存中
}

static inline uint16_t
inw(int port)
{
	uint16_t data;
	asm volatile("inw %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void
insw(int port, void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\tinsw"
		     : "=D" (addr), "=c" (cnt)
		     : "d" (port), "0" (addr), "1" (cnt)
		     : "memory", "cc");
}

static inline uint32_t
inl(int port)
{
	uint32_t data;
	asm volatile("inl %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void
insl(int port, void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\tinsl"
		     : "=D" (addr), "=c" (cnt)
		     : "d" (port), "0" (addr), "1" (cnt)
		     : "memory", "cc");
}

static inline void
outb(int port, uint8_t data)
{
	asm volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

static inline void
outsb(int port, const void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\toutsb"
		     : "=S" (addr), "=c" (cnt)
		     : "d" (port), "0" (addr), "1" (cnt)
		     : "cc");
}

static inline void
outw(int port, uint16_t data)
{
	asm volatile("outw %0,%w1" : : "a" (data), "d" (port));
}

static inline void
outsw(int port, const void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\toutsw"
		     : "=S" (addr), "=c" (cnt)
		     : "d" (port), "0" (addr), "1" (cnt)
		     : "cc");
}

static inline void
outsl(int port, const void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\toutsl"
		     : "=S" (addr), "=c" (cnt)
		     : "d" (port), "0" (addr), "1" (cnt)
		     : "cc");
}

static inline void
outl(int port, uint32_t data)
{
	asm volatile("outl %0,%w1" : : "a" (data), "d" (port));
}

static inline void
invlpg(void *addr)
{
	asm volatile("invlpg (%0)" : : "r" (addr) : "memory");
	// 这段嵌入式汇编是没有输出的.把addr读入到寄存器中去(这里r指定任意常用寄存器)，%0则指向addr所在的寄存器
}

static inline void
lidt(void *p)
{
	asm volatile("lidt (%0)" : : "r" (p));
	// Load interrupt descriptor table
}

static inline void
lgdt(void *p)
{
	asm volatile("lgdt (%0)" : : "r" (p));
	// Load global descriptor table
}

static inline void
lldt(uint16_t sel)
{
	// load local descriptor table LDT) register
	asm volatile("lldt %0" : : "r" (sel));
}

static inline void
ltr(uint16_t sel)
{
	// load task register
	asm volatile("ltr %0" : : "r" (sel));
}

static inline void
lcr0(uint32_t val)
{
	asm volatile("movl %0,%%cr0" : : "r" (val));
}

static inline uint32_t
rcr0(void)
{
	uint32_t val;
	asm volatile("movl %%cr0,%0" : "=r" (val));
	return val;
}

static inline uint32_t
rcr2(void)
{
	uint32_t val;
	asm volatile("movl %%cr2,%0" : "=r" (val));
	return val;
}

static inline void
lcr3(uint32_t val)
{
	asm volatile("movl %0,%%cr3" : : "r" (val));
}

static inline uint32_t
rcr3(void)
{
	uint32_t val;
	asm volatile("movl %%cr3,%0" : "=r" (val));
	return val;
}

static inline void
lcr4(uint32_t val)
{
	asm volatile("movl %0,%%cr4" : : "r" (val));
}

static inline uint32_t
rcr4(void)
{
	uint32_t cr4;
	asm volatile("movl %%cr4,%0" : "=r" (cr4));
	return cr4;
}

static inline void
tlbflush(void)
{
	uint32_t cr3;
	asm volatile("movl %%cr3,%0" : "=r" (cr3));
	asm volatile("movl %0,%%cr3" : : "r" (cr3));
}

static inline uint32_t
read_eflags(void)
{
	uint32_t eflags;
	asm volatile("pushfl; popl %0" : "=r" (eflags));
	return eflags;
}

static inline void
write_eflags(uint32_t eflags)
{
	asm volatile("pushl %0; popfl" : : "r" (eflags));
}

static inline uint32_t
read_ebp(void)
{
	uint32_t ebp;
	asm volatile("movl %%ebp,%0" : "=r" (ebp));
	return ebp;
}

static inline uint32_t
read_esp(void)
{
	uint32_t esp;
	asm volatile("movl %%esp,%0" : "=r" (esp));
	return esp;
}

static inline void
cpuid(uint32_t info, uint32_t *eaxp, uint32_t *ebxp, uint32_t *ecxp, uint32_t *edxp)
{
	uint32_t eax, ebx, ecx, edx;
	asm volatile("cpuid"
		     : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
		     : "a" (info));
	if (eaxp)
		*eaxp = eax;
	if (ebxp)
		*ebxp = ebx;
	if (ecxp)
		*ecxp = ecx;
	if (edxp)
		*edxp = edx;
}

static inline uint64_t
read_tsc(void)
{
	uint64_t tsc;
	asm volatile("rdtsc" : "=A" (tsc));
	return tsc;
}

static inline uint32_t
xchg(volatile uint32_t *addr, uint32_t newval)
{
	uint32_t result;

	// The + in "+m" denotes a read-modify-write operand.
	asm volatile("lock; xchgl %0, %1"
		     : "+m" (*addr), "=a" (result)
		     : "1" (newval)
		     : "cc");
	return result;
}

#endif /* !JOS_INC_X86_H */
