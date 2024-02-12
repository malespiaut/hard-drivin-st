#include "data.h"
#include "ibm.h"
#include "main.h"
#include "missing.h"
#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	attract.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes all functions
                        for the attract mode.

 DATE OF CREATION.....	April 12th 1989
 LAST CHANGE..........	November 13th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

//		LOCAL VARIABLES :
//              =================

#define step 3
#define wait 100 * step
#define start 3 * step
#define scale 70 * step
#define stay 750 * step

#define LineHeight 8
#define TextX 1

static char TextSelect; // Used to select text 1..3.
static char EndFlag;    // Flag indicates end of the selected text.
static char* text;      // Pointer to current text selection.
static char* cursor;    // Pointer to current line position.

static char startline[4] = // First line of a given text.
  {
    0,
    4,
    7,
    11,
};

static char line[81]; // Text line.

/*	ORIGINAL SCRIPT OF THE COINOP
        LEGEND:		\\ : .
                         ] : '
                         [ : ,

*script[] = {	"Above you can see a map of the HARD DRIVIN] racetrack[ composed of an outer",
                "SPEED track for testing your high speed driving skills[ and an inner STUNT",
                "track for the ultimate driving challenge\\#",

                "HARD DRIVIN] is a revolutionary driving simulation combining the accurate",
                "model of race car handling with true _d graphics and force feedback controls\\",
                "You can]t get closer to the thrill of real racing without a fire^proof suit\\#",

                "Your HARD DRIVIN] racer is modelled after a popular sports car but with a more",
                "powerful engine and better tires\\ Many of the stunts and maneuvers possible",
                "in this game should not be tried in a real car[ since that would be extremely",
                "dangerous or fatal\\#",

                "The red flag on the course marks the last point passed\\ Race to the",
                "checkpoint for extra time[ and the finish line for extended play\\#"
            };
*/

static char* script[] = {"Above you can see a map of the HARD DRIVIN] racetrack[ composed of an outer",
                         "SPEED track for testing your high speed driving skills[ and an inner STUNT",
                         "track for the ultimate driving challenge\\",
                         "    Look out for exciting new HARD DRIVIN] racetracks in spring next year\\#",

                         "HARD DRIVIN] is a revolutionary driving simulation combining the accurate",
                         "model of race car handling with true _d graphics\\ You can]t get closer to",
                         "the thrill of real racing without a fire^proof suit\\#",

                         "Your HARD DRIVIN] racer is modelled after a popular sports car but with a more",
                         "powerful engine and better tyres\\ Many of the stunts and manoeuvers possible",
                         "in this game should not be tried in a real car[ since that would be extremely",
                         "dangerous or fatal\\#",

                         "The red flag on the course marks the last point passed\\ Race to the",
                         "checkpoint for extra time[ and the finish line for extended play\\#"};

static int StartY; // Text start line.

static int starty[] = {
  111 - LineHeight,
  111,
  111 - LineHeight,
  111 + LineHeight,
};

static int y; // Cursor position.

//		FUNCTIONS :
//              ===========

// **********************************************************************
// ********  T I T L E    S C R E E N  **********************************
// **********************************************************************

void
ScaleLogo(int factor)
{
  int i;
  int y;
  int* s;
  int* d;

  if (factor < start)
    return;

  if (factor > scale)
    factor = scale;

  s = HardLogo;
  d = coordinate;

  y = LogoY;

  for (i = 0; i < 40; i++)
  {
    *d++ = ratio(*s++, factor, scale);
    *d++ = ratio(*s++, factor, scale) + y;
    *d++ = ratio(*s++, factor, scale);
    *d++ = ratio(*s++, factor, scale) + y;
    *d++ = ratio(*s++, factor, scale);
    *d++ = ratio(*s++, factor, scale) + y;

    d[0] = d[-2] + (d[-6] - d[-4]);
    s++;
    d++;

    *d++ = ratio(*s++, factor, scale) + y;
  }
}

static void
DrawBackground(int time)
{
  int y;
  int dy;

  if (time > scale + 2 * step)
    return;

  y = OrgY + LogoY;
  dy = HardLogo[5];

  CopySmallScreen(y + dy, y - dy, MapScreen, Buffer2);
}

