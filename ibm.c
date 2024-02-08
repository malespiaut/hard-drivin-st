#include <stdio.h>
#include <stdlib.h>
// #include <alloc.h>
// #include <conio.h>
#include <fcntl.h>
// #include <bios.h>
// #include <dos.h>
// #include <io.h>
#include <string.h>
#include <sys/stat.h>

#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	ibm.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes all of the functions,
                        which are specific to the Atari ST hardware
                        or system software.

 DATE OF CREATION.....	March 22th 1989
 LAST CHANGE..........	October 18th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

/*		LOCAL VARIABLES :
                =================
*/

#define BigChunk (0x48000L)

#define verbose FALSE

int GamePalette[] = /* Game color palette */

  /*RGB*/ /* Range: 0..7 for each color */

  {
    0x0000, /* Black */
    0x0020, /* Dark Green */
    0x0130, /* Green */
    0x0346, /* Sky Blue */
    0x0760, /* Yellow */
    0x0630, /* Orange */
    0x0700, /* Red */
    0x0127, /* Dark Blue */
    0x0400, /* Dark Red */
    0x0321, /* Brown */
    0x0222, /* Grey 2 */
    0x0333, /* Grey 3 */
    0x0444, /* Grey 4 */
    0x0555, /* Grey 5 */
    0x0422, /* Redbrown */
    0x0777, /* Gray 7, White */
};

int LoadPalette[] = /* Game color palette */

  /*RGB*/

  {
    0x0000,
    0x0666,
    0x0240,
    0x0237,
    0x0520,
    0x0640,
    0x0650,
    0x0126,
    0x0222,
    0x0333,
    0x0444,
    0x0666,
    0x0300,
    0x0411,
    0x0611,
    0x0711,
};

static char huge *chunk,
  huge *chunk1; /* Aux. pointer for memory management. */

static void interrupt (*OldTimer)(void);
static void interrupt (*OldBreak)(void);
static void interrupt (*OldCtrlC)(void);
static void interrupt (*OldCritic)(void);
static void interrupt (*OldDivision0)(void);
static void interrupt (*OldBreak)(void);
static void interrupt (*OldNMI)(void);
static void interrupt (*OldTRAP)(void);

char BlockButtons;    /* Mouse button control bits. */
char JoyCode;         /* Direction flags of both joysticks. */
char OverflowFlag;    /* TRUE after division overflow. */
char MouseDriverFlag; /* TRUE if mouse driver is existent. */
char OrgDriverFlag;   /* Same as above. */
char CPUspeed;        /* 1-4 depending on cpu speed. */
char TuneSpeed;       /* Tune speed 0-3. */
int latch0,           /* Joystick buttons: J0: bits 0,1  J1: bits 2,3 */
  latch1,             /* Joystick 0:  X-axis */
  latch2,             /* Joystick 0:  Y-axis */
  latch3,             /* Joystick 1:  X-axis */
  latch4;             /* Joystick 1:  Y-axis */
int PlaneSize;        /* Size of a bitplane to move. */
int ibutton;          /* Used to hold button flags. */
int* PokePitch;       /* Pointer to pitch variable. */

/*		FUNCTIONS :
                ===========
*/

void
SystemSpeed(void)
{
  extern int PolyVertex[];
  extern volatile uint frames;
  int i, buf[3 * 100];

  frames = 0;

  for (i = 0; i < 10; i++)
  {
    BlankScreen();
    RotXY(100, 0, 0, buf, buf);
    PolyVertex[0] = -30;
    PolyVertex[1] = -40;
    PolyVertex[2] = -50;
    PolyVertex[3] = 0;
    PolyVertex[4] = 10;
    PolyVertex[5] = 30;
    PolyVertex[6] = 50;
    PolyVertex[7] = 10;
    Polygon(4);
  }

  CPUspeed = frames;

  if (CPUspeed < 12)
    TuneSpeed = 2; /* 32 MHZ */
  else if (CPUspeed < 19)
    TuneSpeed = 1; /* 16 MHZ */
  else if (CPUspeed < 30)
    TuneSpeed = 0; /*  8 MHZ */
  else
    TuneSpeed = 3; /*  5 MHZ */
}

void
CenterMouse(void)
{
  extern char MouseDriverFlag;
  extern int SteerX, SteerY;
  union REGS reg;

  reg.x.cx = SteerX = 320;
  reg.x.dx = SteerY = 200;
  reg.x.ax = 4;

  if (!MouseDriverFlag)
    return;

  int86(0x33, &reg, &reg);
}

