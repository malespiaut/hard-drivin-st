/*                   */
/*  Player           */
/*  Substitution     */
/*  Functions        */
/*   for RBI3        */
/*                   */
/*                   */

//#include "genesis.h"
//#include "rbi.h"
//#include "teamsel.h"

#option separate ERATBL
#option separate TEAMSEL_COL
#option separate TEAMSEL_SCRN
#option separate TENGEN_PRESENTS_STMP
#option separate BAT_BACKGROUND_SCRN
#option separate BAT_BACKGROUND_STMP
#option separate PITCHER_BACKGROUND_SCRN
#option separate PITCHER_BACKGROUND_STMP
#option separate team_names
extern long BAT_BACKGROUND_SCRN;
extern long BAT_BACKGROUND_STMP;
extern long PITCHER_BACKGROUND_SCRN;
extern long PITCHER_BACKGROUND_STMP;
extern long TENGEN_PRESENTS_STMP;
extern long TEAMSEL_COL;
extern long TEAMSEL_SCRN;
extern unsigned char inputs_edge[];
extern unsigned char inputs[];
extern long color_addr;
extern int pal_num;
extern int pass_vars[];
extern int temp[];
extern unsigned char Team_Num[];
extern char message_buffer[];
extern TN_TYPE team_names[];
extern PITCHER_TYPE PITCHERS[2][11];
extern unsigned char curpitcher[];
extern int ERATBL[];
extern unsigned char dhfl;
extern unsigned char lineup[2][15];
extern unsigned char fielders[2][9];
extern BATTER_TYPE BATTERS[2][14];
extern unsigned char teamup;       /* Teamup 0-Player 0, 1-Player 1 */
extern unsigned char curbatter[2]; /* Who's Batting */
extern unsigned char state4;
extern unsigned char Offense_CTRL_edge; /* Offense/Defense Joystick Trigger */
extern unsigned char Defense_CTRL_edge;

extern unsigned char rnnr_on_1b;
extern unsigned char rnnr_on_2b;
extern unsigned char rnnr_on_3b;
extern unsigned char rnnr_on_hb;
extern unsigned char player_on_1b; /* number of player on base(in BATTER struct) */
extern unsigned char player_on_2b;
extern unsigned char player_on_3b;
extern unsigned char Batting_Num[2]; /* Batter who is up */

extern UGH_S();

#define BATTSUB 0
#define FIELDSUB 1
#define PITCHSUB 2

substitute()
{

  if (!state4)
  { /* if substitution is called for the first time */
    flush_mobs();
    process_mobs();
    clear_pf1();
    clear_pf2();
    switch (temp[15])
    {
      case BATTSUB:
        temp[6] = teamup;
        batter_substitution_init();
        break;
      case FIELDSUB:
        temp[6] = teamup ^ 1;
        fielder_substitution_init();
        break;
      case PITCHSUB:
        temp[6] = teamup ^ 1;
        pitcher_substitution_init();
        break;
      default:
        break;
    }
    print_message(4, 10, "Controls");
    print_message(3, 11, "Do Sub (A)");
    print_message(2, 12, "Exit (START)");
    state4++;
  }
  else
  { /* if substitution is called for the nth time */
    switch (temp[15])
    {

      case BATTSUB:
        batter_substitution_move();
        break;
      case FIELDSUB:
        fielder_substitution_move();
        break;
      case PITCHSUB:
        pitcher_substitution_move();
        break;
      default:
        break;
    }
  }
}

/*===================================*/

