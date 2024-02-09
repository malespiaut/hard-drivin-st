#include "main.h"
#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	newfind.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes functions to find all
                        visible street elements.

 DATE OF CREATION.....	December 25th 1989
 LAST CHANGE..........	December 25th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 2.0 for the IBM PC

----------------------------------------------------------------------

*/

#define NewVersion 0 // OLD VERSION: looks at near segments.
                     // NEW VERSION: looks at all segments.

//		VARIABLES :
//              ===========

char NoReturnCheck; // Flag indicates no checking for next returnpoint.

//		FUNCTIONS :
//              ===========

static void CheckReturnPoints(s_track* segment);
static int CheckFirstTrack(int link);
static int CheckSecondTrack(int link);

static void
CheckReturnPoints(s_track* segment)
{

  if (NoReturnCheck)
    return;

  if (OnTrack == 0)
  {
    if (segment == &track1[NextCtr1->segment])
    {
      segment->flags &= (~f_returnpoint);
      NextCtr1->covered = TRUE;
      NextCtr1++;
    }
  }
  else
  {
    if (segment == &track2[NextCtr2->segment])
    {
      segment->flags &= (~f_returnpoint);
      NextCtr2->covered = TRUE;
      NextCtr2++;
    }
  }
}

#define MaxDistance 4000

#if (NewVersion)

static int
CheckFirstTrack(int link)
{

  int i, count;
  int *d, *flag;
  s_track* p;
  unsigned int dz;
  char FirstTime;
  int junction, pos, n, nearest, NearFlags;
  s_track *NearestElement, *first;

  static int VisiFlags[MaxSegments];

  // START WITH AN SEGMENT OUTSIDE THE QUADRANT.

  for (p = track1, i = 0; i < Streets1; i++)
  {
    if (abs(p->x1 - car.x) > MaxDistance)
      break;
    if (abs(p->z1 - car.z) > MaxDistance)
      break;
  }

  // NOW FIND ALL SEGMENTS INSIDE THE QUADRANT.

  junction = 0;
  count = 0;
  flag = VisiFlags;
  d = Street1Vertex;
  FirstTime = TRUE;

  for (i = 0; i < Streets1; i++)
  {
    if (p >= End1)
      p = track1;

    // VISIBILITY TEST:

    if ((abs(p->x1 - car.x) < MaxDistance) &&
        (abs(p->z1 - car.z) < MaxDistance))
    {
      if (FirstTime)
      {
        first = p;
        FirstTime = FALSE;
      }

      *d++ = (p->x1 - car.x);
      *d++ = (p->y1 - car.y);
      *d++ = (p->z1 - car.z);

      *d++ = (p->x2 - car.x);
      *d++ = (p->y2 - car.y);
      *d++ = (p->z2 - car.z);

      count++;

      *flag++ = p->flags;
      junction |= p->flags;
    }
    else
    {
      if (!FirstTime)
        break;
    }

    p++;
  }

  // ROTATE ALL STREET ELEMENTS INTO POSITION.

  n = count << 1;

  if (car.yaw)
    RotXZ(n, yawCOS, yawSIN, Street1Vertex, Street1Vertex);
  if (car.pitch)
    RotYZ(n, pitchCOS, pitchSIN, Street1Vertex, Street1Vertex);
  if (car.roll)
    RotXY(n, rollCOS, rollSIN, Street1Vertex, Street1Vertex);

  nearest = MaxInt;
  flag = VisiFlags;
  d = Street1Vertex;
  p = first;

  for (i = pos = 0; i < count; i++)
  {
    if (p >= End1)
      p = track1;

    // FIND NEAREST STREET ELEMENT.

    if ((d[1] < 0) || (d[4] < 0))
    { // RETURNPOINT PASSED ?

      if (*flag & f_returnpoint)
      {
        int y;

        y = d[1] + d[4];

        // RETURNPOINT MUST BE CLOSE.

        if ((y <= 0) && (y > -100))
        {
          if (d[2] + d[5] < 0)
            if (abs(d[0] + d[3]) < 2500)
              CheckReturnPoints(p);
        }
      }

      dz = abs(d[0] + d[3]) + abs(d[1] + d[4] + 2 * CarHeight) + abs(d[2] + d[5]);

      if (dz < nearest)
      {
        if (i - pos < 10)
        {
          NearestElement = p;
          nearest = dz;
          pos = i;
        }
      }
    }

    d += 6;
    flag++;
    p++;
  }

  if (!link)
  {
    NextStreet = NearestElement;
    nearestStreet = nearest;
  }
  else
  {
    if (nearest < nearestStreet)
    {
      NextStreet = NearestElement;
      nearestStreet = nearest;
      OnTrack = 0;
    }
  }

  NearFlags = VisiFlags[pos];

  if (NearFlags & (f_checkpoint + f_finish))
  {
    if (NearFlags & f_checkpoint)
      PassCheckpoint();
    else
      PassFinish();
  }

  // CHECK CAR'S DIRECTION.

  if (!link)
  {
    i = pos << 1;
    d = &Street1Vertex[i + i + i];
    WrongDirection = (d[2] > d[2 + 2 * 3]);
  }

  // FIND FIRST AND LAST VISIBLE STREET ELEMENT.

  d = First1 = Street1Vertex;
  flag = VisiFlags;

  for (i = count, count = 0; --i;)
  {
    if ((d[2] > -RoadLength) && (d[2] < MaxDistance))
    {
      if (!count)
      {
        Flags1 = flag;
        First1 = d;
      }
      count++;
    }
    else
    {
      if (count)
        break;
      flag++;
    }
    d += 2 * 3;
  }

  VisiStr1 = count;

  return (junction & (f_junct1 + f_junct2));
}

