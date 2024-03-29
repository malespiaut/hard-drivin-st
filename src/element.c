#include <stdio.h>

#include "main.h"
#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	element.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module contains all street elements.

 DATE OF CREATION.....	July 28th 1989
 LAST CHANGE..........	October 18th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

void
SetNearestElement(void)
{
  unsigned int i;
  unsigned int h;
  unsigned int distance;
  s_track* p;

  NextStreet = p = track1;
  distance = MaxInt;

  for (i = Streets1 + 1; --i > 0;)
  {
    if ((p->y1 <= car.my) || (p->y2 <= car.my))
    {
      h = abs(p->x1 - car.mx) + abs(p->y1 - car.my) + abs(p->z1 - car.mz);
      if (h < distance)
      {
        OnTrack = 0;
        distance = h;
        NextStreet = p;
      }
    }
    p++;
  }

  p = track2;

  for (i = Streets2 + 1; --i > 0;)
  {
    if ((p->y1 <= car.my) || (p->y2 <= car.my))
    {
      h = abs(p->x1 - car.mx) + abs(p->y1 - car.my) + abs(p->z1 - car.mz);
      if (h < distance)
      {
        OnTrack = 1;
        distance = h;
        NextStreet = p;
      }
    }
    p++;
  }

  StreetUnderCar = NextStreet;
}
