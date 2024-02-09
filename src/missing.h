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
