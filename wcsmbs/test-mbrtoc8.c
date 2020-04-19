/* Test mbrtoc8.
   Copyright (C) 2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Tom Honermann <tom@honermann.net>, 2020.

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

/* We always want assert to be fully defined.  */
#undef NDEBUG
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

static int
test_utf8 (void)
{
  const char *locale = "de_DE.UTF-8";
  const char *mbs;
  char8_t buf[1];
  mbstate_t s;

  if (!setlocale (LC_ALL, locale))
    {
      fprintf (stderr, "locale '%s' not available!\n", locale);
      exit (1);
    }

  /* No inputs.  */
  mbs = "";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 0, &s) == (size_t) -2); /* no input */
  assert (mbsinit (&s));

  /* Null character.  */
  mbs = "\x00"; /* 0x00 => U+0000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 0); /* null byte written */
  mbs += 1;
  assert (buf[0] == 0x00);
  assert (mbsinit (&s));

  /* First non-null character in the code point range that maps to a single
     code unit.  */
  mbs = "\x01"; /* 0x01 => U+0001 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 1); /* 1st byte processed */
  mbs += 1;
  assert (buf[0] == 0x01);
  assert (mbsinit (&s));

  /* Last character in the code point range that maps to a single code unit.  */
  mbs = "\x7F"; /* 0x7F => U+007F */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 1);  /* 1st byte processed */
  mbs += 1;
  assert (buf[0] == 0x7F);
  assert (mbsinit (&s));

  /* First character in the code point range that maps to two code units.  */
  mbs = "\xC2\x80"; /* 0xC2 0x80 => U+0080 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 2);  /* 1st byte written */
  mbs += 2;
  assert (buf[0] == 0xC2);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0x80);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xC2\x80"; /* 0xC2 0x80 => U+0080 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xC2);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0x80);
  assert (mbsinit (&s));

  /* Last character in the code point range that maps to two code units.  */
  mbs = "\xDF\xBF"; /* 0xDF 0xBF => U+07FF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 2);  /* 1st byte written */
  mbs += 2;
  assert (buf[0] == 0xDF);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xDF\xBF"; /* 0xDF 0xBF => U+07FF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xDF);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* First character in the code point range that maps to three code units.  */
  mbs = u8"\xE0\xA0\x80"; /* 0xE0 0xA0 0x80 => U+0800 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 3);  /* 1st byte written */
  mbs += 3;
  assert (buf[0] == 0xE0);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0xA0);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0x80);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = u8"\xE0\xA0\x80"; /* 0xE0 0xA0 0x80 => U+0800 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xE0);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0xA0);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0x80);
  assert (mbsinit (&s));

  /* Last character in the code point range that maps to three code units
     before the surrogate code point range.  */
  mbs = "\xED\x9F\xBF"; /* 0xED 0x9F 0xBF => U+D7FF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 3);  /* 1st byte written */
  mbs += 3;
  assert (buf[0] == 0xED);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0x9F);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xED\x9F\xBF"; /* 0xED 0x9F 0xBF => U+D7FF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xED);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0x9F);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* First character in the code point range that maps to three code units
     after the surrogate code point range.  */
  mbs = "\xEE\x80\x80"; /* 0xEE 0x80 0x80 => U+E000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 3);  /* 1st byte written */
  mbs += 3;
  assert (buf[0] == 0xEE);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0x80);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0x80);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xEE\x80\x80"; /* 0xEE 0x80 0x80 => U+E000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xEE);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0x80);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0x80);
  assert (mbsinit (&s));

  /* Not a BOM.  */
  mbs = "\xEF\xBB\xBF"; /* 0xEF 0xBB 0xBF => U+FEFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 3);  /* 1st byte written */
  mbs += 3;
  assert (buf[0] == 0xEF);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0xBB);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xEF\xBB\xBF"; /* 0xEF 0xBB 0xBF => U+FEFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xEF);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0xBB);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* Replacement character.  */
  mbs = "\xEF\xBF\xBD"; /* 0xEF 0xBF 0xBD => U+FFFD */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 3);  /* 1st byte written */
  mbs += 3;
  assert (buf[0] == 0xEF);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0xBF);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0xBD);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xEF\xBF\xBD"; /* 0xEF 0xBF 0xBD => U+FFFD */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xEF);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0xBF);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0xBD);
  assert (mbsinit (&s));

  /* Last character in the code point range that maps to three code units.  */
  mbs = "\xEF\xBF\xBF"; /* 0xEF 0xBF 0xBF => U+FFFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 3);  /* 1st byte processed */
  mbs += 3;
  assert (buf[0] == 0xEF);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte processed */
  assert (buf[0] == 0xBF);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte processed */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xEF\xBF\xBF"; /* 0xEF 0xBF 0xBF => U+FFFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xEF);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0xBF);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* First character in the code point range that maps to four code units.  */
  mbs = "\xF0\x90\x80\x80"; /* 0xF0 0x90 0x80 0x80 => U+10000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 4);  /* 1st byte written */
  mbs += 4;
  assert (buf[0] == 0xF0);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0x90);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0x80);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 4th byte written */
  assert (buf[0] == 0x80);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xF0\x90\x80\x80"; /* 0xF0 0x90 0x80 0x80 => U+10000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xF0);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0x90);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0x80);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 4th byte written */
  assert (buf[0] == 0x80);
  assert (mbsinit (&s));

  /* Last character in the code point range that maps to four code units.  */
  mbs = "\xF4\x8F\xBF\xBF"; /* 0xF4 0x8F 0xBF 0xBF => U+10FFFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 4);  /* 1st byte written */
  mbs += 4;
  assert (buf[0] == 0xF4);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0x8F);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0xBF);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 4th byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xF4\x8F\xBF\xBF"; /* 0xF4 0x8F 0xBF 0xBF => U+10FFFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xF4);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0x8F);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0xBF);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 4th byte written */
  assert (buf[0] == 0xBF);
  assert (mbsinit (&s));

  return 0;
}

