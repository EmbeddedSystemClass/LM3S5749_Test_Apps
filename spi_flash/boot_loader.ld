/******************************************************************************
 *
 * Linker configuration file for USB boot loader.
 *
 * LM3S5749.
 *
 *****************************************************************************/

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001800, LENGTH = 0x0001e800 /* 128KB - 6KB for bootloader */
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00010000 /* 64KB */
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
        . = ALIGN (8);
		_end = .;
    } > SRAM
    
    /* .ARM.exidx is sorted, so has to go in its own output section.  */
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > SRAM
}
