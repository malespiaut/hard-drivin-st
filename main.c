

	#include	"proto.h"
	#include	<stdio.h>
	#include	<dos.h>



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






/*		GLOBAL VARIABLES :
		==================
*/




void		/* SYSTEM VARIABLES. */

		far *ScreenBuffer,	/* Pointer to   visible Screen Buffer.			*/
		far *Buffer2,		/* Pointer to invisible Screen Buffer.			*/

		/* HARD DRIVIN' VARIABLES. */

		far *AuxScreen,		/* Pointer to the auxiliary screen.			*/
		far *MapScreen,		/* Pointer to the map window.				*/
		far *AuxBuffer,		/* Pointer to a auxiliary buffer.			*/
		far *Marcus,		/* Pointer to 32k aux buffer.				*/
		far *Tim,		/* Pointer to 32k aux buffer.				*/
		far *SoundModule,	/* Pointer to sound routines.				*/
		far *SelectScreen;	/* Pointer to the select window.			*/


char            MapName[20],		/* Various picture names.				*/
		SelectName[20],
		LoadName[20],
		PanelName[20],

		SoundFX,		/* Flag enables the sound effects.			*/
		GraphMode,		/* Choosen graph mode.					*/
		SoundEnabled,		/* Flag indicates new sound to be loaded.		*/
		EngineKilled,		/* Flag indicates engine sound was interrupted.		*/
		RaceIsOn,		/* Flag indicates that the race has started.		*/
		EngineOn,		/* Flag indicates engine is running.			*/

		GameMode,		/* Flag indicates end of the game.			*/
		DemoMode,		/* Flag indicates end of the demo mode.			*/
		LapMode,		/* Flag indicates that the championship lap is on.	*/
		LapFinished,		/* Flag indicates championship lap was finished.	*/
		NewScreens,		/* Flag indicates the use of the allocated screens.	*/
		UK_Flag,		/* Flag indicates traffic on the lefthand lane.		*/
		QualifyFlag,		/* Flag indicates proper qualify lap time.		*/
		SwitchFlag,		/* Flag indicates switching of the screen pages.	*/
		BonusFlag,		/* Flag indicates passing of the checkpoint.		*/
		TimeOutFlag,		/* Flag indicates the end of the count down.		*/
		ReturnFlag,		/* Flag indicates an offroad time out.			*/
		CheckOffRoad,		/* Flag indicates car is off road.			*/
		CollisionFlag,		/* Flag indicates a collision with a object.		*/
		AccidentFlag,		/* Flag indicates that the car is destroyed.		*/
		SkidFlag,		/* Flag indicates car is skiding (oversteering).	*/
		SpinFlag,		/* Flag indicates car is spinning off.			*/
		OnObject,		/* Flag indicates car is driving on an object.		*/
		NewPhantomPhoton,	/* Flag indicates winning of championship lap.		*/
		SloMoFlag,		/* Flag indicates beginning of the Slow Mo sequence.	*/
		WrongDirection,		/* Flag indicates wrong driving direction.		*/
		OnTrack,		/* Flag indicates on which track the car travels.	*/
		CarInLoop,		/* Flag indicates car is in the loop.			*/
		ExitWait,		/* Flag indicates end of gear select mode.		*/
		TimeFlag,		/* Flag indicates whether the clock is running.		*/
		StartFlag,		/* Flag indicates race has started.			*/
		StartMessage,		/* Flag indicates printing of the start message.	*/
		DirtyFlag,		/* Flag indicates a hiscore file update.		*/
		QuitFlag,		/* Just what the name says.				*/
		OpaqueFlag,		/* Flag indicates hatched Polygon Draw Mode.		*/
		ExtendedClip,		/* Extended 3D clipping if pitch angle too large.	*/
		CharNum,		/* Choosen char during hiscore entry.			*/
		PhantomLane,		/* Start lane of the Phantom Photon.			*/
		MessageTime,		/* Message display duration.				*/
		MessageCode,		/* Message discription code.				*/
		MouseFlag,		/* Enables/Disables the mouse.				*/
		ManualShift,		/* Indicates manual up or down shift.			*/
		Difficulty,		/* Difficulty of game play.				*/
		SteerSelect,		/* Selects steering input device.			*/
		GearSelect,		/* Selects gear shift input device.			*/

		RedSample,		/* Subsampling step of red car's recording.		*/
		BlueSample,		/* Subsampling step of blue car's recording.		*/

		VisiStr1,		/* Number of visible street elements on track 1.	*/
		VisiStr2,		/* Number of visible street elements on track 2.	*/
		VisiLns1,		/* Number of visible lane markers on track 1.		*/
		VisiLns2,		/* Number of visible lane markers on track 2.		*/

		ScoreList[10*39],	/* Array for Hiscore List.				*/
		BestName[30],		/* Best driver's name.					*/
		BestTime[10];		/* Best lap time.					*/