batter_substitution_init()
{
  int j, k, pal, team, number;

  temp[0] = 0; /* out of game flags */
  temp[1] = 0;
  temp[2] = 0;
  temp[3] = 0;
  temp[4] = 0;
  temp[5] = 0;

  temp[7] = 0xFF; /* Initial cursor y position */
  temp[8] = 4;    /* Initial cursor x position */

  pal = 2 + teamup;
  team = Team_Num[teamup];
  number = curbatter[teamup];

  drawscreen(&BAT_BACKGROUND_SCRN, &BAT_BACKGROUND_STMP, &TEAMSEL_COL);
  color_addr = (long)&TEAMSEL_COL + (unsigned char)32;
  pal_num = 1;
  movecolortable();
  color_addr = (long)&TEAMSEL_COL + (unsigned char)64;
  pal_num = 2;
  movecolortable();
  color_addr = (long)&TEAMSEL_COL + (unsigned char)96;
  pal_num = 3;
  movecolortable();

  k = 0;
  message_buffer[0] = team_names[Team_Num[teamup]].initials[0];
  message_buffer[1] = team_names[Team_Num[teamup]].initials[1];
  message_buffer[2] = '\0';
  print_message_pal(9, 1, message_buffer, pal);

  print_message(12, 1, "BATTER SUBSTITUTION");
  print_message_pal(12, 3, "Replace   with", 1);

  print_message_pal(5, 4, "Name", 1);
  print_message_pal(5, 5, "Avg", 1);
  print_message_pal(5, 6, "Hr", 1);
  print_message_pal(5, 7, "Pos", 1);
  print_message_pal(5, 8, "Hand", 1);

  temp[12] = 12;
  temp[11] = pal;
  temp[10] = number;
  full_stats();

  print_message_pal(5, 15, "PLAYING          BENCH     H P", 1);

  j = 0;
  if (curbatter[teamup] < 0x80)
    print_message_pal(5, j + 16, BATTERS[teamup][curbatter[teamup]].name, pal);
  else
    print_message_pal(5, j + 16, PITCHERS[teamup][curpitcher[teamup]].name, pal);
  temp[0] = curbatter[teamup];
  print_message_pal(14, j + 16, "BATTER", 1);

  j++;

  if (rnnr_on_1b)
  {
    if (player_on_1b < 0x80)
      print_message_pal(5, j + 16, BATTERS[teamup][player_on_1b].name, pal);
    else
      print_message_pal(5, j + 16, PITCHERS[teamup][curpitcher[teamup]].name, pal);
    temp[1] = player_on_1b;
    print_message_pal(14, j + 16, "1st", 1);
    j++;
  }
  if (rnnr_on_2b)
  {
    if (player_on_2b < 0x80)
      print_message_pal(5, j + 16, BATTERS[teamup][player_on_2b].name, pal);
    else
      print_message_pal(5, j + 16, PITCHERS[teamup][curpitcher[teamup]].name, pal);
    temp[2] = player_on_2b;
    print_message_pal(14, j + 16, "2nd", 1);
    j++;
  }
  if (rnnr_on_3b)
  {
    if (player_on_3b < 0x80)
      print_message_pal(5, j + 16, BATTERS[teamup][player_on_3b].name, pal);
    else
      print_message_pal(5, j + 16, PITCHERS[teamup][curpitcher[teamup]].name, pal);
    temp[3] = player_on_3b;
    print_message_pal(14, j + 16, "3rd", 1);
    j++;
  }
  temp[4] = j - 1;

  for (k = 9; k < 15; k++)
  {
    pal = 2 + teamup;
    if (BATTERS[teamup][lineup[teamup][k]].playing == 0xFF)
      pal = 1;
    print_message_pal(22, k + 7, BATTERS[teamup][lineup[teamup][k]].name, pal);
    message_buffer[0] = (BATTERS[teamup][lineup[teamup][k]].misc & 0x80) ? 'S' : (BATTERS[teamup][lineup[teamup][k]].misc & 0x01) ? 'L'
                                                                                                                                  : 'R';
    pass_vars[0] = BATTERS[teamup][lineup[teamup][k]].position;
    pass_vars[1] = k;
    get_pposition();
    message_buffer[1] = ' ';
    message_buffer[2] = temp[13];
    message_buffer[3] = temp[14];
    message_buffer[4] = '\0';
    print_message_pal(32, k + 7, message_buffer, pal);
  }
  temp[7] = 0; /* Initial cursor y position */
  prt_cursor();
}

