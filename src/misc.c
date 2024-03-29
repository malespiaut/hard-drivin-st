#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "ibm.h"
#include "main.h"
#include "missing.h"
#include "proto.h"

/*
----------------------------------------------------------------------

 MODULE...............	misc.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes miscellaneous functions.

 DATE OF CREATION.....	March 22th 1989
 LAST CHANGE..........	November 5th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

//		LOCAL VARIABLES :
//              =================

//		FUNCTIONS :
//              ===========

static void ClearControlPoints(void);
static int ReadShapeFiles(void);
static void ReadGameFile(void);
static int ReadTrackFile(void);
static void ReadAllFiles(void);

static void
ClearControlPoints(void)
{
  int i;
  s_control* ct;

  // RESET ALL RETURNPOINTS.

  for (ct = T1ContPoints, i = 0; i < NumOfT1controls; i++)
  {
    ct->covered = FALSE;
    (track1 + ct->segment)->flags |= f_returnpoint;
    ct++;
  }

  for (ct = T2ContPoints, i = 0; i < NumOfT2controls; i++)
  {
    ct->covered = FALSE;
    (track2 + ct->segment)->flags |= f_returnpoint;
    ct++;
  }

  NextCtr1 = T1ContPoints;
  NextCtr2 = T2ContPoints;
}

void
GetPlayerInput(void)
{
  int key;
  int key2;

#define st_add 30
#define st_sub 15

  key2 = 0;

  if (SteerSelect == 4)
  {
    key2 = CheckSteeringKeys();

    if (key2 & 16)
    {
      button |= 1 + 4;
      car.BrakeFlag = TRUE;
    }
    if (key2 & 4)
    {
      SteerX -= st_add;
      if (SteerX < 0)
        SteerX = 0;
      if (SteerX > 320)
        SteerX -= st_sub;
    }
    if (key2 & 8)
    {
      SteerX += st_add;
      if (SteerX > 639)
        SteerX = 639;
      if (SteerX < 320)
        SteerX += st_sub;
    }
    if (key2 & 1)
      button |= 4;
    if (key2 & 2)
      button |= 1;

    if (!(key2 & 0x0C) && GameMode && !DemoMode && TimeFlag)
    {
      if (SteerX > 320)
      {
        SteerX -= st_sub;
        if (SteerX < 320)
          SteerX = 320;
      }
      if (SteerX < 320)
      {
        SteerX += st_sub;
        if (SteerX > 320)
          SteerX = 320;
      }
    }

    if ((key2 >= '1') && (key2 <= '5'))
      key2 = 0;

#undef st_add
  }

  if (!(key2 & 0x7F))
  {
    key = (int)KeyPressed() & 0xFF;

    if (key)
    {
      if (isupper(key))
        key = _tolower(key);

      switch (key)
      { // STANDARD KEYBOARD COMMANDOS

        case 'p':
        case 'P':
          if (!GameMode)
            break;
          TimeFlag = FALSE;
          RealToDummy();
          SolidColor(black);
          NewColor(white);
          SetTextColor(white);
          Box(-50, -10 - 47, 58, 10 - 47);
          MoveTo(-50 + 1, -10 + 1 - 47);
          DrawTo(58 - 1, -10 + 1 - 47);
          DrawTo(58 - 1, 10 - 1 - 47);
          DrawTo(-50 + 1, 10 - 1 - 47);
          DrawTo(-50 + 1, -10 + 1 - 47);
          PrintString(20 - 5, 63 - 47, "GAME PAUSED.");
          Switch();
          StopSound();
          while (!KeyPressed())
            ;
          TimeFlag = TRUE;
          break;

        case 'o':
          SetOptions();
          break;

        case 'a':
          if (OffRoadOut)
            ReturnFlag = TRUE;
          break;

          // KEYBOARD MOUSE EMULATION: IBM PC ONLY

#define st_add (45 - 16)

        case 3:
          CharNum--;
          SteerX -= st_add;
          if (SteerX < 0)
            SteerX = 0;
          break;

        case 4:
          CharNum++;
          SteerX += st_add;
          if (SteerX > 639)
            SteerX = 639;
          break;

#undef st_add

        case 1:
          button |= 4;
          break;

        case 2:
          button |= 1;
          break;

        case ' ':
          button |= 1 + 4;
          car.BrakeFlag = TRUE;
          break;

          // DEBUGGING

        case ESC:
          GameMode = FALSE;
          SystemExit("");
          break;
#if (1)

        case 'c':
          GameMode = FALSE;
          QualifyFlag = TRUE;
          break;

        case 'q':
          GameMode = FALSE;
          QualifyFlag = FALSE;
          break;
#endif
      }

      if ((!car.autoFlag) && (GearSelect == 3))
      {
        switch (key)
        {
          case '1':
            car.ShiftFlag = 1;
            break;
          case '2':
            car.ShiftFlag = 2;
            break;
          case '3':
            car.ShiftFlag = 3;
            break;
          case '4':
            car.ShiftFlag = 4;
            break;
          case 'n':
            car.ShiftFlag = 5;
            break;
        }
      }
    }
  }
  else if (SteerSelect == 4 && GameMode && !DemoMode && TimeFlag)
  {
    if (SteerX > 320)
    {
      SteerX -= st_sub;
      if (SteerX < 320)
        SteerX = 320;
    }
    if (SteerX < 320)
    {
      SteerX += st_sub;
      if (SteerX > 320)
        SteerX = 320;
    }
  }

  if (button && !AccidentFlag && !SloMoFlag)
  { // MOUSE/JOYSTICK BUTTONS PRESSED.

    ExitWait = TRUE;

    if (DemoMode)
    { // END DEMO MODE.

      DemoMode = FALSE;
    }
    else
    { // ACCELERATE/DECELERATE.

      if (button == 4)
      { // ACCELERATING.

        if (EngineOn)
        {
          car.throttle += 2;
          if (car.throttle > MaxThrottle)
            car.throttle = MaxThrottle;
        }
      }
      if (button & 1)
      { // DECELERATING.

        car.throttle -= 2;
        if (car.throttle < 0)
          car.throttle = 0;
      }

      if (StartMessage)
      { // ENGINE STARTED.

        StartMessage = FALSE;

        if (RedLight->active)
        {
          RedLight->active = FALSE;
          YellowLight->active = TRUE;
        }
      }

      if (!StartFlag)
      { // START RACE.

        MakeSound(fx_ignition);
        RaceIsOn = TRUE;
        StartFlag = 1;
      }

      // BRAKE

      switch (SteerSelect)
      {
        case 1: // JOYSTICK 0

          if (button & 4)
            car.BrakeFlag = TRUE;
          break;

        case 2: // JOYSTICK 1

          if (button & 1)
            car.BrakeFlag = TRUE;
          break;

        case 3: // MOUSE

          if (button == 4 + 1)
          { // BRAKING

            car.BrakeFlag = TRUE;
          }
          break;
      }
    }

    button = 0;
  }

  if (TimeOutFlag)
  { // TIME OUT: KILL ENGINE.

    car.throttle = 0;
    EnginePitch = 0;
  }
}

static int
ReadShapeFiles(void)
{
  s_BitImage* mo;
  int i;
  int x;
  int h;
  int* p;
  int ok;

  mo = Mountain;
  ok = TRUE;

  ok &= ReadBlockFile(mo, "data/mnt01.sha");
  ok &= ReadBlockFile(mo + 1, "data/mnt02.sha");
  ok &= ReadBlockFile(mo + 2, "data/mnt03.sha");
  ok &= ReadBlockFile(mo + 3, "data/mnt04.sha");
  ok &= ReadBlockFile(mo + 4, "data/mnt05.sha");
  ok &= ReadBlockFile(mo + 5, "data/mnt06.sha");
  ok &= ReadBlockFile(mo + 6, "data/mnt07.sha");
  ok &= ReadBlockFile(mo + 7, "data/mnt08.sha");
  /*	! UNUSED !
  ok &= ReadBlockFile( mo+8, "data/mnt09.sha");
  */
  ok &= ReadBlockFile(mo + 9, "data/mnt10.sha");

  mo = Mountain;
  for (i = 0; i < 10; i++)
  {
    if (mo)
    {
      Convert3(mo);
      PreShifter(mo);
    }
    mo++;
  }

  ok &= ReadShapeFile(&FlagYellow, "data/yelflag.sha");
  ok &= ReadShapeFile(&FlagRed, "data/redflag.sha");
  ok &= ReadShapeFile(&Cup, "data/cup.sha");
  ok &= ReadShapeFile(&ChampLogo, "data/logo.sha");
  ok &= ReadShapeFile(&Explosion[0], "data/expl1.sha");
  ok &= ReadShapeFile(&Explosion[1], "data/expl2.sha");
  ok &= ReadShapeFile(&Explosion[2], "data/expl3.sha");
  ok &= ReadShapeFile(&Explosion[3], "data/expl4.sha");
  ok &= ReadShapeFile(&Explosion[4], "data/expl5.sha");
  ok &= ReadShapeFile(&Explosion[5], "data/expl6.sha");

  Convert2(FlagYellow);
  Convert2(FlagRed);
  Convert2(Cup);
  Convert2(ChampLogo);
  Convert2(Explosion[0]);
  Convert2(Explosion[1]);
  Convert2(Explosion[2]);
  Convert2(Explosion[3]);
  Convert2(Explosion[4]);
  Convert2(Explosion[5]);

  p = MntPositions;
  x = -(4 * MaxXres) / 2;

  // SET X-COORDINATES OF THE MOUNTAIN ELEMENTS

  for (i = 0; i < 13 + 1; i++)
  {
    h = p[1];
    p[1] = x;
    x += MntLength[p[0]] + h;
    p += 2;
  }

  return (ok);
}

