SECTIONS
{
    .init.pure_init : {
        PROVIDE(__pure_init_start = .);
        KEEP(*(.init.pure_init))
        PROVIDE(__pure_init_end = .);
    } > FLASH
    .init.device_init : {
        PROVIDE(__device_init_start = .);
        KEEP(*(.init.device_init))
        PROVIDE(__device_init_end = .);
    } > FLASH
    .init.post_os_init : {
        PROVIDE(__post_os_init_start = .);
        KEEP(*(.init.post_os_init))
        PROVIDE(__post_os_init_end = .);
    } > FLASH
    . = ALIGN(4);
}
