

	#include	"proto.h"






/*

----------------------------------------------------------------------

 MODULE...............	carmodel.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes all functions to
			move the computer controlled cars.

 DATE OF CREATION.....	December 18th 1989
 LAST CHANGE..........	January 5th 1990
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 2.0 for the IBM PC

----------------------------------------------------------------------

*/







void	InitVehicles( void)

{	extern	 int		NumOfVehicles;
	extern	 s_vehicle	*vehicle;
	extern	 s_track	*track1, *track2;
	register int		i;
	register s_vehicle	*vh;


	vh = vehicle;

	for (i = 0; i < NumOfVehicles; i++)
	{	vh->obj = SearchObject( vh->objNum);

		if (vh->obj->class == c_car)
		{	vh->obj->color  = vh->color;
		}

		vh->TrackUsed = vh->StartTrack;
		vh->StreetUnderCar = ((vh->TrackUsed == 0) ? track1 : track2) + vh->startsegment;
		vh->NextStreet = NIL;
		vh->OnStreet = FALSE;

		vh++;
	}
}










void	SetVehicle( s_vehicle *vh)

{	extern	 char		UK_Flag;
	extern	 s_track	*track1, *track2;
	extern	 s_track	*End1, *End2, *Junct1, *Junct2;
	register int		sub, h1, h2, angle1, angle2;
	register s_track	*st1, *st2;
	register s_object	*obj;
		 uchar		side;


	#define	swing	 35

	obj = vh->obj;
	obj->message = FALSE;

	if (vh->speed < vh->maxspeed)
	{	vh->speed += vh->acceleration;
		obj->message = FALSE;
	}

	sub = vh->position;		/* SUB POSITION ON STREET SEGMENT. */

	if (!vh->NextStreet)
		vh->NextStreet = vh->StreetUnderCar + vh->direction;

	st1 = vh->StreetUnderCar;	/* CURRENTLY USED SEGMENT. */
	st2 = vh->NextStreet;		/* NEXT STREET SEGMENT. */


	if (!vh->OnStreet)
	{
		if (vh->TrackUsed == 0)
		{	if (st2 >= End1)
				st2 = track1;
			if (st2 < track1)
				st2 = End1 - 1;
			if (vh->HomeTrack != 0)
			{	if ((st1 == Junct1) && (vh->direction ==  1))
				{	st2 = track2 + 1;
					vh->TrackUsed = vh->HomeTrack;
				}
				if ((st1 == Junct2) && (vh->direction == -1))
				{	st2 = End2 - 1;
					vh->TrackUsed = vh->HomeTrack;
				}
			}
			vh->NextStreet = st2;
		}
		else	
		{		if (st2 >= End2)
				{	st2 = Junct2;
					vh->TrackUsed = 0;
				}
			else	if (st2 <  track2)
				{	st2 = Junct1;
					vh->TrackUsed = 0;
				}
			vh->NextStreet = st2;
		}
		
		/* GET START- AND ENDVECTOR. */
	
		if (vh->lane == 0)
		{	/* SLOW LANE */
		
			h1 = 1;  h2 = 7;
		}
		else
		{	/* FAST LANE */
		
			h1 = 3;  h2 = 5;
		}
		
		side = (vh->direction == 1) ? 0 : 1;
		if (UK_Flag)	side ^= 1;
	
		if (side == 1)
		{	vh->mx1 = (uint) ((h2 * (ulong) st1->x1 + h1 * (ulong) st1->x2) >> 3);
			vh->my1 = (uint) ((h2 * (ulong) st1->y1 + h1 * (ulong) st1->y2) >> 3);
			vh->mz1 = (uint) ((h2 * (ulong) st1->z1 + h1 * (ulong) st1->z2) >> 3);
			
			vh->mx2 = (uint) ((h2 * (ulong) st2->x1 + h1 * (ulong) st2->x2) >> 3);
			vh->my2 = (uint) ((h2 * (ulong) st2->y1 + h1 * (ulong) st2->y2) >> 3);
			vh->mz2 = (uint) ((h2 * (ulong) st2->z1 + h1 * (ulong) st2->z2) >> 3);
		}
		else
		{	vh->mx1 = (uint) ((h2 * (ulong) st1->x2 + h1 * (ulong) st1->x1) >> 3);
			vh->my1 = (uint) ((h2 * (ulong) st1->y2 + h1 * (ulong) st1->y1) >> 3);
			vh->mz1 = (uint) ((h2 * (ulong) st1->z2 + h1 * (ulong) st1->z1) >> 3);

			vh->mx2 = (uint) ((h2 * (ulong) st2->x2 + h1 * (ulong) st2->x1) >> 3);
			vh->my2 = (uint) ((h2 * (ulong) st2->y2 + h1 * (ulong) st2->y1) >> 3);
			vh->mz2 = (uint) ((h2 * (ulong) st2->z2 + h1 * (ulong) st2->z1) >> 3);
		}
		
		/* GET MOVEMENT VECTOR. */

		vh->dx = vh->mx2 - vh->mx1;
		vh->dy = vh->my2 - vh->my1;
		vh->dz = vh->mz2 - vh->mz1;
		
		vh->OnStreet = TRUE;
	}
	
	/* INTERPOLATE VEHICLE'S POSITION. */
	
	obj->worldX = vh->mx1 + multdiv( vh->dx, sub, MaxRoadPos);
	obj->worldY = vh->my1 + multdiv( vh->dy, sub, MaxRoadPos);
	obj->worldZ = vh->mz1 + multdiv( vh->dz, sub, MaxRoadPos);
	
	
	/* INTERPOLATE YAW AND ROLL ANGLES. */

	angle1 = multdiv( sub, st2->yaw,  MaxRoadPos) +  multdiv( MaxRoadPos-sub, st1->yaw,  MaxRoadPos);
	angle2 = multdiv( sub, st2->roll, MaxRoadPos) +  multdiv( MaxRoadPos-sub, st1->roll, MaxRoadPos);
	
	if (vh->direction == -1)
	{	obj->yaw   = 4*180 - angle1;
		obj->pitch =   st2->pitch;
		obj->roll  =   angle2;
	}
	else
	{	obj->yaw   =       - angle1;
		obj->pitch = - st1->pitch;
		obj->roll  = - angle2;
	}


	/* ----------------------------------------------------------------
	   --- GIVES PROBLEMS AT CERTAIN POSITIONS. -----------------------

	/* SMOOTH TRANSITION TO NEXT SEGMENT'S ANGLES. */

	if (sub > (MaxRoadPos-swing))
	{	if (st1->pitch != st2->pitch)
		{	h1 = sub - (MaxRoadPos-swing);
			h2 = swing - h1;
			angle1 = st1->pitch;
			angle2 = st2->pitch;
			if ((angle1 > 4*90) && (angle2 < 4*0))
				angle2 += 4*360;
			h1 = multdiv( angle2, h1, swing) + multdiv( angle1, h2, swing);
			obj->pitch = (vh->direction == 1) ? -h1 : h1;
		}
	}
	*/
	
	
	/* LAST SUBPOSITION ON THIS SEGMENT ? */
	
	vh->position += vh->speed;
	
	if (vh->position >= MaxRoadPos)
	{	
		/* PROCEDE TO NEXT STREET SEGMENT. */
	
		vh->position -= MaxRoadPos;
		vh->StreetUnderCar = vh->NextStreet;
		vh->NextStreet    += vh->direction;
		vh->OnStreet = FALSE;
	}
	
	else
	
	if (vh->position < 0)
	{
		/* GO BACK TO LAST STREET SEGMENT. */
	
		vh->position += MaxRoadPos;
		vh->NextStreet      = vh->StreetUnderCar;
		vh->StreetUnderCar -= vh->direction;
		vh->OnStreet = FALSE;
	}


	#undef	swing
}