/*===================================*/

full_stats()
{
  int x, i, pal, number;
  x = temp[12];
  pal = temp[11];
  number = temp[10];
  print_message_pal(x, 4, BATTERS[temp[6]][number].name, pal);

  i = BATTERS[temp[6]][number].avg + 111;
  message_buffer[0] = i / 100 + 48;
  i = i % 100;
  message_buffer[1] = i / 10 + 48;
  message_buffer[2] = (i % 10) + 48;
  message_buffer[3] = '\0';
  print_message_pal(x, 5, message_buffer, pal);

  i = BATTERS[temp[6]][number].hr;
  if ((message_buffer[0] = i / 10 + 48) == 48)
    message_buffer[0] = ' ';
  message_buffer[1] = (i % 10) + 48;
  message_buffer[2] = '\0';
  print_message_pal(x, 6, message_buffer, pal);
  if (number < 9)
    pass_vars[0] = BATTERS[temp[6]][number].playing;
  else
    pass_vars[0] = BATTERS[temp[6]][number].position;
  pass_vars[1] = number;
  get_pposition();
  message_buffer[0] = temp[13];
  message_buffer[1] = temp[14];
  message_buffer[2] = '\0';
  print_message_pal(x, 7, message_buffer, pal);
  message_buffer[0] = (BATTERS[temp[6]][number].misc & 0x80) ? 'S' : (BATTERS[temp[6]][number].misc & 0x01) ? 'L'
                                                                                                            : 'R';
  message_buffer[1] = '\0';
  print_message_pal(x, 8, message_buffer, pal);
}

/*===================================*/

full_stats_pit()
{
  int x, i, pal, number;
  x = temp[12];
  pal = temp[11];
  number = temp[10];
  print_message_pal(x, 4, PITCHERS[temp[6]][number].name, pal);

  i = ERATBL[PITCHERS[temp[6]][number].eraval];
  message_buffer[0] = i / 100 + 48;
  i = i % 100;
  message_buffer[1] = '.';
  message_buffer[2] = i / 10 + 48;
  message_buffer[3] = (i % 10) + 48;
  message_buffer[4] = '\0';
  print_message_pal(x, 5, message_buffer, pal);

  message_buffer[0] = (PITCHERS[temp[6]][number].sinkhand & 0x01) ? 'L' : 'R';
  message_buffer[1] = '\0';
  print_message_pal(x, 6, message_buffer, pal);
}

/*===================================*/

