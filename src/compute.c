#include <stdio.h>

#include "main.h"
#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	compute.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes computational functions.

 DATE OF CREATION.....	March 30th 1989
 LAST CHANGE..........	August 22nd 1990
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 2.0 for PCompatibles

----------------------------------------------------------------------

*/

//		LOCAL VARIABLES :
//              =================

#define NormalLength 2048
#define ClipZ 8

static int ClipVert[3 * MaxVertex];  // 3D clipping coordinates.
static int MidPts1[3 * MaxSegments]; // mid points on track 1.
static int MidPts2[3 * MaxSegments]; // mid points on track 2.
static int Coll_Height;              // Height of the collision coordinate.
static int Coll_Visibility;          // Visibility of the collision face.

// FUNCTIONS :
//              ===========

static void
CompCoordinates(s_object*);
static unsigned char ClipProjectPoly(unsigned char, unsigned char, s_object*);
static void ClipCoordinates(s_object*);
static int* ClipStreet(int*, int*, int);
static void MakeStreetElements(void);
static void StreetProject(void);
static void CompTrack(void);
static void CompFields(void);

unsigned int
Sqrt(unsigned long n)
{
  unsigned long s1;
  unsigned long s2;
  unsigned long s3;

  if (n <= 0)
    return (0);

  s3 = s2 = n / 5000L + 8L;

  for (;;)
  {
    s1 = (n / s2 + s2) >> 1;

    if (s1 == s2 || s1 == s3)
      return ((unsigned int)s1);

    s3 = s2;
    s2 = s1;
  }
}

void
CompNormal(int* p1, int* p2, int* p3, int* normal)
{
  long x2;
  long y2;
  long z2;
  long x3;
  long y3;
  long z3;
  long nx;
  long ny;
  long nz;
  unsigned int len;

  x2 = p2[0] - p1[0];
  y2 = p2[1] - p1[1];
  z2 = p2[2] - p1[2];

  x3 = p3[0] - p1[0];
  y3 = p3[1] - p1[1];
  z3 = p3[2] - p1[2];

  nx = y2 * z3 - y3 * z2;
  ny = -x2 * z3 + x3 * z2;
  nz = x2 * y3 - x3 * y2;

  len = Sqrt(nx * nx + ny * ny + nz * nz);

  if (len > 0)
  {
    *normal++ = (int)((NormalLength * nx) / len);
    *normal++ = (int)((NormalLength * ny) / len);
    *normal++ = (int)((NormalLength * nz) / len);
  }
  else
    SystemExit("Normal vector error !");
}

int
CompRadius(s_object* obj)
{
  int i;
  int* p;
  long quad;
  long h;

  quad = 0;
  p = obj->model;

  for (i = obj->vertices + 1; --i;)
  {
    h = mult(p[0], p[0]) + mult(p[1], p[1]) + mult(p[2], p[2]);
    if (h > quad)
      quad = h;
    p += 3;
  }

  return (Sqrt(quad) + 60);
}

