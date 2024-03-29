#include "data.h"
#include "main.h"
#include "missing.h"
#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	display.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes functions to display
                        all kinds of objects.

 DATE OF CREATION.....	March 22th 1989
 LAST CHANGE..........	November 7th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

//		LOCAL VARIABLES :
//              =================

//		FUNCTIONS :
//              ===========

static void PrintMountains(int);
static void ClearCockpit(void);
static void ShowAllCarsInTheLoop(s_object*);
static void PrintHillRoads(int);
static void PrintLowStreets(void);
static void CarOnRoad(void);
static void CarInDip(void);

static void
PrintMountains(int y)
{
  int i;
  int h;
  int x;
  int* p;

  x = -(6 * car.yaw) / 5;

  p = MntPositions;

  for (i = 13 + 1; --i;)
  {
    h = x + p[1];

    if (h > MaxX)
      break;
    if (x + p[1 + 2] > MinX)
      DrawBitBlock(h, y, Mountain[p[0]]);
    p += 2;
  }
}

void
PrintBackground(void)
{
  int y;
  int z;
  int HoriY;
  long h;

#define max 10000

  // ROTATE HORIZON.

  HoriY = -car.y;
  y = rot1(HoriY, horizon, pitchSIN, pitchCOS);
  z = rot2(HoriY, horizon, pitchSIN, pitchCOS);

  // PROJECT HORIZON.

  h = ((long)-y << 8) / ((z) ? z : -1);

  if (h > max)
    y = max;
  else if (h < -max)
    y = -max;
  else
    y = (int)h;

  if (z > 0)
  {
    if (abs(car.roll) < 8)
    {
      SolidColor(blue);
      WideBox(MinY, y);
      SolidColor(green);
      WideBox(y + 1, MaxY);

      if (GraphMode == 2)
      {
        NewColor(black);
        MoveTo(-1000, y + 1);
        DrawTo(1000, y + 1);
      }

      if (y >= MinY)
        PrintMountains(++y);
    }
    else
    {
      if (4 * 30 < car.pitch)
      {
        SolidColor(blue);
        WideBox(MinY, MaxY);
      }
      else if (-4 * 30 > car.pitch)
      {
        SolidColor(green);
        WideBox(MinY, MaxY);
      }
      else
      {
        int x, dx, dy;

        x = rotate(y, rollSIN);
        y = rotate(y, rollCOS);

        dx = rotate(rollCOS, 600);
        dy = rotate(-rollSIN, 600);

        NewColor(white);
        MoveTo(-dx, y - dy);
        DrawTo(dx, y + dy);

        PolyVertex[0] = x + dx;
        PolyVertex[1] = y + dy;
        PolyVertex[2] = x + dx - dy;
        PolyVertex[3] = y + dy + dx;
        PolyVertex[4] = x - dx - dy;
        PolyVertex[5] = y - dy + dx;
        PolyVertex[6] = x - dx;
        PolyVertex[7] = y - dy;

        SolidColor(green);
        Polygon(4);

        PolyVertex[0] = x + dx;
        PolyVertex[1] = y + dy;
        PolyVertex[2] = x + dx + dy;
        PolyVertex[3] = y + dy - dx;
        PolyVertex[4] = x - dx + dy;
        PolyVertex[5] = y - dy - dx;
        PolyVertex[6] = x - dx;
        PolyVertex[7] = y - dy;

        SolidColor(blue);
        Polygon(4);
      }
    }
  }
  else
  {
    SolidColor(blue);
    WideBox(y + 1, MaxY);
    SolidColor(green);
    WideBox(MinY, y);
  }

#undef max
}