batter_substitution_move()
{
  int number, k, pal;
  pal = 2 + teamup;

  if (Offense_CTRL_edge & JS_UP)
  {
    err_cursor();
    if (temp[8] == 4)
    {
      temp[7]--;
      if (temp[7] < 0)
        temp[7] = temp[4];
      temp[12] = 12;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = temp[temp[7]];
    }
    else
    {
      do
      {
        temp[7]--;
        if (temp[7] < 0)
          temp[7] = temp[4];
      } while (BATTERS[temp[6]][lineup[temp[6]][temp[7] + 9]].playing == 0xFF);
      temp[12] = 22;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = lineup[temp[6]][temp[7] + 9];
    }
    full_stats();
    prt_cursor();
  }
  if (Offense_CTRL_edge & JS_DOWN)
  {
    err_cursor();
    if (temp[8] == 4)
    {
      temp[7]++;
      if (temp[7] > temp[4])
        temp[7] = 0;
      temp[12] = 12;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = temp[temp[7]];
    }
    else
    {
      do
      {
        temp[7]++;
        if (temp[7] > temp[4])
          temp[7] = 0;
      } while (BATTERS[temp[6]][lineup[temp[6]][temp[7] + 9]].playing == 0xFF);
      temp[12] = 22;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = lineup[temp[6]][temp[7] + 9];
    }
    full_stats();
    prt_cursor();
  }
  if (Offense_CTRL_edge & JS_A)
  {

    if (temp[8] == 4)
    {
      temp[5] = temp[7]; /* Item selected             */
      temp[8] = 21;      /* Initial cursor x position */
      temp[7] = 0xFF;    /* Initial cursor y position */
      temp[4] = 5;       /* number of items           */
      for (k = 9; k < 15; k++)
      {
        if (BATTERS[temp[6]][lineup[temp[6]][k]].playing != 0xFF)
          if (temp[7] == 0xFF)
            temp[7] = k - 9;
      }
      temp[12] = 22;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = lineup[teamup][temp[7] + 9];
      full_stats();
      prt_cursor();
    }
    else
    {
      for (k = 0; k < 9; k++)
        if (lineup[temp[6]][k] == temp[temp[5]])
          number = k;

      temp[9] = lineup[temp[6]][number];       /* old player */
      temp[10] = lineup[temp[6]][temp[7] + 9]; /* new player */
      lineup[temp[6]][number] = temp[10];
      lineup[temp[6]][temp[7] + 9] = temp[9];

      /* save old player's position */
      temp[11] = BATTERS[temp[6]][temp[9]].playing;

      /* old player is out of game */
      BATTERS[temp[6]][temp[9]].playing = 0xFF;

      /* new player takes old player's position */
      BATTERS[temp[6]][temp[10]].playing = temp[11];

      /* Change curbatter or player_on_xb */

      switch (temp[5])
      {
        case 0:
          curbatter[temp[6]] = temp[10];
          break;
        case 1:
          player_on_1b = temp[10];
          break;
        case 2:
          player_on_2b = temp[10];
          break;
        case 3:
          player_on_3b = temp[10];
          break;
      }

      /* change fielders from old player to new player */
      for (k = 0; k < 9; k++)
        if (fielders[temp[6]][k] == temp[9])
          fielders[temp[6]][k] == temp[10];

      /* IF PITCHER.... */
      if (temp[9] < 0x80)
        ;

      state4 = 0xFF; /* RETURN */
    }
  }

  if (Offense_CTRL_edge & (JS_START))
    state4 = 0xFF; /* RETURN */
}

/*===================================*/
get_pposition()
{
  int i;

  i = pass_vars[0];
  switch (i)
  {
    case 1:
      temp[13] = 'C';
      temp[14] = ' ';
      break;
    case 2:
      temp[13] = '1';
      temp[14] = 'B';
      break;
    case 4:
      temp[13] = '2';
      temp[14] = 'B';
      break;
    case 8:
      temp[13] = '3';
      temp[14] = 'B';
      break;
    case 0x10:
      temp[13] = 'S';
      temp[14] = 'S';
      break;
    case 0x20:
      temp[13] = 'L';
      temp[14] = 'F';
      break;
    case 0x40:
      temp[13] = 'C';
      temp[14] = 'F';
      break;
    case 0x80:
      temp[13] = 'R';
      temp[14] = 'F';
      break;
  }
  if (pass_vars[1] == 8)
  {
    temp[13] = 'P';
    temp[14] = ' ';
  }
  if ((pass_vars[1] == 8) && dhfl)
  {
    temp[13] = 'D';
    temp[14] = 'H';
  }
  if (pass_vars[1] > 8)
  {
    if (i > 1 && i < 0x20)
    {
      temp[13] = 'I';
      temp[14] = 'F';
    }
    else if (i >= 0x20)
    {
      temp[13] = 'O';
      temp[14] = 'F';
    }
  }
}

/*  Print the cursors in message_buffer[0] to the screen */
/*  Calculates position from temp[0-4] */
up_cursor()
{
  message_buffer[1] = '\0';
  print_message(temp[8], temp[7] + 16, message_buffer);
}

/* erase cursors */
err_cursor()
{
  message_buffer[0] = ' ';
  up_cursor();
}

