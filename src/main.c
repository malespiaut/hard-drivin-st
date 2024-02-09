// #include <dos.h>
#include <stdio.h>

#include "ibm.h"
#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	main.c
 PROJECT..............	HARDDRIVING game program.

 DESCRIPTION..........	This module is the main module, from which
                        all other modules are called.

 DATE OF CREATION.....	March 22th 1989
 LAST CHANGE..........	October 18th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

//		GLOBAL VARIABLES :
//              ==================

// SYSTEM VARIABLES.

void* ScreenBuffer; // Pointer to   visible Screen Buffer.
void* Buffer2;      // Pointer to invisible Screen Buffer.

// HARD DRIVIN' VARIABLES.

void* AuxScreen;    // Pointer to the auxiliary screen.
void* MapScreen;    // Pointer to the map window.
void* AuxBuffer;    // Pointer to a auxiliary buffer.
void* Marcus;       // Pointer to 32k aux buffer.
void* Tim;          // Pointer to 32k aux buffer.
void* SoundModule;  // Pointer to sound routines.
void* SelectScreen; // Pointer to the select window.

char MapName[20]; // Various picture names.
char SelectName[20];
char LoadName[20];
char PanelName[20];

char SoundFX;      // Flag enables the sound effects.
char GraphMode;    // Choosen graph mode.
char SoundEnabled; // Flag indicates new sound to be loaded.
char EngineKilled; // Flag indicates engine sound was interrupted.
char RaceIsOn;     // Flag indicates that the race has started.
char EngineOn;     // Flag indicates engine is running.

char GameMode;         // Flag indicates end of the game.
char DemoMode;         // Flag indicates end of the demo mode.
char LapMode;          // Flag indicates that the championship lap is on.
char LapFinished;      // Flag indicates championship lap was finished.
char NewScreens;       // Flag indicates the use of the allocated screens.
char UK_Flag;          // Flag indicates traffic on the lefthand lane.
char QualifyFlag;      // Flag indicates proper qualify lap time.
char SwitchFlag;       // Flag indicates switching of the screen pages.
char BonusFlag;        // Flag indicates passing of the checkpoint.
char TimeOutFlag;      // Flag indicates the end of the count down.
char ReturnFlag;       // Flag indicates an offroad time out.
char CheckOffRoad;     // Flag indicates car is off road.
char CollisionFlag;    // Flag indicates a collision with a object.
char AccidentFlag;     // Flag indicates that the car is destroyed.
char SkidFlag;         // Flag indicates car is skiding (oversteering).
char SpinFlag;         // Flag indicates car is spinning off.
char OnObject;         // Flag indicates car is driving on an object.
char NewPhantomPhoton; // Flag indicates winning of championship lap.
char SloMoFlag;        // Flag indicates beginning of the Slow Mo sequence.
char WrongDirection;   // Flag indicates wrong driving direction.
char OnTrack;          // Flag indicates on which track the car travels.
char CarInLoop;        // Flag indicates car is in the loop.
char ExitWait;         // Flag indicates end of gear select mode.
char TimeFlag;         // Flag indicates whether the clock is running.
char StartFlag;        // Flag indicates race has started.
char StartMessage;     // Flag indicates printing of the start message.
char DirtyFlag;        // Flag indicates a hiscore file update.
char QuitFlag;         // Just what the name says.
char OpaqueFlag;       // Flag indicates hatched Polygon Draw Mode.
char ExtendedClip;     // Extended 3D clipping if pitch angle too large.
char CharNum;          // Choosen char during hiscore entry.
char PhantomLane;      // Start lane of the Phantom Photon.
char MessageTime;      // Message display duration.
char MessageCode;      // Message discription code.
char MouseFlag;        // Enables/Disables the mouse.
char ManualShift;      // Indicates manual up or down shift.
char Difficulty;       // Difficulty of game play.
char SteerSelect;      // Selects steering input device.
char GearSelect;       // Selects gear shift input device.

char RedSample;  // Subsampling step of red car's recording.
char BlueSample; // Subsampling step of blue car's recording.

char VisiStr1; // Number of visible street elements on track 1.
char VisiStr2; // Number of visible street elements on track 2.
char VisiLns1; // Number of visible lane markers on track 1.
char VisiLns2; // Number of visible lane markers on track 2.

char ScoreList[10 * 39]; // Array for Hiscore List.
char BestName[30];       // Best driver's name.
char BestTime[10];       // Best lap time.

// VARIABLES DEPENDING ON SCREEN RESOLUTION.