int		/* VARIABLES DEPENDING ON SCREEN RESOLUTION. */

		OrgX, OrgY,		/* Screen Coordinates of the origin.			*/
		MinX, MaxX, MinY, MaxY,	/* Current Window.					*/
		MinY1, MaxY1, OrgY1,	/* Screen Window.					*/
		MinY2, MaxY2, OrgY2,	/* Cockpit Window.					*/
		ScreenX, ScreenY,	/* Screen resolution.					*/
		MessageY,		/* Y position of the message during demo mode.		*/
		LogoY,			/* Y position of the HardDrivin' logo.			*/
		mphX, mphY,		/* Origin of the speedometer.				*/
		rpmX, rpmY,		/* Origin of the rev gauge.				*/
		ampX, ampY,		/* Origin of the amperemeter.				*/
		tmpX, tmpY,		/* Origin of the thermometer.				*/
		oilX, oilY,		/* Origin of the oil gauge.				*/
		gasX, gasY,		/* Origin of the gasometer.				*/
		largePointer,		/* Length of a large gauge pointer.			*/
		smallPointer,		/* Length of a small gauge pointer.			*/
		ScoreX, ScoreY,		/* Offset to the Score field.				*/
		TimeX, TimeY,		/* Offset to the Time field.				*/
		BestX, BestY,		/* Offset to the Best field.				*/
		LastX, LastY,		/* Offset to the Last field.				*/
		SecondsX, SecondsY,	/* Offset to the Second field.				*/
		OffRoadX, OffRoadY,	/* Offset to the OffRoad field.				*/
		ListX,			/* Offset to the Hiscore field.				*/
		MaxLine,		/* Maximal offset.					*/
		CharWidth,		/* Width  of a large character.				*/
		CharHeight,		/* Height of a large character.				*/
		LineHeight,		/* Height of a line of large characters.		*/
		DigitLength,		/* Length of a digit character.				*/

		/* SYSTEM VARIABLES. */


		hh1, hh2, hh3, hh4, hh5, hh6,


		button,			/* Joysticks/mouse button flags.					*/
		SteerX, SteerY,		/* Steering coordinates.				*/

		OffRoadOut,		/* Countdown: offroad					*/
		frames,			/* count of VBL's.					*/
		MinVBL,			/* Minimal number of VBL's for a new frame.		*/

		/* VARIABLES TO MANAGE STREET ELEMENTS. */

		StartSegment,		/* Number of the start segment.	(Track1)		*/
		StartLane,		/* Start lane.						*/
		StartRow,		/* Start row.						*/
		FinishSegment,		/* Number of the finish segment. (Track1)		*/
		CheckP1_segment,	/* Number of the checkpoint segment on track 1.		*/
		CheckP2_segment,	/* Number of the checkpoint segment on track 2.		*/
		*First1, *First2,	/* Pointer to first visible street element.		*/
		nearestStreet,		/* Distance of the nearest street.			*/
		*LaneM1,
		*LaneM2,
		*Flags1,		/* Flags for visible street elements on track 1.	*/
		*Flags2,		/* Flags for visible street elements on track 1.	*/
		Streets1,		/* Number of street elements on track 1.		*/
		Streets2,		/* Number of street elements on track 2.		*/

		BestLength,		/* Length of the best driver's name.			*/
		BonusCheckpoint,	/* Bonus time when passing the checkpoint.		*/
		BonusFinish,		/* Bonus time when passing the finish.			*/

		EnginePitch,		/* Depends on the engine revs.				*/

		yawSIN,   yawCOS,
		pitchSIN, pitchCOS,
		rollSIN,  rollCOS,	/* trigonometric values of yaw, pitch and roll angles.	*/

		LapViewX,
		LapViewY,
		LapViewZ,		/* Playback viewpoint after championship lap.		*/

		FinishX,
		FinishY,
		FinishZ,		/* Finish coordinates. Used for the championship lap.	*/

		NumOfObjects,		/* Total number of objects in the HardDrivin' universe.	*/
		NumOfVehicles,		/* Total number of cars.				*/
		NumOfBarriers,		/* Total number of barriers.				*/
		NumOfFields,		/* Total number of fields.				*/
		NumOfT1controls,	/* Total number of track1 control points.		*/
		NumOfT2controls,	/* Total number of track2 control points.		*/
		VisibleObjects,		/* Number of visible objects in the next frame.		*/

		*VertIndex,		/* Pointer to the rest of the vertex[] buffer.		*/
		*CoorIndex,		/* Pointer to the rest of the coordinate[] buffer.	*/
		far *ObjDataBase,	/* Pointer to object data base.				*/
		*Barriers,		/* Pointer to barrier coordinates.			*/
		objTable[ObjTypes],	/* Offset table to object data base.			*/
		coordinate[3*MaxKoor],	/* Buffer for 3D coordinates				*/
		vertex[2*(MaxKoor+200)],/* Buffer for 2D coordinates				*/
		Street1Vertex[MaxSegments*3*2],	/* Buffer for visible street elements of track 1.	*/
		Street2Vertex[MaxSegments*3*2],	/* Buffer for visible street elements of track 2.	*/
		ObjFace[3*3],		/* Object surface vectors used by car simulation model.	*/
		TrackData[11000],	/* This array contains all track, control, vehicle data	*/
		PolyVertex[MaxVertex],	/* Vertex paramaters for polygon solid fill		*/
		AuxVertex[MaxVertex];	/* Aux. vertex paramaters for polygon solid fill	*/


