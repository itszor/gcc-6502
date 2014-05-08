#ifndef GCC_6502_OPTS_H
#define GCC_6502_OPTS_H 1

enum processor_type
{
  m6502,
  m6502x,
  w65sc02,
  w65c02,
  huc6280,
  m65x_none
};

enum machine_type
{
  mach_semi65x,
  mach_bbcb,
  mach_bbcmaster,
  mach_c64
};

#endif