int OrgX;
int OrgY; // Screen Coordinates of the origin.
int MinX;
int MaxX;
int MinY;
int MaxY; // Current Window.
int MinY1;
int MaxY1;
int OrgY1; // Screen Window.
int MinY2;
int MaxY2;
int OrgY2; // Cockpit Window.
int ScreenX;
int ScreenY;  // Screen resolution.
int MessageY; // Y position of the message during demo mode.
int LogoY;    // Y position of the HardDrivin' logo.
int mphX;
int mphY; // Origin of the speedometer.
int rpmX;
int rpmY; // Origin of the rev gauge.
int ampX;
int ampY; // Origin of the amperemeter.
int tmpX;
int tmpY; // Origin of the thermometer.
int oilX;
int oilY; // Origin of the oil gauge.
int gasX;
int gasY;         // Origin of the gasometer.
int largePointer; // Length of a large gauge pointer.
int smallPointer; // Length of a small gauge pointer.
int ScoreX;
int ScoreY; // Offset to the Score field.
int TimeX;
int TimeY; // Offset to the Time field.
int BestX;
int BestY; // Offset to the Best field.
int LastX;
int LastY; // Offset to the Last field.
int SecondsX;
int SecondsY; // Offset to the Second field.
int OffRoadX;
int OffRoadY;    // Offset to the OffRoad field.
int ListX;       // Offset to the Hiscore field.
int MaxLine;     // Maximal offset.
int CharWidth;   // Width  of a large character.
int CharHeight;  // Height of a large character.
int LineHeight;  // Height of a line of large characters.
int DigitLength; // Length of a digit character.

// SYSTEM VARIABLES.

int hh1;
int hh2;
int hh3;
int hh4;
int hh5;
int hh6;

int button; // Joysticks/mouse button flags.
int SteerX;
int SteerY; // Steering coordinates.

int OffRoadOut; // Countdown: offroad
int frames;     // count of VBL's.
int MinVBL;     // Minimal number of VBL's for a new frame.

// VARIABLES TO MANAGE STREET ELEMENTS.

int StartSegment;    // Number of the start segment.	(Track1)
int StartLane;       // Start lane.
int StartRow;        // Start row.
int FinishSegment;   // Number of the finish segment. (Track1)
int CheckP1_segment; // Number of the checkpoint segment on track 1.
int CheckP2_segment; // Number of the checkpoint segment on track 2.
int* First1;
int* First2;       // Pointer to first visible street element.
int nearestStreet; // Distance of the nearest street.
int* LaneM1;
int* LaneM2;
int* Flags1;  // Flags for visible street elements on track 1.
int* Flags2;  // Flags for visible street elements on track 1.
int Streets1; // Number of street elements on track 1.
int Streets2; // Number of street elements on track 2.

int BestLength;      // Length of the best driver's name.
int BonusCheckpoint; // Bonus time when passing the checkpoint.
int BonusFinish;     // Bonus time when passing the finish.

int EnginePitch; // Depends on the engine revs.

int yawSIN;
int yawCOS;
int pitchSIN;
int pitchCOS;
int rollSIN;
int rollCOS; // trigonometric values of yaw, pitch and roll angles.

int LapViewX;
int LapViewY;
int LapViewZ; // Playback viewpoint after championship lap.

int FinishX;
int FinishY;
int FinishZ; // Finish coordinates. Used for the championship lap.

int NumOfObjects;    // Total number of objects in the HardDrivin' universe.
int NumOfVehicles;   // Total number of cars.
int NumOfBarriers;   // Total number of barriers.
int NumOfFields;     // Total number of fields.
int NumOfT1controls; // Total number of track1 control points.
int NumOfT2controls; // Total number of track2 control points.
int VisibleObjects;  // Number of visible objects in the next frame.

int* VertIndex;                         // Pointer to the rest of the vertex[] buffer.
int* CoorIndex;                         // Pointer to the rest of the coordinate[] buffer.
int* ObjDataBase;                       // Pointer to object data base.
int* Barriers;                          // Pointer to barrier coordinates.
int objTable[ObjTypes];                 // Offset table to object data base.
int coordinate[3 * MaxKoor];            // Buffer for 3D coordinates
int vertex[2 * (MaxKoor + 200)];        // Buffer for 2D coordinates
int Street1Vertex[MaxSegments * 3 * 2]; // Buffer for visible street elements of track 1.
int Street2Vertex[MaxSegments * 3 * 2]; // Buffer for visible street elements of track 2.
int ObjFace[3 * 3];                     // Object surface vectors used by car simulation model.
int TrackData[11000];                   // This array contains all track, control, vehicle data
int PolyVertex[MaxVertex];              // Vertex paramaters for polygon solid fill
int AuxVertex[MaxVertex];               // Aux. vertex paramaters for polygon solid fill

