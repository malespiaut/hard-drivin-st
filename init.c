
	#include	<stdio.h>

	#include	"proto.h"






/*

----------------------------------------------------------------------

 MODULE...............	init.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes all functions
 			for the game initialization.
			
 DATE OF CREATION.....	May 2nd 1989
 LAST CHANGE..........	November 8th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/







/*		LOCAL VARIABLES :
		=================
*/











/*
***************************************************************************
*/







/*		FUNCTIONS :
		===========
*/


static	uchar near	Filter( uchar n, s_object *obj);





static	uchar near	Filter( uchar n, s_object *obj)

{	uchar	result;

	switch (n)
	{	case   8 :	/* HILLS */
		case   9 :
		case  48 :
		case  61 :
		case 146 :	/* undefined hill, for future use */
		case 147 :	/* undefined hill, for future use */

				obj->class	 = c_hill;
				obj->DisplayProc = PrintObject;
				result		 = TRUE;
				break;


		case   4 :	/* HOUSE, BARN, COW, STORE */
		case   7 :
		case  10 :
		case  94 :
		case  95 :
		case 148 :	/* undefined building, for future use */
		case 149 :	/* undefined building, for future use */

				obj->class	 = c_building;
				obj->DisplayProc = PrintObject;
				result		 = TRUE;
				break;


		case  70 :	/* STRTPOST, TRAFFICLIGHT */
		case  71 :	/* FINISH */
		case  72 :
		case  73 :
		case 112 :	obj->class	 = c_standard;
				obj->DisplayProc = PrintObject;
				result		 = TRUE;
				break;


		case  14 :	/* DIP, LCURVE, RCURVE, TURNOFF */
		case  42 :	/* SORS, FLOAT40, FLOAT60, FLOAT90 */
		case  44 :	/* SIGNPOST, NEW60M, CHECKPOINT */
		case  46 :	/* GRDARROW */
		case  77 :
		case  78 :
		case  79 :
		case  80 :
		case  83 :
		case  97 :
		case 125 :
		case 126 :
		case 144 :
		case 145 :
		case 150 :	/* undefined sign, for future use */
		case 151 :	/* undefined sign, for future use */

				obj->class	 = c_sign;
				obj->DisplayProc = PrintSign;
				result		 = TRUE;
				break;


		case 115 :	/* BARRIER */

				obj->class	 = c_building;
				obj->DisplayProc = PrintSign;
				result		 = TRUE;
				break;


		case  31 :	/* OVERPASS */
		case  32 :
		case  36 :	/* EZJUMP */
		case  37 :
		case  81 :	/* XOVER */
		case 110 :	/* BANKED CURVE */
		case 111 :
		case 152 :	/* undefined chicane, for future use */
		case 153 :	/* undefined chicane, for future use */

				obj->class	 = c_chicane;
				obj->DisplayProc = PrintObject;
				result		 = TRUE;
				break;


		case  57 :	/* UNDERPASS */
		case  58 :
				obj->class	 = c_chicane;
				obj->DisplayProc = PrintUnderpass;
				result		 = TRUE;
				break;


		case  39 :	/* SUSPENSION BRIDGE */
		case  40 :
				obj->class	 = c_chicane;
				obj->DisplayProc = PrintBridge;
				result		 = TRUE;
				break;


		case  28 :	/* LOOP */

				obj->class	 = c_loop;
				obj->DisplayProc = PrintLoop;
				result		 = TRUE;
				break;


		case  62 :	/* CAR1, CAR2, VAN, TRUCK */
		case  20 :	/* PORSCHE, CORVETTE, PICKUP */
		case   1 :
		case  21 :
		case  12 :
		case  22 :
		case  38 :
		case  55 :
		case  56 :
		case  91 :
		case  92 :
		case 103 :
		case 104 :
		case 136 :
		case 137 :
		case 154 :	/* undefined car, for future use */
		case 155 :	/* undefined car, for future use */

				obj->class	 = c_car;
				obj->DisplayProc = PrintCar;
				result		 = TRUE;
				break;


		default :	result = FALSE;
				break;
	}

	return( result);
}








void	InitObjects( void)