static int
CheckSecondTrack(int link)
{

  int i, count;
  int *d, *flag;
  unsigned int dz, nearest;
  s_track* p;
  int junction, pos, n, NearFlags;
  s_track *NearestElement, *first;
  unsigned char FirstTime;

  static int VisiFlags[MaxSegments];

  // NOW FIND ALL SEGMENTS INSIDE THE QUADRANT.

  junction = 0;
  count = 0;
  flag = VisiFlags;
  d = Street2Vertex;
  p = track2;
  FirstTime = TRUE;

  for (i = 0; i < Streets2; i++)
  {
    // VISIBILITY TEST:

    if ((abs(p->x1 - car.x) < MaxDistance) &&
        (abs(p->z1 - car.z) < MaxDistance))
    {
      if (FirstTime)
      {
        first = p;
        FirstTime = FALSE;
      }

      *d++ = (p->x1 - car.x);
      *d++ = (p->y1 - car.y);
      *d++ = (p->z1 - car.z);

      *d++ = (p->x2 - car.x);
      *d++ = (p->y2 - car.y);
      *d++ = (p->z2 - car.z);

      count++;

      *flag++ = p->flags;
      junction |= p->flags;
    }
    else
    {
      if (!FirstTime)
        break;
    }

    p++;
  }

  // ROTATE ALL STREET ELEMENTS INTO POSITION.

  n = count << 1;

  if (car.yaw)
    RotXZ(n, yawCOS, yawSIN, Street2Vertex, Street2Vertex);
  if (car.pitch)
    RotYZ(n, pitchCOS, pitchSIN, Street2Vertex, Street2Vertex);
  if (car.roll)
    RotXY(n, rollCOS, rollSIN, Street2Vertex, Street2Vertex);

  nearest = MaxInt;
  d = Street2Vertex;
  p = NearestElement = first;
  flag = VisiFlags;

  for (i = pos = 0; i < count; i++)
  {
    if (p >= End2)
      break;

    // FIND NEAREST STREET ELEMENT.

    if ((d[1] < 0) || (d[4] < 0))
    { // RETURNPOINT PASSED ?

      if (*flag & f_returnpoint)
      {
        int y;

        y = d[1] + d[4];

        // RETURNPOINT MUST BE CLOSE.

        if ((y <= 0) && (y > -100))
        {
          if (d[2] + d[5] < 0)
            if (abs(d[0] + d[3]) < 2500)
              CheckReturnPoints(p);
        }
      }

      dz = abs(d[0] + d[3]) + abs(d[1] + d[4] + 2 * CarHeight) + abs(d[2] + d[5]);

      if (dz <= nearest)
      {
        if (i - pos < 10)
        {
          NearestElement = p;
          nearest = dz;
          pos = i;
        }
      }
    }

    d += 6;
    flag++;
    p++;
  }

  if (!link)
  {
    NextStreet = NearestElement;
    nearestStreet = nearest;
  }
  else
  {
    if (nearest < nearestStreet)
    {
      NextStreet = NearestElement;
      nearestStreet = nearest;
      OnTrack = 1;
    }
  }

  NearFlags = VisiFlags[pos];

  if (NearFlags & (f_checkpoint + f_finish))
  {
    if (NearFlags & f_checkpoint)
      PassCheckpoint();
    else
      PassFinish();
  }

  // CHECK CAR'S DIRECTION.

  if (!link)
  {
    i = pos << 1;
    if (i + 4 < n)
    {
      d = &Street2Vertex[i + i + i];
      WrongDirection = (d[2] > d[2 + 2 * 3]);
    }
  }

  // FIND FIRST AND LAST VISIBLE STREET ELEMENT.

  d = First2 = Street2Vertex;
  flag = VisiFlags;
  pos -= 2;

  for (i = count, count = 0; --i;)
  {
    if ((d[2] > -RoadLength) && (d[2] < MaxDistance))
    {
      if (!count)
      {
        Flags2 = flag;
        First2 = d;
      }
      count++;
    }
    else
    {
      if (d[2] > MaxDistance)
        break;
      else if (count)
        count++;
      flag++;
    }
    d += 2 * 3;
  }

  VisiStr2 = count;

  return (junction & (f_junct1 + f_junct2));
}