static void
ClearCockpit(void)
{
  s_block* b;

  // DRAW ALL NECESSARY RECTANGLES.

  if (DemoMode)
    b = &MessageRect[0];
  else
  {
    if (LapMode)
      b = &MessageRect[2];
    else if (QualifyFlag)
      b = &MessageRect[1];
    else
      b = &MessageRect[0];
  }
  CopyBlock(b->sx1, b->sy1, b->sx2, b->sy2, b->dx1, b->dy1, AuxScreen, Buffer2);

  b = (car.autoFlag) ? &AutoRect : &ManualRect;
  CopyBlock(b->sx1, b->sy1, b->sx2, b->sy2, b->dx1, b->dy1, AuxScreen, Buffer2);
  b = &ScoreRect;
  CopyBlock(b->sx1, b->sy1, b->sx2, b->sy2, b->dx1, b->dy1, AuxScreen, Buffer2);
  b = &TimeRect;
  CopyBlock(b->sx1, b->sy1, b->sx2, b->sy2, b->dx1, b->dy1, AuxScreen, Buffer2);
  b = &MidRect;
  CopyBlock(b->sx1, b->sy1, b->sx2, b->sy2, b->dx1, b->dy1, AuxScreen, Buffer2);
  b = &BarRect;
  CopyBlock(b->sx1, b->sy1, b->sx2, b->sy2, b->dx1, b->dy1, AuxScreen, Buffer2);

  // HILITE THE CHOOSEN GEAR.

  b = car.gearBlock;
  CopyBlock(b->sx1, b->sy1, b->sx2, b->sy2, b->dx1, b->dy1, AuxScreen, Buffer2);
}

void
PrintCockpit(void)
{
  int x;
  int y;
  int x1;
  int y1;
  char inactive;
  int angle;
  static int OldRpm;

#define width 6

  inactive = (DemoMode | SloMoFlag);

  ClearCockpit();

  MinY = MinY2;
  MaxY = MaxY2;
  OrgY = OrgY2;

  SolidColor(red);
  NewColor(yellow);

  // DRAW RPM GAUGE.

  if (inactive)
    angle = -4 * 135;
  else
  {
    angle = multdiv((car.rpm + OldRpm) >> 1, 4 * 315, 8000) - 4 * 135;
    OldRpm = car.rpm;
  }

  x = rotate(largePointer, sinus(angle));
  y = rotate(largePointer, cosinus(angle));

  x1 = ((x < 0) ? x - width / 2 : x + width / 2) / width;
  y1 = ((y < 0) ? y - width / 2 : y + width / 2) / width;

  PolyVertex[0] = rpmX + x;
  PolyVertex[1] = rpmY - y;
  PolyVertex[2] = rpmX - y1;
  PolyVertex[3] = rpmY - x1;
  PolyVertex[4] = rpmX + y1;
  PolyVertex[5] = rpmY + x1;
  Polygon(3);

  // DRAW MPH GAUGE.

  if (inactive)
    angle = -4 * 135;
  else
    angle = multdiv(car.mph, 4 * 315, 140) - 4 * 135;

  x = rotate(largePointer, sinus(angle));
  y = rotate(largePointer, cosinus(angle));

  x1 = ((x < 0) ? x - width / 2 : x + width / 2) / width;
  y1 = ((y < 0) ? y - width / 2 : y + width / 2) / width;

  PolyVertex[0] = mphX + x;
  PolyVertex[1] = mphY - y;
  PolyVertex[2] = mphX - y1;
  PolyVertex[3] = mphY - x1;
  PolyVertex[4] = mphX + y1;
  PolyVertex[5] = mphY + x1;
  Polygon(3);

  if (!inactive)
  { // DRAW HORIZONTAL STEERING INDICATOR.

    NewColor(blue);
    x = (SteerX - 320) / 30 - 2;
    MoveTo(x, -22);
    DrawTo(x, -20);
    x++;
    MoveTo(x, -22);
    DrawTo(x, -20);

    // DRAW VERTICAL THROTTLE INDICATOR.

    NewColor(red);
    x = -car.throttle / 13 - 10;
    MoveTo(-1, x);
    DrawTo(-2, x);
    x++;
    MoveTo(-1, x);
    DrawTo(-2, x);

    PrintTime();
  }

  // DISPLAY QUALIFY MESSAGE.

  PrintQualify();

  MinY = MinY1;
  MaxY = MaxY1;
  OrgY = OrgY1;

#undef width
}

