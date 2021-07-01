/* Test c8rtomb.
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
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>
#include <support/check.h>

static int
test_truncated_code_unit_sequence (void)
{
  const char8_t *u8s;
  char buf[MB_LEN_MAX];
  mbstate_t s;

  /* Missing trailing code unit for a two code byte unit sequence.  */
  u8s = (const char8_t*) u8"\xC2";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* No trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Missing first trailing code unit for a three byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xE0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* No trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Missing second trailing code unit for a three byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xE0\xA0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t)  0); /* 2nd byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) -1); /* No trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Missing first trailing code unit for a four byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xF0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* No trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Missing second trailing code unit for a four byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xF0\x90";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t)  0); /* 2nd byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) -1); /* No trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Missing third trailing code unit for a four byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xF0\x90\x80";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t)  0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t)  0); /* 3rd byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[3], &s), (size_t) -1); /* No trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  return 0;
}

static int
test_invalid_trailing_code_unit_sequence (void)
{
  const char8_t *u8s;
  char buf[MB_LEN_MAX];
  mbstate_t s;

  /* Invalid trailing code unit for a two code byte unit sequence.  */
  u8s = (const char8_t*) u8"\xC2\xC0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Invalid first trailing code unit for a three byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xE0\xC0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Invalid second trailing code unit for a three byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xE0\xA0\xC0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t)  0); /* 2nd byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Invalid first trailing code unit for a four byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xF0\xC0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Invalid second trailing code unit for a four byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xF0\x90\xC0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t)  0); /* 2nd byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Invalid third trailing code unit for a four byte code unit sequence.  */
  u8s = (const char8_t*) u8"\xF0\x90\x80\xC0";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t)  0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t)  0); /* 3rd byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[3], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  return 0;
}

static int
test_lone_trailing_code_units (void)
{
  const char8_t *u8s;
  char buf[MB_LEN_MAX];
  mbstate_t s;

  /* Lone trailing code unit.  */
  u8s = (const char8_t*) u8"\x80";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) -1); /* Lone trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  return 0;
}

static int
test_overlong_encoding (void)
{
  const char8_t *u8s;
  char buf[MB_LEN_MAX];
  mbstate_t s;

  /* Two byte overlong encoding.  */
  u8s = (const char8_t*) u8"\xC0\x80";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) -1); /* Invalid lead code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Two byte overlong encoding.  */
  u8s = (const char8_t*) u8"\xC1\x80";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) -1); /* Invalid lead code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Three byte overlong encoding.  */
  u8s = (const char8_t*) u8"\xE0\x9F\xBF";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* First byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Four byte overlong encoding.  */
  u8s = (const char8_t*) u8"\xF0\x8F\xBF\xBF";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* First byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  return 0;
}

static int
test_surrogate_range (void)
{
  const char8_t *u8s;
  char buf[MB_LEN_MAX];
  mbstate_t s;

  /* Would encode U+D800.  */
  u8s = (const char8_t*) u8"\xED\xA0\x80";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* First byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Would encode U+DFFF.  */
  u8s = (const char8_t*) u8"\xED\xBF\xBF";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* First byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  return 0;
}

static int
test_out_of_range_encoding (void)
{
  const char8_t *u8s;
  char buf[MB_LEN_MAX];
  mbstate_t s;

  /* Would encode U+00110000.  */
  u8s = (const char8_t*) u8"\xF4\x90\x80\x80";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t)  0); /* First byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) -1); /* Invalid trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  /* Would encode U+00140000.  */
  u8s = (const char8_t*) u8"\xF5\x90\x80\x80";
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  errno = 0;
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) -1); /* Invalid lead code unit */
  TEST_COMPARE (errno, EILSEQ);

  return 0;
}

