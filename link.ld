ENTRY(loader)               /* Nombre de la etiqueta de entrada */

SECTIONS {
    . = 0x00100000;         /* El codigo debe de ser cargado a 1MB */

    .text ALIGN (0x1000) :  /* Se alinea a 4KB */
    {
        *(.text)            /* Todas las secciones de texto de todos los archivos */
    }

    .rodata ALIGN (0x1000) : /* Alinear a 4KB */
    {
        *(.rodata*)         /* Toda la seccion "read-only" de todos los archivos */
    }

    .data ALIGN (0x1000) :
    {
        *(.data)            /* Todas las secciones DATA de todos los archivos */
    }

    .bss ALIGN (0x1000) :
    {
        *(COMMON)           /* Todas las secciones COMMON */
        *(.bss)             /* Todas las secciones BSS */
    }

}