long Score;         // Current game score.
long HiScore;       // Just what the name says.
long Duration;      // Duration of the count down.
long Time;          // Current game time.
long TimeOut;       // End time of the count down.
long BestLapTime;   // Best lap time.
long LastLapTime;   // Last lap time.
long QualifyTime;   // Last lap time which qualified.
long TimeToQualify; // Just what the name says.

long* Pattern; // Pattern for solid fill routines.

s_block ScoreRect;
s_block TimeRect;
s_block MessageRect[3]; // Left  windows bit block description.
s_block MidRect;        // Mid   gauges  bit block description.
s_block AutoRect;       // Bit block for automatic gear display.
s_block ManualRect;     // Bit block for manual gear display.
s_block BarRect;        // Bit block for steering wheel position.
s_block GearARect[5];   // Bit block for 5 gears. (automatic)
s_block GearMRect[5];   // Bit block for 5 gears. (manual)

s_object* field;  // Pointer to field array.
s_object* object; // Array of all objects.
s_object* GreenLight;
s_object* YellowLight;
s_object* RedLight; // Pointer to traffic light objects.
s_object* RedCar;   // Pointer to own car. (Championship Lap)
s_object* BlueCar;  // Pointer to Phantom Photon's car. (Championship Lap)

s_vehicle* vehicle; // Cars on the track.

s_priority PrioList[MaxObj]; // Priority list of visible objects.

s_car car; // Player's car data.

s_score HiScoreList[10]; // HiScore list.

s_frame* RedPointer;  // Pointer used to record red car's movement.
s_frame* BluePointer; // Pointer used to record blue car's movement.
s_frame* OldChampLap; // Array holds Phanton Photon's lap.
s_frame* NewChampLap; // Array holds the challenger's lap.

s_playback SlowMotion[MaxFrame]; // Array for last MaxFrame frames. (Slow Motion).
s_playback* FramePos;            // Pointer to current frame in slow motion array.

s_track* StreetUnderCar; // Pointer to street element in use for demo drive.
s_track* track1;         // Pointer to street elements of track1.
s_track* track2;         // Pointer to street elements of track2.
s_track* Junct1;
s_track* Junct2; // Pointer to both junctions on track1.
s_track* End1;
s_track* End2;       // Pointer to last street segment + 1 on both tracks.
s_track* NextStreet; // Pointer to nearest street element.

s_control* NextCtr1;     // Next control point to pass on track 1.
s_control* NextCtr2;     // Next control point to pass on track 2.
s_control* T1ContPoints; // Speed track control points.
s_control* T2ContPoints; // Stunt track control points.

s_BitImage* FlagYellow;
s_BitImage* FlagRed;      // Two flags to show last lap run.
s_BitImage* ChampLogo;    // Championship logo.
s_BitImage* Cup;          // Championship cup.
s_BitImage* Explosion[6]; // Explosion shapes.
s_BitImage* Mountain[10]; // BitMap description of the mountain shapes.

//		FUNCTIONS :
//              ===========

static void
DemoRun(void)
{
  static char Message;
  int i;

  // SKIP, IF NEW GAME REQUIRED.

  if (!DemoMode)
    return;

  // NO CARS !

  DisableVehicles();

  TimeFlag = FALSE;
  QualifyFlag = FALSE;
  AccidentFlag = FALSE;
  GameMode = FALSE;
  LapMode = FALSE;
  SloMoFlag = FALSE;

  // SET DEMO TEXT TO START POSITION.

  MessageY = 200;

  // FIND NEAREST STREET SEGMENT.

  MoveCar();
  SetNearestElement();

  // PRODUCE FIRST DEMO SCREEN TO BLEND IN.

  MoveCar();
  FindAllVisibleObjects();
  CompAllObjects();
  PrintBackground();
  PrintAllObjects();

  // BLEND FIRST SCREEN IN.

  UpdateTime();
  Switch();
  RealToDummy();

  // DEMO.

  TimeFlag = TRUE;

  for (i = 0; i < 1000; i++)
  {
    GetPlayerInput();

    if (!DemoMode)
      break;

    MoveCar();

    FindAllVisibleObjects();
    CompAllObjects();

    PrintBackground();
    PrintAllObjects();
    PrintCockpit();
    PrintOffRoad();

    if (Message)
      PrintChampion();
    else
      PrintHiscoreList();

#if (0)
    {

      SetTextColor(white);
      PrintValue(1, 10, (unsigned int)VisiStr1);
      PrintValue(1, 20, (unsigned int)Streets1);
      PrintValue(18, 10, (unsigned int)VisiStr2);
      PrintValue(18, 20, (unsigned int)Streets2);
      PrintValue(1, 30, (unsigned int)VisibleObjects);
      PrintValue(1, 40, (unsigned int)QualifyFlag);
      PrintValue(1, 50, (unsigned int)DemoMode);
    }
#endif

    UpdateTime();
    Switch();
  }

  Message ^= TRUE;
}