static int
test_big5_hkscs (void)
{
  const char *locale = "zh_HK.BIG5-HKSCS";
  const char *mbs;
  char8_t buf[1];
  mbstate_t s;

  if (!setlocale (LC_ALL, locale))
    {
      fprintf (stderr, "locale '%s' not available!\n", locale);
      exit (1);
    }

  /* A double byte character that maps to a pair of two byte UTF-8 code unit
     sequences.  */
  mbs = "\x88\x62"; /* 0x88 0x62 => U+00CA U+0304 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 2);  /* 1st byte written */
  mbs += 2;
  assert (buf[0] == 0xC3);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0x8A);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0xCC);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 4th byte written */
  assert (buf[0] == 0x84);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\x88\x62"; /* 0x88 0x62 => U+00CA U+0304 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xC3);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0x8A);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0xCC);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 4th byte written */
  assert (buf[0] == 0x84);
  assert (mbsinit (&s));

  /* Another double byte character that maps to a pair of two byte UTF-8 code
     unit sequences.  */
  mbs = "\x88\xA5"; /* 0x88 0xA5 => U+00EA U+030C */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) 2);  /* 1st byte written */
  mbs += 2;
  assert (buf[0] == 0xC3);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 2nd byte written */
  assert (buf[0] == 0xAA);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 3rd byte written */
  assert (buf[0] == 0xCC);
  assert (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s) == (size_t) -3); /* 4th byte written */
  assert (buf[0] == 0x8C);
  assert (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\x88\xA5"; /* 0x88 0xA5 => U+00EA U+030C */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -2);             /* incomplete */
  mbs += 1;
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) 1);              /* 1st byte written */
  mbs += 1;
  assert (buf[0] == 0xC3);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 2nd byte written */
  assert (buf[0] == 0xAA);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 3rd byte written */
  assert (buf[0] == 0xCC);
  assert (mbrtoc8 (buf, mbs, 1, &s) == (size_t) -3);             /* 4th byte written */
  assert (buf[0] == 0x8C);
  assert (mbsinit (&s));

  return 0;
}

static int
do_test (void)
{
  int result = 0;

  result |= test_utf8 ();
  result |= test_big5_hkscs ();

  return result;
}

#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