long		Score,			/* Current game score.					*/
		HiScore,		/* Just what the name says.				*/
		Duration,		/* Duration of the count down.				*/
		Time,			/* Current game time.					*/
		TimeOut,		/* End time of the count down.				*/
		BestLapTime,		/* Best lap time.					*/
		LastLapTime,		/* Last lap time.					*/
		QualifyTime,		/* Last lap time which qualified.			*/
		TimeToQualify,		/* Just what the name says.				*/

		*Pattern;		/* Pattern for solid fill routines.			*/


s_block		ScoreRect, TimeRect,
		MessageRect[3],		/* Left  windows bit block description.			*/
		MidRect,		/* Mid   gauges  bit block description.			*/
		AutoRect,		/* Bit block for automatic gear display.		*/
		ManualRect,		/* Bit block for manual gear display.			*/
		BarRect,		/* Bit block for steering wheel position.		*/
		GearARect[5],		/* Bit block for 5 gears. (automatic)			*/
		GearMRect[5];		/* Bit block for 5 gears. (manual)			*/

s_object	*field,			/* Pointer to field array.				*/
		*object,		/* Array of all objects.				*/
		*GreenLight,
		*YellowLight,
		*RedLight,		/* Pointer to traffic light objects.			*/
		*RedCar,		/* Pointer to own car. (Championship Lap)		*/
		*BlueCar;		/* Pointer to Phantom Photon's car. (Championship Lap)	*/

s_vehicle	*vehicle;		/* Cars on the track.					*/

s_priority	PrioList[MaxObj];	/* Priority list of visible objects.			*/

s_car		car;			/* Player's car data.					*/

s_score		HiScoreList[10];	/* HiScore list.					*/

s_frame		far *RedPointer,	/* Pointer used to record red car's movement.		*/
		far *BluePointer,	/* Pointer used to record blue car's movement.		*/
		far *OldChampLap,	/* Array holds Phanton Photon's lap.			*/
		far *NewChampLap;	/* Array holds the challenger's lap.			*/

s_playback	SlowMotion[MaxFrame],	/* Array for last MaxFrame frames. (Slow Motion).	*/
		*FramePos;		/* Pointer to current frame in slow motion array.	*/

s_track		*StreetUnderCar,	/* Pointer to street element in use for demo drive.	*/
		*track1,		/* Pointer to street elements of track1.		*/
		*track2,		/* Pointer to street elements of track2.		*/
		*Junct1,
		*Junct2,		/* Pointer to both junctions on track1.			*/
		*End1,
		*End2,			/* Pointer to last street segment + 1 on both tracks.	*/
		*NextStreet;		/* Pointer to nearest street element.			*/

s_control	*NextCtr1,		/* Next control point to pass on track 1.		*/
		*NextCtr2,		/* Next control point to pass on track 2.		*/
		*T1ContPoints,		/* Speed track control points.				*/
		*T2ContPoints;		/* Stunt track control points.				*/


s_BitImage	FlagYellow, FlagRed,	/* Two flags to show last lap run.			*/
		ChampLogo,		/* Championship logo.					*/
		Cup,			/* Championship cup.					*/
		Explosion[6],		/* Explosion shapes.					*/
		Mountain[10];		/* BitMap description of the mountain shapes.		*/








