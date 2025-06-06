# Declare constants for the multiboot header
.set ALIGN,			1<<0					# align loaded modules on page boundaries
.set MEMINFO,		1<<1					# provide memory map
.set MB_FLAGS,		ALIGN | MEMINFO			# this is the Multiboot 'flag' field
.set MB_MAGIC,		0x1BADB002				# 'magic number' lets bootloader find the header
.set MB_CHECKSUM,	-(MB_MAGIC + MB_FLAGS)	#checksum of above, to prove we are multiboot

# Multiboot header
.section .multiboot
	.align 4
	.long MB_MAGIC
	.long MB_FLAGS
	.long MB_CHECKSUM

 # Create stack
.section .bss
	stack_bottom:
	.skip 16384		# 16 KiB
	stack_top:

.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	pushl %eax
	pushl %ebx

	call _init

	call kernel_main

	cli
1:
	hlt
	jmp 1b

.size _start, . - _start