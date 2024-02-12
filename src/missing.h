#pragma once

#include "struct.h"

typedef struct s_teamname s_teamname;
struct s_teamname
{
  char initials[2];
  char name[8];
};

typedef struct s_pitcher s_pitcher;
struct s_pitcher
{
  char available;
  int eraval;
  char name[30];
  char sinkhand;
};

typedef struct s_batter s_batter;
struct s_batter
{
  int avg;
  int hr;
  char misc;
  char name[30];
  char playing;
  int position;
};

typedef s_teamname TN_TYPE;
typedef s_pitcher PITCHER_TYPE;
typedef s_batter BATTER_TYPE;

#define JS_UP 0
#define JS_DOWN 0
#define JS_A 0
#define JS_START 0

static void
AddBCD(long bcd, long* value)
{
  (void)bcd;
  (void)value;
  puts("WARN: Stubbed function AddBCD()");
}

static void
AddTime(long offset, long* time)
{
  (void)offset;
  (void)time;
  puts("WARN: Stubbed function AddTime()");
}

static int
arcsin(int a, int b)
{
  (void)a;
  (void)b;
  puts("WARN: Stubbed function arcsin()");
  return 0;
}

static void
Box(int x1, int y1, int x2, int y2)
{
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
  puts("WARN: Stubbed function Box()");
}

static void
CalibrateJoystick(void)
{
  puts("WARN: Stubbed function CalibrateJoystick()");
}

static int
CheckKeyboard(void)
{
  puts("WARN: Stubbed function CheckKeyboard()");
  return 0;
}

static int
CheckSteeringKeys(void)
{
  puts("WARN: Stubbed function CheckSteeringKeys()");
  return 0;
}

static void
clear_pf1(void)
{
  puts("WARN: Stubbed function clear_pf1()");
}

static void
clear_pf2(void)
{
  puts("WARN: Stubbed function clear_pf2()");
}

static void
ClearVecs(void)
{
  puts("WARN: Stubbed function ClearVecs()");
}

static void
Convert(void* addr, int x, int y)
{
  (void)addr;
  (void)x;
  (void)y;
  puts("WARN: Stubbed function Convert()");
}

static void
Convert2(void* addr)
{
  (void)addr;
  puts("WARN: Stubbed function Convert2()");
}

static void
Convert3(void* addr)
{
  (void)addr;
  puts("WARN: Stubbed function Convert3()");
}

static void
CopyBlock(int sx1, int sy1, int sx2, int sy2, int dx1, int dy1, void* src, void* dst)
{
  (void)sx1;
  (void)sy1;
  (void)sx2;
  (void)sy2;
  (void)dx1;
  (void)dy1;
  (void)src;
  (void)dst;
  puts("WARN: Stubbed function CopyBlock()");
}

static int
cosinus(int arg)
{
  (void)arg;
  puts("WARN: Stubbed function cosinus()");
  return 0;
}

static void
DecCountDown(void)
{
  puts("WARN: Stubbed function DecCountDown()");
}

static void
DecOffRoad(void)
{
  puts("WARN: Stubbed function DecOffRoad()");
}

static void
DivZero(void)
{
  puts("WARN: Stubbed function DivZero()");
}

static void
drawscreen(long* var1, long* var2, long* var3)
{
  (void)var1;
  (void)var2;
  (void)var3;
  puts("WARN: Stubbed function drawscreen()");
}

static void
DrawTo(int x, int y)
{
  (void)x;
  (void)y;
  puts("WARN: Stubbed function DrawTo()");
}

static void
EmptyFunction(void)
{
  puts("WARN: Stubbed function EmptyFunction()");
}

static long
farcoreleft(void)
{
  puts("WARN: Stubbed function farcoreleft()");
  return 0;
}

static void
farfree(char* chunk)
{
  (void)chunk;
  puts("WARN: Stubbed function farfree()");
}

static void*
farmalloc(long size)
{
  (void)size;
  puts("WARN: Stubbed function farmalloc()");
}

static long
filelength(int handle)
{
  (void)handle;
  puts("WARN: Stubbed function filelength()");
  return 0;
}