/*		FUNCTIONS :
		===========
*/








static	void	DemoRun( void)

{	static	char	Message;
		int	i;

	/* SKIP, IF NEW GAME REQUIRED. */

	if (!DemoMode)
		return;

	/* NO CARS ! */

	DisableVehicles();

	TimeFlag     = FALSE;
	QualifyFlag  = FALSE;
	AccidentFlag = FALSE;
	GameMode     = FALSE;
	LapMode      = FALSE;
	SloMoFlag    = FALSE;


	/* SET DEMO TEXT TO START POSITION. */

	MessageY = 200;

	/* FIND NEAREST STREET SEGMENT. */

	MoveCar();
	SetNearestElement();

	/* PRODUCE FIRST DEMO SCREEN TO BLEND IN. */

	MoveCar();
	FindAllVisibleObjects();
	CompAllObjects();
	PrintBackground();
	PrintAllObjects();

	/* BLEND FIRST SCREEN IN. */

	UpdateTime();
	Switch();
	RealToDummy();


	/* DEMO. */

	TimeFlag = TRUE;

	for (i = 0; i < 1000; i++)
	{	GetPlayerInput();

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
		{	extern	char	JoyCode, CPUspeed, TuneSpeed;
			extern	char	OverflowFlag;

			SetTextColor( white);
			PrintValue(  1, 10, (uint) VisiStr1);
			PrintValue(  1, 20, (uint) Streets1);
			PrintValue( 18, 10, (uint) VisiStr2);
			PrintValue( 18, 20, (uint) Streets2);
			PrintValue(  1, 30, (uint) VisibleObjects);
			PrintValue(  1, 40, (uint) QualifyFlag);
			PrintValue(  1, 50, (uint) DemoMode);
		}
		#endif

		UpdateTime();
		Switch();
	}

	Message ^= TRUE;
}







static	void	GotoReturnPoint( void)

{	register int		h;
	register s_control	*cn;
		 s_track 	*str;
	static	 uchar		control = 0;
	static	 s_track	*street;


	#define	end	8


	if (!ReturnFlag)
		return;

	if (!control)
	{	/*  RESET CAR */

		car.GravityY = 0;
		car.ImpulseX = 0;
		car.ImpulseY = 0;
		car.ImpulseZ = 0;
		car.deltaSpin  = 0;
		car.wheelSpeed = 0;
		car.speed      = 0;
		car.throttle   = 0;
		car.rpm        = 0;
		car.BrakeFlag   = FALSE;
		car.ImpulseFlag = FALSE;

		OffRoadOut = 0;

		StopSound();

		StartFlag    = 0;
		EngineOn     = FALSE;
		AccidentFlag = FALSE;
		SpinFlag     = FALSE;

		/*  FIND LAST RETURN POINT PASSED. */

		if (NextCtr1-T1ContPoints > NextCtr2-T2ContPoints)
		{	cn = NextCtr1 - 1;
			if (cn < T1ContPoints)
				cn = T1ContPoints;
			NextStreet = street = &track1[cn->segment];
		}
		else
		{	cn = NextCtr2 - 1;
			if (cn < T2ContPoints)
				cn = T2ContPoints;
			NextStreet = street = &track2[cn->segment];
		}
	}

	str = street;

	if (UK_Flag)
	{	car.newX = (uint) ((3 * (ulong) str->x1 + (ulong) str->x2) >> 2);
		car.newY = (uint) ((3 * (ulong) str->y1 + (ulong) str->y2) >> 2);
		car.newZ = (uint) ((3 * (ulong) str->z1 + (ulong) str->z2) >> 2);
	}
	else
	{	car.newX = (uint) ((3 * (ulong) str->x2 + (ulong) str->x1) >> 2);
		car.newY = (uint) ((3 * (ulong) str->y2 + (ulong) str->y1) >> 2);
		car.newZ = (uint) ((3 * (ulong) str->z2 + (ulong) str->z1) >> 2);
	}

	h = str->yaw;

		if (h >  4*180)		h -= 4*360;
	else	if (h < -4*180)		h -= 4*360;

	car.mx = car.newX;
	car.my = car.newY;
	car.mz = car.newZ;

	car.newYaw   = car.yaw   = h;
	car.newPitch = car.pitch = str->pitch;
	car.newRoll  = car.roll  = 0;

	car.deltaPitch = car.deltaRoll = 0;

	car.throttle = car.speed = 0;
	car.vx	     = car.vy	    = car.vz	   = 0;
	car.ImpulseX = car.ImpulseY = car.ImpulseZ = 0;

	car.newY += 4*5 * (end - control);

	car.preciseX = ((long) car.newX) << 3;
	car.preciseY = ((long) car.newY) << 3;
	car.preciseZ = ((long) car.newZ) << 3;

	StopSound();

	if (++control > end)
	{	control    = 0;
		ReturnFlag = FALSE;
		ClearButtons();
	}

	SetNearestElement();
	CenterMouse();
	SpinFlag = FALSE;

	#undef	end
}