void
ClearButtons(void)
{
  extern char ExitWait;
  extern volatile uint button;

  ExitWait = FALSE;
  button = 0;
  BlockButtons = 1 + 4;
}

void
StopEngine(void)
{
  extern char EngineKilled;
  union REGS regs;

  EngineKilled = TRUE;
  regs.h.ah = 0x01;
  int86(0x81, &regs, &regs);
}

void
StartEngine(void)
{
  extern char EngineKilled, SoundEnabled;
  extern int EnginePitch;
  union REGS regs;

  if (!SoundEnabled)
    return;

  if (EngineKilled)
  {
    regs.h.ah = 3;
    regs.h.al = 23;
    int86(0x81, &regs, &regs);
    EngineKilled = FALSE;
  }

  *PokePitch = 21000 - (10 * EnginePitch);
}

void
DisableMouse(void)
{
  extern char MouseFlag;

  MouseFlag = FALSE;
}

void
EnableMouse(void)
{
  extern char MouseFlag;

  MouseFlag = TRUE;
}

static void
CreditsScreen(void)
{
  extern volatile uint frames;
  union REGS regs;
  char code;

  regs.h.ah = 0x0F;
  int86(0x10, &regs, &regs);
  code = regs.h.al;

  if ((code != 2) && (code != 3) && (code != 7))
  {
    regs.h.ah = 0x00;
    regs.h.al = 0x02;
    int86(0x10, &regs, &regs);
  }

  return;

  printf("\n\n\n\n\n\n");
  printf("               H A R D   D R I V I N '\n");
  printf("               -----------------------\n");
  printf("\n\n");
  printf("               PUBLISHED BY DOMARK LTD.\n");
  printf("   PC VERSION BY JUERGEN FRIEDRICH AND MARCUS GOODEY\n");
  printf("                 (c) 1989 TENGEN INC.\n");
  printf("\n\n");
  printf("             Thanks to : Mike Day and all at\n");
  printf("                      Atari/Tengen.\n");
  printf("\n\n");
  printf("             Graphics   : Juergen Friedrich,\n");
  printf("                          Richard Browne\n");
  printf("\n");
  /*
  printf("             Management : John Kavanagh\n");
  */
  printf("\n");
  printf("\n\n\n");

  for (frames = 0; frames < 4 * 100;)
    ;
}

static void
SelectGraphMode(void)
{
  extern char GraphMode;
  char key;

  GraphMode = 4;
  return;

  printf("\n\n");

  printf("	Mouse emulation through keyboard:\n\n");
  printf("	LEFT .............. press 'Cursor LEFT'\n");
  printf("	RIGHT ............. press 'Cursor RIGHT'\n");
  printf("	LEFT  BUTTON ...... press 'Cursor UP'\n");
  printf("	RIGHT BUTTON ...... press 'Cursor DOWN'\n");
  printf("	BOTH  BUTTONS ..... press SPACEBAR\n");
  printf("\n\n\n");

  printf("	PLEASE SELECT THE SCREENMODE\n");
  printf("	----------------------------\n");
  printf("\n");
  printf("	Press '1' for   HERCULES\n");
  printf("	Press '2' for   TANDY\n");
  printf("	Press '3' for   CGA\n");
  printf("	Press '4' for   EGA\n");
  printf("	Press '5' for   VGA\n");
  printf("\n\n\n\n\n");

  for (GraphMode = -1; GraphMode == -1;)
  {
    key = KeyPressed();

    switch (key)
    {
      case '1':
        GraphMode = 2;
        break;
      case '2':
        GraphMode = 3;
        break;
      case '3':
        GraphMode = 1;
        break;
      case '4':
        GraphMode = 0;
        break;
      case '5':
        GraphMode = 4;
        break;
    }
  }
}

void
Switch(void)
{
  extern volatile uint frames;

  while (frames < 9)
    ;
  frames = 0;
  syncup();
}

void
StopSound(void)
{
  StopEngine();
}

