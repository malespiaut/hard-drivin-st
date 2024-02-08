

#include "proto.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*

----------------------------------------------------------------------

 MODULE...............	text.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes text print functions.

 DATE OF CREATION.....	March  8th 1989
 LAST CHANGE..........	November 5th 1989
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

/*
        int	SpaceLength,		/* Length of a blank. */
CharLength, /* Length of a character. */
  x0, x1, x2, x3, x4, x5, x6,
  y0, y1, y2, y3, y4, y5, y6;

/*		FUNCTIONS :
                ===========
*/

void
ScaleFont(void)

{
  extern int ScreenX, ScreenY;
  register int x, y;

  x = ScreenX;
  y = ScreenY;

  SpaceLength = multdiv(5, x, 320);
  CharLength = multdiv(7, x, 320);

  x0 = multdiv(0, x, 320);
  x1 = multdiv(1, x, 320);
  x2 = multdiv(2, x, 320);
  x3 = multdiv(3, x, 320);
  x4 = multdiv(4, x, 320);
  x5 = multdiv(5, x, 320);
  x6 = multdiv(6, x, 320);

  y0 = multdiv(0, y, 200);
  y1 = multdiv(1, y, 200);
  y2 = multdiv(2, y, 200);
  y3 = multdiv(3, y, 200);
  y4 = multdiv(4, y, 200);
  y5 = multdiv(5, y, 200);
  y6 = multdiv(6, y, 200);
}

#define m(a, b) MoveTo(x + a, y + b);
#define d(a, b) DrawTo(x + a, y + b);

static void
PrintZiffer(int x, int y, int ziffer)

{
  switch (ziffer)
  {
    case 0:
      m(x0, y0) d(x5, y0) d(x5, y5) d(x0, y5) d(x0, y0)
        m(x0, y5) d(x5, y0) break;
    case 1:
      m(x1, y2) d(x3, y0) d(x3, y5)
        m(x1, y5) d(x5, y5) break;
    case 2:
      m(x0, y0) d(x5, y0) d(x5, y2) d(x0, y2) d(x0, y5) d(x5, y5) break;
    case 3:
      m(x0, y0) d(x5, y0) d(x5, y5) d(x0, y5)
        m(x1, y2) d(x5, y2) break;
    case 4:
      m(x0, y0) d(x0, y2) d(x5, y2) d(x5, y0) d(x5, y5) break;
    case 5:
      m(x5, y0) d(x0, y0) d(x0, y2) d(x5, y2) d(x5, y5) d(x0, y5) break;
    case 6:
      m(x0, y0) d(x0, y5) d(x5, y5) d(x5, y2) d(x0, y2) break;
    case 7:
      m(x0, y0) d(x5, y0) d(x5, y5) break;
    case 8:
      m(x0, y0) d(x5, y0) d(x5, y5) d(x0, y5) d(x0, y0)
        m(x0, y2) d(x5, y2) break;
    case 9:
      m(x5, y5) d(x5, y0) d(x0, y0) d(x0, y2) d(x5, y2) break;
    default:
      break;
  }
}

static void
PrintText(int x, int y, char* text, int color)