static	void	ShowSlowMotion( void)

{	register int		i, j, frames, *p;
	register s_playback	*fr;
	register s_vehicle	*vh;
	register s_object	*obj;
		 s_BitImage	*expl;
		 char		soundFlag;
		 int		ysin, ycos, psin, pcos, dx, dy, dz;
		 char		count1, count2;

	if (!SloMoFlag)
		return;

	StopEngine();
	StopSound();


	/* ACTIVATE RED CAR OBJECT AND STOP TIMER. */

	RedCar->active = TRUE;
	RedCar->color  = red1;
	TimeFlag       = FALSE;
	soundFlag      = TRUE;


	/* SET FRAME POINTER TO START OF RECORDING. */

	fr = FramePos - MaxFrame;
	if (fr < SlowMotion)
		fr += MaxFrame;


	/* SKIP EMPTY SECTION. */

	for (frames = MaxFrame; frames > 0;)
	{	if (!(fr->x | fr->y | fr->z))
		{	frames--;
			fr++;
		}
		else	break;

		if (fr >= &SlowMotion[MaxFrame])
			fr = SlowMotion;
	}


	/* SET STREET POINTER TO NEAREST STREET SEGMENT. */

	car.mx = fr->x;
	car.my = fr->y;
	car.mz = fr->z;		SetNearestElement();


	/* START PLAYBACK. */

	ClearButtons();

	expl = Explosion;
	count1 = 0;
	count2 = 0;

	for (i = 0; (i < frames) && !button; i++)
	{
		/* READ RECORDED CAR POSITION */

		RedCar->yaw   = - fr->yaw;
		RedCar->pitch = - fr->pitch;
		RedCar->roll  = - fr->roll;

		RedCar->worldX = car.newX = fr->x;
		RedCar->worldY = car.newY = fr->y;
		RedCar->worldZ = car.newZ = fr->z;

		EnginePitch = fr->engine;

		if (fr->engine & 0x8000)
			MakeSound( s_squeal);

		/* POSITION VIEWPOINT */

		ysin =   sinus( fr->yaw + 4*90);
		ycos = cosinus( fr->yaw + 4*90);
		psin =   sinus( 4*30);
		pcos = cosinus( 4*30);

		#define	dist	700

		dy = rot1( 0, dist, psin, pcos);
		dz = rot2( 0, dist, psin, pcos);
		dx = rot1( 0,   dz, ysin, ycos);
		dz = rot2( 0,   dz, ysin, ycos);

		#undef	dist

		car.newX -= dx;
		car.newY -= dy;
		car.newZ += dz;

		car.preciseX = (long) car.newX << 3;
		car.preciseY = (long) car.newY << 3;
		car.preciseZ = (long) car.newZ << 3;

		car.newYaw   = fr->yaw - 4*90;
		car.newPitch =         - 4*28;
		car.newRoll  = 0;

		/* SET CARS. */

		vh = vehicle;
		p  = fr->carpos;

		for (j = NumOfVehicles+1; --j;)
		{	obj = (vh++)->obj;
			obj->worldX = (uint) *p++;
			obj->worldY =  (int) *p++;
			obj->worldZ = (uint) *p++;
			obj->yaw   = *p++;
			obj->pitch = *p++;
			obj->roll  = *p++;
		}

		/* PRINT FRAME */

		GetPlayerInput();
		MoveCar();
		FindAllVisibleObjects();
		CompAllObjects();

		PrintBackground();
		PrintAllObjects();

		if (i >= (frames - (AccidentDelay+1)))
		{	DrawShape( (Random() >> 14)-4, (Random() >> 14)+10, *expl);

			if (++count1 > 2)
			{	count1 = 0;
				if (++count2 < 6)
					expl++;
				else	expl--;
			}


			if (soundFlag)
			{	MakeSound( s_crash);
				soundFlag = FALSE;
			}
		}

		StartEngine();

		PrintMessage();
		PrintCockpit();

		SetTextColor(white);
		PrintString( 12, 10, "INSTANT REPLAY.");

		UpdateTime();
		Switch();

		/* NEXT FRAME */

		if (++fr >= &SlowMotion[MaxFrame])
			fr = SlowMotion;
	}


	/* RESET FRAME ARRAY. */

	InitRecord();


	/* DEACTIVATE RED CAR, START TIMER */

	SloMoFlag	= FALSE;
	RedCar->active	= FALSE;
	AccidentFlag	= FALSE;
	SpinFlag	= FALSE;
	SkidFlag	= FALSE;
	car.ImpulseFlag	= FALSE;
	TimeFlag	= TRUE;
	ReturnFlag	= TRUE;

	ClearButtons();
	StopSound();


	/* FINISH CHAMPIONSHIP LAP. */

	LapMode     = FALSE;
	LapFinished = FALSE;


	/* PUT INTO NEUTRAL, IF MANUAL SHIFT. */

	if (!car.autoFlag)
	{	car.ShiftFlag = 5;
	}
}