void	MoveVehicles( void)

{	extern	 char		RaceIsOn;
	extern	 int		NumOfVehicles;
	extern	 s_vehicle	*vehicle;
	register s_vehicle	*vh;
	register int		i;

	if (!RaceIsOn)
		return;

	vh = vehicle;

	for (i = 0; i < NumOfVehicles; i++)
	{	if (vh->obj->class == c_car)
			SetVehicle(vh);
		vh++;
	}
}








void	RevertVehicleMovement( void)

{	extern	 int		NumOfVehicles;
	extern	 s_vehicle	*vehicle;
	register s_vehicle	*vh;
	register int		i;

	vh = vehicle;

	for (i = 0; i < NumOfVehicles; i++)
	{	if (vh->obj->class == c_car)
		{	vh->dx = - vh->dx;
			vh->dy = - vh->dy;
			vh->dz = - vh->dz;
			vh->OnStreet  =   FALSE;
			vh->direction = - vh->direction;
		}
		vh++;
	}
}







void	EnableVehicles( void)

{	extern	 int		NumOfVehicles;
	extern	 s_track	*track1, *track2;
	extern	 s_vehicle	*vehicle;
	register s_vehicle	*vh;
	register int		i;

	vh = vehicle;

	for (i = 0; i < NumOfVehicles; i++)
	{
		vh->TrackUsed      = vh->StartTrack;
		vh->StreetUnderCar = ((vh->TrackUsed == 0) ? track1 : track2) + vh->startsegment;
		vh->NextStreet     = NIL;

		vh->OnStreet = FALSE;
		vh->position = vh->startposition;
		vh->speed    = 0;

		if (vh->obj->class == c_car)
		{	vh->obj->active = TRUE;
			SetVehicle(vh);
		}

		vh++;
	}
}








void	DisableVehicles( void)

{	extern	 int		NumOfVehicles;
	extern	 s_vehicle	*vehicle;
	register s_vehicle	*vh;
	register int		i;

	vh = vehicle;

	for (i = 0; i < NumOfVehicles; i++)
	{	if (vh->obj->class == c_car)
			vh->obj->active = FALSE;
		vh++;
	}
}