{
  register int ch;

  NewColor(color);

  while ((ch = *text++) != 0)
  {
    if (ch == ' ')
    {
      x += SpaceLength;
      continue;
    }
    if (isupper(ch))
      ch |= 0x20;
    else if (isdigit(ch))
    {
      PrintZiffer(x, y, ch - '0');
      x += CharLength;
      continue;
    }

    switch (ch)
    {
      case 'a':
        m(x0, y5) d(x0, y2) d(x2, y0) d(x3, y0) d(x5, y2)
          d(x5, y5) m(x0, y2) d(x5, y2) break;
      case 'b':
        m(x0, y2) d(x4, y2) d(x5, y2) d(x5, y1)
          d(x4, y0) d(x0, y0) d(x0, y5) d(x4, y5)
            d(x5, y4) d(x5, y3) d(x4, y2) break;
      case 'c':
        m(x5, y0) d(x0, y0) d(x0, y5) d(x5, y5) break;
      case 'd':
        m(x0, y0) d(x0, y5) d(x3, y5) d(x5, y3) d(x5, y2)
          d(x3, y0) d(x0, y0) break;
      case 'e':
        m(x5, y0) d(x0, y0) d(x0, y5) d(x5, y5)
          m(x0, y2) d(x4, y2) break;
      case 'f':
        m(x5, y0) d(x0, y0) d(x0, y5) m(x0, y2) d(x4, y2) break;
      case 'g':
        m(x5, y0) d(x0, y0) d(x0, y5) d(x5, y5) d(x5, y2)
          d(x3, y2) break;
      case 'h':
        m(x5, y0) d(x5, y5) m(x0, y0) d(x0, y5)
          m(x0, y2) d(x5, y2) break;
      case 'i':
        m(x2, y0) d(x2, y5) m(x0, y0) d(x4, y0)
          m(x0, y5) d(x4, y5) break;
      case 'j':
        m(x5, y0) d(x5, y5) d(x2, y5) d(x0, y3) break;
      case 'k':
        m(x0, y0) d(x0, y5)
          m(x5, y0) d(x0, y2) d(x5, y5) break;
      case 'l':
        m(x0, y0) d(x0, y5) d(x5, y5) break;
      case 'm':
        m(x0, y5) d(x0, y0) d(x2, y2) d(x3, y2) d(x5, y0)
          d(x5, y5) break;
      case 'n':
        m(x0, y5) d(x0, y0) d(x5, y5) d(x5, y0) break;
      case 'o':
        m(x0, y0) d(x0, y5) d(x5, y5) d(x5, y0) d(x0, y0) break;
      case 'p':
        m(x0, y5) d(x0, y0) d(x5, y0) d(x5, y2) d(x0, y2) break;
      case 'q':
        m(x5, y5) d(x5, y0) d(x0, y0) d(x0, y5) d(x5, y5)
          d(x3, y3) break;
      case 'r':
        m(x0, y5) d(x0, y0) d(x5, y0) d(x5, y2) d(x0, y2)
          m(x2, y2) d(x5, y5) break;
      case 's':
        m(x5, y0) d(x0, y0) d(x0, y2) d(x5, y2) d(x5, y5)
          d(x0, y5) break;
      case 't':
        m(x0, y0) d(x4, y0) m(x2, y0) d(x2, y5) break;
      case 'u':
        m(x0, y0) d(x0, y5) d(x5, y5) d(x5, y0) break;
      case 'v':
        m(x0, y0) d(x3, y5) d(x6, y0) x++;
        break;
      case 'w':
        m(x0, y0) d(x0, y5) d(x2, y3) d(x3, y3) d(x5, y5)
          d(x5, y0) break;
      case 'x':
        m(x0, y0) d(x5, y5) m(x0, y5) d(x5, y0) break;
      case 'y':
        m(x0, y0) d(x2, y2) d(x4, y0) m(x2, y2) d(x2, y5) break;
      case 'z':
        m(x0, y0) d(x5, y0) d(x0, y5) d(x5, y5) break;
      case '-':
        m(x0, y2) d(x5, y2) break;
      case '_':
        m(x0, y5) d(x5, y5) break;
      case '.':
        m(x1, y5) d(x1, y5) break;
      case ',':
        m(x2, y4) d(x2, y5) d(x1, y6) break;
      case '\'':
        m(x1, y0) d(x1, y1) break;
      case ':':
        m(x2, y4) d(x2, y4) m(x2, y2) d(x2, y2) break;
      case '@':
        m(x1, y0) d(x5, y0) d(x6, y1) d(x6, y5)
          d(x5, y6) d(x1, y6) d(x0, y5) d(x0, y1)
            d(x1, y0)
              m(x4, y2) d(x2, y2) d(x2, y4) d(x4, y4)
                x++;
        break;
      case '*':
        m(x1, y0) d(x5, y0) d(x6, y1) d(x6, y5)
          d(x5, y6) d(x1, y6) d(x0, y5) d(x0, y1)
            d(x1, y0)
              m(x2, y2) d(x2, y4) m(x3, y3) d(x4, y4)
                m(x2, y2) d(x3, y2) d(x4, y2)
                  x++;
        break;
      case '+':
        m(x1, y3) d(x5, y3) m(x3, y1) d(x3, y5) break;
      case '(':
        m(x2, y0) d(x0, y2) d(x0, y4) d(x2, y6) break;
      case ')':
        m(x4, y0) d(x6, y2) d(x6, y4) d(x4, y6) break;
      case '/':
        m(x0, y5) d(x5, y0) break;
      case '&':
        m(x5, y3) d(x3, y5) d(x2, y5) d(x1, y4)
          d(x1, y3) d(x3, y1) d(x2, y0) d(x1, y1)
            d(x5, y5) break;
    }
    x += CharLength;
  }
}