static	void	GameRun( void)

{       int	i;


	ClearButtons();
	CenterMouse();
	ResetKeys();


	while (GameMode)
	{
		/* CHECK FOR TIME RUNNING OUT. */

		if (TimeOutFlag)
		{	if (car.speed == 0)
				GameMode = FALSE;
			TimeFlag = FALSE;
		}

		/* WAS THERE AN ACCIDENT ? */

		if (AccidentFlag)
		{	if (AccidentFlag == 1)
			{	MessageTime = 0;
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
		{	extern	char	JoyCode, CPUspeed, TuneSpeed;
			extern	char	OverflowFlag;

			SetTextColor( white);
			PrintValue( 1, 10, hh1);
			/*
			PrintValue( 1, 20, (uint) Streets1);
			PrintValue( 1, 30, (uint) QualifyFlag);
			*/
		}
		#endif

		UpdateTime();
		Switch();
	}


	/* GAME OVER SEQUENCE */

	StopSound();
	TimeFlag = FALSE;

	for (i = 0; i < 20+15; i++)
	{	int	h;

		GetPlayerInput();
		FindAllVisibleObjects();
		CompAllObjects();

		PrintBackground();
		PrintAllObjects();
		if (AccidentFlag)
			PrintBrokenWindow();

		h = (i > 20) ? 20 : i;
		SetTextColor(white);
		PrintString( 36 - h, 70, "GAME.");
		PrintString(  1 + h, 70, "OVER.");

		PrintOffRoad();
		PrintCockpit();
		PrintScore();

		UpdateTime();
		Switch();
	}

	ClearButtons();
	StopSound();
}











static	void	ChampPlayBack( void)

{	s_frame		far *redpos, far *bluepos;
	s_object	*winner;
	int		yaw, dx, dz, distance, lastdistance;


	StopSound();

	/* ACTIVATE RED CAR. */

	RedCar->active = TRUE;


	/* WHO HAS WON ? */

	winner = (NewPhantomPhoton) ? RedCar : BlueCar;

	/* REWIND FRAME POINTER TO SHOW THE LAST SECONDS. */

	redpos  = RedPointer;
	bluepos = BluePointer;

	BluePointer -= 30;
	RedPointer  -= 30;

	if (BluePointer < OldChampLap)   BluePointer = OldChampLap;
	if (RedPointer  < NewChampLap)   RedPointer  = NewChampLap;


	/* SET STREET POINTER TO NEAREST STREET SEGMENT. */

	car.newX = car.mx = car.x = LapViewX;
	car.newY = car.my = car.y = LapViewY;
	car.newZ = car.mz = car.z = LapViewZ;

	car.newPitch = car.pitch = -5;
	car.newRoll  = car.roll  =  0;

	pitchSIN =   sinus(car.pitch);
	pitchCOS = cosinus(car.pitch);
	rollSIN  =   sinus(car.roll);
	rollCOS  = cosinus(car.roll);

	car.preciseX = car.newX << 3;
	car.preciseY = car.newY << 3;
	car.preciseZ = car.newZ << 3;

	SetNearestElement();
	lastdistance = 1000;


	/* START PLAYBACK. */

	while ((BluePointer < bluepos-1) && (RedPointer < redpos-1))
	{
		/* READ RECORDED CAR POSITIONS. */

		ReadRedCar();
		ReadBlueCar();

		/* VIEWDIRECTION */

		dx = winner->worldX - car.newX;
		dz = winner->worldZ - car.newZ;

		distance = Sqrt( mult(dx,dx) + mult(dz,dz));

		/* GET THE OLD DOPPLER EFFECT WORKING. */

		EnginePitch = 800 - ((distance - lastdistance) << 3);
		lastdistance = distance;

		StartEngine();

		yaw = arcsin( dx, distance);

		if (dz < 0)
		{	if (yaw > 0)	yaw =  4*180 - yaw;
			else		yaw = -4*180 - yaw;
		}
		car.yaw = car.newYaw = yaw;

		yawSIN =   sinus(car.yaw);
		yawCOS = cosinus(car.yaw);

		/* PRINT FRAME */

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


	/* DEACTIVATE RED CAR. */

	RedCar->active = FALSE;


	/* RESTORE OLD POINTER VALUES. */

	RedPointer  = redpos;
	BluePointer = bluepos;
}









static	void	ChampTitle( void)

{	extern	char	TuneSpeed;
		int	i, ysin, ycos, psin, pcos;
		int	dx, dy, dz, yaw, pitch;


	#define	deltaYaw	-4*152
	#define	deltaPitch	     4
	#define	length		   800


	ClearButtons();

	SetCar( StartSegment, 0, 1, (PhantomLane ^ 1));
	SetNearestElement();

	yaw   = car.newYaw   + deltaYaw;
	pitch = car.newPitch + deltaPitch;


	/* POSITION VIEWPOINT */

	ysin =   sinus( yaw);
	ycos = cosinus( yaw);
	psin =   sinus( pitch);
	pcos = cosinus( pitch);

	dy = rotate( length, psin);
	dz = rotate( length, pcos);
	dx = rotate(     dz, ysin);
	dz = rotate(     dz, ycos);

	dy += 80;

	car.newX += dx;     car.preciseX = (long) car.newX << 3;
	car.newY += dy;     car.preciseY = (long) car.newY << 3;
	car.newZ += dz;     car.preciseZ = (long) car.newZ << 3;

	car.newYaw   =  yaw - 4*180;
	car.newPitch = -pitch;

	/* NORMALIZE ANGLE. */

		if (car.newYaw >  4*180)   car.newYaw -= 4*360;
	else	if (car.newYaw < -4*180)   car.newYaw += 4*360;

	MoveCar();
	FindAllVisibleObjects();
	CompAllObjects();


	PrintBackground();
	PrintAllObjects();

	SetTextColor( black);
	PrintString( (40 - BestLength) >> 1, 5, BestName);

	SetTextColor( white);
	TypeString(  30, 15, "CHALLENGES YOU TO THE#");


	DrawShape( 0, -12, ChampLogo);
	DrawShape( 0,  12, Cup);

	TypeString( 3,  97, "NO CRASHING#");
	TypeString( 3, 106, "ONLY TEN SECONDS OFFROAD#");
	TypeString( 3, 115, "IF YOU WIN[ FUTURE#");
	TypeString( 3, 124, "CHALLENGERS RACE YOU#");

	UpdateTime();
	Switch();
	RealToDummy();

	LogoY += 13;

	for (i = 15; i < 105; i += 3)
	{	GetPlayerInput();
		ScaleLogo(i);
		SolidColor( blue);
		Box( -70, LogoY-10, 70, LogoY+10);
		DrawLogo();
		PrintCockpit();
		PrintScore();
		UpdateTime();
		Switch();
	}

	InitTune( TuneSpeed, 0);
	ClearButtons();
	for (;!(button | KeyPressed());)
		RefreshTune();

	StopSound();
	ClearButtons();
	LogoY -= 13;

	#define	count	7

	dx = ratio( dx, 1, count+1);
	dy = ratio( dy, 1, count+1);
	dz = ratio( dz, 1, count+1);

	for (i = 0; i < count; i++)
	{	car.newX -= dx;
		car.newY -= dy;
		car.newZ -= dz;

		car.newPitch += 1;
		car.newYaw   -= (4*180+deltaYaw) / count;

		car.preciseX = (long) car.newX << 3;
		car.preciseY = (long) car.newY << 3;
		car.preciseZ = (long) car.newZ << 3;

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


	/* DISABLE RED CAR AGAIN. */

	RedCar->active = FALSE;

	StopSound();

	#undef	deltaYaw
	#undef	deltaPitch
	#undef	length
	#undef	count
}












static	void	ChampRace( void)

{	GetPlayerInput();

	/* CHECK IF A CAR HAS REACHED THE FINISH. */

	if (TimeOutFlag)
	{	if (car.speed == 0)
		{	LapMode = FALSE;
		}
		TimeFlag = FALSE;
	}

	/* WAS THERE AN ACCIDENT ? */

	if (AccidentFlag)
	{	if (AccidentFlag == 1)
		{	SloMoFlag = TRUE;
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












static	void	ChampionChipLap( void)

{	char	SaveUK_flag;

	SaveUK_flag      = UK_Flag;
	LapFinished      = FALSE;
	NewPhantomPhoton = FALSE;


	/* SET CHAMPIONSHIP FLAGS. */

	QualifyFlag  = FALSE;
	AccidentFlag = FALSE;
	DemoMode     = FALSE;
	StartFlag    = 0;
	UK_Flag      = FALSE;
	TimeFlag     = FALSE;
	TimeOutFlag  = FALSE;
	BonusFlag    = FALSE;
	EngineOn     = FALSE;
	RaceIsOn     = FALSE;
	ReturnFlag   = FALSE;

	LapMode      = TRUE;
	StartMessage = TRUE;

	MessageTime  = 0;
	MessageCode  = 0;
	Time	     = 0x0000L;

	StopSound();


	/* TRAFFIC LIGHT IS RED. */

	GreenLight->active  = FALSE;
	YellowLight->active = FALSE;
	RedLight->active    = TRUE;


	/* INIT SOLID RED AND HATCHED BLUE CAR. */

	DisableVehicles();
	RedCar->active  = TRUE;
	BlueCar->active = TRUE;
	SetCar( StartSegment, 0, 1, (PhantomLane ^ 1));	  CopyCarPosition( RedCar);
	SetCar( StartSegment, 0, 1, (PhantomLane));	  CopyCarPosition( BlueCar);


	/* ACTIVATE ARROW SIGNS TO SHOW THE WAY. */

	SetArrows(TRUE);


	/* DISPLAY START SCREEN. */

	ChampTitle();


	/* INIT CAR. */

	SetCar( StartSegment, 0, 1, (PhantomLane ^ 1));

	car.deltaPitch = 0;
	car.deltaRoll  = 0;
	car.deltaSpin  = 0;

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

	StopEngine();
	StopSound();
	SetNearestElement();


	/* REWIND CHAMPIONSHIP RECORDING. */

	RedPointer  = NewChampLap;	RedSample  = 0;
	BluePointer = OldChampLap;	BlueSample = 0;


	/* INIT SETTING. */

	OffRoadOut = 0x1099;
	TimeOut    = Duration;
	TimeFlag   = TRUE;

	ClearButtons();
	CenterMouse();


	while (LapMode)
	{	ChampRace();
		UpdateTime();
		Switch();
	}


	/* PLAYBACK OF LAST SECONDS. */

	if (LapFinished)
		ChampPlayBack();


	/* DEACTIVATE ARROW SIGNS. */

	SetArrows(FALSE);


	/* DEACTIVATE BLUE AND RED CAR. */

	EnableVehicles();
	RedCar->active  = FALSE;
	BlueCar->active = FALSE;


	/* SAVE NEW CHAMPION'S LAP. */

	if (NewPhantomPhoton)
	{	s_frame		far *fr1, far *fr2;

		PhantomLane ^= 1;

		fr1 = OldChampLap;
		fr2 = NewChampLap;

		while (fr2 < RedPointer)
			*fr1++ = *fr2++;
		fr2--;
		while (fr1 < &OldChampLap[MaxSample])
			*fr1++ = *fr2;
	}


	/* RESTORE UK FLAG. */

	UK_Flag = SaveUK_flag;

	StopSound();
}











static	void	PlayAGame( void)

{	ChooseTransmission();

	GameInit();
	GameRun();


	if (QualifyFlag)
	{	ChampionChipLap();

		if (NewPhantomPhoton)
		{	/*
			TimeToQualify = Time;
			AddTime( 0x0001, &TimeToQualify);
			*/

			/* if (Time < BestLapTime) */
			{	BestLapTime = Time;
				LapUpdate();
			}
		}
		else	ScoreUpdate();
	}
	else
	{	ShowRestOfLap();
		AddTime( 0x0001, &TimeToQualify);
		ScoreUpdate();
	}

	if (TimeToQualify > 0x020000L)
		TimeToQualify = 0x020000L;

	DemoMode = TRUE;
	GameMode = FALSE;
}






	extern	uint	_stklen = 2000;




main( void)

{	char	ever = TRUE;

	SystemInit();
	HardDrivingInit();

	for (;ever;)
	{	while (DemoMode)
		{	ShowTitle();
			DemoRun();
		}
		PlayAGame();
	}

	SystemExit("");

	return(0);
}
