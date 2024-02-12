#include "struct.h"

/*

----------------------------------------------------------------------

 MODULE...............	data.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module contains all static data.

 DATE OF CREATION.....	March 22th 1989
 LAST CHANGE..........	September 6th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

//			S I N E  -  T A B L E  :
//                      ========================

int SineData[] = {0, 71, 143, 214, 286, 357, 429, 500, 572, 643, 715, 786, 857, 929, 1000, 1072, 1143, 1214, 1285, 1357, 1428, 1499, 1570, 1641, 1713, 1784, 1855, 1926, 1997, 2068, 2139, 2209, 2280, 2351, 2422, 2492, 2563, 2634, 2704, 2775, 2845, 2915, 2986, 3056, 3126, 3196, 3266, 3336, 3406, 3476, 3546, 3616, 3686, 3755, 3825, 3894, 3964, 4033, 4102, 4171, 4240, 4310, 4378, 4447, 4516, 4585, 4653, 4722, 4790, 4859, 4927, 4995, 5063, 5131, 5199, 5266, 5334, 5402, 5469, 5536, 5604, 5671, 5738, 5805, 5872, 5938, 6005, 6071, 6138, 6204, 6270, 6336, 6402, 6467, 6533, 6599, 6664, 6729, 6794, 6859, 6924, 6989, 7053, 7118, 7182, 7246, 7311, 7374, 7438, 7502, 7565, 7629, 7692, 7755, 7818, 7881, 7943, 8006, 8068, 8130, 8192, 8254, 8316, 8337, 8438, 8500, 8561, 8621, 8682, 8743, 8803, 8863, 8923, 8983, 9043, 9102, 9162, 9221, 9280, 9339, 9397, 9456, 9514, 9572, 9630, 9688, 9746, 9803, 9860, 9917, 9974, 10031, 10087, 10143, 10199, 10255, 10311, 10366, 10422, 10477, 10531, 10586, 10641, 10695, 10749, 10803, 10856, 10910, 10963, 11016, 11069, 11121, 11174, 11226, 11278, 11330, 11381, 11433, 11484, 11535, 11585, 11636, 11686, 11736, 11786, 11835, 11885, 11934, 11982, 12031, 12080, 12128, 12176, 12223, 12271, 12318, 12365, 12412, 12458, 12505, 12551, 12597, 12642, 12688, 12733, 12778, 12822, 12867, 12911, 12955, 12998, 13042, 13085, 13128, 13170, 13213, 13255, 13297, 13338, 13380, 13421, 13462, 13502, 13543, 13583, 13623, 13662, 13702, 13741, 13780, 13818, 13856, 13894, 13932, 13970, 14007, 14044, 14081, 14117, 14153, 14189, 14225, 14260, 14295, 14330, 14364, 14399, 14433, 14466, 14500, 14533, 14566, 14598, 14631, 14663, 14694, 14726, 14757, 14788, 14819, 14849, 14879, 14909, 14938, 14968, 14996, 15025, 15053, 15082, 15109, 15137, 15164, 15191, 15218, 15244, 15270, 15296, 15321, 15346, 15371, 15396, 15420, 15444, 15468, 15491, 15515, 15537, 15560, 15582, 15604, 15626, 15647, 15668, 15689, 15709, 15729, 15749, 15769, 15788, 15807, 15826, 15844, 15862, 15880, 15897, 15914, 15931, 15948, 15964, 15980, 15996, 16011, 16026, 16041, 16055, 16069, 16083, 16096, 16110, 16123, 16135, 16147, 16159, 16171, 16182, 16193, 16204, 16214, 16225, 16234, 16244, 16253, 16262, 16270, 16279, 16287, 16294, 16302, 16309, 16315, 16322, 16328, 16333, 16339, 16344, 16349, 16353, 16358, 16362, 16365, 16368, 16371, 16374, 16376, 16378, 16380, 16382, 16383, 16383, 16384, 16384};

//              M o u n t a i n   S h a p e s  :
//              ================================

// Length in Pixel.

char MntLength[] = {
  42,  // left end
  160, // element 1
  160, // element 2
  96,  // mountain 1
  94,  // twin mountains
  142, // connection 1
  110, // mountain 2
  88,  // connection 2
  224, // connection 3
  20,  // right end
};

//     code    offset

int MntPositions[] = {
  0,
  0, // left end
  1,
  0,
  2,
  0,
  3,
  0, // mountain 1
  4,
  0, // twin mnts
  6,
  0, // mountain 2
  5,
  0,
  6,
  0, // mountain 2
  7,
  0, // connection 2
  3,
  0, // mountain 1
  7,
  0,
  //	 8,	 0,
  1,
  0,
  //	 2,	 0,
  //	 1,	 0,
  9,
  0, // right end

  0,
  0, // dummy
};

//              S o l i d   F i l l   P a t t e r n  :
//              ======================================

long LoResPattern[] = {
  // Black

  0x00000000L,
  0x00000000L,
  0x00000000L,
  0x00000000L,

  // Green

  0xFFFF0000L,
  0x00000000L,
  0xFFFF0000L,
  0x00000000L,

  // Dark Green

  0x0000FFFFL,
  0x00000000L,
  0x0000FFFFL,
  0x00000000L,

  // Blue

  0xFFFFFFFFL,
  0x00000000L,
  0xFFFFFFFFL,
  0x00000000L,

  // Yellow

  0x00000000L,
  0xFFFF0000L,
  0x00000000L,
  0xFFFF0000L,

  // Orange

  0xFFFF0000L,
  0xFFFF0000L,
  0xFFFF0000L,
  0xFFFF0000L,

  // Red

  0x0000FFFFL,
  0xFFFF0000L,
  0x0000FFFFL,
  0xFFFF0000L,

  // Dark Blue

  0xFFFFFFFFL,
  0xFFFF0000L,
  0xFFFFFFFFL,
  0xFFFF0000L,

  // Dark Red

  0x00000000L,
  0x0000FFFFL,
  0x00000000L,
  0x0000FFFFL,

  // Brown

  0xFFFF0000L,
  0x0000FFFFL,
  0xFFFF0000L,
  0x0000FFFFL,

  // Gray 2

  0x0000FFFFL,
  0x0000FFFFL,
  0x0000FFFFL,
  0x0000FFFFL,

  // Gray 3

  0xFFFFFFFFL,
  0x0000FFFFL,
  0xFFFFFFFFL,
  0x0000FFFFL,

  // Gray 4

  0x00000000L,
  0xFFFFFFFFL,
  0x00000000L,
  0xFFFFFFFFL,

  // Gray 5

  0xFFFF0000L,
  0xFFFFFFFFL,
  0xFFFF0000L,
  0xFFFFFFFFL,

  // Redbrown

  0x0000FFFFL,
  0xFFFFFFFFL,
  0x0000FFFFL,
  0xFFFFFFFFL,

  // White

  0xFFFFFFFFL,
  0xFFFFFFFFL,
  0xFFFFFFFFL,
  0xFFFFFFFFL,

  // Mixed Colors to be mixed.

  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
};

//		H A R D D R I V I N G  -  L o g o  :
//              ====================================

int HardLogo[] = {
  // H

  -94,
  -14,
  -88,
  -14,
  -86,
  -16,
  -92,
  -16,
  -117,
  9,
  -111,
  9,
  -109,
  7,
  -114,
  7,
  -75,
  -14,
  -69,
  -14,
  -67,
  -16,
  -74,
  -16,
  -98,
  9,
  -92,
  9,
  -90,
  7,
  -97,
  7,
  -80,
  -3,
  -99,
  -3,
  -100,
  -4,
  -81,
  -4,
  -109,
  7,
  -88,
  -14,
  -90,
  -14,
  -111,
  7,
  -90,
  7,
  -69,
  -14,
  -71,
  -14,
  -92,
  7,

  // a

  -76,
  1,
  -66,
  1,
  -65,
  0,
  -75,
  0,
  -82,
  9,
  -74,
  1,
  -77,
  1,
  -84,
  9,
  -68,
  -5,
  -58,
  -5,
  -57,
  -6,
  -67,
  -6,
  -72,
  9,
  -58,
  -5,
  -60,
  -5,
  -74,
  9,
  -85,
  10,
  -64,
  10,
  -63,
  9,
  -84,
  9,

  // r

  -63,
  9,
  -48,
  -6,
  -50,
  -6,
  -65,
  9,
  -52,
  -1,
  -44,
  -5,
  -44,
  -6,
  -52,
  -2,
  -45,
  -5,
  -39,
  -5,
  -38,
  -6,
  -44,
  -6,

  // d

  -19,
  -14,
  -12,
  -14,
  -10,
  -16,
  -17,
  -16,
  -34,
  8,
  -12,
  -14,
  -14,
  -14,
  -36,
  8,
  -33,
  -5,
  -23,
  -5,
  -22,
  -6,
  -32,
  -6,
  -44,
  8,
  -31,
  -5,
  -33,
  -5,
  -46,
  8,
  -47,
  9,
  -35,
  9,
  -34,
  8,
  -46,
  8,

  // -

  -123,
  16,
  -41,
  16,
  -39,
  14,
  -121,
  14,

  // D

  4,
  -14,
  25,
  -14,
  27,
  -16,
  6,
  -16,
  -11,
  8,
  11,
  -14,
  9,
  -14,
  -13,
  8,
  3,
  8,
  25,
  -14,
  23,
  -14,
  1,
  8,
  -17,
  9,
  2,
  9,
  4,
  7,
  -15,
  7,

  // r

  13,
  9,
  28,
  -6,
  26,
  -6,
  11,
  9,
  24,
  -1,
  32,
  -5,
  32,
  -6,
  24,
  -2,
  31,
  -5,
  37,
  -5,
  38,
  -6,
  32,
  -6,

  // i

  50,
  -10,
  52,
  -12,
  50,
  -12,
  48,
  -10,
  31,
  9,
  46,
  -6,
  44,
  -6,
  29,
  9,

  // v

  52,
  9,
  52,
  -6,
  50,
  -6,
  50,
  9,
  52,
  9,
  67,
  -6,
  65,
  -6,
  50,
  9,

  // i

  83,
  -10,
  85,
  -12,
  83,
  -12,
  81,
  -10,
  64,
  9,
  79,
  -6,
  77,
  -6,
  62,
  9,

  // n

  76,
  9,
  91,
  -6,
  89,
  -6,
  74,
  9,
  89,
  -4,
  102,
  -4,
  103,
  -5,
  90,
  -5,
  91,
  7,
  102,
  -4,
  100,
  -4,
  89,
  7,
  87,
  9,
  95,
  9,
  97,
  7,
  89,
  7,

  // '

  117,
  -12,
  121,
  -16,
  118,
  -16,
  114,
  -12,

  // -

  -23,
  16,
  90,
  16,
  92,
  14,
  -21,
  14,
};

int Debris[] = {-26, 3, -23, 14, -5, 17, -4, 15, 0, 5, -6, 2, -2, -7, 11, -4, 5, -28, 26, -18, 18, -67, 79, -67, 86, -3, 95, 1, 159, -3, 9, 23, 21, 54, 52, 43, 30, 68, 102, 67, -96, 67, -84, -1, -65, -34, -157, -8, -100, -67};