static int
test_null_output_buffer (void)
{
  char buf[MB_LEN_MAX];
  mbstate_t s;

  /* Null character with an initial state.  */
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (NULL, u8"X"[0], &s), (size_t) 1); /* null byte processed */
  TEST_VERIFY (mbsinit (&s));    /* Assert the state is now in the initial state.  */

  /* Null buffer with a state corresponding to an incompletely read code
     unit sequence.  In this case, an error occurs since insufficient
     information is available to complete the already started code unit
     sequence and return to the initial state.  */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8"\xC2"[0], &s), (size_t)  0);  /* 1st byte processed */
  errno = 0;
  TEST_COMPARE (c8rtomb (NULL, u8"\x80"[0], &s), (size_t) -1); /* No trailing code unit */
  TEST_COMPARE (errno, EILSEQ);

  return 0;
}

static int
test_utf8 (void)
{
  const char8_t *u8s;
  char buf[MB_LEN_MAX];
  mbstate_t s;

  TEST_VERIFY_EXIT (setlocale (LC_ALL, "de_DE.UTF-8") != NULL);

  /* Null character.  */
  u8s = (const char8_t*) u8"\x00"; /* U+0000 => 0x00 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 1); /* 1st byte processed */
  TEST_COMPARE (buf[0], (char) 0x00);
  TEST_VERIFY (mbsinit (&s));

  /* First non-null character in the code point range that maps to a single
     code unit.  */
  u8s = (const char8_t*) u8"\x01"; /* U+0001 => 0x01 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 1); /* 1st byte processed */
  TEST_COMPARE (buf[0], (char) 0x01);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to a single code unit.  */
  u8s = (const char8_t*) u8"\x7F"; /* U+007F => 0x7F */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 1); /* 1st byte processed */
  TEST_COMPARE (buf[0], (char) 0x7F);
  TEST_VERIFY (mbsinit (&s));

  /* First character in the code point range that maps to two code units.  */
  u8s = (const char8_t*) u8"\xC2\x80"; /* U+0080 => 0xC2 0x80 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 2); /* 2nd byte processed */
  TEST_COMPARE (buf[0], (char) 0xC2);
  TEST_COMPARE (buf[1], (char) 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to two code units.  */
  u8s = (const char8_t*) u8"\u07FF"; /* U+07FF => 0xDF 0xBF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 2); /* 2nd byte processed */
  TEST_COMPARE (buf[0], (char) 0xDF);
  TEST_COMPARE (buf[1], (char) 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* First character in the code point range that maps to three code units.  */
  u8s = (const char8_t*) u8"\u0800"; /* U+0800 => 0xE0 0xA0 0x80 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 3); /* 3rd byte processed */
  TEST_COMPARE (buf[0], (char) 0xE0);
  TEST_COMPARE (buf[1], (char) 0xA0);
  TEST_COMPARE (buf[2], (char) 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to three code units
     before the surrogate code point range.  */
  u8s = (const char8_t*) u8"\uD7FF"; /* U+D7FF => 0xED 0x9F 0xBF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 3); /* 3rd byte processed */
  TEST_COMPARE (buf[0], (char) 0xED);
  TEST_COMPARE (buf[1], (char) 0x9F);
  TEST_COMPARE (buf[2], (char) 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* First character in the code point range that maps to three code units
     after the surrogate code point range.  */
  u8s = (const char8_t*) u8"\uE000"; /* U+E000 => 0xEE 0x80 0x80 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 3); /* 3rd byte processed */
  TEST_COMPARE (buf[0], (char) 0xEE);
  TEST_COMPARE (buf[1], (char) 0x80);
  TEST_COMPARE (buf[2], (char) 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Not a BOM.  */
  u8s = (const char8_t*) u8"\uFEFF"; /* U+FEFF => 0xEF 0xBB 0xBF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 3); /* 3rd byte processed */
  TEST_COMPARE (buf[0], (char) 0xEF);
  TEST_COMPARE (buf[1], (char) 0xBB);
  TEST_COMPARE (buf[2], (char) 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* Replacement character.  */
  u8s = (const char8_t*) u8"\uFFFD"; /* U+FFFD => 0xEF 0xBF 0xBD */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 3); /* 3rd byte processed */
  TEST_COMPARE (buf[0], (char) 0xEF);
  TEST_COMPARE (buf[1], (char) 0xBF);
  TEST_COMPARE (buf[2], (char) 0xBD);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to three code units.  */
  u8s = (const char8_t*) u8"\uFFFF"; /* U+FFFF => 0xEF 0xBF 0xBF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 3); /* 3rd byte processed */
  TEST_COMPARE (buf[0], (char) 0xEF);
  TEST_COMPARE (buf[1], (char) 0xBF);
  TEST_COMPARE (buf[2], (char) 0xBF);
  TEST_VERIFY (mbsinit (&s));

  /* First character in the code point range that maps to four code units.  */
  u8s = (const char8_t*) u8"\U00010000"; /* U+10000 => 0xF0 0x90 0x80 0x80 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 0); /* 3rd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[3], &s), (size_t) 4); /* 4th byte processed */
  TEST_COMPARE (buf[0], (char) 0xF0);
  TEST_COMPARE (buf[1], (char) 0x90);
  TEST_COMPARE (buf[2], (char) 0x80);
  TEST_COMPARE (buf[3], (char) 0x80);
  TEST_VERIFY (mbsinit (&s));

  /* Last character in the code point range that maps to four code units.  */
  u8s = (const char8_t*) u8"\U0010FFFF"; /* U+10FFFF => 0xF4 0x8F 0xBF 0xBF */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 0); /* 3rd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[3], &s), (size_t) 4); /* 4th byte processed */
  TEST_COMPARE (buf[0], (char) 0xF4);
  TEST_COMPARE (buf[1], (char) 0x8F);
  TEST_COMPARE (buf[2], (char) 0xBF);
  TEST_COMPARE (buf[3], (char) 0xBF);
  TEST_VERIFY (mbsinit (&s));

  return 0;
}

static int
test_big5_hkscs (void)
{
  const char8_t *u8s;
  char buf[MB_LEN_MAX];
  mbstate_t s;

  TEST_VERIFY_EXIT (setlocale (LC_ALL, "zh_HK.BIG5-HKSCS") != NULL);

  /* A pair of two byte UTF-8 code unit sequences that map a Unicode code
     point and combining character to a single double byte character.  */
  u8s = (const char8_t*) u8"\u00CA\u0304"; /* U+00CA U+0304 => 0x88 0x62 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 0); /* 3rd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[3], &s), (size_t) 2); /* 4th byte processed */
  TEST_COMPARE (buf[0], (char) 0x88);
  TEST_COMPARE (buf[1], (char) 0x62);
  TEST_VERIFY (mbsinit (&s));

  /* Another pair of two byte UTF-8 code unit sequences that map a Unicode code
     point and combining character to a single double byte character.  */
  u8s = (const char8_t*) u8"\u00EA\u030C"; /* U+00EA U+030C => 0x88 0xA5 */
  memset (buf, 0, sizeof (buf));
  memset (&s, 0, sizeof (s));
  TEST_COMPARE (c8rtomb (buf, u8s[0], &s), (size_t) 0); /* 1st byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[1], &s), (size_t) 0); /* 2nd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[2], &s), (size_t) 0); /* 3rd byte processed */
  TEST_COMPARE (c8rtomb (buf, u8s[3], &s), (size_t) 2); /* 4th byte processed */
  TEST_COMPARE (buf[0], (char) 0x88);
  TEST_COMPARE (buf[1], (char) 0xA5);
  TEST_VERIFY (mbsinit (&s));

  return 0;
}

static int
do_test (void)
{
  test_truncated_code_unit_sequence ();
  test_invalid_trailing_code_unit_sequence ();
  test_lone_trailing_code_units ();
  test_overlong_encoding ();
  test_surrogate_range ();
  test_out_of_range_encoding ();
  test_null_output_buffer ();
  test_utf8 ();
  test_big5_hkscs ();
  return 0;
}

#include <support/test-driver.c>