#else

static int
CheckFirstTrack(int link)
{
  unsigned char i, count;
  int *d, *flag;
  s_track* p;
  unsigned int dz;
  int junction, pos, n, nearest, NearFlags, dist;
  s_track *NearestElement, *first;
  static int VisiFlags[MaxSegments];

  junction = 0;
  count = 2 * lookahead + 1; // TOTAL NUMBER OF STREET SEGMENTS TO LOOK AT.

  if (link)
  {
    p = (link & f_junct1) ? Junct1 : Junct2;
    p -= lookahead;
  }
  else
  {
    p = NextStreet - lookahead;

    if (p < track1)
      p = End1 + (int)(p - track1);
  }

  first = p;
  flag = VisiFlags;
  d = Street1Vertex;

  for (i = 0; i < count; i++)
  {
    if (p >= End1)
      p = track1;

    *d++ = (p->x1 - car.x);
    *d++ = (p->y1 - car.y);
    *d++ = (p->z1 - car.z);

    *d++ = (p->x2 - car.x);
    *d++ = (p->y2 - car.y);
    *d++ = (p->z2 - car.z);

    if (p->flags & f_invisible)
    { // DON'T COUNT INVISIBLE SEGMENTS.

      if (++count >= MaxSegments)
        break;
    }

    *flag++ = p->flags;
    junction |= p->flags;

    p++;
  }

  // ROTATE ALL STREET ELEMENTS INTO POSITION.

  n = count << 1;

  if (car.yaw)
    RotXZ(n, yawCOS, yawSIN, Street1Vertex, Street1Vertex);
  if (car.pitch)
    RotYZ(n, pitchCOS, pitchSIN, Street1Vertex, Street1Vertex);
  if (car.roll)
    RotXY(n, rollCOS, rollSIN, Street1Vertex, Street1Vertex);

  nearest = MaxInt;
  flag = VisiFlags;
  d = Street1Vertex;
  p = first;

  for (i = pos = 0; i < count; i++)
  {
    if (p >= End1)
      p = track1;

    // FIND NEAREST STREET ELEMENT.

    if ((d[1] < 0) || (d[4] < 0))
    { // RETURNPOINT PASSED ?

      if (*flag & f_returnpoint)
      {
        int y;

        if (!CarInLoop)
        {
          y = d[1] + d[4];

          // RETURNPOINT MUST BE CLOSE.

          if ((y <= 0) && (y > -100))
          {
            if (d[2] + d[5] < 0)
              if (abs(d[0] + d[3]) < 2500)
                CheckReturnPoints(p);
          }
        }
      }

      dz = abs(d[0] + d[3]) + abs(d[1] + d[4] /*+2*CarHeight*/) + abs(d[2] + d[5]);

      if (dz < nearest)
      {
        if (i - pos < 10)
        {
          NearestElement = p;
          nearest = dz;
          pos = i;
        }
      }
    }

    d += 6;
    flag++;
    p++;
  }

  if (!link)
  {
    NextStreet = NearestElement;
    nearestStreet = nearest;
  }
  else
  {
    if (nearest < nearestStreet)
    {
      NextStreet = NearestElement;
      nearestStreet = nearest;
      OnTrack = 0;
    }
  }

  NearFlags = VisiFlags[pos];

  if (NearFlags & (f_checkpoint + f_finish))
  {
    if (NearFlags & f_checkpoint)
      PassCheckpoint();
    else
      PassFinish();
  }

  // CHECK CAR'S DIRECTION.

  if (!link)
  {
    i = pos << 1;
    d = &Street1Vertex[i + i + i];
    WrongDirection = (d[2] > d[2 + 2 * 3]);
  }

  // FIND FIRST AND LAST VISIBLE STREET ELEMENT.

  d = First1 = Street1Vertex;
  flag = VisiFlags;

  for (i = count, count = 0; --i;)
  {
    dist = (d[2] + d[5]) >> 1;

    if ((dist > -RoadLength) && (dist < MaxDistance))
    {
      if (!count)
      {
        Flags1 = flag;
        First1 = d;
      }
      count++;
    }
    else
    {
      if (count)
        break;
      flag++;
    }
    d += 2 * 3;
  }

  VisiStr1 = count;

  return (junction & (f_junct1 + f_junct2));
}

