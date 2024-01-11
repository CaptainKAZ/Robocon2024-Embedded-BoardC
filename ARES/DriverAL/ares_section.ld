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
    PROVIDE(_siccmram = _siccmram);
    PROVIDE(_sccmram = _sccmram);
    PROVIDE(_eccmram = _eccmram);
    PROVIDE(_sidata = _sidata);
    PROVIDE(_sdata = _sdata);
    PROVIDE(_edata = _edata);
}