void
CompVertices(s_object* obj)
{
  int i;
  int h;
  int sin;
  int cos;
  int* s;
  int* d;
  int count;

  s = obj->model;
  d = obj->vert = VertIndex;

  count = obj->vertices + obj->normals;

  if (!obj->pre_rotated)
  {
    if (obj->roll)
    { // IF NECESSARY, ROTATE AROUND OBJECT Z-AXIS.

      sin = sinus(obj->roll);
      cos = cosinus(obj->roll);
      RotXY(count, cos, sin, (int*)d, s);
      s = d = obj->vert;
    }

    if (obj->pitch)
    { // IF NECESSARY, ROTATE AROUND OBJECT X-AXIS.

      sin = sinus(obj->pitch);
      cos = cosinus(obj->pitch);
      RotYZ(count, cos, sin, (int*)d, s);
      s = d = obj->vert;
    }

    if (obj->yaw)
    { // IF NECESSARY, ROTATE AROUND OBJECT Y-AXIS.

      sin = sinus(obj->yaw);
      cos = cosinus(obj->yaw);
      RotXZ(count, cos, sin, (int*)d, s);
      s = d = obj->vert;
    }
  }

  // GET ORIGIN OF THE OBJECT, IN EYE COORDINATE SYSTEM.

  h = obj->eyeX;
  sin = obj->eyeY;
  cos = obj->eyeZ;

  // TRANSLATE TO THE NEW ORIGIN.

  if (obj->pre_rotated || !(obj->yaw | obj->pitch))
  {
    for (i = count + 1; --i;)
    {
      *d++ = *s++ + h;
      *d++ = *s++ + sin;
      *d++ = *s++ + cos;
    }
  }
  else
  {
    for (i = count + 1; --i;)
    {
      *d++ += h;
      *d++ += sin;
      *d++ += cos;
    }
  }

  VertIndex = d;

  h = obj->eyeX;
  sin = obj->eyeY;
  cos = obj->eyeZ;

  if (car.yaw)
  { // ROTATE AROUND VIEWPOINT Y-AXIS.

    RotXZ(count, yawCOS, yawSIN, obj->vert, obj->vert);

    // ROTATE OBJECT ORIGIN AROUND VIEWPOINT Y-AXIS.

    h = rot1(obj->eyeX, cos, yawSIN, yawCOS);
    cos = rot2(obj->eyeX, cos, yawSIN, yawCOS);
  }

  if (car.pitch)
  { // ROTATE AROUND VIEWPOINT X-AXIS.

    RotYZ(count, pitchCOS, pitchSIN, obj->vert, obj->vert);

    // ROTATE OBJECT ORIGIN AROUND VIEWPOINT X-AXIS.

    sin = rot1(obj->eyeY, cos, pitchSIN, pitchCOS);
    cos = rot2(obj->eyeY, cos, pitchSIN, pitchCOS);
  }

  if (car.roll)
  { // ROTATE AROUND VIEWPOINT Z-AXIS.

    RotXY(count, rollCOS, rollSIN, obj->vert, obj->vert);

    // ROTATE OBJECT ORIGIN AROUND VIEWPOINT Z-AXIS.

    i = h;
    h = rot1(i, sin, rollSIN, rollCOS);
    sin = rot2(i, sin, rollSIN, rollCOS);
  }

  // SUBTRACT ROTATED ORIGIN FROM NORMAL VECTORS.

  i = obj->vertices;
  d = &obj->vert[i + i + i];

  for (i = obj->normals + 1; --i;)
  {
    *d++ -= h;
    *d++ -= sin;
    *d++ -= cos;
  }
}

static void
CompCoordinates(s_object* obj)
{

  obj->koor = CoorIndex;
  CoorIndex = ProjectSmall(obj->vertices, obj->koor, obj->vert);
}

