


	#include	"proto.h"




/*

----------------------------------------------------------------------

 MODULE...............	display.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes functions for object
 			movement.

 DATE OF CREATION.....	March 22th 1989
 LAST CHANGE..........	October 18th 1989
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



static	void near	CarDemoDrive( void);
static	void near	FindFields( void);







static	void near	CarDemoDrive( void)

{	extern	 char		UK_Flag;
	extern	 s_track	*track1, *track2;
	extern	 s_track	*End1, *End2, *Junct1, *Junct2;
	extern	 s_car		car;
	register char		sub, h1, h2;
	register s_track	*st1, *st2;
		 int		sinY, cosY, sinP, cosP, sinR, cosR;
		 int		angle1, angle2, h;

	static	 int		dx, dy, dz, hx, hy, hz;
	static	 uint		mx1, my1, mz1, mx2, my2, mz2;
	static	 char		SubPosition, OnStreet;
	static	 char		TrackUsed, Choose;
	static	 s_track	*NextSegment, *SegmentUnderCar;


	#define	MaxPos	11	/* STEPS PER STREET SEGMENT */
	#define	swing	 7	/* TRANSITION TO NEXT PITCH ANGLE */


	/* MAKE SURE NEXTSTREET IS NOT NIL ! */

	if (!SegmentUnderCar)
		SegmentUnderCar = track1;
	if (!NextSegment)
		NextSegment = SegmentUnderCar + 1;

	st1 = SegmentUnderCar;
	st2 = NextSegment;
	sub = SubPosition;

	/* TAKE CARE OF JUNCTIONS AND TRACK ENDS. */

		if (TrackUsed == 0)
		{	if (st1->flags & f_junct1)
			{	if (!Choose)
				{	st2 = track2 + 1;
					NextSegment = st2;
					TrackUsed = 1;
					OnStreet = FALSE;
				}
			}
			else	if (st2 >= End1)
				{	st2 = track1;
					NextSegment = st2;
					Choose ^= TRUE;
				}
		}
	else	if (st2 >= End2)
		{	st2 = Junct2;
			NextSegment = st2;
			TrackUsed = 0;
		}


	/* SET VARIABLES WHEN REACHING NEW STREET SEGMENT. */

	if (!OnStreet)
	{	if (UK_Flag)
		{	mx1 = (uint) ((3 * (ulong) st1->x1 + (ulong) st1->x2) >> 2);
			my1 = (uint) ((3 * (ulong) st1->y1 + (ulong) st1->y2) >> 2);
			mz1 = (uint) ((3 * (ulong) st1->z1 + (ulong) st1->z2) >> 2);

			mx2 = (uint) ((3 * (ulong) st2->x1 + (ulong) st2->x2) >> 2);
			my2 = (uint) ((3 * (ulong) st2->y1 + (ulong) st2->y2) >> 2);
			mz2 = (uint) ((3 * (ulong) st2->z1 + (ulong) st2->z2) >> 2);
		}
		else
		{	mx1 = (uint) ((3 * (ulong) st1->x2 + (ulong) st1->x1) >> 2);
			my1 = (uint) ((3 * (ulong) st1->y2 + (ulong) st1->y1) >> 2);
			mz1 = (uint) ((3 * (ulong) st1->z2 + (ulong) st1->z1) >> 2);

			mx2 = (uint) ((3 * (ulong) st2->x2 + (ulong) st2->x1) >> 2);
			my2 = (uint) ((3 * (ulong) st2->y2 + (ulong) st2->y1) >> 2);
			mz2 = (uint) ((3 * (ulong) st2->z2 + (ulong) st2->z1) >> 2);
		}
		
		dx = mx2 - mx1;
		dy = my2 - my1;
		dz = mz2 - mz1;

		sinY =   sinus( - st1->yaw);
		cosY = cosinus( - st1->yaw);
		sinP =   sinus( - st1->pitch);
		cosP = cosinus( - st1->pitch);
		sinR =   sinus(   st1->roll);
		cosR = cosinus(   st1->roll);
		
		car.pitch = st1->pitch;
		car.roll  = st1->roll;
		
		h  = rotate( CarHeight, cosR);
		hx = rotate( CarHeight, sinR);
		hy = rot1(  h,  0, sinP, cosP);
		hz = rot2(  h,  0, sinP, cosP);
		h  = rot1( hx, hz, sinY, cosY);
		hz = rot2( hx, hz, sinY, cosY);
		hx = h;

		mx1 += hx;
		my1 += hy;
		mz1 += hz;

		OnStreet = TRUE;
	}

	car.x = mx1 + (dx * sub) / MaxPos;
	car.y = my1 + (dy * sub) / MaxPos;
	car.z = mz1 + (dz * sub) / MaxPos;

	car.yaw  = (st2->yaw  * sub + st1->yaw  * (MaxPos-sub)) / MaxPos;
	car.roll = (st2->roll * sub + st1->roll * (MaxPos-sub)) / MaxPos;
	
	if (sub > (MaxPos-swing))
	{	h1 = sub - (MaxPos-swing);
		h2 = swing - h1;
		angle1 = st1->pitch;
		angle2 = st2->pitch;
		if ((angle1 > 4*90) && (angle2 < 4*0))
			angle2 += 4*360;
		car.pitch = (angle2 * h1 + angle1 * h2) / swing;
	}

	if (++SubPosition >= MaxPos)
	{	SubPosition = 0;
		SegmentUnderCar = NextSegment++;
		OnStreet = FALSE;
	}

	car.newX = car.mx = car.x;	car.newYaw   = car.yaw;
	car.newY = car.my = car.y;	car.newPitch = car.pitch;
	car.newZ = car.mz = car.z;	car.newRoll  = car.roll;

	#undef	MaxPos
}