static void
GotoReturnPoint(void)
{
  int h;
  s_control* cn;
  s_track* str;
  static unsigned char control = 0;
  static s_track* street;

#define end 8

  if (!ReturnFlag)
    return;

  if (!control)
  { //  RESET CAR

    car.GravityY = 0;
    car.ImpulseX = 0;
    car.ImpulseY = 0;
    car.ImpulseZ = 0;
    car.deltaSpin = 0;
    car.wheelSpeed = 0;
    car.speed = 0;
    car.throttle = 0;
    car.rpm = 0;
    car.BrakeFlag = FALSE;
    car.ImpulseFlag = FALSE;

    OffRoadOut = 0;

    StopSound();

    StartFlag = 0;
    EngineOn = FALSE;
    AccidentFlag = FALSE;
    SpinFlag = FALSE;

    //  FIND LAST RETURN POINT PASSED.

    if (NextCtr1 - T1ContPoints > NextCtr2 - T2ContPoints)
    {
      cn = NextCtr1 - 1;
      if (cn < T1ContPoints)
        cn = T1ContPoints;
      NextStreet = street = &track1[cn->segment];
    }
    else
    {
      cn = NextCtr2 - 1;
      if (cn < T2ContPoints)
        cn = T2ContPoints;
      NextStreet = street = &track2[cn->segment];
    }
  }

  str = street;

  if (UK_Flag)
  {
    car.newX = (unsigned int)((3 * (unsigned long)str->x1 + (unsigned long)str->x2) >> 2);
    car.newY = (unsigned int)((3 * (unsigned long)str->y1 + (unsigned long)str->y2) >> 2);
    car.newZ = (unsigned int)((3 * (unsigned long)str->z1 + (unsigned long)str->z2) >> 2);
  }
  else
  {
    car.newX = (unsigned int)((3 * (unsigned long)str->x2 + (unsigned long)str->x1) >> 2);
    car.newY = (unsigned int)((3 * (unsigned long)str->y2 + (unsigned long)str->y1) >> 2);
    car.newZ = (unsigned int)((3 * (unsigned long)str->z2 + (unsigned long)str->z1) >> 2);
  }

  h = str->yaw;

  if (h > 4 * 180)
    h -= 4 * 360;
  else if (h < -4 * 180)
    h -= 4 * 360;

  car.mx = car.newX;
  car.my = car.newY;
  car.mz = car.newZ;

  car.newYaw = car.yaw = h;
  car.newPitch = car.pitch = str->pitch;
  car.newRoll = car.roll = 0;

  car.deltaPitch = car.deltaRoll = 0;

  car.throttle = car.speed = 0;
  car.vx = car.vy = car.vz = 0;
  car.ImpulseX = car.ImpulseY = car.ImpulseZ = 0;

  car.newY += 4 * 5 * (end - control);

  car.preciseX = ((long)car.newX) << 3;
  car.preciseY = ((long)car.newY) << 3;
  car.preciseZ = ((long)car.newZ) << 3;

  StopSound();

  if (++control > end)
  {
    control = 0;
    ReturnFlag = FALSE;
    ClearButtons();
  }

  SetNearestElement();
  CenterMouse();
  SpinFlag = FALSE;

#undef end
}

