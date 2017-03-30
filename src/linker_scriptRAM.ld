/* useful reference: www.linuxselfhelp.com/gnu/ld/html_chapter/ld_toc.html */
MEMORY
{
    flash : org = 0x00000000, len = 32k
    ram : org = 0x10000000, len = 8k-768 /* dejar espacio para pilas */
}
  
SECTIONS
{
        
	. = ORIGIN(ram);
    .text : {
		*(.vectors); /* The interrupt vectors */
		*(.bcrp);		/* space before CRP */
		/* . = 0x2fc; */
		*(.crp);		/* code read protection word */
		*(.text);
		*(.text.startup);
    } >ram
	. = ORIGIN(ram);
    .data : {
	  INIT_DATA_VALUES = LOADADDR(.data);
	  INIT_DATA_START = .;
	    *(.data);
	  INIT_DATA_END = .;
    } >ram 
	BSS_START = .;
	__bss_start__ = .;
	.bss : {
	  
	    *(.bss);
	  
	} > ram
	BSS_END = .;
	__bss_end__ = .;
}