void
MakeSound(char sound)
{
  extern char SloMoFlag, AccidentFlag, DemoMode;
  extern char ReturnFlag, EngineKilled, SoundEnabled;
  static char /*
              lastsound,
              lastprio,
              */
    ts[] = {
      0,  /* Tune 0 */
      0,  /* Tune 1 */
      0,  /* VOID */
      0,  /* VOID */
      0,  /* VOID */
      13, /* Finish */
      11, /* ExtraTime */
      15, /* Squeal */
      6,  /* Flag */
      1,  /* Crash */
      0,  /* Clonk */
      13, /* Moo */
      20, /* Ignition */
      10, /* Offroad */
    };
  union REGS regs;

  if (DemoMode | ReturnFlag || !SoundEnabled)
  {
    StopSound();
    return;
  }

  if (AccidentFlag && (sound != s_crash))
    return;

  regs.h.ah = 3;
  regs.h.al = ts[sound];
  int86(0x81, &regs, &regs);

  EngineKilled = TRUE;
}

int
Random(void)
{ /* Random 16 bit number -32768..32767. */

  static long rand;

  if (rand)
    rand *= 31415926L;
  rand += 1;

  return ((int)rand);
}

static void huge*
GetBuffer(long size)
{
  void huge* addr;
  long AllocChunk;

  AllocChunk = farcoreleft();

  if (AllocChunk < size)
  {
    ;
#if (verbose)
    printf(" Free Memory: %lx\n", AllocChunk);
#endif
    SystemExit("Not enough memory !");
  }

  addr = (void huge*)farmalloc(size);

  if (addr == NULL)
    SystemExit("Memory allocation didn't work !");

  return (addr);
}

void
CopySmallScreen(int y1, int y2, void* source, void* dest)
{
  PlaneSize = WindowY * 40;

  SysToVid(0, y1, 319, y2, source, dest);

  PlaneSize = 200 * 40;
}

static long
FileLength(char* name)
{
  extern int _fmode;
  int handle;
  long size;

  _fmode = O_BINARY;

  handle = open(name, O_RDONLY | O_BINARY);

  if (handle > 0)
  {
    size = filelength(handle);
#if (verbose)
    printf(" FILE %s HAS LENGTH: %ld\n", name, size);
#endif
    close(handle);
    return (size);
  }
  else
  {
    ;
#if (verbose)
    printf(" CAN'T READ LENGTH OF %s\n", name);
#endif
    return (0L);
  }
}

static int
DOS_open(char* name)
{
  int handle;
  union REGS regs;

  regs.h.ah = 0x3D;
  regs.h.al = 0x12;
  regs.x.dx = (uint)name;

  handle = intdos(&regs, &regs);

  if (regs.x.cflag)
    handle = 0;

  return (handle);
}

static int
DOS_create(char* name)
{
  int handle;
  union REGS regs;

  regs.h.ah = 0x3C;
  regs.x.cx = 0x00;
  regs.x.dx = (uint)name;

  handle = intdos(&regs, &regs);

  if (regs.x.cflag)
    handle = 0;

  return (handle);
}

static void
DOS_read(int handle, void* addr, uint size)
{
  union REGS regs;
  struct SREGS sregs;

  regs.h.ah = 0x3F;
  regs.x.bx = handle;
  regs.x.cx = size;
  regs.x.dx = FP_OFF(addr);
  sregs.ds = FP_SEG(addr);

  intdosx(&regs, &regs, &sregs);
}

static void
DOS_write(int handle, void* addr, uint size)
{
  union REGS regs;
  struct SREGS sregs;

  regs.h.ah = 0x40;
  regs.x.bx = handle;
  regs.x.cx = size;
  regs.x.dx = FP_OFF(addr);
  sregs.ds = FP_SEG(addr);

  intdosx(&regs, &regs, &sregs);
}

static void
DOS_close(int handle)
{
  union REGS regs;

  regs.h.ah = 0x3E;
  regs.x.bx = handle;

  intdos(&regs, &regs);
}

int
FileRead(char* name, void* addr)
{
  int handle;
  long size;

  size = FileLength(name);

  if (size)
  {
    handle = DOS_open(name);

    if (handle)
    {
      DOS_read(handle, addr, (uint)size);
      DOS_close(handle);
#if (verbose)
      printf(" FILE %s IS READ.\n", name);
#endif
      return (TRUE);
    }
    return (FALSE);
  }
  else
  {
    ;
#if (verbose)
    printf(" FILE %s NOT fOUND.\n", name);
#endif
    return (FALSE);
  }
}