#define NameFlag 1 /* Indicates, whether the     \
                      best lap time and driver's \
                      name will be saved !!      \
                   */

static void
ReadGameFile(void)
{
  long DefaultBestTime;

#define size ((int)(sizeof(HiScoreList) + sizeof(BestName) + sizeof(BestTime) + MaxSample * sizeof(s_frame) + sizeof(int) + sizeof(long)))

  char* p;
  int i;

  DirtyFlag = !FileRead(ScoreName, AuxBuffer);
  p = AuxBuffer;

  if (DirtyFlag)
  { // CLEAR BUFFER

    for (i = size + 1; --i;)
      *p++ = ' ';
    p = AuxBuffer;

    DefaultBestTime = BestLapTime;
  }

  movebytes(p, HiScoreList, (int)(sizeof(HiScoreList)));
  p += sizeof(HiScoreList);

  movebytes(p, OldChampLap, (int)(MaxSample * sizeof(s_frame)));
  p += MaxSample * sizeof(s_frame);

  PhantomLane = (*p & 1);
  p += sizeof(int);

  BestLapTime = *(long*)p;
  p += sizeof(long);

#if (NameFlag)
  movebytes(p, BestName, (int)sizeof(BestName));
  p += sizeof(BestName);
  movebytes(p, BestTime, (int)sizeof(BestTime));
  p += sizeof(BestTime);
#endif

  if (DirtyFlag)
  { // CLEAR HISCORE LIST.

    for (i = 0; i < 10; i++)
      HiScoreList[i].score = 0;

    BestLapTime = DefaultBestTime;

    strcpy(BestName, "PHANTOM PHOTON.");
  }

#if (NameFlag)
  BestName[29] = BestTime[9] = '.';
#endif

  BestLength = StringLength(BestName);
}

