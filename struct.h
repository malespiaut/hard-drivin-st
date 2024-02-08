



/*

----------------------------------------------------------------------

 MODULE...............	strukt.h
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes all the global
			data structures, typedef`s and define`s.

 DATE OF CREATION.....	March 22th 1989
 LAST CHANGE..........	November 5th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/


	#define	DemoEnabled		0

	#define	TRUE			(-1)
	#define	FALSE			0
	#define	NIL			((void *) 0)
	#define	ESC			27
	#define	CR			13


	/* SIMPLE DATA TYPES. */

	typedef	unsigned char	uchar;
	typedef	unsigned int	uint;
	typedef	unsigned long	ulong;


	/* SCREEN COLORS. */

	#define	black			 0
	#define	darkgreen		 1
	#define	green			 2
	#define	blue			 3
	#define	yellow			 4
	#define	orange			 5
	#define	red			 6
	#define	darkblue		 7
	#define	darkred			 8
	#define	brown			 9
	#define	grey2			10
	#define	grey3			11
	#define	grey4			12
	#define	grey5			13
	#define	redbrown		14
	#define	grey6			15  /* SAME AS WHITE ! */
	#define	white			15

	#define	blue1			16
	#define	blue2			17
	#define	blue3			18
	#define	blue4			19
	
	#define	grey12			20
	#define	grey23			21
	#define	grey34			22
	#define	grey45			23
	#define	grey56			24

	#define	green1			25
	#define	green2			26
	#define	green3			27
	#define	green4			28
	
	#define	red1			29
	#define	red2			30
	#define	red3			31
	#define	red4			32
	
	#define	brown1			33
	#define	brown2			34
	#define	brown3			35
	#define	brown4			36
	
	#define	orange1			37
	#define	orange2			38
	#define	orange3			39
	#define	orange4			40
	
	#define	redbrown1		41
	#define	redbrown2		42
	#define	redbrown3		43
	#define	redbrown4		44
	
	#define	darkyellow		45
	#define	orangered		46
	#define	midgreen		47
	#define	transparent		48
	
	#define	hatchedblack		60
	#define	hatchedblue		61


	/* SOUND EFFECTS AND TUNES. */

	#define	s_champmusic		 0
	#define	s_titlemusic		 1
	#define	s_finish		 5
	#define	s_extratime		 6
	#define	s_squeal		 7
	#define	s_flag			 8
	#define	s_crash			 9
	#define	s_clonk			10
	#define	s_offroad		13

	#define	fx_moo			 8
	#define	fx_ignition		12


	/* OBJECT CLASSES. */

	#define	c_sign			0x01
	#define	c_chicane		0x02
	#define	c_building		0x04
	#define	c_car			0x08
	#define	c_hill			0x10
	#define	c_loop			0x20
	#define	c_standard		0x40
	#define	c_barrier		0x80


	/* STREET SEGMENT FLAGS */

	#define	f_color			0x0001
	#define	f_junct1		0x0002
	#define	f_junct2		0x0004
	#define	f_invisible		0x0008
	#define	f_hill1			0x0010
	#define	f_hill2			0x0020
	#define	f_hill3			0x0040
	#define	f_hill4			0x0080
	#define	f_underpass		0x0100
	#define	f_loop			0x0200
	#define	f_nil			0x0400
	#define	f_returnpoint		0x0800
	#define	f_checkpoint		0x1000
	#define	f_finish		0x2000

	/* MESSAGES */

	#define	msg_duration		40

	#define	msg_extra		 1
	#define	msg_extended		 2
	#define	msg_qualified		 3
	#define	msg_great		 4
	#define	msg_disqualified	 5
	#define	msg_won			 6
	#define	msg_lost		 7


	/* NAMES OF USED FILES. */

	#define	TrackName		"Files\\Track.DAT"
	#define	DataBaseName		"Files\\Objects.DAT"
	#define	SoundName		"Files\\Snd.PIN"
	#define	ScoreName		"Files\\List"

	/* SCALE DOWN BY FACTOR. */

	#define	ScaleFactor		8

	/* SIZE OF SCREEN RAM IN BYTES. */

	#define	ScreenSize		(32000L)

	/* NUMBER OF DIFFERENT OBJECT TYPES. */

	#define	ObjTypes		145

	/* VERTICAL SIZE OF THE CAR'S WINDOW ON A 320x200 SCREEN. */

	#define	WindowY			135

	/* Vertical size of the cockpit window on a 320x200 screen. */

	#define	PanelY			(200-WindowY)

	/* Size of the window buffer in bytes. */

	#define	WindowSize		(WindowY*160L)

	/* Distance of the horizon. */

	#define	horizon			20000

	/* Largest positive 16 bit integer. */

	#define	MaxInt			(0x7FFF)

	/* Highest screen resolution. */

	#define	MaxXres			320
	#define	MaxYres			200

	/* Maximal number of polygon vertices. */

	#define	MaxVertex		(2*40)

	/* Maximal number of frames for slow motion. */

	#define	MaxFrame		 90

	/* Maximal number of 3D coordinates. */

	#define	MaxKoor			700

	/* HEIGHT OF THE DRIVERS VIEWPOINT. */

	#define	CarHeight		(5*4)

	/* WIDTH OF A STREET ELEMENT. */

	#define	RoadWidth		5*23

	/* MAXIMAL LENGTH OF A STREET ELEMENT. */

	#define	RoadLength		650

	/* TOTAL NUMBER OF SUBPOSITIONS ON A STREETELEMENT. */

	#define	MaxRoadPos		200

	/* NUMBER OF STAGES OF THE WIPE EFFECT. */

	#define	BlendSteps		  8

	/* MAXIMAL NUMBER OF OBJECTS. */

	#define	MaxObj			100

	/* MAXIMAL NUMBER OF POLYGONS PER OBJECT. */

	#define	MaxFace			100

	/* NUMBER OF PRESHIFTS PER SHAPE. */

	#define	preshift		  8

	/* NUMBER OF STREET ELEMENTS TO LOOK FORWARD AND BACKWARD. */

	#define	lookahead		  6

	/* DELAY BETWEEN ACCIDENT AND PLAYBACK. */

	#define	AccidentDelay		 25

	/* THROTTLE GOES FROM 0..100. */

	#define	MaxThrottle		100

	/* MAXIMAL NUMBER OF YELLOW LANE MARKERS. */

	#define	MaxLaneMarker		  6

	/* MAXIMAL NUMBER OF STREET SEGMENTS. */

	#define	MaxSegments		 45

	/* SAMPLING AFTER GIVEN PERIOD. */

	#define	SamplePeriod		  6

	/* MAX NUMBER OF SAMPLES. */

	#define	MaxSample		350






	/* DATA STRUCTURES */



	typedef	struct	so

		{	 int	eyeX, eyeY, eyeZ;		/* coordinate after rotation & translation. */
			uint	radius;				/* object's radius. */
			uint	worldX;				/* position in world coordinate system. */
			 int	worldY;
			uint	worldZ;
			 int	pitch, yaw, roll;		/* object's angles. */
			uint	vertices;			/* number of vectors. */
			uchar	normals;			/* number of normal vectors. */
			uchar	polygons;			/* number polygons. */
			uchar	small, large;			/* models for 'small' and 'large' version. */
			uchar	pre_rotated;			/* TRUE, if object model is prerotated. */
			uchar	type;				/* object model in use. */
			uchar	class;				/* object's class. */
			uchar	visible;			/* visibility flag. */
			uchar	active;				/* TRUE, if object in use */
			uchar	message;
			uchar	collision;			/* TRUE, if viewpoint inside the object */
			uchar	color;				/* individual color, used for cars */
			uchar	clip3D;				/* TRUE, part of the object is behind viewpoint. */
			void	far (*DisplayProc)( struct so *obj);
			 int	far *model, *vert, *koor;	/* pointer to source model, rotated/translated model, and screen coordinates. */
			uchar	far *faces;			/* pointer to polygon description list. */
		}

		s_object;





	typedef	struct		/* ENTRANCE IN THE PRIORITY LIST. */

		{	int		distance;
			s_object	*obj;
		}

		s_priority;




	typedef	struct

		{	int	sx1, sy1, sx2, sy2;
			int	dx1, dy1;
		}

		s_block;





	typedef	struct

		{	uint x;     int y;     uint z;			/* CURRENT VIEWPOINT */
			uint mx;    int my;    uint mz;			/* MIDPOINT, SAME LEVEL AS WHEELS */
			uint newX;  int newY;  uint newZ;		/* MIDPOINT, USED NEXT FRAME */

			long	preciseX;				/* CAR POSITION IN EXTENDED 32 BIT PRECISION. */
			long	preciseY;
			long	preciseZ;

			int	ImpulseX, ImpulseY, ImpulseZ;		/* IMPULSE VECTOR */
			int	GravityY;

			char	WheelFlags, BrakeFlag, ImpulseFlag;	/* WHICH WHEELS ARE ON THE GROUND ? */
			int	deltaRoll, deltaPitch;			/* SPEED OF ROTATION */
			int	spin, deltaSpin;			/* SPIN */

			char	throttle;				/* THROTTLE POSITION IN 0..100% */
			char	ShiftFlag;				/* Indicates a manual gear shift. */
			int	vx, vy, vz, acceleration;
			int	wheelSpeed, speed;
			int	rpm, mph, oil, amp, gas, tmp, gear;
			char	warn[6], autoFlag;
			s_block	*gearBlock;
			int	yaw,    pitch,    roll;			/* CURRENT POSITION ANGLES */
			int	newYaw, newPitch, newRoll;		/* ANGLES USED NEXT FRAME */
		}

		s_car;





	typedef	struct

		{	char	name[29];
			long	score;
		}

		s_score;





	typedef	struct

		{	uint	x1, y1, z1;
			uint	x2, y2, z2;
			int	yaw, pitch, roll;
			uint	flags;
		}

		s_track;





	typedef	struct

		{	uint	x, z;
			int	y;
			int	yaw, pitch, roll;
		}

		s_frame;





	typedef	struct

		{	uint	x, z;
			 int	y;
			 int	yaw, pitch, roll;
		}

		s_carpos;





	typedef	struct

		{	uint		x, z;
			int		y;
			int		yaw, pitch, roll;
			int		engine;
			int		carpos[12*6];
		}

		s_playback;





	typedef	struct

		{	int	screenX, screenY;
			int	segment;
			uchar	covered;
		}

		s_control;	/* CONTROL POINTS */





	typedef	/* struct

		{	int	width;		/* Width in pixels.	*/
			int	height;		/* Height in lines.	*/
			int	words;		/* Width in words.	*/
			int	hotX, hotY;	/* Sweet spot.		*/
			int	size;		/* BLOCK: size in words.
						   SHAPE: bitplane size.*/
			int	far *data;	/* Pointer to data.	*/
		}
		*/

		void far *	s_BitImage;





	typedef	struct

		{	s_track		*StreetUnderCar;	/* STREET SEGMENT UNDER VEHICLE. */
			s_track		*NextStreet;		/* NEXT STREET SEGMENT. */
			s_object	*obj;			/* USED OBJECT TO RENDER VEHICLE. */
			uchar		objNum;			/* NUMBER OF USED OBJECT. */
			uchar		TrackUsed;
			uchar		StartTrack;
			uchar		HomeTrack;
			int		position;		/* POSITION ON SEGMENT. */
			uchar		lane;			/* LANES: 0 = SLOW,BORDER, 1 = FAST,MIDDLE */
			char		OnStreet;		/* TRUE, IF CAR REACHES A NEW SEGMENT. */
			char		direction;		/* 1 = IN TRACK DIRECTION, -1 = AGAINST TRACK DIRECTION */
			uchar		acceleration;		/* CAR'S MAXIMAL ACCELERATION. */
			char		maxspeed;		/* NOMINAL SPEED. */
			char		speed;			/* VEHICLE'S SPEED. */
			uchar		color;			/* VEHICLE'S COLOR. */
			uchar		startposition;
			int		startsegment;		/* CAR IS STARTING ON THIS SEGMENT. */
			int		 dx,  dy,  dz;
			int		mx1, my1, mz1;
			int		mx2, my2, mz2;
		}

		s_vehicle;