static unsigned char
ClipProjectPoly(unsigned char n, unsigned char visible, s_object* obj)
{
  int h;
  int c;
  int test1;
  int test2;
  int* v1;
  int* v2;
  int x;
  int y;
  int z;
  int dx;
  int dy;
  int dz;
  int k;
  int auxbuf[3 * MaxVertex];
  char CopyFlag;

  CopyFlag = FALSE;

  // CLIP TO VIEWANGLE IN X DIMENSION.

  v1 = ClipVert;
  v2 = auxbuf;

  test2 = ((v1[2] - ClipZ) << 1) > v1[0];

  for (h = 0, ++n; --n;)
  {
    test1 = test2;
    test2 = ((v1[5] - ClipZ) << 1) > v1[3];

    if (test1)
    {
      *v2++ = v1[0];
      *v2++ = v1[1];
      *v2++ = v1[2];
      h++;
    }

    if (test1 ^ test2)
    {
      if (v1[5] < v1[2])
      {
        dx = v1[0] - v1[3];
        dy = v1[1] - v1[4];
        dz = v1[2] - v1[5];
        x = v1[3];
        y = v1[4];
        z = v1[5] - ClipZ;
      }
      else
      {
        dx = v1[3] - v1[0];
        dy = v1[4] - v1[1];
        dz = v1[5] - v1[2];
        x = v1[0];
        y = v1[1];
        z = v1[2] - ClipZ;
      }

      c = z + z - x;
      k = dx - (dz + dz);
      z += multdiv(dz, c, k);
      y += multdiv(dy, c, k);
      *v2++ = z << 1;
      *v2++ = y;
      *v2++ = z + ClipZ;
      h++;
    }

    v1 += 3;
  }

  *v2++ = auxbuf[0];
  *v2++ = auxbuf[1];
  *v2++ = auxbuf[2];

  v1 = auxbuf;
  v2 = ClipVert;

  test2 = ((v1[2] - ClipZ) << 1) > -v1[0];

  for (n = 0, ++h; --h;)
  {
    test1 = test2;
    test2 = ((v1[5] - ClipZ) << 1) > -v1[3];

    if (test1)
    {
      *v2++ = v1[0];
      *v2++ = v1[1];
      *v2++ = v1[2];
      n++;
    }

    if (test1 ^ test2)
    {
      if (v1[5] < v1[2])
      {
        dx = v1[0] - v1[3];
        dy = v1[1] - v1[4];
        dz = v1[2] - v1[5];
        x = v1[3];
        y = v1[4];
        z = v1[5] - ClipZ;
      }
      else
      {
        dx = v1[3] - v1[0];
        dy = v1[4] - v1[1];
        dz = v1[5] - v1[2];
        x = v1[0];
        y = v1[1];
        z = v1[2] - ClipZ;
      }

      c = x + (z + z);
      k = dx + (dz + dz);
      z -= multdiv(dz, c, k);
      y -= multdiv(dy, c, k);
      x = -z << 1;
      *v2++ = x;
      *v2++ = y;
      *v2++ = z + ClipZ;
      n++;

      if (!x) // VIEWPOINT DIRECTLY ON POLYGON !
      {
        if (obj->class & (c_hill + c_chicane))
        {
          if ((y < 5 * 5) && !CheckOffRoad)
          {
            CopyFlag = TRUE;
            OnObject = TRUE;
          }
        }

        if (obj->class & (c_building + c_chicane))
        {
          if (y > -5 * 2)
          {
            if (y < Coll_Height)
            {
              if (obj->large == 10) // BARN
                visible = FALSE;
              Coll_Height = y;
              Coll_Visibility = visible;
            }
            else // SPECIAL CASE: CROSS OVER
            {
              if (visible && (y < Coll_Height + 5))
              {
                Coll_Height = y;
                Coll_Visibility = visible;
              }
            }
          }
        }
      }
    }

    v1 += 3;
  }

  // CLIP TO VIEWANGLE IN Y DIMENSION.

  if (ExtendedClip)
  {
    *v2++ = ClipVert[0];
    *v2++ = ClipVert[1];
    *v2++ = ClipVert[2];

    v1 = ClipVert;
    v2 = auxbuf;

    test2 = (v1[2] - ClipZ) > v1[1];

    for (h = 0, ++n; --n;)
    {
      test1 = test2;
      test2 = (v1[5] - ClipZ) > v1[4];

      if (test1)
      {
        *v2++ = v1[0];
        *v2++ = v1[1];
        *v2++ = v1[2];
        h++;
      }

      if (test1 ^ test2)
      {
        if (v1[5] < v1[2])
        {
          dx = v1[0] - v1[3];
          dy = v1[1] - v1[4];
          dz = v1[2] - v1[5];
          x = v1[3];
          y = v1[4];
          z = v1[5] - ClipZ;
        }
        else
        {
          dx = v1[3] - v1[0];
          dy = v1[4] - v1[1];
          dz = v1[5] - v1[2];
          x = v1[0];
          y = v1[1];
          z = v1[2] - ClipZ;
        }

        c = z - y;
        k = dy - dz;
        x += multdiv(dx, c, k);
        z += multdiv(dz, c, k);
        *v2++ = x;
        *v2++ = z;
        *v2++ = z + ClipZ;
        h++;
      }

      v1 += 3;
    }

    *v2++ = auxbuf[0];
    *v2++ = auxbuf[1];
    *v2++ = auxbuf[2];

    v1 = auxbuf;
    v2 = ClipVert;

    test2 = (v1[2] - ClipZ) > -v1[1];

    for (n = 0, ++h; --h;)
    {
      test1 = test2;
      test2 = (v1[5] - ClipZ) > -v1[4];

      if (test1)
      {
        *v2++ = v1[0];
        *v2++ = v1[1];
        *v2++ = v1[2];
        n++;
      }

      if (test1 ^ test2)
      {
        if (v1[5] < v1[2])
        {
          dx = v1[0] - v1[3];
          dy = v1[1] - v1[4];
          dz = v1[2] - v1[5];
          x = v1[3];
          y = v1[4];
          z = v1[5] - ClipZ;
        }
        else
        {
          dx = v1[3] - v1[0];
          dy = v1[4] - v1[1];
          dz = v1[5] - v1[2];
          x = v1[0];
          y = v1[1];
          z = v1[2] - ClipZ;
        }

        c = y + z;
        k = dy + dz;
        x -= multdiv(dx, c, k);
        z -= multdiv(dz, c, k);
        *v2++ = x;
        *v2++ = -z;
        *v2++ = z + ClipZ;
        n++;
      }

      v1 += 3;
    }
  }

  if (CopyFlag)
  { // SAVE SURFACE VECTORS FOR USE IN SIMULATION MODEL.

    ObjFace[0] = ClipVert[0];
    ObjFace[1] = ClipVert[1];
    ObjFace[2] = ClipVert[2];
    ObjFace[3] = ClipVert[3];
    ObjFace[4] = ClipVert[4];
    ObjFace[5] = ClipVert[5];
    ObjFace[6] = ClipVert[6];
    ObjFace[7] = ClipVert[7];
    ObjFace[8] = ClipVert[8];
  }

  return (n);
}