void	MoveCar( void)

{	extern	 char	DemoMode, ExtendedClip, SloMoFlag, EngineOn;
	extern	 char	StartFlag, LapMode, GameMode;
	extern	 int	rollSIN, rollCOS, pitchSIN, pitchCOS;
	extern	 int	yawSIN, yawCOS, OffRoadOut;
	extern	 long	Score;
	extern	 s_car	car;
		 int	h, hx, hy, hz;

	if (DemoMode)
	{	/* READ NEXT POSITION FROM TRACK DATA */

		CarDemoDrive();

		/* SET TRIG. VALUES ACCORDING TO NEW POSITION ANGLES. */

		yawSIN   =   sinus(car.yaw);
		yawCOS   = cosinus(car.yaw);
		pitchSIN =   sinus(car.pitch);
		pitchCOS = cosinus(car.pitch);
		rollSIN  =   sinus(car.roll);
		rollCOS  = cosinus(car.roll);
	}
	else
	{	/* USE NEW POSITION COMPUTED BY THE CAR SIMULATION
		   MODEL. */

		car.mx = car.newX;	car.yaw   = car.newYaw;
		car.my = car.newY;	car.pitch = car.newPitch;
		car.mz = car.newZ;	car.roll  = car.newRoll;

		/* SET TRIG. VALUES ACCORDING TO NEW POSITION ANGLES. */

		yawSIN   =   sinus(car.yaw);
		yawCOS   = cosinus(car.yaw);
		pitchSIN =   sinus(car.pitch);
		pitchCOS = cosinus(car.pitch);
		rollSIN  =   sinus(car.roll);
		rollCOS  = cosinus(car.roll);

		/* COMPUTE OFFSET TO VIEWPOINT. */

		h  = rotate( CarHeight, rollCOS);
		hx = rotate( CarHeight, rollSIN);
		hy = rotate( h,  pitchCOS);
		hz = rotate( h, -pitchSIN);
		h  = rot1( hx, hz, -yawSIN, yawCOS);
		hz = rot2( hx, hz, -yawSIN, yawCOS);
		hx = h;

		car.x = car.mx + hx;
		car.y = car.my + hy;
		car.z = car.mz + hz;

		/* STORE NEW VIEWPOINT POSITION. */

		if (!SloMoFlag && (StartFlag > 5))
		{	RecordCar();
			if (!OffRoadOut && GameMode)
			{	AddBCD( (long) (car.mph >> 4), &Score);
			}
		}
	}

	if ((StartFlag > 0) && (StartFlag < 7))
	{	StartFlag++;
		if (StartFlag >= 6)
			EngineOn = TRUE;
	}

	ExtendedClip = (abs(car.pitch) > 4*35) || (abs(car.roll) > 4*45);
}









static	void near	FindFields( void)