static void
ShowAllCarsInTheLoop(s_object* loop)
{
  s_object* obj;
  s_priority* pr;
  unsigned char i;
  int radius;
  int x;
  int z;

  // DISPLAY ALL CARS IN THE LOOP.

  pr = PrioList;
  x = loop->worldX;
  z = loop->worldZ;
  radius = loop->radius;

  for (i = VisibleObjects + 1; --i;)
  {
    obj = (pr++)->obj;
    if (obj->class == c_car)
    {
      if (obj->visible && obj->worldY >= 0)
      {
        if (abs(obj->worldX - x) < radius && abs(obj->worldZ - z) < radius)
        {
          PrintCar(obj);
          obj->visible = FALSE;
        }
      }
    }
  }
}

void
PrintLoop(s_object* obj)
{
  unsigned char* p;
  unsigned char i;
  unsigned char j;
  unsigned char norm;
  long* k;
  long* pv;
  long* q;
  unsigned char stripes[20];
  unsigned char* ps;
  unsigned char FullDetails;
  int para[2];

  FullDetails = (obj->eyeZ < 1500);

  k = (long*)obj->koor;

  if (!obj->clip3D)
  {
    p = obj->faces;
    q = k + 2 * 13;
    ps = stripes;

    for (i = 12 + 1; --i;)
    {
      j = *p++;
      norm = *p++;

      if ((*ps++ = IsVisible(p[0], norm, obj->vert)) == FALSE)
      { // TRANSPARENT ROAD PARTS

        pv = (long*)PolyVertex;
        *pv++ = k[*p++];
        *pv++ = k[*p++];
        *pv++ = k[*p++];
        *pv++ = k[*p++];
        p++;
        SolidColor(hatchedblack);
        Polygon(4);

        // YELLOW STRIPES

        if (FullDetails)
        {
          pv = (long*)PolyVertex;
          *pv++ = *q++;
          *pv++ = *q++;
          *pv++ = *q++;
          *pv++ = *q++;
          SolidColor(orange);
          Polygon(4);
        }
        else
          q += 4;
      }
      else
      {
        q += 4;
        p += 4 + 1;
      }
    }

    // DISPLAY ALL CARS.

    ShowAllCarsInTheLoop(obj);

    p = obj->faces;
    ps = stripes;

    for (i = 12 + 1; --i;)
    {
      if (*ps++)
      {
        p += 2;
        pv = (long*)PolyVertex;
        *pv++ = k[*p++];
        *pv++ = k[*p++];
        *pv++ = k[*p++];
        *pv++ = k[*p++];
        SolidColor((int)*p++);
        Polygon(4);
      }
      else
        p += 4 + 3;
    }
  }
  else
  {
    for (q = k, i = 12 + 1; --i;)
      q += ((int*)q)[0] + 1;

    ps = stripes;
    p = obj->faces;

    for (i = 12 + 1; --i;)
    {
      *(long*)para = *k++;
      p++;
      norm = *p++;

      if ((*ps++ = IsVisible(p[0], norm, obj->vert)) == FALSE)
      { // HATCHED SEGMENTS

        pv = (long*)PolyVertex;
        for (j = para[0] + 1; --j;)
          *pv++ = *k++;
        SolidColor(hatchedblack);
        Polygon(para[0]);

        // LANE MARKERS

        *(long*)para = *q++;
        pv = (long*)PolyVertex;
        for (j = para[0] + 1; --j;)
          *pv++ = *q++;
        SolidColor(orange);
        Polygon(para[0]);
      }
      else
      {
        k += para[0];
        q += ((int*)q)[0] + 1;
      }

      p += 5;
    }

    // DISPLAY ALL CARS.

    ShowAllCarsInTheLoop(obj);

    k = (long*)obj->koor;
    ps = stripes;

    for (i = 12 + 1; --i;)
    {
      *(long*)para = *k++;

      if (*ps++)
      {
        pv = (long*)PolyVertex;
        for (j = para[0] + 1; --j;)
          *pv++ = *k++;
        SolidColor(para[1]);
        Polygon(para[0]);
      }
      else
        k += para[0];
    }
  }
}

// **********************************************************************

