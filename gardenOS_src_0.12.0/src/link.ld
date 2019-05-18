/*
OUTPUT_FORMAT(pe-i386)
OUTPUT_FORMAT("elf32-i386")
OUTPUT_FORMAT(elf64-x86-64)
*/
SECTIONS
{ 
	. = 0x100000;            /* loading address = grub header, must be 32 bits aligned */
	_ker_load_base_ = .;
	.text : 
	{ 
	  *(.text) 
	}
	. = ALIGN(4);
	.rdata : 
	{ 
	  *(.rdata) 
	}
	. = ALIGN(4);
	_ker_rdata_end_ = .;
	.data : 
	{ 
	  *(.data) 
	}
	. = ALIGN(4);
	_ker_load_end_ = .;
	.bss :                   /* must be cleared in boot.s, not done by grub */
	{ 
	  *(.bss)
	}
	. = ALIGN(4);
	_ker_bss_end_ = .;
}