static void
ShowSlowMotion(void)
{
  int i, j, frames, *p;
  s_playback* fr;
  s_vehicle* vh;
  s_object* obj;
  s_BitImage* expl;
  char soundFlag;
  int ysin, ycos, psin, pcos, dx, dy, dz;
  char count1, count2;

  if (!SloMoFlag)
    return;

  StopEngine();
  StopSound();

  // ACTIVATE RED CAR OBJECT AND STOP TIMER.

  RedCar->active = TRUE;
  RedCar->color = red1;
  TimeFlag = FALSE;
  soundFlag = TRUE;

  // SET FRAME POINTER TO START OF RECORDING.

  fr = FramePos - MaxFrame;
  if (fr < SlowMotion)
    fr += MaxFrame;

  // SKIP EMPTY SECTION.

  for (frames = MaxFrame; frames > 0;)
  {
    if (!(fr->x | fr->y | fr->z))
    {
      frames--;
      fr++;
    }
    else
      break;

    if (fr >= &SlowMotion[MaxFrame])
      fr = SlowMotion;
  }

  // SET STREET POINTER TO NEAREST STREET SEGMENT.

  car.mx = fr->x;
  car.my = fr->y;
  car.mz = fr->z;
  SetNearestElement();

  // START PLAYBACK.

  ClearButtons();

  expl = Explosion;
  count1 = 0;
  count2 = 0;

  for (i = 0; (i < frames) && !button; i++)
  {
    // READ RECORDED CAR POSITION

    RedCar->yaw = -fr->yaw;
    RedCar->pitch = -fr->pitch;
    RedCar->roll = -fr->roll;

    RedCar->worldX = car.newX = fr->x;
    RedCar->worldY = car.newY = fr->y;
    RedCar->worldZ = car.newZ = fr->z;

    EnginePitch = fr->engine;

    if (fr->engine & 0x8000)
      MakeSound(s_squeal);

    // POSITION VIEWPOINT

    ysin = sinus(fr->yaw + 4 * 90);
    ycos = cosinus(fr->yaw + 4 * 90);
    psin = sinus(4 * 30);
    pcos = cosinus(4 * 30);

#define dist 700

    dy = rot1(0, dist, psin, pcos);
    dz = rot2(0, dist, psin, pcos);
    dx = rot1(0, dz, ysin, ycos);
    dz = rot2(0, dz, ysin, ycos);

#undef dist

    car.newX -= dx;
    car.newY -= dy;
    car.newZ += dz;

    car.preciseX = (long)car.newX << 3;
    car.preciseY = (long)car.newY << 3;
    car.preciseZ = (long)car.newZ << 3;

    car.newYaw = fr->yaw - 4 * 90;
    car.newPitch = -4 * 28;
    car.newRoll = 0;

    // SET CARS.

    vh = vehicle;
    p = fr->carpos;

    for (j = NumOfVehicles + 1; --j;)
    {
      obj = (vh++)->obj;
      obj->worldX = (unsigned int)*p++;
      obj->worldY = (int)*p++;
      obj->worldZ = (unsigned int)*p++;
      obj->yaw = *p++;
      obj->pitch = *p++;
      obj->roll = *p++;
    }

    // PRINT FRAME

    GetPlayerInput();
    MoveCar();
    FindAllVisibleObjects();
    CompAllObjects();

    PrintBackground();
    PrintAllObjects();

    if (i >= (frames - (AccidentDelay + 1)))
    {
      DrawShape((Random() >> 14) - 4, (Random() >> 14) + 10, expl);

      if (++count1 > 2)
      {
        count1 = 0;
        if (++count2 < 6)
          expl++;
        else
          expl--;
      }

      if (soundFlag)
      {
        MakeSound(s_crash);
        soundFlag = FALSE;
      }
    }

    StartEngine();

    PrintMessage();
    PrintCockpit();

    SetTextColor(white);
    PrintString(12, 10, "INSTANT REPLAY.");

    UpdateTime();
    Switch();

    // NEXT FRAME

    if (++fr >= &SlowMotion[MaxFrame])
      fr = SlowMotion;
  }

  // RESET FRAME ARRAY.

  InitRecord();

  // DEACTIVATE RED CAR, START TIMER

  SloMoFlag = FALSE;
  RedCar->active = FALSE;
  AccidentFlag = FALSE;
  SpinFlag = FALSE;
  SkidFlag = FALSE;
  car.ImpulseFlag = FALSE;
  TimeFlag = TRUE;
  ReturnFlag = TRUE;

  ClearButtons();
  StopSound();

  // FINISH CHAMPIONSHIP LAP.

  LapMode = FALSE;
  LapFinished = FALSE;

  // PUT INTO NEUTRAL, IF MANUAL SHIFT.

  if (!car.autoFlag)
  {
    car.ShiftFlag = 5;
  }
}