/* print cursors */
prt_cursor()
{
  /* if (temp[8] < 15)
          message_buffer[0] = '1';
     else
          message_buffer[0] = '2';
  */
  message_buffer[0] = '>';

  up_cursor();
}

/*===================================*/

fielder_substitution_init()
{
  int j, k, i, pal, team, number;

  temp[0] = 0; /* out of game flags */
  temp[1] = 0;
  temp[2] = 0;
  temp[3] = 0;
  temp[4] = 0;
  temp[5] = 0;

  temp[7] = 0x00; /* Initial cursor position */
  temp[8] = 4;

  pal = 2 + teamup ^ 1;
  number = lineup[temp[6]][0];

  drawscreen(&PITCHER_BACKGROUND_SCRN, &PITCHER_BACKGROUND_STMP, &TEAMSEL_COL);
  color_addr = (long)&TEAMSEL_COL + (unsigned char)32;
  pal_num = 1;
  movecolortable();
  color_addr = (long)&TEAMSEL_COL + (unsigned char)64;
  pal_num = 2;
  movecolortable();
  color_addr = (long)&TEAMSEL_COL + (unsigned char)96;
  pal_num = 3;
  movecolortable();

  j = Team_Num[teamup ^ 1];
  message_buffer[0] = team_names[j].initials[0];
  message_buffer[1] = team_names[j].initials[1];
  message_buffer[2] = '\0';
  print_message_pal(9, 1, message_buffer, pal);

  print_message(12, 1, "PLAYER SUBSTITUTION");
  print_message_pal(12, 3, "Replace   with", 1);

  print_message_pal(5, 4, "Name", 1);
  print_message_pal(5, 5, "Avg", 1);
  print_message_pal(5, 6, "Hr", 1);
  print_message_pal(5, 7, "Pos", 1);
  print_message_pal(5, 8, "Hand", 1);

  temp[12] = 12;  /* x pos */
  temp[11] = pal; /* pal   */
  temp[10] = number;
  full_stats();
  prt_cursor();

  print_message_pal(5, 15, "FIELDERS   H P   BENCH     H P", 1);

  for (k = 0; k < 9; k++)
  {
    if (lineup[temp[6]][k] < 0x80)
    {
      print_message_pal(5, k + 16, BATTERS[temp[6]][lineup[temp[6]][k]].name, pal);
      message_buffer[0] = (BATTERS[temp[6]][lineup[temp[6]][k]].misc & 0x80) ? 'S' : (BATTERS[temp[6]][lineup[temp[6]][k]].misc & 0x01) ? 'L'
                                                                                                                                        : 'R';
      pass_vars[0] = BATTERS[temp[6]][lineup[temp[6]][k]].playing;
      pass_vars[1] = k;
      get_pposition();
      message_buffer[1] = ' ';
      message_buffer[2] = temp[13];
      message_buffer[3] = temp[14];
      message_buffer[4] = '\0';
      print_message_pal(16, k + 16, message_buffer, pal);
    }
  }
  temp[4] = 7;
  for (; k < 15; k++)
  {
    pal = 2 + teamup ^ 1;
    if (BATTERS[temp[6]][lineup[temp[6]][k]].playing == 0xFF)
      pal = 1;
    print_message_pal(22, k + 7, BATTERS[temp[6]][lineup[temp[6]][k]].name, pal);
    message_buffer[0] = (BATTERS[temp[6]][lineup[temp[6]][k]].misc & 0x80) ? 'S' : (BATTERS[temp[6]][lineup[temp[6]][k]].misc & 0x01) ? 'L'
                                                                                                                                      : 'R';
    pass_vars[0] = BATTERS[temp[6]][lineup[temp[6]][k]].position;
    pass_vars[1] = k;
    get_pposition();
    message_buffer[1] = ' ';
    message_buffer[2] = temp[13];
    message_buffer[3] = temp[14];
    message_buffer[4] = '\0';
    print_message_pal(32, k + 7, message_buffer, pal);
  }
  /*	for (k=0;k<8;k++){
                  for (i=0;i<32;i++){
                          message_buffer[0] = (k*32)+i ;
                          message_buffer[1] = '\0';
                          print_message(i+5,k+17,message_buffer);
                  }
          }	 */
}