static void
ClipCoordinates(s_object* obj)
{
  unsigned char* p;
  int j;
  int i;
  int h;
  int z;
  int* d;
  int* s;
  int* coor;
  unsigned char edges;
  unsigned char normal;
  unsigned char NoSign;
  unsigned char visible;
  int minZ;
  int maxZ;

  obj->koor = coor = CoorIndex;
  NoSign = !(obj->class & (c_sign + c_loop));

  p = obj->faces;
  i = obj->polygons;

  Coll_Height = MaxInt;
  Coll_Visibility = TRUE;

  for (++i; --i;)
  {
    edges = *p++;
    normal = *p++;
    visible = TRUE;

    // IS THE POLYGON INVISIBLE ?

    if (NoSign && (edges > 2))
      visible = IsVisible((int)p[0], normal, obj->vert);

    d = ClipVert;
    minZ = MaxInt;
    maxZ = -MaxInt;

    // COPY ALL VECTORS OF THE POLYGON TO AN ARRAY.

    for (j = edges + 1; --j;)
    {
      h = *p++;
      s = obj->vert + (h + h + h);
      *d++ = *s++;
      *d++ = *s++;
      *d++ = z = *s++;
      if (z > maxZ)
        maxZ = z;
      if (z < minZ)
        minZ = z;
    }

    // IS THE POLYGON BEHIND THE VIEWPOINT ?

    if (maxZ < 0)
    {
      *coor++ = 0;
      *coor++ = *p++;
      continue;
    }

    // CLIP AND PROJECT THE POLYGON.

    if (minZ < 100)
    {
      *d++ = ClipVert[0];
      *d++ = ClipVert[1];
      *d++ = ClipVert[2];
      edges = ClipProjectPoly(edges, visible, obj);
    }

    if (visible)
    {
      *coor++ = edges;
      *coor++ = *p++;
      coor = ProjectSmall((int)edges, coor, ClipVert);
    }
    else
    {
      *coor++ = 0;
      *coor++ = *p++;
    }
  }

  if (!Coll_Visibility)
  { // THE VIEWPOINT IS INSIDE THE OBJECT IF THE LOWEST
    // COLLISION FACE IS INVISIBLE.

    obj->collision = TRUE;
    CollisionFlag = TRUE;
  }

  CoorIndex = coor;
}