static void
GameRun(void)
{
  int i;

  ClearButtons();
  CenterMouse();
  ResetKeys();

  while (GameMode)
  {
    // CHECK FOR TIME RUNNING OUT.

    if (TimeOutFlag)
    {
      if (car.speed == 0)
        GameMode = FALSE;
      TimeFlag = FALSE;
    }

    // WAS THERE AN ACCIDENT ?

    if (AccidentFlag)
    {
      if (AccidentFlag == 1)
      {
        MessageTime = 0;
        SloMoFlag = TRUE;
      }
      AccidentFlag--;
    }

    GetPlayerInput();
    GotoReturnPoint();

    MoveCar();
    MoveVehicles();
    FindAllVisibleObjects();

    CompAllObjects();
    CarModel();

    PrintBackground();
    PrintAllObjects();

    if (AccidentFlag)
      PrintBrokenWindow();
    PrintCountDown();
    PrintOffRoad();
    PrintMessage();
    PrintCockpit();
    PrintScore();

    ShowSlowMotion();

#if (0)
    {

      SetTextColor(white);
      PrintValue(1, 10, hh1);
      // PrintValue( 1, 20, (unsigned int) Streets1);
      // PrintValue( 1, 30, (unsigned int) QualifyFlag);
    }
#endif

    UpdateTime();
    Switch();
  }

  // GAME OVER SEQUENCE

  StopSound();
  TimeFlag = FALSE;

  for (i = 0; i < 20 + 15; i++)
  {
    int h;

    GetPlayerInput();
    FindAllVisibleObjects();
    CompAllObjects();

    PrintBackground();
    PrintAllObjects();
    if (AccidentFlag)
      PrintBrokenWindow();

    h = (i > 20) ? 20 : i;
    SetTextColor(white);
    PrintString(36 - h, 70, "GAME.");
    PrintString(1 + h, 70, "OVER.");

    PrintOffRoad();
    PrintCockpit();
    PrintScore();

    UpdateTime();
    Switch();
  }

  ClearButtons();
  StopSound();
}

static void
ChampPlayBack(void)
{
  s_frame *redpos, *bluepos;
  s_object* winner;
  int yaw, dx, dz, distance, lastdistance;

  StopSound();

  // ACTIVATE RED CAR.

  RedCar->active = TRUE;

  // WHO HAS WON ?

  winner = (NewPhantomPhoton) ? RedCar : BlueCar;

  // REWIND FRAME POINTER TO SHOW THE LAST SECONDS.

  redpos = RedPointer;
  bluepos = BluePointer;

  BluePointer -= 30;
  RedPointer -= 30;

  if (BluePointer < OldChampLap)
    BluePointer = OldChampLap;
  if (RedPointer < NewChampLap)
    RedPointer = NewChampLap;

  // SET STREET POINTER TO NEAREST STREET SEGMENT.

  car.newX = car.mx = car.x = LapViewX;
  car.newY = car.my = car.y = LapViewY;
  car.newZ = car.mz = car.z = LapViewZ;

  car.newPitch = car.pitch = -5;
  car.newRoll = car.roll = 0;

  pitchSIN = sinus(car.pitch);
  pitchCOS = cosinus(car.pitch);
  rollSIN = sinus(car.roll);
  rollCOS = cosinus(car.roll);

  car.preciseX = car.newX << 3;
  car.preciseY = car.newY << 3;
  car.preciseZ = car.newZ << 3;

  SetNearestElement();
  lastdistance = 1000;

  // START PLAYBACK.

  while ((BluePointer < bluepos - 1) && (RedPointer < redpos - 1))
  {
    // READ RECORDED CAR POSITIONS.

    ReadRedCar();
    ReadBlueCar();

    // VIEWDIRECTION

    dx = winner->worldX - car.newX;
    dz = winner->worldZ - car.newZ;

    distance = Sqrt(mult(dx, dx) + mult(dz, dz));

    // GET THE OLD DOPPLER EFFECT WORKING.

    EnginePitch = 800 - ((distance - lastdistance) << 3);
    lastdistance = distance;

    StartEngine();

    yaw = arcsin(dx, distance);

    if (dz < 0)
    {
      if (yaw > 0)
        yaw = 4 * 180 - yaw;
      else
        yaw = -4 * 180 - yaw;
    }
    car.yaw = car.newYaw = yaw;

    yawSIN = sinus(car.yaw);
    yawCOS = cosinus(car.yaw);

    // PRINT FRAME

    GetPlayerInput();
    FindAllVisibleObjects();
    CompAllObjects();

    PrintBackground();
    PrintAllObjects();
    PrintCockpit();
    PrintScore();

    UpdateTime();
    Switch();
  }

  // DEACTIVATE RED CAR.

  RedCar->active = FALSE;

  // RESTORE OLD POINTER VALUES.

  RedPointer = redpos;
  BluePointer = bluepos;
}

