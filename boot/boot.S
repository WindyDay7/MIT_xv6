#include <inc/mmu.h>

# Start the CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

.set PROT_MODE_CSEG, 0x8         # kernel code segment selector
.set PROT_MODE_DSEG, 0x10        # kernel data segment selector
.set CR0_PE_ON,      0x1         # protected mode enable flag

.globl start
start:
  .code16                     # Assemble for 16-bit mode
  cli                         # Disable interrupts
  cld                         # String operations increment

  # Set up the important data segment registers (DS, ES, SS).
  # 初始化一些寄存器
  xorw    %ax,%ax             # Segment number zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment

  # Enable A20:
  #   For backwards compatibility with the earliest PCs, physical
  #   address line 20 is tied low, so that addresses higher than
  #   1MB wrap around to zero by default.  This code undoes this.

  #   in PortAddress, %al     把端口地址为PortAddress的端口中的值读入寄存器al中
  #   out %al, PortAddress    向端口地址为PortAddress的端口写入值，值为al寄存器中的值
  #   标准规定端口操作必须要用al寄存器作为缓冲。
seta20.1:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al                # 判断数据是否写完
  jnz     seta20.1

  movb    $0xd1,%al               # 0xd1 -> port 0x64
  outb    %al,$0x64               
/*
当0x64端口准备好读入数据后，现在就可以写入数据了，
所以38, 39这两条指令是把0xd1这条数据写入到0x64端口中。当向0x64端口写入数据时，
则代表向键盘控制器804x发送指令。这个指令将会被送给0x60端口。
 */

seta20.2:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     seta20.2
/*
等待上面的 D1 指令被读取到
D1指令代表下一次写入0x60端口的数据将被写入给804x控制器的输出端口。可以理解为下一个写入0x60端口的数据是一个控制指令。
 */
  movb    $0xdf,%al               # 0xdf -> port 0x60
  outb    %al,$0x60
/*
如果指令被读取了，指令会向控制器输入新的指令0xdf。通过查询我们看到0xDF指令的含义如下
这个指令的含义可以从图中看到，使能A20线，代表可以进入保护模式了
 */

  # Switch from real to protected mode, using a bootstrap GDT
  # and segment translation that makes virtual addresses 
  # identical to their physical addresses, so that the 
  # effective memory map does not change during the switch.
  lgdt    gdtdesc
  # Load the Global/Interrupt Descriptor Table Register from memory address gdtdesc:
  # The GDT table contains a number of entries called Segment Descriptors.
  #  Each is 8 bytes long and contains information on the starting point of the segment, 
  #  the length of the segment, and the access rights of the segment.
  movl    %cr0, %eax
  # CR0寄存器的0 bit是PE位，启动保护位，当该位被置1，代表开启了保护模式
  orl     $CR0_PE_ON, %eax
  # $CR0_PE_ON 的值一定是 1,
  movl    %eax, %cr0
  # 所以这一步就是将寄存器 cr0 的 PE 位置为 1
  
  # Jump to next instruction, but in 32-bit code segment.
  # Switches processor into 32-bit mode.
  ljmp    $PROT_MODE_CSEG, $protcseg
  # 关于这个指令, 例子为下面这个, 上面的 PROT_MODE_CSEG 也就是 CS 寄存器, 后面的立即数是偏移
  # Long jump, use 0xfebc for the CS register and 0x12345678 for the EIP register:
  # ljmp $0xfebc, $0x12345678
  .code32                     # Assemble for 32-bit mode
protcseg:
  # Set up the protected-mode data segment registers
  movw    $PROT_MODE_DSEG, %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS
  movw    %ax, %ss                # -> SS: Stack Segment
  
  # Set up the stack pointer and call into C.
  movl    $start, %esp
  call bootmain

  # If bootmain returns (it shouldn't), loop.
spin:
  jmp spin

# Bootstrap GDT
.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULL				# null seg
  SEG(STA_X|STA_R, 0x0, 0xffffffff)	# code seg
  SEG(STA_W, 0x0, 0xffffffff)	        # data seg

gdtdesc:
  .word   0x17                            # sizeof(gdt) - 1
  .long   gdt                             # address gdt