void
SaveGameFile(void)
{
  char* p;

  p = AuxBuffer;

  movebytes(HiScoreList, p, (int)sizeof(HiScoreList));
  p += sizeof(HiScoreList);

  movebytes(OldChampLap, p, (int)(MaxSample * sizeof(s_frame)));
  p += MaxSample * sizeof(s_frame);

  *p = (PhantomLane & 1);
  p += sizeof(int);

  *(long*)p = BestLapTime;
  p += sizeof(long);

#if (NameFlag)
  movebytes(BestName, p, (int)sizeof(BestName));
  p += sizeof(BestName);
  movebytes(BestTime, p, (int)sizeof(BestTime));
  p += sizeof(BestTime);
#endif

  FileWrite(ScoreName, AuxBuffer, (long)size);
}

#undef NameFlag
#undef size

static int
ReadTrackFile(void)
{
  int ok;
  int* p;

  ok = FileRead(TrackName, (void*)TrackData);

  if (ok)
  {
    p = TrackData;

    NumOfFields = p[0];
    field = (s_object*)&p[p[1]];
    NumOfObjects = p[2];
    object = (s_object*)&p[p[3]];
    Streets1 = p[4];
    track1 = (s_track*)&p[p[5]];
    Streets2 = p[6];
    track2 = (s_track*)&p[p[7]];
    NumOfBarriers = p[8];
    Barriers = (int*)&p[p[9]];
    NumOfT1controls = p[10];
    T1ContPoints = (s_control*)&p[p[11]];
    NumOfT2controls = p[12];
    T2ContPoints = (s_control*)&p[p[13]];
    NumOfVehicles = p[14];
    vehicle = (s_vehicle*)&p[p[15]];

    Junct1 = track1 + p[16];
    Junct2 = track1 + p[17];

    BestLapTime = *(long*)&p[18];
    TimeToQualify = *(long*)&p[20];
    Duration = *(long*)&p[22];

    BonusCheckpoint = p[24];
    BonusFinish = p[25];

    StartSegment = p[26];
    StartLane = p[27];
    StartRow = p[28];
    FinishSegment = p[29];
    CheckP1_segment = p[30];
    CheckP2_segment = p[31];

    LapViewX = p[32];
    LapViewY = p[33];
    LapViewZ = p[34];
    FinishX = p[35];
    FinishY = p[36];
    FinishZ = p[37];

    End1 = track1 + Streets1;
    End2 = track2 + Streets2;
  }

  return (ok);
}