/*===================================*/

fielder_substitution_move()
{
  int pal, k;
  pal = 2 + teamup ^ 1;

  if (Defense_CTRL_edge & JS_UP)
  {
    err_cursor();
    if (temp[8] == 4)
    {
      temp[7]--;
      if (temp[7] < 0)
        temp[7] = temp[4];
      temp[12] = 12;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = lineup[temp[6]][temp[7]];
    }
    else
    {
      do
      {
        temp[7]--;
        if (temp[7] < 0)
          temp[7] = temp[4];
      } while (BATTERS[temp[6]][lineup[temp[6]][temp[7] + 9]].playing == 0xFF);
      temp[12] = 22;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = lineup[temp[6]][temp[7] + 9];
    }
    full_stats();
    prt_cursor();
  }
  if (Defense_CTRL_edge & JS_DOWN)
  {
    err_cursor();
    if (temp[8] == 4)
    {
      temp[7]++;
      if (temp[7] > temp[4])
        temp[7] = 0;
      temp[12] = 12;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = lineup[temp[6]][temp[7]];
    }
    else
    {
      do
      {
        temp[7]++;
        if (temp[7] > temp[4])
          temp[7] = 0;
      } while (BATTERS[temp[6]][lineup[temp[6]][temp[7] + 9]].playing == 0xFF);
      temp[12] = 22;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = lineup[temp[6]][temp[7] + 9];
    }
    full_stats();
    prt_cursor();
  }
  if (Defense_CTRL_edge & JS_A)
  {

    if (temp[8] == 4)
    {
      temp[5] = temp[7]; /* Item selected             */
      temp[8] = 21;      /* Initial cursor x position */
      temp[7] = 0xFF;    /* Initial cursor y position */
      temp[4] = 5;       /* number of items           */
      for (k = 9; k < 15; k++)
      {
        if (BATTERS[temp[6]][lineup[temp[6]][k]].playing != 0xFF)
          if (temp[7] == 0xFF)
            temp[7] = k - 9;
      }
      temp[12] = 22;  /* x pos */
      temp[11] = pal; /* pal   */
      temp[10] = lineup[teamup][temp[7] + 9];
      full_stats();
      prt_cursor();
    }
    else
    {

      state4 = 0xFF; /* RETURN */
    }
  }
  if (Defense_CTRL_edge & (JS_START))
    state4 = 0xFF; /* RETURN */
}

/*===================================*/