static void
flush_mobs(void)
{
  puts("WARN: Stubbed function flush_mobs()");
}

static unsigned int
FP_OFF(char* chunk)
{
  (void)chunk;
  puts("WARN: Stubbed function FP_OFF()");
  return 0;
}

static unsigned int
FP_SEG(char* chunk)
{
  (void)chunk;
  puts("WARN: Stubbed function FP_SEG()");
  return 0;
}

typedef void (*FunctionPointer)(void);
static FunctionPointer
getvect(char var1)
{
  (void)var1;
  puts("WARN: Stubbed function getvect()");
  return 0;
}

static void
IncTime(void)
{
  puts("WARN: Stubbed function IncTime()");
}

static void
InitTune(int speed, int tune)
{
  (void)speed;
  (void)tune;
  puts("WARN: Stubbed function InitTune()");
}

static char
IsVisible(int i, int normal, int* v)
{
  (void)i;
  (void)normal;
  (void)v;
  puts("WARN: Stubbed function IsVisible()");
  return 0;
}

static void*
MK_FP(unsigned int segment, unsigned offset)
{
  (void)segment;
  (void)offset;
  puts("WARN: Stubbed function MK_FP()");
  return 0;
}

static void
movebytes(void* s, void* d, int num)
{
  (void)s;
  (void)d;
  (void)num;
  puts("WARN: Stubbed function movebytes()");
}

static void
movecolortable(void)
{
  puts("WARN: Stubbed function movecolortable()");
}

static void
MoveTo(int x, int y)
{
  (void)x;
  (void)y;
  puts("WARN: Stubbed function MoveTo()");
}

static void
movewords(void* s, void* d, int num)
{
  (void)s;
  (void)d;
  (void)num;
  puts("WARN: Stubbed function movewords()");
}

static long
mult(int a, int b)
{
  (void)a;
  (void)b;
  puts("WARN: Stubbed function mult()");
  return 0;
}

static int
multdiv(int a, int b, int c)
{
  (void)a;
  (void)b;
  (void)c;
  puts("WARN: Stubbed function multdiv()");
  return 0;
}

static void
NewColor(int color)
{
  (void)color;
  puts("WARN: Stubbed function NewColor()");
}

static int
Pal(int no, int rgb)
{
  (void)no;
  (void)rgb;
  puts("WARN: Stubbed function Pal()");
  return 0;
}

static void
PalOn(void)
{
  puts("WARN: Stubbed function PalOn()");
}

static void
Polygon(int edges)
{
  (void)edges;
  puts("WARN: Stubbed function Polygon()");
}

static void
PreShifter(s_BitImage* shape)
{
  (void)shape;
  puts("WARN: Stubbed function PreShifter()");
}

static void
PrintDigit(char digit, int x, int offset)
{
  (void)digit;
  (void)x;
  (void)offset;
  puts("WARN: Stubbed function PrintDigit()");
}

static void
print_message(int var1, int var2, char* msg)
{
  (void)var1;
  (void)var2;
  (void)msg;
  puts("WARN: Stubbed function print_message()");
}

static void
print_message_pal(int var1, int var2, char* msg, int var3)
{
  (void)var1;
  (void)var2;
  (void)msg;
  (void)var3;
  puts("WARN: Stubbed function print_message_pal()");
}

static void
PrintString(int x, int y, char* str)
{
  (void)x;
  (void)y;
  (void)str;
  puts("WARN: Stubbed function PrintString()");
}

static void
process_mobs(void)
{
  puts("WARN: Stubbed function process_mobs()");
}

static int*
ProjectSmall(int n, int* d, int* s)
{
  (void)n;
  (void)d;
  (void)s;
  puts("WARN: Stubbed function ProjectSmall()");
  return 0;
}

static void
PutBlock(char parameter[14])
{
  (void)parameter;
  puts("WARN: Stubbed function PutBlock()");
}

static void
PutShape(int* mask, int size, int offset, int height, int x, int y, int words, int shift)
{
  (void)mask;
  (void)size;
  (void)offset;
  (void)height;
  (void)x;
  (void)y;
  (void)words;
  (void)shift;
  puts("WARN: Stubbed function PutShape()");
}