static void
ReadAllFiles(void)
{
  int filesOK;

  filesOK = TRUE;

  // LOAD AND DISPLAY TITLE SCREEN, START SOUND.

  FixedRead(LoadName, Tim, ScreenSize);
  Uncomp(AuxScreen);
  Convert(AuxScreen, ScreenX, ScreenY);
  SysToVid(0, 0, 319, 199, AuxScreen, Buffer2);
  Switch();

  filesOK &= ReadShapeFiles();
  filesOK &= ReadTrackFile();

  ReadGameFile();
  ReadScoreList();

  filesOK &= FixedRead(MapName, Tim, WindowSize);
  Uncomp(MapScreen);
  filesOK &= FixedRead(SelectName, Tim, WindowSize);
  Uncomp(SelectScreen);
  filesOK &= FixedRead(PanelName, Tim, ScreenSize);
  Uncomp(AuxScreen);
  filesOK &= FileRead(SoundName, SoundModule);
  filesOK &= FileRead(DataBaseName, ObjDataBase);

  Convert(MapScreen, ScreenX, WindowY);
  Convert(SelectScreen, ScreenX, WindowY);
  Convert(AuxScreen, ScreenX, ScreenY);

  if (!filesOK)
  {
    SystemExit("One or more files are missing !");
  }

  // PLAY MUSIC, WAIT FOR MOUSE OR KEY.

  InitTune(TuneSpeed, 1);
  ClearButtons();

  /*
  while (!(KeyPressed() | button))
  {	if (!RefreshTune())
                  break;
  }
  */

  StopSound();
  ClearButtons();

  BlankScreen();
  Switch();
  PalOn();

  CopySmallScreen(0, WindowY, MapScreen, Buffer2);
  SysToVid(0, WindowY, ScreenX - 1, ScreenY - 1, AuxScreen, Buffer2);
  Switch();
  RealToDummy();
}

void
ReadScoreList(void)
{
  char c;
  char* p;
  s_score* h;
  int i;
  int j;
  long score;

  p = ScoreList;
  h = HiScoreList;

  for (i = 1; i <= 10; i++)
  {
    *p++ = (i == 10) ? 1 : ' ';
    *p++ = (i != 10) ? i : 0;
    *p++ = ' ';

    for (j = 0; j < 26; j++)
    {
      c = h->name[j];
      *p++ = (c) ? c : ' ';
    }

    *p++ = ' ';
    score = h->score;

    for (j = 7; j >= 0; j--)
    {
      c = (char)score & 15;
      if (!c && !score && (j < 7))
        c = ' ';
      p[j] = c;
      score >>= 4;
    }

    p += 8;
    *p++ = '.';

    h++;
  }
}

