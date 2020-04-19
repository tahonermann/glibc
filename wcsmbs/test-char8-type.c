/* Test char8_t types consistent with compiler.
   Copyright (C) 2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

/* Enable char8_t related library declarations regardless of whether core
   language support is enabled.  This is necessary to allow independent
   testing of library features.  */
#include <features.h>
#ifdef __GLIBC_USE_CHAR8_T
# undef __GLIBC_USE_CHAR8_T
#endif
#define __GLIBC_USE_CHAR8_T 1

#include <uchar.h>

/* Verify that the char8_t type is recognized and can be assigned to a
   UTF-8 string literal.  The cast is used to avoid a compiler warning if
   char8_t core language support is not enabled.  */
#if __STDC_CHAR8_T
const char8_t *s8 = u8"";
#else
const char8_t *s8 = (const char8_t*)u8"";
#endif

static int
do_test (void)
{
  /* This is a compilation test.  */
  return 0;
}

#include <support/test-driver.c>