void
DrawLogo(void)
{
  int i;
  int* p;
  int koor[4];

  SolidColor(darkred);
  NewColor(black);

  p = coordinate;

  for (i = 0; i < 40; i++)
  {
    PolyVertex[0] = *p++;
    PolyVertex[1] = *p++;
    PolyVertex[2] = *p++;
    PolyVertex[3] = *p++;
    PolyVertex[4] = *p++;
    PolyVertex[5] = *p++;
    PolyVertex[6] = *p++;
    PolyVertex[7] = *p++;

    koor[0] = PolyVertex[0];
    koor[1] = PolyVertex[1];
    koor[2] = PolyVertex[2];
    koor[3] = PolyVertex[3];

    Polygon(4);

    MoveTo(koor[0], koor[1]);
    DrawTo(koor[2], koor[3]);
  }
}

static void
TypeText(int time)
{
  char ch;
  static char halt;

  if ((time < scale) || EndFlag)
    return;

  TypeString(TextX, StartY + y, line);

  if (halt++)
  {
    ch = *text++;
    *cursor++ = ch;
    *cursor = '#';
    halt = 0;

    if (ch == '#')
    {
      EndFlag = TRUE;
      return;
    }

    if (!ch)
    {
      cursor = line;
      line[0] = '#';
      y += LineHeight;
    }
  }
}

static void
TitleScreen(void)
{
  int i;

  for (i = start - wait; (i < scale + stay) && DemoMode; i += step)
  {
    GetPlayerInput();
    ScaleLogo(i);

    DrawBackground(i);
    if ((i >= start) && (i <= scale + 2 * step))
      DrawLogo();
    TypeText(i);

    Switch();
  }
}

void
ShowTitle(void)
{
  int height;

  SetTextColor(yellow);

  if (++TextSelect > 3)
    TextSelect = 1;

  text = script[startline[TextSelect - 1]];
  StartY = starty[TextSelect - 1];
  cursor = line;
  *cursor = '#';

  y = 0;
  EndFlag = FALSE;

  height = OrgY + MaxY;

  TimeFlag = FALSE;

  CopySmallScreen(0, height, MapScreen, Buffer2);
  Time = Score = 0;
  PrintCockpit();

  Switch();
  RealToDummy();

  TitleScreen();
}

#undef wait
#undef start
#undef stay

// **********************************************************************
// ********  L A S T   L A P  *******************************************
// **********************************************************************

#define duration 5

static char Track1Used;
static int max1;

static void
DisplayFlags(int time)
{
  int i;
  int h;
  int max;
  s_control* cp;
  s_BitImage* flag;

  if ((time < 0) || (time >= max1))
    return;

  if (Track1Used)
  {
    cp = T1ContPoints;
    max = NumOfT1controls - 1;
  }
  else
  {
    cp = T2ContPoints;
    max = NumOfT2controls - 1;
  }

  h = time / duration;

  for (i = 0; i < h; i++)
  {
    if (cp->covered)
    {
      if (!(cp + 1)->covered || (i >= max))
        flag = FlagRed;
      else
        flag = FlagYellow;
      if (cp->covered != 0xFE)
      {
        MakeSound(s_flag);
        cp->covered = 0xFE;
      }
      DrawShape(cp->screenX, cp->screenY, flag);
    }
    cp++;
  }
}

static void
ShowFlags(void)
{
  int i;
  int max2;

  max2 = max1 + 400;
  ClearButtons();
  ExitWait = FALSE;

  for (i = -30; (i < max2) && !ExitWait && !button; i++)
  {
    GetPlayerInput();

    DisplayFlags(i);

    if (i > max1)
      TypeText(scale + i);

    Switch();
  }

  ClearButtons();
  StopSound();
}

void
ShowRestOfLap(void)
{
  int height;
  int color;
  int h;

  SetTextColor(yellow);

  text = script[startline[4 - 1]];
  StartY = starty[3];
  cursor = line;
  *cursor = '#';

  y = 0;

  EndFlag = FALSE;
  height = OrgY + MaxY;
  TimeFlag = FALSE;

  Track1Used = (NextCtr1 - T1ContPoints > NextCtr2 - T2ContPoints);

  color = SetNewColor(grey2 - 1, black);
  h = (Track1Used) ? NumOfT1controls : NumOfT2controls;
  max1 = duration * (h + 1);

  CopySmallScreen(0, height, MapScreen, Buffer2);
  PrintCockpit();
  Switch();
  RealToDummy();

  ShowFlags();

  CopySmallScreen(0, height, MapScreen, Buffer2);
  SetNewColor(grey2 - 1, color);
  Switch();
}