void
HardDrivingInit(void)
{
  MinVBL = 2;

  // DEFAULT INPUT: MOUSE, KEYBOARD.

  SteerSelect = 4;
  MouseFlag = FALSE;
  GearSelect = 3;
  Difficulty = 1;

  GetResolution();

  SetTextColor(yellow);

  ReadAllFiles();

  InitObjects();
  InitCar();
  InitVehicles();
  InitControlPoints();
  InitPhantomPhoton();

  InitRecord();
  SetArrows(FALSE);

  TimeString(BestTime, BestLapTime);

  GameMode = FALSE;
  DemoMode = TRUE;
  UK_Flag = FALSE;
  car.autoFlag = TRUE;
  NewScreens = TRUE;
  SoundEnabled = TRUE;

  StreetUnderCar = track1;

  NextCtr1 = T1ContPoints;
  NextCtr2 = T2ContPoints;
}

void
GameInit(void)
{

  InitRecord();
  InitControlPoints();
  InitCar();
  EnableVehicles();

  SetNearestElement();

  TimeOutFlag = ReturnFlag = OpaqueFlag = FALSE;
  QualifyFlag = DemoMode = SloMoFlag = FALSE;
  AccidentFlag = SpinFlag = BonusFlag = FALSE;
  EngineOn = RaceIsOn = FALSE;

  StartFlag = 0;
  MessageTime = 0;
  LastLapTime = 0;
  Score = 0;
  Time = 0;

  OffRoadOut = 0x1099;
  TimeOut = Duration;

  ClearControlPoints();

  GreenLight->active = FALSE;
  YellowLight->active = FALSE;
  RedLight->active = TRUE;

  PrintQualify();

  GameMode = TRUE;
  TimeFlag = TRUE;
  StartMessage = TRUE;
  EngineKilled = TRUE;

  ClearButtons();
}

#if (0)

void
FramePoly(int n)
{
  int i;
  int* p;

  if (n < 3)
    NewColor(2);
  else
    NewColor(1);

  p = PolyVertex;
  MoveTo(p[n + n - 2], p[n + n - 1]);

  for (i = n + 1; --i;)
  {
    DrawTo(p[0], p[1]);
    p += 2;
  }
}

#endif

void
Insert(int distance, s_object* obj)
{
  int i;
  s_priority* p;

  p = PrioList;

  for (i = ++VisibleObjects; --i;)
  {
    if (distance >= p->distance)
    {
      movewords(p, p + 1, (int)(sizeof(s_priority) / sizeof(int)) * i);
      p->distance = distance;
      p->obj = obj;
      return;
    }
    p++;
  }

  p->distance = distance;
  p->obj = obj;
}

void
InitRecord(void)
{
  s_playback* fr;
  int i;

  fr = FramePos = SlowMotion;

  for (i = 0; i < MaxFrame; i++)
  {
    fr->x = 0;
    fr->y = 0;
    fr->z = 0;
    fr++;
  }
}

void
RewindRecord(void)
{

  if (--FramePos < SlowMotion)
  {
    FramePos = &SlowMotion[MaxFrame - 1];
  }
}

void
RecordCar(void)
{
  int i;
  int* p;
  s_object* obj;
  s_playback* fr;
  s_vehicle* vh;

  fr = FramePos;

  fr->x = car.mx;
  fr->yaw = car.yaw;
  fr->y = car.my;
  fr->pitch = car.pitch;
  fr->z = car.mz;
  fr->roll = car.roll;

  fr->engine = EnginePitch;

  if (SpinFlag)
    fr->engine |= 0x8000;

  p = fr->carpos;
  vh = vehicle;

  for (i = NumOfVehicles + 1; --i;)
  {
    obj = (vh++)->obj;
    *p++ = obj->worldX;
    *p++ = obj->worldY;
    *p++ = obj->worldZ;
    *p++ = obj->yaw;
    *p++ = obj->pitch;
    *p++ = obj->roll;
  }

  if (++fr >= &SlowMotion[MaxFrame])
    fr = SlowMotion;

  FramePos = fr;
}

void
RecordRedCar(void)
{
  s_frame* fr;

  if (!StartFlag)
    return;

  if (++RedSample < SamplePeriod)
    return;

  RedSample = 0;
  fr = RedPointer;

  fr->x = car.mx;
  fr->yaw = -car.yaw;
  fr->y = car.my;
  fr->pitch = -car.pitch;
  fr->z = car.mz;
  fr->roll = -car.roll;

  if (++fr >= (NewChampLap + MaxSample))
    return;
  else
    RedPointer = fr;
}