int
FixedRead(char* name, void* addr, long size)
{
  extern int _fmode;
  int handle;

  _fmode = O_BINARY;

  handle = DOS_open(name);

  if (handle > 0)
  {
    DOS_read(handle, addr, (uint)size);
    DOS_close(handle);
#if (verbose)
    printf(" FILE %s IS READ.\n", name);
#endif
    return (TRUE);
  }
  else
  {
    ;
#if (verbose)
    printf(" FILE %s NOT FOUND.\n", name);
#endif
    return (FALSE);
  }
}

void
FileWrite(char* name, void* addr, long size)
{
  extern int _fmode;
  int handle;

  _fmode = O_BINARY;

  handle = DOS_create(name);

  if (handle > 0)
  {
    DOS_write(handle, addr, (uint)size);
    DOS_close(handle);
  }
}

int
ReadBlockFile(s_BitImage* bi, char* name)
{
  extern void* AuxBuffer;
  register int *p, ok;
  int width, height, words, size;

  ok = FileRead(name, AuxBuffer);

  if (ok)
  {
    p = (int*)AuxBuffer;
    width = *p++;
    height = *p++;
    p += 10;
    words = width >> 4;
    size = 4 * words * height;
    size = preshift * size + 2 + 10;
    *bi = GetPieceOfChunk(size * sizeof(int));
    movewords(AuxBuffer, *bi, size);
  }

  return (ok);
}

int
ReadShapeFile(s_BitImage* shape, char* FileName)
{
  int* buffer;
  long size;

  size = FileLength(FileName);

  if (size > 0)
  {
    buffer = GetPieceOfChunk(size);
    FileRead(FileName, buffer);
    *shape = buffer;
  }

  return ((size > 0));
}

int
KeyPressed(void)
{
  return (CheckKeyboard());
}

int
SetNewColor(int no, int col)
{
  return (Pal(no, col));
}

void
SetColorPalette(int* addr)
{
  int i;

  for (i = 0; i < 16; i++)
    SetNewColor(i, *addr++);
}

void
BlankScreen(void)
{
  SolidColor(0);
  WideBox(-67, 67);
}

void*
GetPieceOfChunk(long amount)
{
  void* save;
  uint segment, offset;

  amount += 16;

  if ((chunk1 + amount) < (chunk + BigChunk))
  {
    segment = FP_SEG(chunk1);
    offset = FP_OFF(chunk1);
    chunk1 += amount;
#if (verbose)
    printf("\t%ld bytes allocated.\n", amount);
#endif
    offset = 0;
    segment++;
    save = MK_FP(segment, offset);
    return (save);
  }
  else
  {
    SystemExit("Allocated memory not sufficient !");
    return ((void*)0);
  }
}

static void
Allocate(void)
{
  extern void *AuxScreen, *SoundModule;
  extern void *MapScreen, *SelectScreen, *AuxBuffer;
  extern int *BitMask, *PolyLeft, *PolyRight, *ObjDataBase;
  extern void *Marcus, *Tim;
  extern s_frame *OldChampLap, *NewChampLap;

  chunk = chunk1 = GetBuffer(BigChunk);

  Marcus = GetPieceOfChunk(0x08000L);
  Tim = GetPieceOfChunk(0x08000L);
  AuxBuffer = GetPieceOfChunk(0x08000L);

  AuxScreen = GetPieceOfChunk(ScreenSize);
  SelectScreen = GetPieceOfChunk(WindowSize);
  MapScreen = GetPieceOfChunk(WindowSize);

  ObjDataBase = GetPieceOfChunk(FileLength(DataBaseName));
  SoundModule = GetPieceOfChunk(FileLength(SoundName));

  OldChampLap = GetPieceOfChunk(MaxSample * sizeof(s_frame));
  NewChampLap = GetPieceOfChunk(MaxSample * sizeof(s_frame));
}

static void
FreeMemory(void)
{
  if (chunk)
    farfree((void*)chunk);
}

static void
SetBlock(int x1, int y1, int x2, int y2, int both, s_block* bl)
{
  if (both)
  {
    bl->dx1 = x1;
    bl->dy1 = y1;
  }
  bl->sx1 = x1;
  bl->sy1 = y1;
  bl->sx2 = x2;
  bl->sy2 = y2;
}