static void
ChampTitle(void)
{
  int i, ysin, ycos, psin, pcos;
  int dx, dy, dz, yaw, pitch;

#define deltaYaw -4 * 152
#define deltaPitch 4
#define length 800

  ClearButtons();

  SetCar(StartSegment, 0, 1, (PhantomLane ^ 1));
  SetNearestElement();

  yaw = car.newYaw + deltaYaw;
  pitch = car.newPitch + deltaPitch;

  // POSITION VIEWPOINT

  ysin = sinus(yaw);
  ycos = cosinus(yaw);
  psin = sinus(pitch);
  pcos = cosinus(pitch);

  dy = rotate(length, psin);
  dz = rotate(length, pcos);
  dx = rotate(dz, ysin);
  dz = rotate(dz, ycos);

  dy += 80;

  car.newX += dx;
  car.preciseX = (long)car.newX << 3;
  car.newY += dy;
  car.preciseY = (long)car.newY << 3;
  car.newZ += dz;
  car.preciseZ = (long)car.newZ << 3;

  car.newYaw = yaw - 4 * 180;
  car.newPitch = -pitch;

  // NORMALIZE ANGLE.

  if (car.newYaw > 4 * 180)
    car.newYaw -= 4 * 360;
  else if (car.newYaw < -4 * 180)
    car.newYaw += 4 * 360;

  MoveCar();
  FindAllVisibleObjects();
  CompAllObjects();

  PrintBackground();
  PrintAllObjects();

  SetTextColor(black);
  PrintString((40 - BestLength) >> 1, 5, BestName);

  SetTextColor(white);
  TypeString(30, 15, "CHALLENGES YOU TO THE#");

  DrawShape(0, -12, ChampLogo);
  DrawShape(0, 12, Cup);

  TypeString(3, 97, "NO CRASHING#");
  TypeString(3, 106, "ONLY TEN SECONDS OFFROAD#");
  TypeString(3, 115, "IF YOU WIN[ FUTURE#");
  TypeString(3, 124, "CHALLENGERS RACE YOU#");

  UpdateTime();
  Switch();
  RealToDummy();

  LogoY += 13;

  for (i = 15; i < 105; i += 3)
  {
    GetPlayerInput();
    ScaleLogo(i);
    SolidColor(blue);
    Box(-70, LogoY - 10, 70, LogoY + 10);
    DrawLogo();
    PrintCockpit();
    PrintScore();
    UpdateTime();
    Switch();
  }

  InitTune(TuneSpeed, 0);
  ClearButtons();
  for (; !(button | KeyPressed());)
    RefreshTune();

  StopSound();
  ClearButtons();
  LogoY -= 13;

#define count 7

  dx = ratio(dx, 1, count + 1);
  dy = ratio(dy, 1, count + 1);
  dz = ratio(dz, 1, count + 1);

  for (i = 0; i < count; i++)
  {
    car.newX -= dx;
    car.newY -= dy;
    car.newZ -= dz;

    car.newPitch += 1;
    car.newYaw -= (4 * 180 + deltaYaw) / count;

    car.preciseX = (long)car.newX << 3;
    car.preciseY = (long)car.newY << 3;
    car.preciseZ = (long)car.newZ << 3;

    MoveCar();
    FindAllVisibleObjects();
    CompAllObjects();

    PrintBackground();
    PrintAllObjects();
    PrintCockpit();
    PrintScore();

    UpdateTime();
    Switch();
  }

  // DISABLE RED CAR AGAIN.

  RedCar->active = FALSE;

  StopSound();

#undef deltaYaw
#undef deltaPitch
#undef length
#undef count
}

static void
ChampRace(void)
{
  GetPlayerInput();

  // CHECK IF A CAR HAS REACHED THE FINISH.

  if (TimeOutFlag)
  {
    if (car.speed == 0)
    {
      LapMode = FALSE;
    }
    TimeFlag = FALSE;
  }

  // WAS THERE AN ACCIDENT ?

  if (AccidentFlag)
  {
    if (AccidentFlag == 1)
    {
      SloMoFlag = TRUE;
      MessageTime = msg_duration + 20;
      MessageCode = msg_disqualified;
    }
    AccidentFlag--;
  }

  MoveCar();

  RecordRedCar();
  ReadBlueCar();

  FindAllVisibleObjects();
  CompAllObjects();
  CarModel();

  PrintBackground();
  PrintAllObjects();

  if (AccidentFlag)
    PrintBrokenWindow();

  PrintCountDown();
  PrintOffRoad();
  PrintMessage();

  PrintCockpit();
  PrintScore();
  ShowSlowMotion();
}

