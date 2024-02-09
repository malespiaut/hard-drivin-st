#include "main.h"
#include "missing.h"
#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	shape.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes all of the functions
                        for displaying bit images.

 DATE OF CREATION.....	April 18th 1989
 LAST CHANGE..........	June 21th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

/*		LOCAL VARIABLES :
                =================
*/

/*		FUNCTIONS :
                ===========
*/

static void
Shift(int* source, int* dest, int shift, s_BitImage* shape)
{
  int x;
  int y;
  unsigned long chunk;
  int buf[2];

  for (y = shape->height + 1; --y;)
  {
    for (x = 4 + 1; --x;)
      *dest++ = (unsigned int)(*source++) >> shift;
    for (x = 4 * shape->words - 4 + 1; --x;)
    {
      buf[0] = source[-4];
      buf[1] = *source++;
      chunk = (*(unsigned long*)buf);
      *dest++ = (int)(chunk >> shift);
    }
  }
}

void
PreShift(s_BitImage* shape, int* buf, int* dest)
{
  int i;
  int length;

  length = shape->size;

  for (i = 0; i < 16; i += (16 / preshift))
  {
    Shift(buf, dest, i, shape);
    dest += length;
  }
}

#define C_code FALSE /* Use C routine or Assembly routine */

void
DrawBitBlock(int x, int y, s_BitImage* shape)
{
#if (C_code)

  int mod1;
  int mod2;
  int* data;
  int* pict;
  long mask;

#else
  char parameter[14];
#endif

#if (C_code)

  int h;
  int dx;
  int dy;
  static long bitmask[] = {
    0x00000000L,
    0xC000C000L,
    0xF000F000L,
    0xFC00FC00L,
    0xFF00FF00L,
    0xFFC0FFC0L,
    0xFFF0FFF0L,
    0xFFFCFFFCL,
  };
#endif

  if ((x > MaxX) || (x + shape->width < MinX))
    return;
  if (y - shape->height > MaxY)
    return;

#if (~C_code)

  *(char*)&parameter[0] = shape->words;
  *(char*)&parameter[1] = shape->height;
  *(int*)&parameter[2] = shape->width;
  *(long*)&parameter[4] = (long)shape->data;
  *(int*)&parameter[8] = shape->size;
  *(int*)&parameter[10] = x;
  *(int*)&parameter[12] = y;

  PutBlock(parameter);

#endif

#if (C_code)

  dx = shape->words;
  dy = shape->height;

  data = shape->data;
  mod1 = (bytesInLine >> 1) - (dx << 2);
  mod2 = 0;

  if (y - dy < MinY)
  {
    h = MinY - (y - dy);
    data += (h * dx) << 2;
    dy -= h;
    y = MinY + dy;
  }
  else if (y > MaxY)
  {
    dy -= (y - MaxY);
    y = MaxY + 1;
  }

  if (x + shape->width > MaxX)
  {
    h = ((x + shape->width) - MaxX - 1) >> 4;
    dx -= h;
    h <<= 2;
    mod1 += h;
    mod2 += h;
  }
  else if (x < MinX)
  {
    h = (MinX - x + 15) >> 4;
    dx -= h;
    h <<= 2;
    data += h;
    mod1 += h;
    mod2 += h;
  }

  x += OrgX;
  y += OrgY - dy;

  h = (x & 15) >> 1;
  data += h * shape->size;
  pict = (int*)((long)Buffer2 + (bytesInLine * y));

  if (x < 0)
  {
    for (++dy, ++dx; --dy;)
    {
      for (x = dx; --x;)
      {
        *((long*)pict)++ = *((long*)data)++;
        *((long*)pict)++ = *((long*)data)++;
      }
      pict += mod1;
      data += mod2;
    }
  }
  else
  {
    mask = bitmask[h];
    pict += (x >> 4) << 2;

    for (++dy; --dy;)
    {
      *((long*)pict) &= mask;
      *((long*)pict)++ |= *((long*)data)++;
      *((long*)pict) &= mask;
      *((long*)pict)++ |= *((long*)data)++;

      for (x = dx; --x;)
      {
        *((long*)pict)++ = *((long*)data)++;
        *((long*)pict)++ = *((long*)data)++;
      }
      pict += mod1;
      data += mod2;
    }
  }

#endif
}

#undef C_code

void
DrawShape(int x, int y, s_BitImage* shape)
{
  int shift;
  int h;
  int words;
  int height;
  int offset;
  int size;
  int* mask;

  x -= shape->hotX;
  y -= shape->hotY;

  if ((y + shape->height <= MinY) || (y > MaxY))
    return;
  if ((x + shape->width <= MinX) || (x > MaxX))
    return;

  height = shape->height;
  mask = shape->data;
  words = shape->words + 1;
  size = shape->size;
  offset = 1;
  shift = (x + OrgX) & 15;

  if (y + height > MaxY)
    height = MaxY - y + 1;

  if (y < MinY)
  {
    height += y - MinY;
    h = (MinY - y) * (words + 1);
    mask += h;
    y = MinY;
  }

  if (x + shape->width >= MaxX)
  {
    h = (x + shape->width + 15 - MaxX) >> 4;
    words -= h;
    offset += h;
  }

  if (x < MinX)
  {
    h = (MinX - x + 15) >> 4;
    mask += h;
    words -= h;
    offset += h;
    x = MinX;
  }

  PutShape(mask, size, offset, height, x, y, words, shift);
}
