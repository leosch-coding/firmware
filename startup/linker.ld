MEMORY
{
  FLASH(rx) : ORIGIN = 0x08000000, LENGTH = 512K
  RAM  (rwx): ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
  .text :
  {
    *(.isr_vector)
    *(.text)
    *(.rodata)
  } > FLASH

  .data :
  {
    _data_start = .;
    *(.data)
    _data_end = .;
  } > RAM AT > FLASH

  .bss :
  {
    _bss_start = .;
    *(.bss)
    *(COMMON)
    _bss_end = .;
  } > RAM
}

_data_load = LOADADDR(.data);