void
PrintSign(s_object* obj)
{
  unsigned char* p;
  unsigned char i;
  unsigned char j;
  unsigned char h;
  long* pv;
  long* k;
  int para[2];

  p = obj->faces;
  k = (long*)obj->koor;

  if (!obj->clip3D)
  {
    for (i = obj->polygons + 1; --i;)
    {
      h = *p++;
      p++;
      if (h == 2)
      {
        NewColor(p[2]);
        pv = (long*)PolyVertex;
        *pv++ = k[*p++];
        *pv++ = k[*p++];
        MoveTo(PolyVertex[0], PolyVertex[1]);
        DrawTo(PolyVertex[2], PolyVertex[3]);
        p++;
      }
      else
      {
        pv = (long*)PolyVertex;
        for (j = h + 1; --j;)
          *pv++ = k[*p++];
        SolidColor(*p++);
        Polygon(h);
      }
    }
  }
  else
  {
    for (i = obj->polygons + 1; --i;)
    {
      *(long*)para = *k++;
      pv = (long*)PolyVertex;

      if (para[0] == 2)
      {
        NewColor(para[1]);
        *pv++ = *k++;
        *pv++ = *k++;
        MoveTo(PolyVertex[0], PolyVertex[1]);
        DrawTo(PolyVertex[2], PolyVertex[3]);
      }
      else
      {
        for (j = para[0] + 1; --j;)
          *pv++ = *k++;
        SolidColor(para[1]);
        Polygon(para[0]);
      }
    }
  }
}

void
PrintBridge(s_object* obj)
{
  unsigned char* p;
  unsigned char i;
  unsigned char j;
  unsigned char h;
  long* pv;
  long* k;
  int para[2];

  p = obj->faces;
  k = (long*)obj->koor;

  if (!obj->clip3D)
  {
    for (i = obj->polygons + 1; --i;)
    {
      h = *p++;
      j = *p++;
      if (h == 2)
      {
        pv = (long*)PolyVertex;
        *pv++ = k[*p++];
        *pv++ = k[*p++];
        NewColor(*p++);
        MoveTo(PolyVertex[0], PolyVertex[1]);
        DrawTo(PolyVertex[2], PolyVertex[3]);
      }
      else
      {
        if (IsVisible(p[0], j, obj->vert))
        {
          pv = (long*)PolyVertex;
          for (j = h + 1; --j;)
            *pv++ = k[*p++];
          SolidColor(*p++);
          Polygon(h);
        }
        else
          p += h + 1;
      }
    }
  }
  else
  {
    for (i = obj->polygons + 1; --i;)
    {
      *(long*)para = *k++;
      pv = (long*)PolyVertex;
      for (j = para[0] + 1; --j;)
        *pv++ = *k++;
      if (para[0] == 2)
      {
        NewColor(para[1]);
        MoveTo(PolyVertex[0], PolyVertex[1]);
        DrawTo(PolyVertex[2], PolyVertex[3]);
      }
      else
      {
        SolidColor(para[1]);
        Polygon(para[0]);
      }
    }
  }
}

void
PrintCar(s_object* obj)
{
  unsigned char* p;
  unsigned char i;
  unsigned char j;
  unsigned char h;
  long* pv;
  long* k;
  char PhantomPhoton;
  int para[2];

  // CHECK, IF CAR IS ALREADY DISPLAYED (LOOP).
  // IF NOT, MARK AS DISPLAYED.

  if (!obj->visible)
    return;
  obj->visible = FALSE;

  PhantomPhoton = (obj->color == hatchedblue);

  p = obj->faces;
  k = (long*)obj->koor;

  if (!obj->clip3D)
  {
    for (i = obj->polygons + 1; --i;)
    {
      h = *p++;
      j = *p++;
      if (IsVisible(p[0], j, obj->vert))
      {
        pv = (long*)PolyVertex;
        for (j = h + 1; --j;)
          *pv++ = k[*p++];
        j = *p++;

        if (j == darkred)
        {
          if (obj->message)
            j = red;
        }
        else
        {
          if (blue1 <= j && j <= blue4)
          {
            if (PhantomPhoton)
              j = hatchedblue;
            else
              j += obj->color - blue1;
          }
        }
        SolidColor(j);
        Polygon(h);
      }
      else
        p += h + 1;
    }
  }
  else
  {
    for (i = obj->polygons + 1; --i;)
    {
      *(long*)para = *k++;

      pv = (long*)PolyVertex;
      for (j = para[0] + 1; --j;)
        *pv++ = *k++;
      j = para[1];

      if (j == darkred)
      {
        if (obj->message)
          j = red;
      }
      else
      {
        if (blue1 <= j && j <= blue4)
        {
          if (PhantomPhoton)
            j = hatchedblue;
          else
            j += obj->color - blue1;
        }
      }
      SolidColor(j);
      Polygon(para[0]);
    }
  }
}