void
ReadRedCar(void)
{
  s_object* rc;
  s_frame* f1;
  s_frame* f2;
  int yaw;
  unsigned long h1;
  unsigned long h2;

  if (!StartFlag)
    return;

  f1 = RedPointer;
  h1 = SamplePeriod - RedSample;
  f2 = f1 + 1;
  h2 = RedSample;

  rc = RedCar;

  rc->worldX = (unsigned int)((h1 * (unsigned long)f1->x + h2 * (unsigned long)f2->x) / SamplePeriod);
  rc->worldY = multdiv((int)h1, f1->y, SamplePeriod) + multdiv((int)h2, f2->y, SamplePeriod);
  rc->worldZ = (unsigned int)((h1 * (unsigned long)f1->z + h2 * (unsigned long)f2->z) / SamplePeriod);

  yaw = f1->yaw;

  if (abs(yaw - f2->yaw) > 4 * 180)
  {
    if (yaw < 4 * 0)
      yaw += 4 * 360;
    else
      yaw -= 4 * 360;
  }

  rc->yaw = ((int)h1 * yaw + (int)h2 * f2->yaw) / SamplePeriod;
  rc->pitch = ((int)h1 * f1->pitch + (int)h2 * f2->pitch) / SamplePeriod;
  rc->roll = ((int)h1 * f1->roll + (int)h2 * f2->roll) / SamplePeriod;

  if (++RedSample >= SamplePeriod)
  {
    RedSample = 0;
    RedPointer++;
  }
}

void
ReadBlueCar(void)
{
  s_object* bc;
  s_frame* f1;
  s_frame* f2;
  int yaw;
  unsigned long h1;
  unsigned long h2;

  if (!StartFlag)
    return;

  f1 = BluePointer;
  h1 = SamplePeriod - BlueSample;
  f2 = f1 + 1;
  h2 = BlueSample;

  bc = BlueCar;

  bc->worldX = (unsigned int)((h1 * (unsigned long)f1->x + h2 * (unsigned long)f2->x) / SamplePeriod);
  bc->worldY = multdiv((int)h1, f1->y, SamplePeriod) + multdiv((int)h2, f2->y, SamplePeriod);
  bc->worldZ = (unsigned int)((h1 * (unsigned long)f1->z + h2 * (unsigned long)f2->z) / SamplePeriod);

  yaw = f1->yaw;

  if (abs(yaw - f2->yaw) > 4 * 180)
  {
    if (yaw < 4 * 0)
      yaw += 4 * 360;
    else
      yaw -= 4 * 360;
  }

  bc->yaw = ((int)h1 * yaw + (int)h2 * f2->yaw) / SamplePeriod;
  bc->pitch = ((int)h1 * f1->pitch + (int)h2 * f2->pitch) / SamplePeriod;
  bc->roll = ((int)h1 * f1->roll + (int)h2 * f2->roll) / SamplePeriod;

  if (!TimeOutFlag)
  {
    if ((abs(bc->worldX - FinishX) < 200) && (abs(bc->worldZ - FinishZ) < 200) && (abs(bc->worldY - FinishY) < 40))
    {
      TimeFlag = FALSE;
      TimeOutFlag = TRUE;
      LapFinished = TRUE;
      MessageTime = msg_duration + 20;
      MessageCode = msg_lost;
      NewPhantomPhoton = FALSE;
      MakeSound(s_extratime);
    }
  }

  if (++BlueSample >= SamplePeriod)
  {
    BlueSample = 0;
    BluePointer++;
  }
}

void
TimeString(char* p, long time)
{
  char c;
  char i;

  p += 9;
  *p-- = '.';

  for (i = 0; i < 6; i++)
  {
    c = (char)time & 15;
    if (i == 5 && !c)
      c = ' ';
    *p-- = c;
    time >>= 4;

    if (i == 1 || i == 3)
      *p-- = 10;
  }
}