void
TypeValue(int x, int y, long value, int color)

{
  char text[100];

  sprintf(text, "%ld", value);
  PrintText(x, y, text, color);
}
* /

  void
  PrintScore(void)

{
  extern int ScoreY, ScoreX, DigitLength;
  extern long Score;
  register int x;
  register unsigned long h;

  x = ScoreX;
  h = Score;

  do
  {
    PrintDigit((char)h & 15, x, ScoreY);
    h >>= 4;
    x -= DigitLength;
  } while (h);
}

void
PrintTime(void)

{
  extern char RaceIsOn;
  extern int TimeX, TimeY, DigitLength;
  extern long Time;
  extern s_object *YellowLight, *GreenLight;
  register int i, x;
  register ulong h;

  x = TimeX;
  h = Time;

  if (h > 0x000150L)
  {
    YellowLight->active = FALSE;
    GreenLight->active = TRUE;
  }

  if (!RaceIsOn)
    return;

  for (i = 2 + 1; --i;)
  {
    PrintDigit((char)h & 15, x, TimeY);
    h >>= 4;
    x -= DigitLength;
    PrintDigit((char)h & 15, x, TimeY);
    h >>= 4;
    x -= DigitLength;

    PrintDigit((char)10, x, TimeY);
    x -= DigitLength;
  }

  PrintDigit((char)h & 15, x, TimeY);
}

void
PrintQualify(void)

{
  extern char QualifyFlag, LapMode, DemoMode;
  extern void *Buffer2, *ScreenBuffer;
  extern int LastX, LastY, BestX, BestY, DigitLength;
  extern long TimeToQualify, LastLapTime;
  register int i, x;
  register ulong h;

  if (LapMode)
    return;

  if (QualifyFlag && !DemoMode)
  { /* YOU ARE QUALIFIED */

    x = LastX;
    h = LastLapTime;

    for (i = 2 + 1; --i;)
    {
      PrintDigit((char)h & 15, x, LastY);
      h >>= 4;
      x -= DigitLength;
      PrintDigit((char)h & 15, x, LastY);
      h >>= 4;
      x -= DigitLength;

      PrintDigit((char)10, x, LastY);
      x -= DigitLength;
    }

    PrintDigit((char)h & 15, x, LastY);

    VidToVid(0, 188, 65, 188 + 7, Buffer2, ScreenBuffer);
  }
  else
  { /* BEAT X:XX:XX TO QUALIFY */

    x = BestX;
    h = TimeToQualify;

    for (i = 2 + 1; --i;)
    {
      PrintDigit((char)h & 15, x, BestY);
      h >>= 4;
      x -= DigitLength;
      PrintDigit((char)h & 15, x, BestY);
      h >>= 4;
      x -= DigitLength;

      PrintDigit((char)10, x, BestY);
      x -= DigitLength;
    }

    PrintDigit((char)h & 15, x, BestY);

    VidToVid(0, 182, 65, 182 + 7, Buffer2, ScreenBuffer);
  }
}

void
PrintMessage(void)

{
  extern char MessageTime, MessageCode;

  if (!MessageTime)
    return;

  MessageTime--;

  switch (MessageCode)
  {
    case msg_extra:
      SetTextColor(white);
      PrintString(15, 25, "EXTRA TIME.");
      break;

    case msg_extended:
      SetTextColor(white);
      PrintString(14, 25, "EXTENDED PLAY.");
      break;

    case msg_qualified:
      SetTextColor(white);
      PrintString(13, 15, "EXTENDED PLAY.");
      SetTextColor(yellow);
      PrintString(7, 25, "QUALIFIED FOR CHAMPIONSHIP.");
      break;

    case msg_great:
      SetTextColor(white);
      PrintString(13, 15, "EXTENDED PLAY.");
      SetTextColor(yellow);
      PrintString(11, 25, "ANOTHER GREAT LAP.");
      break;

    case msg_won:
      SetTextColor(white);
      PrintString(17, 20, "YOU WON \014\014.");
      break;

    case msg_lost:
      SetTextColor(white);
      PrintString(17, 20, "YOU LOST \014\014.");
      break;

    case msg_disqualified:
      SetTextColor(yellow);
      PrintString(7, 30, "YOU HAVE BEEN DISQUALIFIED\015.");
      PrintString(8, 40, "GET ANOTHER LOW LAP TIME.");
      PrintString(14, 50, "AND TRY AGAIN\015.");
      break;
  }
}