void
PrintUnderpass(s_object* obj)
{
  unsigned char* p;
  unsigned char i;
  unsigned char j;
  unsigned char h;
  long* pv;
  long* k;
  int para[2];

  p = obj->faces;
  k = (long*)obj->koor;

  if (!obj->clip3D)
  {
    for (i = obj->polygons + 1; --i;)
    {
      h = *p++;
      j = *p++;
      if (IsVisible(p[0], j, obj->vert))
      {
        pv = (long*)PolyVertex;
        for (j = h + 1; --j;)
          *pv++ = k[*p++];
        j = *p++;
        if (j != transparent)
        {
          SolidColor(j);
          Polygon(h);
        }
      }
      else
        p += h + 1;
    }
  }
  else
  {
    for (i = obj->polygons + 1; --i;)
    {
      *(long*)para = *k++;

      pv = (long*)PolyVertex;
      for (j = para[0] + 1; --j;)
        *pv++ = *k++;
      if (para[1] != transparent)
      {
        SolidColor(para[1]);
        Polygon(para[0]);
      }
    }
  }
}

void
PrintObject(s_object* obj)
{
  unsigned char* p;
  unsigned char i;
  unsigned char j;
  unsigned char h;
  long* pv;
  long* k;
  int para[2];

  p = obj->faces;
  k = (long*)obj->koor;

  if (!obj->clip3D)
  {
    for (i = obj->polygons + 1; --i;)
    {
      h = *p++;
      j = *p++;
      if (IsVisible(p[0], j, obj->vert))
      {
        pv = (long*)PolyVertex;
        for (j = h + 1; --j;)
          *pv++ = k[*p++];
        SolidColor(*p++);
        Polygon(h);
      }
      else
        p += h + 1;
    }
  }
  else
  {
    for (i = obj->polygons + 1; --i;)
    {
      *(long*)para = *k++;

      pv = (long*)PolyVertex;
      for (j = para[0] + 1; --j;)
        *pv++ = *k++;
      SolidColor(para[1]);
      Polygon(para[0]);
    }
  }
}

void
PrintBrokenWindow(void)
{

  NewColor(white);

#define MoveP(n) MoveTo(Debris[n + n - 2], Debris[n + n - 1]);
#define DrawP(n) DrawTo(Debris[n + n - 2], Debris[n + n - 1]);

  // CENTER

  MoveP(1) DrawP(2) DrawP(3) DrawP(5)
    DrawP(6) DrawP(1)

    // RIGHT UP

    MoveP(6) DrawP(11) MoveP(5) DrawP(12)
      MoveP(7) DrawP(8) MoveP(9) DrawP(10)

    // RIGHT

    MoveP(4) DrawP(13) DrawP(14) DrawP(15)

    // RIGHT DOWN

    MoveP(3) DrawP(19) MoveP(16) DrawP(20)
      MoveP(17) DrawP(18)

    // LEFT DOWN

    MoveP(2) DrawP(21)

    // LEFT UP

    MoveP(24) DrawP(1) DrawP(25)
      MoveP(22) DrawP(23)

#undef MoveP
#undef DrawP
}