void
GetResolution(void)
{
  extern int ScoreX, ScoreY, TimeX, TimeY;
  extern int BestX, BestY, LastX, LastY, DigitLength;
  extern int SecondsX, SecondsY, OffRoadX, OffRoadY;
  extern int OrgX, OrgY, MinX, MaxX, MinY, MaxY;
  extern int ScreenX, ScreenY;
  extern int mphX, mphY, rpmX, rpmY, ampX, ampY;
  extern int tmpX, tmpY, oilX, oilY, gasX, gasY;
  extern int MinY1, MaxY1, OrgY1;
  extern int MinY2, MaxY2, OrgY2, ListX, LogoY;
  extern int largePointer, smallPointer, MaxLine;
  extern int CharWidth, CharHeight, LineHeight;
  extern s_block ScoreRect, TimeRect, MessageRect[];
  extern s_block AutoRect, ManualRect, BarRect, MidRect;
  extern s_block GearARect[], GearMRect[];

  OrgX = 160;
  OrgY1 = WindowY / 2;
  OrgY2 = WindowY + PanelY / 2;
  MinX = -160;
  MaxX = 159;
  MinY1 = -OrgY1;
  MaxY1 = (WindowY - 1) / 2;
  MinY2 = -PanelY / 2;
  MaxY2 = (PanelY - 1) / 2;

  ScreenX = 320;
  ScreenY = 200;

  mphX = 188 - OrgX;
  mphY = 171 - OrgY2;
  rpmX = 129 - OrgX;
  rpmY = 171 - OrgY2;
  ampX = 88 - OrgX;
  ampY = 159 - OrgY2;
  tmpX = 88 - OrgX;
  tmpY = 186 - OrgY2;
  oilX = 230 - OrgX;
  oilY = 159 - OrgY2;
  gasX = 230 - OrgX;
  gasY = 186 - OrgY2;

  largePointer = 13;
  smallPointer = 7;

  SetBlock(16, 156, 63, 161, TRUE, &ScoreRect);
  SetBlock(16, 172, 63, 177, TRUE, &TimeRect);
  SetBlock(0, 182, 79, 193, TRUE, &MessageRect[0]);
  SetBlock(0, 182, 79, 193, TRUE, &MessageRect[1]);
  SetBlock(176, 0, 255, 11, FALSE, &MessageRect[1]);
  SetBlock(0, 182, 79, 192, TRUE, &MessageRect[2]);
  SetBlock(48, 7, 127, 17, FALSE, &MessageRect[2]);
  SetBlock(112, 158, 207, 184, TRUE, &MidRect);
  SetBlock(288, 152, 319, 189, TRUE, &AutoRect);
  SetBlock(144, 145, 175, 157, TRUE, &BarRect);
  SetBlock(288, 152, 319, 189, TRUE, &ManualRect);
  SetBlock(288, 25, 319, 62, FALSE, &ManualRect);
  SetBlock(288, 170, 319, 178, TRUE, &GearARect[0]);
  SetBlock(288, 84, 319, 92, FALSE, &GearARect[0]);
  SetBlock(288, 159, 303, 167, TRUE, &GearARect[1]);
  SetBlock(288, 74, 303, 82, FALSE, &GearARect[1]);
  SetBlock(288, 181, 303, 189, TRUE, &GearARect[2]);
  SetBlock(288, 94, 303, 102, FALSE, &GearARect[2]);
  SetBlock(304, 159, 319, 167, TRUE, &GearARect[3]);
  SetBlock(304, 74, 319, 82, FALSE, &GearARect[3]);
  SetBlock(304, 181, 319, 189, TRUE, &GearARect[4]);
  SetBlock(304, 94, 319, 102, FALSE, &GearARect[4]);
  SetBlock(288, 170, 319, 178, TRUE, &GearMRect[0]);
  SetBlock(288, 84, 319, 92, FALSE, &GearMRect[0]);
  SetBlock(288, 159, 303, 167, TRUE, &GearMRect[1]);
  SetBlock(288, 5, 303, 13, FALSE, &GearMRect[1]);
  SetBlock(288, 181, 303, 189, TRUE, &GearMRect[2]);
  SetBlock(288, 15, 303, 23, FALSE, &GearMRect[2]);
  SetBlock(304, 159, 319, 167, TRUE, &GearMRect[3]);
  SetBlock(304, 5, 319, 13, FALSE, &GearMRect[3]);
  SetBlock(304, 181, 319, 189, TRUE, &GearMRect[4]);
  SetBlock(304, 15, 319, 23, FALSE, &GearMRect[4]);

  ScoreY = 156;
  ScoreX = 50;
  TimeY = 172;
  TimeX = 50;
  BestY = 182;
  BestX = 60;
  LastY = 188;
  LastX = 60;
  SecondsY = 123;
  SecondsX = 18;
  OffRoadY = 15;
  OffRoadX = 14;
  ListX = 11;

  MaxLine = OrgY + MaxY;
  CharWidth = 8;
  CharHeight = 8;
  LineHeight = 9;
  LogoY = -47;

  DigitLength = 4;

  MinY = MinY1;
  MaxY = MaxY1;
  OrgY = OrgY1;

  SolidColor(white);
}

