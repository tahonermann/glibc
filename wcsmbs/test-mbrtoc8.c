/* Test mbrtoc8.
   Copyright (C) 2021 Free Software Foundation, Inc.
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

#include <features.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>
#include <support/check.h>

static int
test_utf8 (void)
{
  const char *mbs;
  char8_t buf[1];
  mbstate_t s;

  TEST_VERIFY_EXIT (setlocale (LC_ALL, "de_DE.UTF-8") != NULL);

  /* No inputs.  */
  mbs = "";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 0, &s), (size_t) -2); /* no input */
  TEST_VERIFY (mbsinit (&s));

  /* Null character.  */
  mbs = "\x00"; /* 0x00 => U+0000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 0); /* null byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0x00);
  TEST_VERIFY (mbsinit (&s));

  /* First non-null character in the code point range that maps to a single
     code unit.  */
  mbs = "\x01"; /* 0x01 => U+0001 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 1); /* 1st byte processed */
  mbs += 1;
  TEST_COMPARE (buf[0], 0x01);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to a single code unit.  */
  mbs = "\x7F"; /* 0x7F => U+007F */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 1);  /* 1st byte processed */
  mbs += 1;
  TEST_COMPARE (buf[0], 0x7F);
  TEST_VERIFY (mbsinit (&s));

  /* First character in the code point range that maps to two code units.  */
  mbs = "\xC2\x80"; /* 0xC2 0x80 => U+0080 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 2);  /* 1st byte written */
  mbs += 2;
  TEST_COMPARE (buf[0], 0xC2);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xC2\x80"; /* 0xC2 0x80 => U+0080 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xC2);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to two code units.  */
  mbs = "\xDF\xBF"; /* 0xDF 0xBF => U+07FF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 2);  /* 1st byte written */
  mbs += 2;
  TEST_COMPARE (buf[0], 0xDF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xDF\xBF"; /* 0xDF 0xBF => U+07FF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xDF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* First character in the code point range that maps to three code units.  */
  mbs = u8"\xE0\xA0\x80"; /* 0xE0 0xA0 0x80 => U+0800 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 3);  /* 1st byte written */
  mbs += 3;
  TEST_COMPARE (buf[0], 0xE0);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xA0);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = u8"\xE0\xA0\x80"; /* 0xE0 0xA0 0x80 => U+0800 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xE0);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xA0);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to three code units
     before the surrogate code point range.  */
  mbs = "\xED\x9F\xBF"; /* 0xED 0x9F 0xBF => U+D7FF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 3);  /* 1st byte written */
  mbs += 3;
  TEST_COMPARE (buf[0], 0xED);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x9F);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xED\x9F\xBF"; /* 0xED 0x9F 0xBF => U+D7FF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xED);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x9F);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* First character in the code point range that maps to three code units
     after the surrogate code point range.  */
  mbs = "\xEE\x80\x80"; /* 0xEE 0x80 0x80 => U+E000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 3);  /* 1st byte written */
  mbs += 3;
  TEST_COMPARE (buf[0], 0xEE);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xEE\x80\x80"; /* 0xEE 0x80 0x80 => U+E000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xEE);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Not a BOM.  */
  mbs = "\xEF\xBB\xBF"; /* 0xEF 0xBB 0xBF => U+FEFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 3);  /* 1st byte written */
  mbs += 3;
  TEST_COMPARE (buf[0], 0xEF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xBB);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xEF\xBB\xBF"; /* 0xEF 0xBB 0xBF => U+FEFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xEF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xBB);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* Replacement character.  */
  mbs = "\xEF\xBF\xBD"; /* 0xEF 0xBF 0xBD => U+FFFD */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 3);  /* 1st byte written */
  mbs += 3;
  TEST_COMPARE (buf[0], 0xEF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBD);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xEF\xBF\xBD"; /* 0xEF 0xBF 0xBD => U+FFFD */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xEF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBD);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to three code units.  */
  mbs = "\xEF\xBF\xBF"; /* 0xEF 0xBF 0xBF => U+FFFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 3);  /* 1st byte processed */
  mbs += 3;
  TEST_COMPARE (buf[0], 0xEF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte processed */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte processed */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xEF\xBF\xBF"; /* 0xEF 0xBF 0xBF => U+FFFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xEF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* First character in the code point range that maps to four code units.  */
  mbs = "\xF0\x90\x80\x80"; /* 0xF0 0x90 0x80 0x80 => U+10000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 4);  /* 1st byte written */
  mbs += 4;
  TEST_COMPARE (buf[0], 0xF0);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x90);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 4th byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xF0\x90\x80\x80"; /* 0xF0 0x90 0x80 0x80 => U+10000 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xF0);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x90);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 4th byte written */
  TEST_COMPARE (buf[0], 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to four code units.  */
  mbs = "\xF4\x8F\xBF\xBF"; /* 0xF4 0x8F 0xBF 0xBF => U+10FFFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 4);  /* 1st byte written */
  mbs += 4;
  TEST_COMPARE (buf[0], 0xF4);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x8F);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 4th byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\xF4\x8F\xBF\xBF"; /* 0xF4 0x8F 0xBF 0xBF => U+10FFFF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xF4);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x8F);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 4th byte written */
  TEST_COMPARE (buf[0], 0xBF);
  TEST_VERIFY (mbsinit (&s));

  return 0;
}