static void
ChampionChipLap(void)
{
  char SaveUK_flag;

  SaveUK_flag = UK_Flag;
  LapFinished = FALSE;
  NewPhantomPhoton = FALSE;

  // SET CHAMPIONSHIP FLAGS.

  QualifyFlag = FALSE;
  AccidentFlag = FALSE;
  DemoMode = FALSE;
  StartFlag = 0;
  UK_Flag = FALSE;
  TimeFlag = FALSE;
  TimeOutFlag = FALSE;
  BonusFlag = FALSE;
  EngineOn = FALSE;
  RaceIsOn = FALSE;
  ReturnFlag = FALSE;

  LapMode = TRUE;
  StartMessage = TRUE;

  MessageTime = 0;
  MessageCode = 0;
  Time = 0x0000L;

  StopSound();

  // TRAFFIC LIGHT IS RED.

  GreenLight->active = FALSE;
  YellowLight->active = FALSE;
  RedLight->active = TRUE;

  // INIT SOLID RED AND HATCHED BLUE CAR.

  DisableVehicles();
  RedCar->active = TRUE;
  BlueCar->active = TRUE;
  SetCar(StartSegment, 0, 1, (PhantomLane ^ 1));
  CopyCarPosition(RedCar);
  SetCar(StartSegment, 0, 1, (PhantomLane));
  CopyCarPosition(BlueCar);

  // ACTIVATE ARROW SIGNS TO SHOW THE WAY.

  SetArrows(TRUE);

  // DISPLAY START SCREEN.

  ChampTitle();

  // INIT CAR.

  SetCar(StartSegment, 0, 1, (PhantomLane ^ 1));

  car.deltaPitch = 0;
  car.deltaRoll = 0;
  car.deltaSpin = 0;

  car.rpm = 0;
  car.mph = 0;
  car.speed = car.wheelSpeed = 0;

  car.BrakeFlag = FALSE;
  car.ImpulseFlag = FALSE;
  car.ShiftFlag = FALSE;

  car.gear = (car.autoFlag) ? 1 : 0;
  car.throttle = 0;
  car.gearBlock = (car.autoFlag) ? GearARect : GearMRect;
  car.gearBlock += car.gear;

  StopEngine();
  StopSound();
  SetNearestElement();

  // REWIND CHAMPIONSHIP RECORDING.

  RedPointer = NewChampLap;
  RedSample = 0;
  BluePointer = OldChampLap;
  BlueSample = 0;

  // INIT SETTING.

  OffRoadOut = 0x1099;
  TimeOut = Duration;
  TimeFlag = TRUE;

  ClearButtons();
  CenterMouse();

  while (LapMode)
  {
    ChampRace();
    UpdateTime();
    Switch();
  }

  // PLAYBACK OF LAST SECONDS.

  if (LapFinished)
    ChampPlayBack();

  // DEACTIVATE ARROW SIGNS.

  SetArrows(FALSE);

  // DEACTIVATE BLUE AND RED CAR.

  EnableVehicles();
  RedCar->active = FALSE;
  BlueCar->active = FALSE;

  // SAVE NEW CHAMPION'S LAP.

  if (NewPhantomPhoton)
  {
    s_frame *fr1, *fr2;

    PhantomLane ^= 1;

    fr1 = OldChampLap;
    fr2 = NewChampLap;

    while (fr2 < RedPointer)
      *fr1++ = *fr2++;
    fr2--;
    while (fr1 < &OldChampLap[MaxSample])
      *fr1++ = *fr2;
  }

  // RESTORE UK FLAG.

  UK_Flag = SaveUK_flag;

  StopSound();
}

static void
PlayAGame(void)
{
  ChooseTransmission();

  GameInit();
  GameRun();

  if (QualifyFlag)
  {
    ChampionChipLap();

    if (NewPhantomPhoton)
    {
      // TimeToQualify = Time;
      // AddTime( 0x0001, &TimeToQualify);

      // if (Time < BestLapTime)
      {
        BestLapTime = Time;
        LapUpdate();
      }
    }
    else
      ScoreUpdate();
  }
  else
  {
    ShowRestOfLap();
    AddTime(0x0001, &TimeToQualify);
    ScoreUpdate();
  }

  if (TimeToQualify > 0x020000L)
    TimeToQualify = 0x020000L;

  DemoMode = TRUE;
  GameMode = FALSE;
}

int
main(void)
{
  char ever = TRUE;

  SystemInit();
  HardDrivingInit();

  for (; ever;)
  {
    while (DemoMode)
    {
      ShowTitle();
      DemoRun();
    }
    PlayAGame();
  }

  SystemExit("");

  return (0);
}