{	extern	 int		NumOfObjects, NumOfFields;
	extern	 s_object	*object, *field;
	extern	 s_object	*GreenLight, *YellowLight, *RedLight,
				*RedCar, *BlueCar;
	register int		i, j, max;
	register s_object	*obj, *obj2;
		 int		n, far *s, sin, cos;


	/* INITIALIZE FIELDS. */

	obj = field;
	max = NumOfFields;
	NumOfFields = 0;

	for (i = 0; i < max; i++)
	{	obj->active = SetObjClass(obj);

		if (obj->active)
		{	obj->radius	 = CompRadius(obj) + 50;
			obj->class	 = c_standard;
			obj->DisplayProc = PrintObject;
			NumOfFields++;
			obj++;
		}
	}


	/* INITIALIZE OBJECTS. */

	obj = object;
	max = NumOfObjects;
	NumOfObjects = 0;

	for (i = 0; i < max; i++)
	{       obj->active = FALSE;
		if (Filter( obj->large, obj))
		{	obj->active = SetObjClass(obj);
			obj->radius = CompRadius(obj);

			if (obj->active)
			{	NumOfObjects++;
				obj++;
			}
		}
	}


	/* FIND ALL OBJECTS TO PRE_ROTATE. */

	obj = object;

	for (i = 0; i < NumOfObjects; i++)
	{	if (obj->class != c_car)
			obj->pre_rotated = TRUE;
		obj++;
	}

	obj = object;

	for (i = 0; i < NumOfObjects; i++)
	{	if (obj->pre_rotated)
		{	obj2 = object;
			for (j = 0; j < NumOfObjects; j++)
			{	if (j != i)
				{	if (obj->type == obj2->type)
					{	obj->pre_rotated = FALSE;
						obj2->pre_rotated = FALSE;
					}
				}
				obj2++;
			}

			if (obj->pre_rotated)
			{	if (obj->small)
				{	obj->type = obj->small;
					if (SetObjClass(obj))
					{	n = obj->normals + obj->vertices;
						s = obj->model;
						sin =   sinus( obj->pitch);
						cos = cosinus( obj->pitch);	
						RotYZ( n, cos, sin, s, s);
						sin =   sinus( obj->yaw);
						cos = cosinus( obj->yaw);	
						RotXZ( n, cos, sin, s, s);
					}
				}
				obj->type = obj->large;
				if (SetObjClass(obj))
				{	n = obj->normals + obj->vertices;
					s = obj->model;
					sin =   sinus( obj->pitch);
					cos = cosinus( obj->pitch);
					RotYZ( n, cos, sin, s, s);
					sin =   sinus( obj->yaw);
					cos = cosinus( obj->yaw);	
					RotXZ( n, cos, sin, s, s);
				}
			}
		}
		obj++;
	}


	/* SET POINTER TO TRAFFIC LIGHTS. */
	
	GreenLight  = SearchObject( 72);
	YellowLight = SearchObject( 71);
	RedLight    = SearchObject( 70);    GreenLight->active = TRUE;


	/* SET POINTER TO RED AND BLUE CAR. */

	RedCar  = SearchObject( 103);	RedCar->color  = red1;
	BlueCar = SearchObject(   1);	BlueCar->color = hatchedblue;
}









void	InitCar( void)

{	extern	 int		StartSegment, StartLane, StartRow;
	extern	 s_car		car;
	extern	 s_block	GearARect[], GearMRect[];


	SetCar( StartSegment, StartLane, StartRow, 1);

	#if (0)
	{	extern	s_track	*track1, *track2;
			s_track	*swap;

		swap   = track1;
		track1 = track2;

		SetCar( 33, StartLane, StartRow, 1);

		track1 = swap;
	}
	#endif

	car.preciseX = (long) car.newX << 3;
	car.preciseY = (long) car.newY << 3;
	car.preciseZ = (long) car.newZ << 3;

	car.deltaSpin  = 0;
	car.deltaPitch = 0;
	car.deltaRoll  = 0;

	car.rpm = 0;
	car.mph = 0;
	car.speed = car.wheelSpeed = 0;

	car.BrakeFlag   = FALSE;
	car.ImpulseFlag = FALSE;
	car.ShiftFlag	= FALSE;

	car.gear = (car.autoFlag) ? 1 : 0;
	car.throttle = 0;
	car.gearBlock  = (car.autoFlag) ? GearARect : GearMRect;
	car.gearBlock += car.gear;
}








void	InitPhantomPhoton( void)

{	extern	 char		BlueSample, DirtyFlag, PhantomLane;
	extern	 int		StartSegment;
	extern	 s_track	*track1;
	extern	 s_frame	far *OldChampLap;
		 s_vehicle	phantom;
		 s_object	dummy;
	register uchar		sample;
	register s_frame	far *fr, far *end;


	if (!DirtyFlag)
		/* PHANTOM PHOTON IS ALREADY READ FROM DISK. */
		return;


	phantom.StreetUnderCar	= track1 + StartSegment;
	phantom.NextStreet	= NIL;
	phantom.TrackUsed	=  0;
	phantom.StartTrack	=  0;
	phantom.HomeTrack	=  1;
	phantom.position	= (1*200/4);
	phantom.lane		=  0;
	phantom.OnStreet	= FALSE;
	phantom.direction	=  1;
	phantom.acceleration	=  2;
	phantom.speed		=  0;
	phantom.maxspeed	= 16;
	phantom.obj		= &dummy;
	
	PhantomLane = 1;

	sample = 0;
	fr  = OldChampLap;
	end = OldChampLap + MaxSample-1;

	while (fr < end)
	{	SetVehicle( &phantom);
	
		if (++sample >= SamplePeriod)
		{	fr->x = dummy.worldX;
			fr->y = dummy.worldY;
			fr->z = dummy.worldZ;
			fr->yaw   = dummy.yaw;
			fr->pitch = dummy.pitch;
			fr->roll  = dummy.roll;
			sample = 0;
			fr++;
		}
	}
}








void	InitControlPoints( void)

{	extern	 int		NumOfT1controls, NumOfT2controls;
	extern	 int		CheckP1_segment, CheckP2_segment;
	extern	 int		FinishSegment;
	extern	 s_control	*T1ContPoints, *T2ContPoints;
	extern	 s_track	*track1, *track2;
	register int		i;
	register s_track	*tr;
	register s_control	*ct;


	/* INIT CONTROL POINTS. */

	/* SPEED TRACK. */

	ct = T1ContPoints;
	tr = track1;

	for (i = 0; i < NumOfT1controls; i++)
	{	ct->covered = FALSE;
		ct++;
	}
	(tr + CheckP1_segment)->flags |= f_checkpoint;
	(tr + FinishSegment)->flags   |= f_finish;


	/* STUNT TRACK. */

	ct = T2ContPoints;
	tr = track2;

	for (i = 0; i < NumOfT2controls; i++)
	{	ct->covered = FALSE;
		ct++;
	}
	(tr + CheckP2_segment)->flags |= f_checkpoint;
}