static void
PrintHillRoads(int type)
{
  char i;
  char j;
  int h;
  int* flag;
  int* p;
  int* l;
  long* d;

  p = coordinate;

  l = LaneM1;
  flag = Flags1;

  for (i = 1; i < VisiStr1; i++)
  {
    h = *flag++;
    if (!(h & f_invisible) && (h & type))
    { // DISPLAY LOW STREET SEGMENT.

      SolidColor((h & f_color) ? grey2 : grey12);
      h = *p++;
      d = (long*)PolyVertex;
      for (j = (unsigned char)h + 1; --j;)
      {
        // FIXME: Fix this pre-ANSI line of C code!
        //*d++ = *((long*)p)++;
      }
      Polygon(h);

      // DISPLAY LANE MARKER

      if (i <= VisiLns1)
      {
        SolidColor(orange);
        h = *l++;
        d = (long*)PolyVertex;
        for (j = (unsigned char)h + 1; --j;)
        {
          // FIXME: Fix this pre-ANSI line of C code!
          //*d++ = *((long*)l)++;
        }
        Polygon(h);
      }
    }
    else
    {
      p += (p[0] << 1) + 1;
      if (i <= VisiLns1)
        l += (l[0] << 1) + 1;
    }
  }

  l = LaneM2;
  flag = Flags2;

  for (i = 1; i < VisiStr2; i++)
  {
    h = *flag++;
    if (!(h & f_invisible) && (h & type))
    { // DISPLAY LOW STREET SEGMENT.

      SolidColor((h & f_color) ? grey2 : grey12);
      h = *p++;
      d = (long*)PolyVertex;
      for (j = (unsigned char)h + 1; --j;)
      {
        // FIXME: Fix this pre-ANSI line of C code!
        //*d++ = *((long*)p)++;
      }
      Polygon(h);

      // DISPLAY LANE MARKER

      if (i <= VisiLns2)
      {
        SolidColor(orange);
        h = *l++;
        d = (long*)PolyVertex;
        for (j = (unsigned char)h + 1; --j;)
        {
          // FIXME: Fix this pre-ANSI line of C code!
          //*d++ = *((long*)l)++;
        }
        Polygon(h);
      }
    }
    else
    {
      p += (p[0] << 1) + 1;
      if (i <= VisiLns2)
        l += (l[0] << 1) + 1;
    }
  }
}

static void
PrintLowStreets(void)
{
  char i;
  char j;
  int h;
  int* flag;
  int* p;
  int* l;
  long* d;

#define wire FALSE

  p = coordinate;

#if (wire)
  NewColor(white);
#endif

  l = LaneM1;
  flag = Flags1;

#define mask (f_hill1 + f_hill2 + f_hill3 + f_hill4 + f_invisible)

  for (i = 1; i < VisiStr1; i++)
  {
    h = *flag++;
    if (!(h & mask))
    { // DISPLAY LOW STREET SEGMENT.

      SolidColor((h & f_color) ? grey2 : grey12);
      h = *p++;
      d = (long*)PolyVertex;
      for (j = (unsigned char)h + 1; --j;)
      {
        // FIXME: Fix this pre-ANSI line of C code!
        //*d++ = *((long*)p)++;
      }
#if (wire)
      FramePoly(h);
#else
      Polygon(h);
#endif

      // DISPLAY LANE MARKER

      if (i <= VisiLns1)
      {
        SolidColor(orange);
        h = *l++;
        d = (long*)PolyVertex;
        for (j = (unsigned char)h + 1; --j;)
        {
          // FIXME: Fix this pre-ANSI line of C code!
          //*d++ = *((long*)l)++;
        }
#if (wire)
        FramePoly(h);
#else
        Polygon(h);
#endif
      }
    }
    else
    {
      p += (p[0] << 1) + 1;
      if (i <= VisiLns1)
        l += (l[0] << 1) + 1;
    }
  }

  l = LaneM2;
  flag = Flags2;

  for (i = 1; i < VisiStr2; i++)
  {
    h = *flag++;
    if (!(h & mask))
    { // DISPLAY LOW STREET SEGMENT.

      SolidColor((h & f_color) ? grey2 : grey12);
      h = *p++;
      d = (long*)PolyVertex;
      for (j = (unsigned char)h + 1; --j;)
      {
        // FIXME: Fix this pre-ANSI line of C code!
        //*d++ = *((long*)p)++;
      }
#if (wire)
      FramePoly(h);
#else
      Polygon(h);
#endif

      // DISPLAY LANE MARKER

      if (i <= VisiLns2)
      {
        SolidColor(orange);
        h = *l++;
        d = (long*)PolyVertex;
        for (j = (unsigned char)h + 1; --j;)
        {
          // FIXME: Fix this pre-ANSI line of C code!
          //*d++ = *((long*)l)++;
        }
#if (wire)
        FramePoly(h);
#else
        Polygon(h);
#endif
      }
    }
    else
    {
      p += (p[0] << 1) + 1;
      if (i <= VisiLns2)
        l += (l[0] << 1) + 1;
    }
  }

#undef mask
}