#undef duration

// **********************************************************************
// ********  C H O O S E   T R A N S M I S S I O N  *********************
// **********************************************************************

static int
DrawArrowAndBox(void)
{
  int x;

#define y -37

#define dy 9
#define dy1 3
#define dx1 5
#define dx2 (9 + dy1)

  SolidColor(black);
  NewColor(white);

  x = multdiv((SteerX >> 1) - OrgX, 22, 30);

  MoveTo(x - dx1, y - dy);
  DrawTo(x + dx1, y - dy);
  DrawTo(x + dx1, y - dy1);
  DrawTo(x + dx2, y - dy1);
  DrawTo(x, y + dy);
  DrawTo(x - dx2, y - dy1);
  DrawTo(x - dx1, y - dy1);
  DrawTo(x - dx1, y - dy);
  MoveTo(x - dx1, y - dy1);
  DrawTo(x + dx1, y - dy1);

  NewColor(black);

#undef dx1
#undef dx2
#undef dy
#undef dy1

  if (x > 0)
  {
    Box(-138, 0, -100, 49);
    MoveTo(-150 - 2, -27);
    DrawTo(-86 - 2, -27);
    DrawTo(-86 - 2, -7);
    DrawTo(-150 - 2, -7);
    DrawTo(-150 - 2, -27);
  }
  else
  {
    Box(98, 0, 136, 49);
    MoveTo(83 + 2, -27);
    DrawTo(147 + 2, -27);
    DrawTo(147 + 2, -7);
    DrawTo(83 + 2, -7);
    DrawTo(83 + 2, -27);
  }

  return (x);

#undef y
}

static void
Choose(void)
{
  char sec;
  char string[] = "00.";
  int x;
  long TimeOut;
  long wait;
  long h;

  TimeFlag = TRUE;
  GameMode = TRUE;
  ExitWait = FALSE;

  TimeOut = Time;
  AddBCD(0x1599L, &TimeOut);
  h = TimeOut;

  ClearButtons();

  for (; TimeOut > Time;)
  {
    GetPlayerInput();

    h = TimeOut;
    SubBCD(Time, &h);
    sec = (h >> 8);
    string[1] = sec & 15;
    sec >>= 4;
    string[0] = (sec) ? sec : ' ';

    for (wait = 0; wait < 1000; wait++)
      ;

    // RESET BACKGROUND

    CopySmallScreen(0, WindowY - 1, SelectScreen, Buffer2);

    NewColor(darkgreen);
    SetTextColor(darkgreen);
    TypeString(40 - (24 / 2), 25, "Press O for Option Screen#");

    // PRINT ARROW

    x = DrawArrowAndBox();

    // PRINT SECONDS LEFT

    SetTextColor(darkred);
    PrintString(19, 45, string);

    // CHECK PLAYER'S CHOICE

    car.autoFlag = (x < 0);

    if (ExitWait)
    {
      ClearButtons();
      break;
    }

    UpdateTime();

    Switch();
  }

  GameMode = FALSE;
  ClearButtons();
}

void
ChooseTransmission(void)
{
  int height;

  height = OrgY + MaxY;
  DemoMode = TRUE;

  CopySmallScreen(0, height, SelectScreen, Buffer2);
  SetTextColor(darkred);
  PrintString(19, 45, "\001\005.");
  DrawArrowAndBox();

  Switch();
  RealToDummy();

  Choose();
}

// **********************************************************************
// ********  O P T I O N S    S C R E E N  ******************************
// **********************************************************************

static void
DrawBox(int x1, int y1, int x2, int y2, int color)
{
  NewColor(color);

  MoveTo(x1, y1);
  DrawTo(x2, y1);
  DrawTo(x2, y2);
  DrawTo(x1, y2);
  DrawTo(x1, y1);
}