static int*
ClipStreet(int* vert,   // pointer to 3D coordinates
           int* coor,   // pointer to 2D coordinates
           int distance // distance of midpoint
)
{
  unsigned char h;
  unsigned char n;
  int test1;
  int test2;
  int c;
  int* v1;
  int* v2;
  int x;
  int y;
  int z;
  int k;
  int dx;
  int dy;
  int dz;
  int auxbuf[3 * MaxVertex];

  // RETURN, IF STREET ELEMENT IS BEHIND VIEWPOINT.

  if (distance < -RoadLength)
  {
    *coor++ = 0;
    return (coor);
  }

  // PROJECT AND RETURN, IF STREET ELEMENT IS FAR AWAY.

  if (distance > (3 * RoadLength / 2))
  {
    *coor++ = 4;
    return (ProjectSmall(4, coor, vert));
  }

  // CLIP AND PROJECT STREET ELEMENT.

  for (v2 = ClipVert, n = 4 + 1; --n;)
  {
    *v2++ = *vert++;
    *v2++ = *vert++;
    *v2++ = *vert++;
  }
  *v2++ = ClipVert[0];
  *v2++ = ClipVert[1];
  *v2++ = ClipVert[2];

  n = 4;
  v1 = ClipVert;
  v2 = auxbuf;

  test2 = ((v1[2] - ClipZ) << 1) > v1[0];

  for (h = 0, ++n; --n;)
  {
    test1 = test2;
    test2 = ((v1[5] - ClipZ) << 1) > v1[3];

    if (test1)
    {
      *v2++ = v1[0];
      *v2++ = v1[1];
      *v2++ = v1[2];
      h++;
    }

    if (test1 ^ test2)
    {
      if (v1[5] < v1[2])
      {
        dx = v1[0] - v1[3];
        dy = v1[1] - v1[4];
        dz = v1[2] - v1[5];
        x = v1[3];
        y = v1[4];
        z = v1[5] - ClipZ;
      }
      else
      {
        dx = v1[3] - v1[0];
        dy = v1[4] - v1[1];
        dz = v1[5] - v1[2];
        x = v1[0];
        y = v1[1];
        z = v1[2] - ClipZ;
      }

      c = z + z - x;
      k = dx - (dz + dz);
      z += multdiv(dz, c, k);
      y += multdiv(dy, c, k);
      *v2++ = z << 1;
      *v2++ = y;
      *v2++ = z + ClipZ;
      h++;
    }

    v1 += 3;
  }

  *v2++ = auxbuf[0];
  *v2++ = auxbuf[1];
  *v2++ = auxbuf[2];

  v1 = auxbuf;
  v2 = ClipVert;

  test2 = ((v1[2] - ClipZ) << 1) > -v1[0];

  for (n = 0, ++h; --h;)
  {
    test1 = test2;
    test2 = ((v1[5] - ClipZ) << 1) > -v1[3];

    if (test1)
    {
      *v2++ = v1[0];
      *v2++ = v1[1];
      *v2++ = v1[2];
      n++;
    }

    if (test1 ^ test2)
    {
      if (v1[5] < v1[2])
      {
        dx = v1[0] - v1[3];
        dy = v1[1] - v1[4];
        dz = v1[2] - v1[5];
        x = v1[3];
        y = v1[4];
        z = v1[5] - ClipZ;
      }
      else
      {
        dx = v1[3] - v1[0];
        dy = v1[4] - v1[1];
        dz = v1[5] - v1[2];
        x = v1[0];
        y = v1[1];
        z = v1[2] - ClipZ;
      }

      c = x + (z + z);
      k = dx + (dz + dz);
      z -= multdiv(dz, c, k);
      y -= multdiv(dy, c, k);
      x = -z << 1;
      *v2++ = x;
      *v2++ = y;
      *v2++ = z + ClipZ;
      n++;

      if (!x)
      { // CAR IS ON THE ROAD

        if (y < 0)
        {
          CheckOffRoad = TRUE;
        }
      }
    }

    v1 += 3;
  }

  // CLIP TO VIEWING ANGLE IN Y - DIRECTION.

  if (ExtendedClip)
  {
    *v2++ = ClipVert[0];
    *v2++ = ClipVert[1];
    *v2++ = ClipVert[2];

    v1 = ClipVert;
    v2 = auxbuf;

    test2 = (v1[2] - ClipZ) > v1[1];

    for (h = 0, ++n; --n;)
    {
      test1 = test2;
      test2 = (v1[5] - ClipZ) > v1[4];

      if (test1)
      {
        *v2++ = v1[0];
        *v2++ = v1[1];
        *v2++ = v1[2];
        h++;
      }

      if (test1 ^ test2)
      {
        if (v1[5] < v1[2])
        {
          dx = v1[0] - v1[3];
          dy = v1[1] - v1[4];
          dz = v1[2] - v1[5];
          x = v1[3];
          y = v1[4];
          z = v1[5] - ClipZ;
        }
        else
        {
          dx = v1[3] - v1[0];
          dy = v1[4] - v1[1];
          dz = v1[5] - v1[2];
          x = v1[0];
          y = v1[1];
          z = v1[2] - ClipZ;
        }

        c = z - y;
        x += multdiv(dx, c, dy - dz);
        y += multdiv(dy, c, dy - dz);
        *v2++ = x;
        *v2++ = y;
        *v2++ = y + ClipZ;
        h++;
      }

      v1 += 3;
    }

    *v2++ = auxbuf[0];
    *v2++ = auxbuf[1];
    *v2++ = auxbuf[2];

    v1 = auxbuf;
    v2 = ClipVert;

    test2 = (v1[2] - ClipZ) > -v1[1];

    for (n = 0, ++h; --h;)
    {
      test1 = test2;
      test2 = (v1[5] - ClipZ) > -v1[4];

      if (test1)
      {
        *v2++ = v1[0];
        *v2++ = v1[1];
        *v2++ = v1[2];
        n++;
      }

      if (test1 ^ test2)
      {
        if (v1[5] < v1[2])
        {
          dx = v1[0] - v1[3];
          dy = v1[1] - v1[4];
          dz = v1[2] - v1[5];
          x = v1[3];
          y = v1[4];
          z = v1[5] - ClipZ;
        }
        else
        {
          dx = v1[3] - v1[0];
          dy = v1[4] - v1[1];
          dz = v1[5] - v1[2];
          x = v1[0];
          y = v1[1];
          z = v1[2] - ClipZ;
        }

        c = y + z;
        x -= multdiv(dx, c, dy + dz);
        y -= multdiv(dy, c, dy + dz);
        *v2++ = x;
        *v2++ = y;
        *v2++ = ClipZ - y;
        n++;
      }

      v1 += 3;
    }
  }

  *coor++ = (int)n;
  coor = ProjectSmall((int)n, coor, ClipVert);

  return (coor);
}