static void
CarOnRoad(void)
{
  s_object* obj;
  s_priority* p;
  unsigned char i;
  unsigned char hills;

  //	DISPLAY PRIORITY :
  //      ==================

  // DISPLAY ALL FIELDS.

  obj = field;
  for (i = NumOfFields + 1; --i;)
  {
    if (obj->visible)
    {
      (obj->DisplayProc)(obj);
    }
    obj++;
  }

  // DISPLAY ALL DIPs AND CHECK FOR HILLS.

  p = PrioList;
  hills = FALSE;

  for (i = VisibleObjects + 1; --i;)
  {
    obj = (p++)->obj;
    if (obj->large == 57) // DIP
    {
      (obj->DisplayProc)(obj);
      obj->visible = FALSE;
    }
    else
    {
      if (obj->class == c_hill)
        hills = TRUE;
    }
  }

  // DISPLAY ALL CARS IN DIPs.

  p = PrioList;
  for (i = VisibleObjects + 1; --i;)
  {
    obj = (p++)->obj;
    if (obj->class == c_car)
    {
      if (obj->worldY < -5)
      {
        PrintCar(obj);
        obj->visible = FALSE;
      }
    }
  }

  // DISPLAY ALL LOW STREET ELEMENTS.

  PrintLowStreets();

  // DISPLAY ALL OBJECTS ON TOP OF THE HILLS.

  if (hills)
  {
    p = PrioList;
    for (i = VisibleObjects + 1; --i;)
    {
      obj = (p++)->obj;
      if (obj->worldY >= 200)
      {
        if (obj->eyeY > 0)
        {
          (obj->DisplayProc)(obj);
          obj->visible = FALSE;
        }
      }
    }
  }

  p = PrioList;
  for (i = VisibleObjects + 1; --i;)
  {
    obj = (p++)->obj;
    if (obj->visible)
    {
      (obj->DisplayProc)(obj);
      obj->visible = FALSE;
      if (obj->class == c_hill)
      {
        switch (obj->large)
        {
          case 8:
            PrintHillRoads(f_hill1);
            break;

          case 9:
            PrintHillRoads(f_hill2);
            break;

          case 48:
            PrintHillRoads(f_hill4);
            break;

          case 61:
            PrintHillRoads(f_hill3);
            break;
        }
      }
    }
  }
}

static void
CarInDip(void)
{
  s_object* obj;
  s_priority* p;
  unsigned char i;

  //	DISPLAY PRIORITY :
  //      ==================

  // DISPLAY ALL OBJECTS EXCEPT DIPs AND CARs IN DIPs.

  p = PrioList;
  for (i = VisibleObjects + 1; --i;)
  {
    obj = (p++)->obj;
    if (obj->large != 57) // DIP
    {
      if (obj->class != c_car)
      {
        (obj->DisplayProc)(obj);
        obj->visible = FALSE;
      }
      else if (obj->worldY >= 0)
      {
        (obj->DisplayProc)(obj);
        obj->visible = FALSE;
      }
    }
  }

  // DISPLAY ALL DIPs.

  p = PrioList;
  for (i = VisibleObjects + 1; --i;)
  {
    obj = (p++)->obj;
    if (obj->visible && (obj->large == 57)) // DIP
    {
      (obj->DisplayProc)(obj);
      obj->visible = FALSE;
    }
  }

  // DISPLAY ALL CARs IN DIPs.

  p = PrioList;
  for (i = VisibleObjects + 1; --i;)
  {
    obj = (p++)->obj;
    if (obj->visible && (obj->class == c_car))
    {
      (obj->DisplayProc)(obj);
    }
  }
}

void
PrintAllObjects(void)
{
  if (car.y < 0)
    CarInDip();
  else
    CarOnRoad();
}
