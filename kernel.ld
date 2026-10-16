/* This is basically a packaging blueprint used during compilation to turn all the raw .cpp 
* and .o files into a single final excutable binary that will be used by opensbi to run kuro
*/

ENTRY(boot) /* OpenSBI boot entry point */

SECTIONS {
    . = 0x80200000; /* RAM base (after OpenSBI) */

    .text : {
        KEEP(*(.text.boot)); /* Boot code first */
        *(.text .text.*);    /* Executable code */
    }

    .rodata : ALIGN(4) {
        *(.rodata .rodata.*); /* Read-only data (strings, constants) */
    }

    .data : ALIGN(4) {
        *(.data .data.*); /* Initialized global/static variables */
    }

    .bss : ALIGN(4) {
        __bss = .; /* Start of zero-initialized data */
        *(.bss .bss.* .sbss .sbss.*);
        __bss_end = .; /* End of zero-initialized data */
    }

    . = ALIGN(4);
    . += 128 * 1024; /* Reserve 128KB for kernel stack */
    __stack_top = .; /* Top of stack (sp register target) */
}