static void
MakeStreetElements(void)
{
  unsigned char i;
  int* flag;
  int* p;
  int* d;
  int* mid;
  int dx;
  int dy;
  int dz;
  unsigned char flip;
  int* lanes1;
  int* lanes2;

  d = VertIndex;
  flip = 0;

  // GENERATE STREET ELEMENTS OF TRACK 1.

  p = First1;
  mid = MidPts1;
  flag = Flags1;
  lanes1 = d;

  for (i = VisiStr1 + 1; --i;)
  {
    if (flip ^= TRUE)
    {
      *d++ = p[0];
      *d++ = p[1];
      *d++ = p[2];

      *d++ = p[3];
      *d++ = p[4];
      *d++ = p[5];
    }
    else
    {
      *d++ = p[3];
      *d++ = p[4];
      *d++ = p[5];

      *d++ = p[0];
      *d++ = p[1];
      *d++ = p[2];
    }

    *mid++ = (p[0] + p[9]) >> 1;
    *mid++ = (p[1] + p[10]) >> 1;
    *mid++ = (p[2] + p[11]) >> 1;

    p += 2 * 3;
  }

  // GENERATE STREET ELEMENTS OF TRACK 2.

  p = First2;
  mid = MidPts2;
  lanes2 = d;

  for (i = VisiStr2 + 1; --i;)
  {
    if (flip ^= TRUE)
    {
      *d++ = p[0];
      *d++ = p[1];
      *d++ = p[2];

      *d++ = p[3];
      *d++ = p[4];
      *d++ = p[5];
    }
    else
    {
      *d++ = p[3];
      *d++ = p[4];
      *d++ = p[5];

      *d++ = p[0];
      *d++ = p[1];
      *d++ = p[2];
    }

    *mid++ = (p[0] + p[9]) >> 1;
    *mid++ = (p[1] + p[10]) >> 1;
    *mid++ = (p[2] + p[11]) >> 1;

    p += 2 * 3;
  }

  // GENERATE LANE MARKERS OF TRACK 1.

  if (VisiStr1 > 0)
  {
    p = lanes1;
    mid = MidPts1;
    flag = Flags1;
    VisiLns1 = VisiStr1 - 1;

    if (VisiLns1 > MaxLaneMarker)
      VisiLns1 = MaxLaneMarker;

    for (p = VertIndex, i = VisiLns1 + 1; --i;)
    {
      if (*flag++ ^ f_invisible)
      {
        dx = *mid++;
        dy = *mid++;
        dz = *mid++;

        *d++ = dx + ((*p++ - dx) >> 4);
        *d++ = dy + ((*p++ - dy) >> 4);
        *d++ = dz + ((*p++ - dz) >> 4);

        *d++ = dx + ((*p++ - dx) >> 4);
        *d++ = dy + ((*p++ - dy) >> 4);
        *d++ = dz + ((*p++ - dz) >> 4);

        *d++ = dx + ((*p++ - dx) >> 4);
        *d++ = dy + ((*p++ - dy) >> 4);
        *d++ = dz + ((*p++ - dz) >> 4);

        *d++ = dx + ((*p++ - dx) >> 4);
        *d++ = dy + ((*p++ - dy) >> 4);
        *d++ = dz + ((*p++ - dz) >> 4);

        p -= 2 * 3;
      }
      else
      {
        p += 2 * 3;
        d += 4 * 3;
        mid += 3;
      }
    }
  }
  else
    VisiLns1 = 0;

  // GENERATE LANE MARKERS OF TRACK 2.

  if (VisiStr2 > 0)
  {
    p = lanes2;
    mid = MidPts2;
    flag = Flags2;
    VisiLns2 = VisiStr2 - 1;

    if (VisiLns2 > MaxLaneMarker)
      VisiLns2 = MaxLaneMarker;

    for (i = VisiLns2 + 1; --i;)
    {
      if (*flag++ ^ f_invisible)
      {
        dx = *mid++;
        dy = *mid++;
        dz = *mid++;

        *d++ = dx + ((*p++ - dx) >> 4);
        *d++ = dy + ((*p++ - dy) >> 4);
        *d++ = dz + ((*p++ - dz) >> 4);

        *d++ = dx + ((*p++ - dx) >> 4);
        *d++ = dy + ((*p++ - dy) >> 4);
        *d++ = dz + ((*p++ - dz) >> 4);

        *d++ = dx + ((*p++ - dx) >> 4);
        *d++ = dy + ((*p++ - dy) >> 4);
        *d++ = dz + ((*p++ - dz) >> 4);

        *d++ = dx + ((*p++ - dx) >> 4);
        *d++ = dy + ((*p++ - dy) >> 4);
        *d++ = dz + ((*p++ - dz) >> 4);

        p -= 2 * 3;
      }
      else
      {
        p += 2 * 3;
        d += 4 * 3;
        mid += 3;
      }
    }
  }
  else
    VisiLns2 = 0;

  VertIndex = d;
}