static char xpos, ypos, saveflag;

static int
PrintSelectScreen(void)
{
  char key;

#define y1 -24 - 22
#define y2 -2 - 22
#define y3 20 - 22
#define y4 42 - 22
#define y5 64 - 22
#define ye 54
#define dy 70

  BlankScreen();

  SetTextColor(orange);
  TypeString(7, 0, "USE ARROW AND RETURN KEYS#");

  SetTextColor(white);
  TypeString(1, y1 + dy - 11, "STEERING#");
  TypeString(1, y2 + dy - 11, "GEAR SHIFT#");
  TypeString(1, y3 + dy - 11, "DIFFICULTY#");
  TypeString(1, y4 + dy - 11, "SAVE GAME#");
  TypeString(1, y5 + dy - 11, "SOUND#");

  SetTextColor((SteerSelect == 1) ? red : darkred);
  TypeString(10, y1 + dy, "STICK A#");
  SetTextColor((SteerSelect == 2) ? red : darkred);
  TypeString(19, y1 + dy, "STICK B#");
  SetTextColor((SteerSelect == 3) ? red : darkred);
  TypeString(29, y1 + dy, "MOUSE#");
  SetTextColor((SteerSelect == 4) ? red : darkred);
  TypeString(39, y1 + dy, "KEYS#");

  SetTextColor((GearSelect == 1) ? red : darkred);
  TypeString(10, y2 + dy, "STICK A#");
  SetTextColor((GearSelect == 2) ? red : darkred);
  TypeString(19, y2 + dy, "STICK B#");
  SetTextColor((GearSelect == 3) ? red : darkred);
  TypeString(29, y2 + dy, "KEYBOARD#");

  SetTextColor((Difficulty == 1) ? red : darkred);
  TypeString(10, y3 + dy, "EASY#");
  SetTextColor((Difficulty == 2) ? red : darkred);
  TypeString(19, y3 + dy, "MEDIUM#");
  SetTextColor((Difficulty == 3) ? red : darkred);
  TypeString(29, y3 + dy, "HARD#");

  SetTextColor((saveflag) ? red : darkred);
  TypeString(10, y4 + dy, "YES#");
  SetTextColor((!saveflag) ? red : darkred);
  TypeString(19, y4 + dy, "NO#");
  SetTextColor((UK_Flag) ? brown : orange);
  TypeString(29, y4 + dy, (UK_Flag) ? "LH DRIVE#" : "RH DRIVE#");

  SetTextColor((SoundEnabled) ? red : darkred);
  TypeString(10, y5 + dy, "YES#");
  SetTextColor((!SoundEnabled) ? red : darkred);
  TypeString(19, y5 + dy, "NO#");

  SetTextColor(blue);
  TypeString(18, ye + dy, "EXIT#");

  if (ypos == 1)
  {
    if (xpos == 1)
      // STEERING: JOYSTICK 0 BOX
      DrawBox(-124, y1, -88, y1 + 11, red);
    if (xpos == 2)
      // STEERING: JOYSTICK 1 BOX
      DrawBox(-88, y1, -52, y1 + 11, red);
    if (xpos == 3)
      // STEERING: MOUSE BOX
      DrawBox(-48, y1, -20, y1 + 11, red);
    if (xpos == 4)
      // STEERING: KEYBOARD
      DrawBox(-8, y1, 16, y1 + 11, red);
  }

  if (ypos == 2)
  {
    if (xpos == 1)
      // GEAR SHIFT: JOYSTICK 0 BOX
      DrawBox(-124, y2, -88, y2 + 11, red);
    if (xpos == 2)
      // GEAR SHIFT: JOYSTICK 1 BOX
      DrawBox(-88, y2, -52, y2 + 11, red);
    if (xpos == 3)
      // GEAR SHIFT: KEYBOARD BOX
      DrawBox(-48, y2, -8, y2 + 11, red);
  }

  if (ypos == 3)
  {
    if (xpos == 1)
      // DIFFICULTY: EASY
      DrawBox(-124, y3, -100, y3 + 11, red);
    if (xpos == 2)
      // DIFFICULTY: MEDIUM
      DrawBox(-88, y3, -56, y3 + 11, red);
    if (xpos == 3)
      // DIFFICULTY: HARD
      DrawBox(-48, y3, -24, y3 + 11, red);
  }

  if (ypos == 4)
  {
    if (xpos == 1)
      // SAVE GAME: YES
      DrawBox(-124, y4, -104, y4 + 11, red);
    if (xpos == 2)
      // SAVE GAME: NO
      DrawBox(-88, y4, -72, y4 + 11, red);
    if (xpos == 3)
      // LHD/RHD
      DrawBox(-48, y4, -8, y4 + 11, (UK_Flag) ? brown : orange);
  }

  if (ypos == 5)
  { // SOUND FX

    if (xpos == 1)
      // SOUND: ON
      DrawBox(-124, y5, -104, y5 + 11, red);
    if (xpos == 2)
      // SOUND: OFF
      DrawBox(-88, y5, -72, y5 + 11, red);
  }

  if (ypos == 6)
  { // RESUME BOX
    DrawBox(-98, ye, -65, ye + 11, blue);
  }

  SetTextColor(white);
  TypeString(52, 18 + 25, "KEYBOARD STEERING#");
  TypeString(45, 30 + 25, "Cursor UP \\\\\\\\\\\\\\\\\\\\\\\\ ACCELERATION#");
  TypeString(45, 40 + 25, "Cursor LEFT \\\\\\\\\\\\\\\\\\\\ STEER LEFT#");
  TypeString(45, 50 + 25, "Cursor RIGHT \\\\\\\\\\\\\\\\\\\ STEER RIGHT#");
  TypeString(45, 60 + 25, "Spacebar \\\\\\\\\\\\\\\\\\\\\\\\\\ BRAKE#");
  TypeString(45, 70 + 25, "Cursor DOWN \\\\\\\\\\\\\\\\\\\\ DECELERATION#");

  key = KeyPressed();

  if (key == 1)
    if (--ypos < 1)
      ypos = 6;
  if (key == 2)
    if (++ypos > 6)
      ypos = 1;
  if (key == 3)
    if (--xpos < 1)
      xpos = 4;
  if (key == 4)
    if (++xpos > 4)
      xpos = 1;

  if ((ypos == 5) && (xpos == 3))
    xpos = 1;
  if ((ypos != 1) && (xpos == 4))
    xpos = 1;

  if (key == CR)
  {
    if (ypos == 1)
    {
      SteerSelect = xpos;

      if (xpos < 4)
      {
        SteerSelect = xpos;
        if ((GearSelect == xpos) && (xpos < 3))
          GearSelect ^= 1 + 2;
        if (SteerSelect == 3)
          MouseDriverFlag = OrgDriverFlag;
      }
      else
      {
        MouseDriverFlag = FALSE;
      }
    }
    if (ypos == 2)
    {
      GearSelect = xpos;
      if ((SteerSelect == xpos) && (xpos < 3))
        SteerSelect ^= 1 + 2;
    }
    if (ypos == 3)
    {
      Difficulty = xpos;
    }
    if (ypos == 4)
    {
      if (xpos == 1)
        saveflag = TRUE;
      if (xpos == 2)
        saveflag = FALSE;
      if (xpos == 3)
        UK_Flag ^= TRUE;
    }
    if (ypos == 5)
    {
      if (xpos == 1)
        SoundEnabled = TRUE;
      if (xpos == 2)
        SoundEnabled = FALSE;
    }
    if (ypos == 6)
    {
      if ((SteerSelect == 1) || (GearSelect == 1))
        DisableMouse();
      if (SteerSelect == 3)
        EnableMouse();
      if (saveflag)
        SaveGameFile();

      return (TRUE);
    }
  }

  return (FALSE);

#undef y1
#undef y2
#undef y3
#undef y4
#undef y5
#undef dy
}

void
SetOptions(void)
{
  char end;

  if (!GameMode || SloMoFlag)
    return;

  StopSound();

  Switch();
  RealToDummy();
  TimeFlag = FALSE;
  end = FALSE;
  saveflag = FALSE;

  xpos = ypos = 1;

  while (!end)
  {
    end = PrintSelectScreen();
    Switch();
  }

  TimeFlag = TRUE;
}