void
SetCar(int segment, int lane, int row, int side)
{
  s_track* st1;
  s_track* st2;
  unsigned int mx1;
  unsigned int my1;
  unsigned int mz1;
  unsigned int mx2;
  unsigned int my2;
  unsigned int mz2;
  unsigned int h1;
  unsigned int h2;
  int dx;
  int dy;
  int dz;
  int h;

  st1 = track1 + segment;
  st2 = st1 + 1;

  if (lane == 0)
  {
    h1 = 7;
    h2 = 1;
  }
  else
  {
    h1 = 5;
    h2 = 3;
  }

  if (UK_Flag)
    side ^= 1;

  if (side)
  {
    mx1 = (unsigned int)((h2 * (unsigned long)st1->x1 + h1 * (unsigned long)st1->x2) >> 3);
    my1 = (unsigned int)((h2 * (unsigned long)st1->y1 + h1 * (unsigned long)st1->y2) >> 3);
    mz1 = (unsigned int)((h2 * (unsigned long)st1->z1 + h1 * (unsigned long)st1->z2) >> 3);

    mx2 = (unsigned int)((h2 * (unsigned long)st2->x1 + h1 * (unsigned long)st2->x2) >> 3);
    my2 = (unsigned int)((h2 * (unsigned long)st2->y1 + h1 * (unsigned long)st2->y2) >> 3);
    mz2 = (unsigned int)((h2 * (unsigned long)st2->z1 + h1 * (unsigned long)st2->z2) >> 3);
  }
  else
  {
    mx1 = (unsigned int)((h2 * (unsigned long)st1->x2 + h1 * (unsigned long)st1->x1) >> 3);
    my1 = (unsigned int)((h2 * (unsigned long)st1->y2 + h1 * (unsigned long)st1->y1) >> 3);
    mz1 = (unsigned int)((h2 * (unsigned long)st1->z2 + h1 * (unsigned long)st1->z1) >> 3);

    mx2 = (unsigned int)((h2 * (unsigned long)st2->x2 + h1 * (unsigned long)st2->x1) >> 3);
    my2 = (unsigned int)((h2 * (unsigned long)st2->y2 + h1 * (unsigned long)st2->y1) >> 3);
    mz2 = (unsigned int)((h2 * (unsigned long)st2->z2 + h1 * (unsigned long)st2->z1) >> 3);
  }

  dx = mx2 - mx1;
  dy = my2 - my1;
  dz = mz2 - mz1;

  h = 2 * row + 1;

  car.x = car.mx = car.newX = mx1 + ratio(h, dx, 4);
  car.y = car.my = car.newY = my1 + ratio(h, dy, 4);
  car.z = car.mz = car.newZ = mz1 + ratio(h, dz, 4);

  car.preciseX = (long)car.newX << 3;
  car.preciseY = (long)car.newY << 3;
  car.preciseZ = (long)car.newZ << 3;

  car.yaw = car.newYaw = st1->yaw;
  car.pitch = car.newPitch = st1->pitch;
  car.roll = car.newRoll = st1->roll;
}

unsigned char
SetObjClass(s_object* obj)
{
  int vectors;
  int h;
  int* p;
  unsigned char class;

  h = ObjDataBase[obj->type];

  if (!h)
    return (FALSE);
  else
  {
    p = &ObjDataBase[h];

    obj->vertices = (unsigned int)*p++;
    obj->normals = (unsigned char)*p++;
    obj->polygons = (unsigned char)*p++;
    obj->type = (unsigned char)*p++;
    class = (unsigned char)*p++;
    obj->model = ++p;

    vectors = obj->vertices + obj->normals;
    obj->faces = (unsigned char*)&p[vectors + vectors + vectors];

    if (class && !obj->class)
      obj->class = class;

    return (TRUE);
  }
}

s_object*
SearchObject(int type)
{
  int i;
  s_object* obj;

  obj = object;

  for (i = NumOfObjects + 1; --i;)
  {
    if (obj->active)
    {
      if (obj->large == type)
      {
        obj->active = FALSE;
        return (obj);
      }
    }
    obj++;
  }

  return (object);
}

void
SetArrows(unsigned char status)
{
  int i;
  s_object* obj;

  obj = object;

  for (i = 0; i < NumOfObjects; i++)
  {
    if (obj->large == 144)
      obj->active = status;
    obj++;
  }
}