int
SystemInit(void)
{
  extern char MouseFlag, GraphMode, SteerSelect, GearSelect;
  extern char MapName[], SelectName[], PanelName[], LoadName[];
  extern int resolution;
  void interrupt (*mouse)(void);
  union REGS regs;

  PlaneSize = 8000;
  MouseFlag = FALSE;

  SteerSelect = 4;
  GearSelect = 3;

  /* INSTALL INTERRUPT FUNCTIONS. */

  OldBreak = getvect(0x1B);
  OldTimer = getvect(0x1C);
  OldCtrlC = getvect(0x23);
  OldCritic = getvect(0x24);
  OldDivision0 = getvect(0x00);
  OldTRAP = getvect(0x01);
  OldNMI = getvect(0x02);
  OldBreak = getvect(0x03);
  mouse = getvect(0x33);

  setvect(0x00, DivZero);
  setvect(0x01, EmptyFunction);
  setvect(0x02, EmptyFunction);
  setvect(0x03, EmptyFunction);
  setvect(0x1C, Vblank);
  setvect(0x1B, EmptyFunction);
  setvect(0x23, EmptyFunction);
  setvect(0x24, EmptyFunction);

  SetVecs();

  CreditsScreen();

  MouseDriverFlag = TRUE;

  if (!mouse)
    MouseDriverFlag = FALSE;
  else
  {
    if (!(*(long*)mouse))
      MouseDriverFlag = FALSE;
  }

  if (MouseDriverFlag)
  { /* INITIALIZE MOUSE */

    regs.x.ax = 0x00;
    int86(0x33, &regs, &regs);
    if (!regs.x.ax)
      MouseDriverFlag = FALSE;
  }

  OrgDriverFlag = MouseDriverFlag;

  /* ALLOCATE ALL NECESSARY BUFFERS. */

  Allocate();

  /* SET GRAPHICS MODE. */

  GetResolution();
  SelectGraphMode();

  screenmode(GraphMode);
  CalibrateJoystick();
  SetAudioVector();

  strcpy(PanelName, "Files\\Panel.");
  strcpy(MapName, "Files\\Map.");
  strcpy(SelectName, "Files\\Select.");
  strcpy(LoadName, "Files\\Loading.");

  switch (GraphMode)
  {
    case 1:
    case 2:
      strcat(PanelName, "CPX");
      strcat(MapName, "CPX");
      strcat(SelectName, "CPX");
      strcat(LoadName, "CPX");
      break;
    case 0:
    case 3:
      strcat(PanelName, "EPX");
      strcat(MapName, "EPX");
      strcat(SelectName, "EPX");
      strcat(LoadName, "EPX");
      break;

    case 4:
      strcat(PanelName, "VPX");
      strcat(MapName, "VPX");
      strcat(SelectName, "VPX");
      strcat(LoadName, "EPX");
      break;
  }

  SystemSpeed();
  ClearButtons();

  return (0);
}

void
SystemExit(char* message)
{
  union REGS regs;

  StopSound();

  FreeMemory();

  /* RE-INSTALL INTERRUPTS. */

  ClearVecs();

  setvect(0x23, OldCtrlC);
  setvect(0x24, OldCritic);
  setvect(0x1B, OldBreak);
  setvect(0x00, OldDivision0);
  setvect(0x01, OldTRAP);
  setvect(0x02, OldNMI);
  setvect(0x03, OldBreak);
  setvect(0x1C, OldTimer);

  regs.h.ah = 0x00;
  regs.h.al = 0x03;
  int86(0x10, &regs, &regs);

  ;
#if (0)
  printf(" %s\n", message);
  getchar();
#else
  message = message;
#endif

  exit(0);
}
