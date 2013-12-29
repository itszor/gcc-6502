/* Subroutines for the gcc driver.
   Copyright (C) 2013 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include <stdio.h>

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"

/* Rewrite -l<foo> as lib<foo>.a, since ld65 does not understand the former.  */

const char*
m65x_fix_dash_l_libs (int argc, const char **argv)
{
  int i;
  char *outlist = xstrdup (" ");
  
  for (i = 0; i < argc; i++)
    if (argv[i][0] == '-' && argv[i][1] == 'l')
      outlist = concat (outlist, "lib", argv[i] + 2, ".a ", NULL);
    else
      outlist = concat (outlist, argv[i], " ", NULL);

  return outlist;
}