static int
CheckSecondTrack(int link)
{
  unsigned char i, count;
  int *d, *flag;
  unsigned int dz, nearest;
  s_track* p;
  int junction, pos, n, NearFlags, dist;
  s_track *NearestElement, *first;
  static int VisiFlags[MaxSegments];

  junction = 0;
  count = 2 * lookahead + 1; // TOTAL NUMBER OF STREET SEGMENTS TO LOOK AT.

  if (link)
  {
    p = (link & f_junct1) ? track2 : &End2[-lookahead];
    count = lookahead + 1;
  }
  else
  {
    p = &NextStreet[-lookahead];

    if (p < track2)
    {
      count -= (track2 - p);
      p = track2;
      junction = f_junct1;
    }
  }

  first = p;
  flag = VisiFlags;
  d = Street2Vertex;

  for (i = 0; i < count; i++)
  {
    if (p >= End2)
    {
      junction = f_junct2;
      count = i + 1;
      break;
    }

    *d++ = (p->x1 - car.x);
    *d++ = (p->y1 - car.y);
    *d++ = (p->z1 - car.z);

    *d++ = (p->x2 - car.x);
    *d++ = (p->y2 - car.y);
    *d++ = (p->z2 - car.z);

    *flag++ = p->flags;

    if (p->flags & f_invisible)
    { // DON'T COUNT INVISIBLE SEGMENTS.

      if (++count >= MaxSegments)
        break;
    }

    p++;
  }

  // ROTATE ALL STREET ELEMENTS INTO POSITION.

  n = count << 1;

  if (car.yaw)
    RotXZ(n, yawCOS, yawSIN, Street2Vertex, Street2Vertex);
  if (car.pitch)
    RotYZ(n, pitchCOS, pitchSIN, Street2Vertex, Street2Vertex);
  if (car.roll)
    RotXY(n, rollCOS, rollSIN, Street2Vertex, Street2Vertex);

  nearest = MaxInt;
  d = Street2Vertex;
  p = NearestElement = first;
  flag = VisiFlags;

  for (i = pos = 0; i < count; i++)
  {
    if (p >= End2)
      break;

    // FIND NEAREST STREET ELEMENT.

    if ((d[1] < 0) || (d[4] < 0))
    { // RETURNPOINT PASSED ?

      if (*flag & f_returnpoint)
      {
        int y;

        // NO RETURNPOINT INSIDE THE LOOP.

        if (!CarInLoop)
        {
          y = d[1] + d[4];

          // RETURNPOINT MUST BE CLOSE.

          if ((y <= 0) && (y > -100))
          {
            if (d[2] + d[5] < 0)
              if (abs(d[0] + d[3]) < 2500)
                CheckReturnPoints(p);
          }
        }
      }

      dz = abs(d[0] + d[3]) + abs(d[1] + d[4] /*+2*CarHeight*/) + abs(d[2] + d[5]);

      if (dz <= nearest)
      {
        if (i - pos < 10)
        {
          NearestElement = p;
          nearest = dz;
          pos = i;
        }
      }
    }

    d += 6;
    flag++;
    p++;
  }

  if (!link)
  {
    NextStreet = NearestElement;
    nearestStreet = nearest;
  }
  else
  {
    if (nearest < nearestStreet)
    {
      NextStreet = NearestElement;
      nearestStreet = nearest;
      OnTrack = 1;
    }
  }

  NearFlags = VisiFlags[pos];

  if (NearFlags & (f_checkpoint + f_finish))
  {
    if (NearFlags & f_checkpoint)
      PassCheckpoint();
    else
      PassFinish();
  }

  // CHECK CAR'S DIRECTION.

  if (!link)
  {
    i = pos << 1;
    if (i + 4 < n)
    {
      d = &Street2Vertex[i + i + i];
      WrongDirection = (d[2] > d[2 + 2 * 3]);
    }
  }

  // FIND FIRST AND LAST VISIBLE STREET ELEMENT.

  d = First2 = Street2Vertex;
  flag = VisiFlags;
  pos -= 2;

  for (i = count, count = 0; --i;)
  {
    dist = (d[2] + d[5]) >> 1;

    if ((dist > -RoadLength) && (dist < MaxDistance))
    {
      if (!count)
      {
        Flags2 = flag;
        First2 = d;
      }
      count++;
    }
    else
    {
      if (dist > MaxDistance)
        break;
      else if (count)
        count++;
      flag++;
    }
    d += 2 * 3;
  }

  VisiStr2 = count;

  return (junction & (f_junct1 + f_junct2));
}

#endif

#undef MaxDistance

void
FindStreets(void)
{
  int code;

  VisiStr1 = VisiStr2 = 0;

  if (OnTrack == 1)
  {
    code = CheckSecondTrack(0);

    if (code)
      CheckFirstTrack(code);
  }
  else
  {
    code = CheckFirstTrack(0);

    if (code)
      CheckSecondTrack(code);
  }
}