static void
StreetProject(void)
{
  unsigned char i;
  int* flag;
  int* d;
  int* p;
  int* mid;

  d = CoorIndex; // d POINTS TO 2D COORDINATE BUFFER
  p = vertex;    // p POINTS TO 3D VERTEX BUFFER

  // PROJECT ALL STREET ELEMENTS OF TRACK 1.

  if (VisiStr1)
  {
    mid = &MidPts1[2];
    flag = Flags1;
    for (i = VisiStr1; --i;)
    {
      if (*flag++ & f_nil)
        *d++ = 0;
      else
        d = ClipStreet(p, d, mid[0]);

      p += 2 * 3;
      mid += 1 * 3;
    }
    p += 2 * 3;
  }

  // PROJECT ALL STREET ELEMENTS OF TRACK 2.

  if (VisiStr2)
  {
    mid = &MidPts2[2];
    flag = Flags2;
    for (i = VisiStr2; --i;)
    {
      if (*flag++ & f_nil)
        *d++ = 0;
      else
        d = ClipStreet(p, d, mid[0]);

      p += 2 * 3;
      mid += 1 * 3;
    }
    p += 2 * 3;
  }

  // PROJECT LANE MARKERS OF TRACK 1.

  if (VisiLns1)
  {
    LaneM1 = d;
    flag = Flags1;
    mid = &MidPts1[2];
    for (i = VisiLns1 + 1; --i;)
    {
      if (*flag++ & f_invisible)
        *d++ = 0;
      else
        d = ClipStreet(p, d, mid[0]);

      p += 4 * 3;
      mid += 1 * 3;
    }
  }

  // PROJECT LANE MARKERS OF TRACK 2.

  if (VisiLns2)
  {
    LaneM2 = d;
    flag = Flags2;
    mid = &MidPts2[2];
    for (i = VisiLns2 + 1; --i;)
    {
      if (*flag++ & f_invisible)
        *d++ = 0;
      else
        d = ClipStreet(p, d, mid[0]);

      p += 4 * 3;
      mid += 1 * 3;
    }
  }

  CoorIndex = d;
}