void
CopyCarPosition(s_object* obj)
{

  obj->worldX = car.mx;
  obj->worldY = car.my;
  obj->worldZ = car.mz;

  obj->yaw = -car.yaw;
  obj->pitch = car.pitch;
  obj->roll = car.roll;
}

void
PassCheckpoint(void)
{
  long bonus;

  if (BonusFlag || WrongDirection)
    return;

  if (LapMode)
  {
    BonusFlag = TRUE;
    return;
  }

  TimeFlag = TRUE;
  BonusFlag = TRUE;
  TimeOutFlag = FALSE;

  // MAKE BONUS TIME SOUND EFFECT.

  MakeSound(s_extratime);

  // ADD BONUS TIME.

  bonus = BonusCheckpoint;

  switch (Difficulty)
  {
    case 1:
      SubBCD(0x0003L, &bonus);
      break;
    case 2:
      SubBCD(0x0005L, &bonus);
      break;
    case 3:
      SubBCD(0x0007L, &bonus);
      break;
  }

  AddTime(bonus, &TimeOut);

  // DISPLAY MESSAGE.

  MessageTime = msg_duration;
  MessageCode = msg_extra;
}

void
PassFinish(void)
{
  char msg;
  long bonus;

  if (!BonusFlag || WrongDirection)
    return;

  if (LapMode)
  { // CHAMPIONSHIP LAP:

    TimeFlag = FALSE;
    BonusFlag = FALSE;

    if (!TimeOutFlag)
    { // YOU MADE IT !

      TimeOutFlag = TRUE;
      LapFinished = TRUE;
      NewPhantomPhoton = TRUE;
      MessageTime = msg_duration;
      MessageCode = msg_won;
      MakeSound(s_finish);
    }

    return;
  }

  BonusFlag = FALSE;
  msg = msg_extended;

  // CHECK CHAMPIONSHIP TIME TO QUALIFY.

  if (Time < TimeToQualify)
  {
    QualifyTime = Time;

    if (!QualifyFlag)
    { // QUALIFIED

      QualifyFlag = TRUE;
      msg = msg_qualified;
    }
    else
    { // ANOTHER GREAT LAP !

      msg = msg_great;
    }
  }

  // FINISH SOUND EFFECT.

  MakeSound(s_finish);

  // ADD TIME BONUS.

  bonus = BonusFinish;

  switch (Difficulty)
  {
    case 1:
      SubBCD(0x0003L, &bonus);
      break;
    case 2:
      SubBCD(0x0005L, &bonus);
      break;
    case 3:
      SubBCD(0x0007L, &bonus);
      break;
  }

  AddTime(bonus, &TimeOut);

  // RESET ALL RETURNPOINTS.

  ClearControlPoints();

  // START NEW LAP TIMING.

  LastLapTime = Time;
  Time = 0x0000L;
  TimeOutFlag = FALSE;
  TimeFlag = TRUE;

  // DISPLAY MESSAGE.

  MessageTime = msg_duration;
  MessageCode = msg;
}

int
StringLength(char* string)
{
  int count;

  for (count = 0; (*string != '.') && (count < 39);)
  {
    count++;
    string++;
  }

  return (count);
}

void
UpdateTime(void)
{

  if (TimeFlag)
  {
    DecCountDown();
    IncTime();
    if (OffRoadOut)
      DecOffRoad();
  }
}

void
PrintValue(int x, int y, int value)
{
  static char string[] = "00000.";
  int h;
  int num;
  int flag;
  char* p;

  if (value < 0)
  {
    PrintString(x, y, "M.");
    value = -value;
  }

  flag = TRUE;

  for (p = string, h = 10000; h > 0; h /= 10)
  {
    num = (unsigned int)value / (unsigned int)h;
    value %= h;

    if (flag && !num && (h > 1))
      *p++ = ' ';
    else
    {
      *p++ = num;
      flag = FALSE;
    }
  }

  PrintString(++x, y, string);
}

/*
void
FramePoly(int n)
{
  int i, *p;

  p = PolyVertex;
  MoveTo(p[2 * n - 2], p[2 * n - 1]);

  for (i = 0; i < n; i++)
  {
    DrawTo(p[0], p[1]);
    p += 2;
  }
}
*/