static int
test_big5_hkscs (void)
{
  const char *mbs;
  char8_t buf[1];
  mbstate_t s;

  TEST_VERIFY_EXIT (setlocale (LC_ALL, "zh_HK.BIG5-HKSCS") != NULL);

  /* A double byte character that maps to a pair of two byte UTF-8 code unit
     sequences.  */
  mbs = "\x88\x62"; /* 0x88 0x62 => U+00CA U+0304 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 2);  /* 1st byte written */
  mbs += 2;
  TEST_COMPARE (buf[0], 0xC3);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x8A);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xCC);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 4th byte written */
  TEST_COMPARE (buf[0], 0x84);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\x88\x62"; /* 0x88 0x62 => U+00CA U+0304 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xC3);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0x8A);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xCC);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 4th byte written */
  TEST_COMPARE (buf[0], 0x84);
  TEST_VERIFY (mbsinit (&s));

  /* Another double byte character that maps to a pair of two byte UTF-8 code
     unit sequences.  */
  mbs = "\x88\xA5"; /* 0x88 0xA5 => U+00EA U+030C */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) 2);  /* 1st byte written */
  mbs += 2;
  TEST_COMPARE (buf[0], 0xC3);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xAA);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xCC);
  TEST_COMPARE (mbrtoc8 (buf, mbs, strlen(mbs)+1, &s), (size_t) -3); /* 4th byte written */
  TEST_COMPARE (buf[0], 0x8C);
  TEST_VERIFY (mbsinit (&s));

  /* Same as last test, but one code unit at a time.  */
  mbs = "\x88\xA5"; /* 0x88 0xA5 => U+00EA U+030C */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -2);             /* incomplete */
  mbs += 1;
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) 1);              /* 1st byte written */
  mbs += 1;
  TEST_COMPARE (buf[0], 0xC3);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 2nd byte written */
  TEST_COMPARE (buf[0], 0xAA);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 3rd byte written */
  TEST_COMPARE (buf[0], 0xCC);
  TEST_COMPARE (mbrtoc8 (buf, mbs, 1, &s), (size_t) -3);             /* 4th byte written */
  TEST_COMPARE (buf[0], 0x8C);
  TEST_VERIFY (mbsinit (&s));

  return 0;
}

static int
do_test (void)
{
  test_utf8 ();
  test_big5_hkscs ();
  return 0;
}

#include <support/test-driver.c>