pitcher_substitution_init()
{
  int j, i, k, pal, team, number;

  temp[0] = 0; /* out of game flags */
  temp[1] = 0;
  temp[2] = 0;
  temp[3] = 0;
  temp[4] = 0;
  temp[5] = 0;

  temp[7] = 0xFF; /* Initial cursor position */
  temp[8] = 11;
  pal = 2 + teamup ^ 1;
  team = Team_Num[teamup ^ 1];
  number = curpitcher[teamup ^ 1];

  drawscreen(&PITCHER_BACKGROUND_SCRN, &PITCHER_BACKGROUND_STMP, &TEAMSEL_COL);
  color_addr = (long)&TEAMSEL_COL + (unsigned char)32;
  pal_num = 1;
  movecolortable();
  color_addr = (long)&TEAMSEL_COL + (unsigned char)64;
  pal_num = 2;
  movecolortable();
  color_addr = (long)&TEAMSEL_COL + (unsigned char)96;
  pal_num = 3;
  movecolortable();

  j = Team_Num[teamup ^ 1];
  message_buffer[0] = team_names[j].initials[0];
  message_buffer[1] = team_names[j].initials[1];
  message_buffer[2] = '\0';
  print_message_pal(9, 1, message_buffer, pal);

  print_message(12, 1, "PITCHER SUBSTITUTION");
  print_message_pal(12, 3, "Replace   with", 1);

  print_message_pal(5, 4, "Name", 1);
  print_message_pal(5, 5, "Era", 1);
  print_message_pal(5, 6, "Hand", 1);

  if (curpitcher[temp[6]] == 0x80)
    print_message_pal(12, 4, "None", pal);
  else
  {
    temp[12] = 12;     /* x pos      */
    temp[11] = pal;    /* pal        */
    temp[10] = number; /* curpitcher */
    full_stats_pit();
  }
  print_message_pal(12, 15, "BULL PEN    H  ERA", 1);

  for (k = 0; k < 10; k++)
  {
    pal = 2 + temp[6];
    if (PITCHERS[temp[6]][k].available)
      pal = 1;
    else if (temp[7] == 0xFF)
      temp[7] = k;
    print_message_pal(12, k + 16, PITCHERS[temp[6]][k].name, pal);

    message_buffer[0] = (PITCHERS[temp[6]][k].sinkhand & 0x01) ? 'L' : 'R';
    i = ERATBL[PITCHERS[temp[6]][k].eraval];
    message_buffer[1] = ' ';
    message_buffer[2] = ' ';
    message_buffer[3] = i / 100 + 48;
    i = i % 100;
    message_buffer[4] = '.';
    message_buffer[5] = i / 10 + 48;
    message_buffer[6] = (i % 10) + 48;
    message_buffer[7] = '\0';
    print_message_pal(24, k + 16, message_buffer, pal);
  }
  prt_cursor();
}

/*===================================*/

pitcher_substitution_move()
{
  int pal;
  pal = 2 + temp[6];
  if (Defense_CTRL_edge & JS_UP)
  {
    err_cursor();
    do
    {
      temp[7]--;
      if (temp[7] < 0)
        temp[7] = 9;
    } while (PITCHERS[temp[6]][temp[7]].available);
    temp[12] = 22;  /* x pos */
    temp[11] = pal; /* pal   */
    temp[10] = lineup[temp[6]][temp[7]];
    full_stats_pit();
    prt_cursor();
  }
  if (Defense_CTRL_edge & JS_DOWN)
  {
    err_cursor();
    do
    {
      temp[7]++;
      if (temp[7] > 9)
        temp[7] = 0;
    } while (PITCHERS[temp[6]][temp[7]].available);
    temp[12] = 22;  /* x pos */
    temp[11] = pal; /* pal   */
    temp[10] = lineup[temp[6]][temp[7]];
    full_stats_pit();
    prt_cursor();
  }
  if (Defense_CTRL_edge & JS_A)
  {

    state4 = 0xFF; /* RETURN */
  }
  if (Defense_CTRL_edge & (JS_START))
    if (curpitcher[temp[6]] == 0x80)
      print_message_pal(18, 12, "You must select a relief pitcher", pal);
    else
      state4 = 0xFF; /* RETURN */
}

/*===================================*/

/* Print names of teams
//   pass_vars[0] = x position
//   pass_vars[1] = y position
//   pass_vars[2] = palette number
*/
pr_line_names()
{
  int k;
  for (k = 0; k < 8; k++)
  {
    message_buffer[k] = lineup[curbatter[0]][k];
  }
  print_message_pal(pass_vars[0], pass_vars[1], message_buffer, pass_vars[2]);
}

/* Print names of teams
//   pass_vars[0] = x position
//   pass_vars[1] = y position
//   pass_vars[2] = 0 = team1, 1 = team2
//   pass_vars[3] = palette number
*/
pr_lineup_names()
{
  int j, k;
  j = Team_Num[pass_vars[2]];
  if (j < 128)
  {
    for (k = 0; k < 8; k++)
    {
      message_buffer[k] = team_names[j].name[k];
    }
  }
}