static void
CompTrack(void)
{ // GENERATE STREET ELEMENT VERTICES.

  MakeStreetElements();

  // PUT ALL VISIBLE STREET ELEMENTS THROUGH PERSPECTIVE.

  StreetProject();
}

static void
CompFields(void)
{
  int i;
  s_object* obj;

  obj = field;

  for (i = NumOfFields + 1; --i;)
  {
    if (obj->visible)
    {
      CompVertices(obj);

      if (!obj->clip3D)
        CompCoordinates(obj);
      else
        ClipCoordinates(obj);
    }
    obj++;
  }
}

void
CompAllObjects(void)
{
  int i;
  s_object* obj;

  CheckOffRoad = FALSE;
  CollisionFlag = FALSE;
  OnObject = FALSE;

  CoorIndex = coordinate;
  VertIndex = vertex;

  CompTrack();
  CompFields();

  obj = object;

  for (i = NumOfObjects + 1; --i;)
  {
    if (obj->active && obj->visible)
    {
      CompVertices(obj);

      if (!obj->clip3D)
        CompCoordinates(obj);
      else
        ClipCoordinates(obj);
    }
    obj++;
  }

  // IS THE CAR STILL ON THE STREET ?

  if (!CheckOffRoad && !SloMoFlag)
  {
    if (OnObject || (car.y < 5 * 5))
    { // MakeSound( s_offroad);

      if (!OffRoadOut)
      {
        OffRoadOut = 0x1099;
      }
    }
  }
  else
    OffRoadOut = 0;
}
