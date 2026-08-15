/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);    /* end of RAM */ /* 栈顶使用 */

HEAP_SIZE = 640K;

/* Specify the memory areas */
MEMORY
{
  RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 704K
  CCMRAM (xrw)   : ORIGIN = 0x10000000, LENGTH = 64K
  FLASH (rx)     : ORIGIN = 0x8000000,  LENGTH = 2048K
}

/* Define output sections */
SECTIONS
{
  /* 中断向量表 The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    _stext = .;
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    /*  OHOS  */
    . = ALIGN(4);
    __zinitcall_bsp_start = .;
    KEEP (*(.zinitcall.bsp0.init))
    KEEP (*(.zinitcall.bsp1.init))
    KEEP (*(.zinitcall.bsp2.init))
    KEEP (*(.zinitcall.bsp3.init))
    KEEP (*(.zinitcall.bsp4.init))
    __zinitcall_bsp_end = .;
    __zinitcall_device_start = .;
    KEEP (*(.zinitcall.device0.init))
    KEEP (*(.zinitcall.device1.init))
    KEEP (*(.zinitcall.device2.init))
    KEEP (*(.zinitcall.device3.init))
    KEEP (*(.zinitcall.device4.init))
    __zinitcall_device_end = .;
    __zinitcall_core_start = .;
    KEEP (*(.zinitcall.core0.init))
    KEEP (*(.zinitcall.core1.init))
    KEEP (*(.zinitcall.core2.init))
    KEEP (*(.zinitcall.core3.init))
    KEEP (*(.zinitcall.core4.init))
    __zinitcall_core_end = .;
    __zinitcall_sys_service_start = .;
    KEEP (*(.zinitcall.sys.service0.init))
    KEEP (*(.zinitcall.sys.service1.init))
    KEEP (*(.zinitcall.sys.service2.init))
    KEEP (*(.zinitcall.sys.service3.init))
    KEEP (*(.zinitcall.sys.service4.init))
    __zinitcall_sys_service_end = .;
    __zinitcall_sys_feature_start = .;
    KEEP (*(.zinitcall.sys.feature0.init))
    KEEP (*(.zinitcall.sys.feature1.init))
    KEEP (*(.zinitcall.sys.feature2.init))
    KEEP (*(.zinitcall.sys.feature3.init))
    KEEP (*(.zinitcall.sys.feature4.init))
    __zinitcall_sys_feature_end = .;
    __zinitcall_run_start = .;
    KEEP (*(.zinitcall.run0.init))
    KEEP (*(.zinitcall.run1.init))
    KEEP (*(.zinitcall.run2.init))
    KEEP (*(.zinitcall.run3.init))
    KEEP (*(.zinitcall.run4.init))
    __zinitcall_run_end = .;
    __zinitcall_app_service_start = .;
    KEEP (*(.zinitcall.app.service0.init))
    KEEP (*(.zinitcall.app.service1.init))
    KEEP (*(.zinitcall.app.service2.init))
    KEEP (*(.zinitcall.app.service3.init))
    KEEP (*(.zinitcall.app.service4.init))
    __zinitcall_app_service_end = .;
    __zinitcall_app_feature_start = .;
    KEEP (*(.zinitcall.app.feature0.init))
    KEEP (*(.zinitcall.app.feature1.init))
    KEEP (*(.zinitcall.app.feature2.init))
    KEEP (*(.zinitcall.app.feature3.init))
    KEEP (*(.zinitcall.app.feature4.init))
    __zinitcall_app_feature_end = .;
    __zinitcall_test_start = .;
    KEEP (*(.zinitcall.test0.init))
    KEEP (*(.zinitcall.test1.init))
    KEEP (*(.zinitcall.test2.init))
    KEEP (*(.zinitcall.test3.init))
    KEEP (*(.zinitcall.test4.init))
    __zinitcall_test_end = .;
    __zinitcall_exit_start = .;
    KEEP (*(.zinitcall.exit0.init))
    KEEP (*(.zinitcall.exit1.init))
    KEEP (*(.zinitcall.exit2.init))
    KEEP (*(.zinitcall.exit3.init))
    KEEP (*(.zinitcall.exit4.init))
    __zinitcall_exit_end = .;
    _hdf_drivers_start = .;
    KEEP (*(.hdf.driver))
    _hdf_drivers_end = .;
    . = ALIGN(4);
    
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sstack = .;
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH
 
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

    ._user_heap :
    {
        . = ALIGN(8);
        __heap_start = .;
        . += HEAP_SIZE;
        __heap_end = .;
    } >RAM

    __heap_size = __heap_end - __heap_start;

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = ALIGN(8);
  } >RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}