void
PrintCountDown(void)

{
  extern char TimeOutFlag, StartMessage;
  extern char LapMode, RaceIsOn;
  extern int SecondsX, SecondsY;
  extern long TimeOut, Time, Duration;
  extern s_object *RedLight, *YellowLight;
  register int sec;
  register ulong h;

  static char number[] = {0, 10, 0, 0, '.'};
  static char countdown[] = {0, '.'};
  static int seconds;

  if (TimeOutFlag)
    return;

  if (StartMessage)
  {
    SetTextColor(white);
    PrintString(10, 10, "PRESS BUTTON TO START.");
  }

  if (!RaceIsOn)
  { /* START COUNTDOWN. */

    h = Time;

    if (h > 0x000900L)
    {
      RedLight->active = FALSE;
      YellowLight->active = TRUE;
    }

    sec = 0x09 - ((int)(h >> 8) & 0xFF);

    if (sec < 0)
    { /* COUNT DOWN IS OVER, RACE STARTS. */

      RaceIsOn = TRUE;
      Time = 0x000000L;
      TimeOut = Duration;
    }
    else
    { /* DISPLAY COUNTDOWN. */

      countdown[0] = sec;

      SetTextColor(darkred);
      PrintString(20, 26, countdown);
    }

    return;
  }

  if (!LapMode)
  {
    sec = (int)(TimeOut >> 2 * 4);

    if (sec != seconds)
    {
      seconds = sec;
      number[3] = sec & 15;
      sec >>= 4;
      number[2] = sec & 15;
      sec >>= 4;
      number[0] = sec & 15;
    }

    SetTextColor((seconds < 0x20) ? darkred : yellow);
    PrintString(SecondsX, SecondsY, number);
  }
}

void
PrintOffRoad(void)

{
  extern char ReturnFlag, WrongDirection, LapMode;
  extern char AccidentFlag, MessageTime, MessageCode;
  extern char TimeOutFlag, LapFinished, SloMoFlag;
  extern int OffRoadX, OffRoadY, OffRoadOut;
  extern long Time;
  register char sec;
  static char string[] = "OFF ROAD 00.";

  if (AccidentFlag)
    return;

  if (!OffRoadOut)
  {
    if (WrongDirection)
    {
      SetTextColor(darkred);
      PrintString(OffRoadX - 1, OffRoadY, "WRONG DIRECTION.");
    }
    return;
  }

  if (OffRoadOut < 0x0020)
  {
    ReturnFlag = TRUE;
    OffRoadOut = 0;

    if (LapMode)
    {
      MessageTime = msg_duration + 20;
      MessageCode = msg_disqualified;
      LapFinished = FALSE;
      TimeOutFlag = TRUE;
      SloMoFlag = TRUE;
    }
    return;
  }

  sec = (char)(OffRoadOut >> 8);
  SetTextColor(darkred);

  string[10] = sec & 15;
  sec >>= 4;
  string[9] = (sec) ? sec : ' ';

  PrintString(OffRoadX, OffRoadY, string);
}

void
PrintHiscoreList(void)

{
  extern char ScoreList[];
  extern int ListX, LineHeight, MessageY;
  register char* string;
  register int i, y;

  y = MessageY--;

  if (y < 15)
  {
    if (y > 15 - 100)
      y = 15;
    else
      y += 100;
  }

  SetTextColor(yellow);

  PrintString(ListX, y, "TEN HARDEST DRIVERS.");

  string = ScoreList;
  y += (LineHeight << 1);

  SetTextColor(white);

  for (i = 10 + 1; --i;)
  {
    PrintString(1, y, string);
    string += 39;
    y += LineHeight;
  }
}

void
PrintChampion(void)

{
  extern char BestName[], BestTime[];
  extern int LineHeight, MessageY, BestLength;
  register int y, dy;

  dy = LineHeight << 1;
  y = MessageY--;

  if (y < 15)
  {
    if (y > 15 - 100)
      y = 15;
    else
      y += 100;
  }

  SetTextColor(yellow);

  PrintString(10, y, "HARD DRIVIN CHAMPION.");
  y += dy;
  PrintString(19, y, "IS.");
  y += dy;

  SetTextColor(white);

  PrintString((40 - BestLength) >> 1, y, BestName);
  y += dy;

  SetTextColor(yellow);

  PrintString(16, y, "LAP TIME.");
  y += dy;

  SetTextColor(white);

  PrintString(15, y, BestTime);
}