{	extern	 int		NumOfFields;
	extern	 s_object	*field;
	extern	 s_car		car;
	register s_object	*obj;
	register int		i, r, x, y, z;
		 uint		x_min, x_max, z_min, z_max;

	#define	border	(int) (32000L / ScaleFactor)

	x_min = car.x - border;
	z_min = car.z - border;
	x_max = x_min + 2*border;
	z_max = z_min + 2*border;

	obj = field;

	for (i = NumOfFields+1; --i; obj++)
	{	obj->visible = FALSE;

		x = obj->worldX;
		if (x_min > (uint) x || (uint) x > x_max)   continue;
		z = obj->worldZ;
		if (z_min > (uint) z || (uint) z > z_max)   continue;

		obj->clip3D = FALSE;

		y = obj->worldY;

		obj->eyeX = x - car.x;
		obj->eyeY = y - car.y;
		obj->eyeZ = z - car.z;

		x = rot1( obj->eyeX, obj->eyeZ, yawSIN, yawCOS);
		z = rot2( obj->eyeX, obj->eyeZ, yawSIN, yawCOS);
		y = rot1( obj->eyeY, z, pitchSIN, pitchCOS);
		z = rot2( obj->eyeY, z, pitchSIN, pitchCOS);

		r = obj->radius;

		if (z > -r)
		{	if (z < r)
				obj->clip3D = TRUE;

			r = ((z+r) >> 1) + r;

			if (abs(x) < r)
			{	if (abs(y) < r)
					obj->visible = TRUE;
			}
		}
	}

	#undef	border
}








void	FindAllVisibleObjects( void)

{	extern	 char		CarInLoop, DemoMode, SloMoFlag, AccidentFlag;
	extern	 uchar		WrongDirection, NoReturnCheck;
	extern	 int		pitchSIN, pitchCOS, yawSIN, yawCOS;
	extern	 int		VisibleObjects, NumOfObjects, OffRoadOut;
	extern	 s_object	*object;
	extern	 s_car		car;
	register s_object	*obj;
	register int		i, r, x, y, z;
		 uint		x_min, x_max, z_min, z_max;


	NoReturnCheck = (DemoMode | SloMoFlag | WrongDirection | AccidentFlag);

	FindStreets();
	FindFields();

	#define	border	(32000 / ScaleFactor)

	x_min = car.x -   border;
	z_min = car.z -   border;
	x_max = x_min + 2*border;
	z_max = z_min + 2*border;

	CarInLoop = FALSE;
	VisibleObjects = 0;
	obj = object;


	for (i = NumOfObjects+1; --i; obj++)
	{	if (!obj->active)
			continue;

		obj->visible = FALSE;

		x = obj->worldX;
		if (x_min > (uint) x || (uint) x > x_max)   continue;
		z = obj->worldZ;
		if (z_min > (uint) z || (uint) z > z_max)   continue;

		obj->clip3D = FALSE;

		y = obj->worldY;

		obj->eyeX = x - car.x;
		obj->eyeY = y - car.y;
		obj->eyeZ = z - car.z;

		x = rot1( obj->eyeX, obj->eyeZ, yawSIN, yawCOS);
		z = rot2( obj->eyeX, obj->eyeZ, yawSIN, yawCOS);
		y = rot1( obj->eyeY, z, pitchSIN, pitchCOS);
		z = rot2( obj->eyeY, z, pitchSIN, pitchCOS);

		r = obj->radius;

		if (z > -r)
		{	if (z < r)
				obj->clip3D = TRUE;

			r = (3*(z+r) >> 2) + r;

			if (abs(x) < r)
			{	if (abs(y) < r)
				{	obj->visible = TRUE;

					if (obj->small)
					{	r = ((z < 1800) || (z < obj->radius)) ? obj->large : obj->small;
						if (r != obj->type)
						{	obj->type = r;
							SetObjClass(obj);
						}
					}

					     if (obj->class & (c_building + c_hill))
						z += obj->radius;
					else if (obj->large == 111 /* BANKED CURVE */)
						z += obj->radius;
					else if (obj->large ==  28 /* LOOP */)
					     {	z += obj->radius;
						if (obj->clip3D)
							CarInLoop = TRUE;
					     }

					Insert( z, obj);
				}
			}
		}
	}

	#undef	border
}