static int
ratio(int a, int b, int c)
{
  (void)a;
  (void)b;
  (void)c;
  puts("WARN: Stubbed function ratio()");
  return 0;
}

static void
RealToDummy(void)
{
  puts("WARN: Stubbed function RealToDummy()");
}

static int
RefreshTune(void)
{
  puts("WARN: Stubbed function RefreshTune()");
  return 0;
}

static void
ResetKeys(void)
{
  puts("WARN: Stubbed function ResetKeys()");
}

static int
rot1(int x, int y, int SIN, int COS)
{
  (void)x;
  (void)y;
  (void)SIN;
  (void)COS;
  puts("WARN: Stubbed function rot1()");
  // return x * COS - y * SIN; //???
  return 0;
}

static int
rot2(int x, int y, int SIN, int COS)
{
  (void)x;
  (void)y;
  (void)SIN;
  (void)COS;
  puts("WARN: Stubbed function rot2()");
  // return x * SIN + y * COS; //???
  return 0;
}

static int
rotate(int a, int b)
{
  (void)a;
  (void)b;
  puts("WARN: Stubbed function rotate()");
  // return (a*b) >> 14; //??
  return 0;
}

static int*
RotXY(int count, int cos, int sin, int* dst, int* src)
{
  (void)count;
  (void)cos;
  (void)sin;
  (void)dst;
  (void)src;
  puts("WARN: Stubbed function RotXY()");
  return 0;
}

static int*
RotXZ(int count, int cos, int sin, int* dst, int* src)
{
  (void)count;
  (void)cos;
  (void)sin;
  (void)dst;
  (void)src;
  puts("WARN: Stubbed function RotXZ()");
  return 0;
}

static int*
RotYZ(int count, int cos, int sin, int* dst, int* src)
{
  (void)count;
  (void)cos;
  (void)sin;
  (void)dst;
  (void)src;
  puts("WARN: Stubbed function RotYZ()");
  return 0;
}

static void
screenmode(int mode)
{
  (void)mode;
  puts("WARN: Stubbed function screenmode()");
}

static void
SetAudioVector(void)
{
  puts("WARN: Stubbed function SetAudioVector()");
}

static void
SetTextColor(int color)
{
  (void)color;
  puts("WARN: Stubbed function SetTextColor()");
}

static void
SetVecs(void)
{
  puts("WARN: Stubbed function SetVecs()");
}

static void
setvect(char var1, void (*func)())
{
  (void)var1;
  (void)func;
  puts("WARN: Stubbed function setvect()");
}

static int
sinus(int arg)
{
  (void)arg;
  puts("WARN: Stubbed function sinus()");
  return 0;
}

static void
SolidColor(int color)
{
  (void)color;
  puts("WARN: Stubbed function SolidColor()");
}

static void
SubBCD(long bcd, long* value)
{
  (void)bcd;
  (void)value;
  puts("WARN: Stubbed function SubBCD()");
}

static void
syncup(void)
{
  puts("WARN: Stubbed function syncup()");
}

static void
SysToVid(int x1, int y1, int x2, int y2, void* src, void* dst)
{
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
  (void)src;
  (void)dst;
  puts("WARN: Stubbed function SysToVid()");
}

static void
TypeString(int x, int y, char* string)
{
  (void)x;
  (void)y;
  (void)string;
  puts("WARN: Stubbed function TypeString()");
}

static void
Uncomp(void* addr)
{
  (void)addr;
  puts("WARN: Stubbed function Uncomp()");
}

static void
Vblank(void)
{
  puts("WARN: Stubbed function Vblank()");
}

static void
VidToVid(int x1, int y1, int x2, int y2, void* src, void* dst)
{
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
  (void)src;
  (void)dst;
  puts("WARN: Stubbed function VidToVid()");
}

static void
WideBox(int y1, int y2)
{
  (void)y1;
  (void)y2;
  puts("WARN: Stubbed function WideBox()");
}
