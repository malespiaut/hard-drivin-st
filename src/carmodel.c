#include "main.h"
#include "missing.h"
#include "proto.h"

/*

----------------------------------------------------------------------

 MODULE...............	carmodel.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes all functions to
                        simulate the car's behaviour on the road.

 DATE OF CREATION.....	August  18th 1989
 LAST CHANGE..........	October 31th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/

#define OnRoad1 0x01
#define OnRoad2 0x02
#define OnRoad3 0x04
#define OnRoad4 0x08

#define Diagonal1 (OnRoad1 + OnRoad4)
#define Diagonal2 (OnRoad2 + OnRoad3)
#define LeftSide (OnRoad1 + OnRoad3)
#define RightSide (OnRoad2 + OnRoad4)
#define FrontSide (OnRoad1 + OnRoad2)
#define RearSide (OnRoad3 + OnRoad4)

#define RoadTolerance 4
#define gravityConstant 5
#define ImpulseDuration 30

#define CarWidth 5 * 6  // Half width of the car
#define CarLength 5 * 9 // Half length of the car

#define s_OnTheRoad 1 // car status
#define s_Offroad 2
#define s_OnObject 3

#define Limit 4 * 13000L // Maximal allowed acceleration, measured in (DeltaAngle * Speed).
#define FallLimit 45     // Crash limit for landing.
#define SpeedLimit 45    // Crash limit for object collision.

#define NormLength 16384 // Normal vector length

//		LOCAL VARIABLES :
//              =================

static char OnSameSide, // Auxiliary Flag, used by CompHeight().
  status;               // Status indicates car's position.

static int sinY, cosY, // New yaw   trig. values
  sinP, cosP,          // New pitch trig. values
  sinR, cosR,          // New roll  trig. values
  StreetCoor[6 * 3],   // Street surface coordinates.
  x1 = -CarWidth,      // Position of front left  contact point.
  y1 = 0,
  z1 = CarLength,
  x2 = CarWidth, // Position of front right contact point.
  y2 = 0,
  z2 = CarLength,
  x3 = -CarWidth, // Position of rear  left  contact point.
  y3 = 0,
  z3 = -CarLength,
  x4 = CarWidth, // Position of rear  right contact point.
  y4 = 0,
  z4 = -CarLength,
  CarNormX, // Car normal vector.
  CarNormY,
  CarNormZ,
  height1,          // Relative height of wheel 1.
  height2,          // Relative height of wheel 2.
  height3,          // Relative height of wheel 3.
  height4,          // Relative height of wheel 4.
  nx, nz,           // Normal vector to connection between face1 and face2.
  dx11, dy11, dz11, // Vector1 of face1.
  dx12, dy12, dz12, // Vector2 of face1.
  dx13, dy13, dz13, // Vector3 of face1.
  dx14, dy14, dz14, // Vector4 of face1.
  dx21, dy21, dz21, // Vector1 of face2.
  dx22, dy22, dz22, // Vector2 of face2.
  dx23, dy23, dz23, // Vector3 of face2.
  dx24, dy24, dz24; // Vector4 of face2.

//		FUNCTIONS :
//              ===========

static void TurnPitch(int);
static void TurnYaw(int);
static void SetNormalVector(void);
static void PlayerControl(void);
static void CarMotion(void);
static void CarEngine(void);
static char CheckCollision(void);
static void GetNearestStreetSegments(void);
static int CompDistance(int, int, int, char);
static int CompHeight(int, int, int);
static void OnTheRoad(void);
static void OnTheGreen(void);
static void CarOnObject(void);
static void InteractionModel(void);

static void
TurnPitch(int deltaPitch)
{
  int cosR;

  // ROTATION AROUND X-AXIS IN THE EYE-CS.

  if (!deltaPitch)
    return;

  cosR = cosinus(car.newRoll);

  deltaPitch = rotate(deltaPitch, cosR);

  car.newPitch += deltaPitch;
}

static void
TurnYaw(int deltaYaw)
{
  int deltaPitch, deltaRoll, delta;
  int sinR, sinP, cosP;

  // ROTATION AROUND Y-AXIS IN THE EYE-CS.

  if (!deltaYaw)
    return;

  delta = deltaYaw;
  sinR = sinus(car.newRoll);

  sinP = sinus(car.newPitch);
  cosP = cosinus(car.newPitch);

  deltaRoll = rotate(delta, -sinP);

  if (cosP < 0)
    deltaYaw = -3 * deltaYaw >> 1;

  deltaPitch = rotate(delta, -sinR);

  car.newYaw += deltaYaw;
  car.newPitch += deltaPitch;
  car.newRoll += deltaRoll;
}

static void
SetNormalVector(void)
{
  int sinY, cosY, sinP, cosP, sinR, cosR;
  int h;

  // USE NEGATIVE VIEW DIRECTION ANGLES.

  h = -car.newYaw;
  sinY = sinus(h);
  cosY = cosinus(h);
  h = -car.newPitch;
  sinP = sinus(h);
  cosP = cosinus(h);
  h = -car.newRoll;
  sinR = sinus(h);
  cosR = cosinus(h);

  // SET NORMAL VECTOR IN THE EYE COORDINATE SYSTEM.

  // PERFORM INVERSE ROTATION ON THE NORMAL VECTOR.

  h = NormLength;
  CarNormX = rotate(h, -sinR);
  CarNormY = rotate(h, cosR);

  h = CarNormY;
  CarNormY = rotate(h, cosP);
  CarNormZ = rotate(h, sinP);

  h = CarNormX;
  CarNormX = rot1(h, CarNormZ, sinY, cosY);
  CarNormZ = rot2(h, CarNormZ, sinY, cosY);

  // RESULT: NORMAL VECTOR IN THE WORLD COORDINATE SYSTEM.
}

static void
PlayerControl(void)
{
  if (!AccidentFlag)
  {
    // SIMULATE MOMENT OF INERTIA.

    car.newRoll = car.roll + car.deltaRoll;
    car.newPitch = car.pitch;
    TurnPitch(car.deltaPitch);
    // TurnYaw( car.deltaSpin);

    // SIMULATE SPIN OFF.

    SkidFlag = FALSE;

    // STEERING WHEELS: READ INPUT.

    if (car.WheelFlags & FrontSide)
    {
      int h, turn, maxturn;

      h = (SteerX - 320) / 30;

      if (car.speed)
      {
        if (car.mph > 20)
        {
          turn = abs(h) * (abs(car.speed) + 1);

          if (turn > 1300)
          {
            SkidFlag = TRUE;
            h >>= 1;
            if (!CarInLoop && !SpinFlag && (car.mph > 70))
            {
              maxturn = (car.BrakeFlag) ? 1500 : 2000;
              if (turn > maxturn)
              {
                SpinFlag = TRUE;
                car.deltaSpin = h + h + h;
              }
            }
          }
        }

        TurnYaw(h);
      }
    }
  }

  // NORMALIZE ANGLES

  if (car.newYaw > 4 * 180)
    car.newYaw -= 4 * 360;
  else if (car.newYaw < -4 * 180)
    car.newYaw += 4 * 360;

  if (car.newPitch > 4 * 180)
    car.newPitch -= 4 * 360;
  else if (car.newPitch < -4 * 180)
    car.newPitch += 4 * 360;

  if (car.newRoll > 4 * 180)
    car.newRoll -= 4 * 360;
  else if (car.newRoll < -4 * 180)
    car.newRoll += 4 * 360;

  // COMPUTE TEMPORARY TRIG. VALUES FOR SIMULATION MODEL.

  sinY = sinus(car.newYaw);
  cosY = cosinus(car.newYaw);
  sinP = sinus(car.newPitch);
  cosP = cosinus(car.newPitch);
  sinR = sinus(car.newRoll);
  cosR = cosinus(car.newRoll);
}

static void
CarMotion(void)
{
  int h;

  // MOVE FORWARD.

  if (!AccidentFlag)
  { // PERFORM INVERSE ROTATION ON (0,0,speed) VECTOR.

    if (!SpinFlag)
    {
      car.vz = car.speed;

      car.vy = rotate(car.vz, sinP);
      car.vz = rotate(car.vz, cosP);
      car.vx = rotate(car.vz, sinY);
      car.vz = rotate(car.vz, cosY);
    }

    // MOVE CAR FORWARD.

    car.preciseX += car.vx;
    car.preciseY += car.vy;
    car.preciseZ += car.vz;
  }

  // APPLY IMPULSE VECTOR, IF NECESSARY.

  if (car.ImpulseFlag)
  {
    h = --car.ImpulseFlag;
    car.preciseX += multdiv(car.ImpulseX, h, ImpulseDuration);
    car.preciseY += multdiv(car.ImpulseY, h, ImpulseDuration);
    car.preciseZ += multdiv(car.ImpulseZ, h, ImpulseDuration);
  }

  // SIMULATE GRAVITY.

  car.preciseY -= car.GravityY;

  // SCALE 32 BIT POSITION TO 16 BIT.

  car.newX = (unsigned int)(car.preciseX >> 3);
  car.newY = (int)(car.preciseY >> 3);
  car.newZ = (unsigned int)(car.preciseZ >> 3);

  // SIMULATE SPIN OFF.

  if (SpinFlag)
  {
    MakeSound(s_squeal);

    car.deltaSpin = (9 * car.deltaSpin) / 10;

    if (!car.deltaSpin || (car.mph < 20))
    {
      SpinFlag = FALSE;
      car.throttle = (7 * (int)car.throttle) >> 3;
    }
  }
}

static void
CarEngine(void)
{
  int h, *r, *q;
  static int gearRatio[] = {
    0,
    800,
    7,
    800,
    9,
    750,
    10,
    600,
    10,
    450,
  };

  int resistance,
    brake,
    gravity,
    deltaSpeed,
    newRPM,
    deltaRPM;

  unsigned int power,
    speed,
    acceleration;

#define TopSpeed 5 * 100
#define TopMPH 140
#define TopRPM 8000
#define MaxPower 450
#define MaxAcceleration 15
#define MaxDeceleration -20
#define MaxDeltaRPM 450

  if (AccidentFlag)
    return;

  // NO BRAKING, IF ALL WHEELS ARE IN THE AIR.

  if (!car.WheelFlags)
    car.BrakeFlag = FALSE;

  r = &gearRatio[car.gear << 1];

  if (car.gear == 0)
    power = 0;
  else
  {
    h = multdiv((int)car.throttle, MaxPower, MaxThrottle);
    power = multdiv(h, r[1], r[0]);

    if (!CheckOffRoad)
    {
      power /= 10;
    }
  }

  speed = (unsigned int)((car.speed > 0) ? car.speed : 0);

  if ((car.gear == 0) || (!car.WheelFlags))
  {
    // NEUTRAL OR REAR WHEELS SPINNING IN THE AIR.

    newRPM = multdiv((int)car.throttle, TopRPM, MaxThrottle);
    deltaRPM = newRPM - car.rpm;

    if (deltaRPM > MaxDeltaRPM)
      deltaRPM = MaxDeltaRPM;
    else if (deltaRPM < -MaxDeltaRPM)
      deltaRPM = -MaxDeltaRPM;

    car.rpm += deltaRPM;
  }
  else
  {
    if (SpinFlag || (abs(car.wheelSpeed - car.speed) > 10))
    { // SPINNING WHEELS

      // MakeSound( s_squeal); DOESN'T SOUND WELL.

      power >>= 1;
    }

    resistance = (car.speed < 0) ? -1 : 1;

    resistance += (car.speed / 32); // ROLLRESISTANCE ON STREET

    if (SkidFlag)
      resistance += (resistance >> 1);

    if (SpinFlag)
      resistance *= 2;

    if (abs(resistance) > abs(car.speed))
      resistance = car.speed;

    if (car.BrakeFlag)
    {
      brake = car.speed / 35;
      if (!brake)
      {
        brake = (car.speed << 1) / 3;
      }
    }
    else
      brake = 0;

    if (!CarInLoop)
      gravity = rotate(sinP, 50);
    else
      gravity = 0;

    h = (speed + speed + speed) >> 1;
    acceleration = Sqrt(mult(h, h) + power) - h;

    deltaSpeed = acceleration; // ACCELERATION BY ENGINE POWER.
    deltaSpeed -= resistance;  // ROLL RESISTANCE
    deltaSpeed -= brake;       // BRAKE FORCE
    deltaSpeed -= gravity;     // GRAVITY FORCE

    if (deltaSpeed > MaxAcceleration)
      deltaSpeed = MaxAcceleration;

    if (!car.BrakeFlag)
    {
      if (deltaSpeed < MaxDeceleration)
        deltaSpeed = MaxDeceleration;
    }

    car.speed += deltaSpeed;

    h = multdiv(car.speed, TopMPH, TopSpeed);
    h = multdiv(h, r[1], r[0]);
    h -= car.rpm;

    if (h > MaxDeltaRPM)
      h = MaxDeltaRPM;
    else if (h < -MaxDeltaRPM)
      h = -MaxDeltaRPM;

    car.rpm += h;
    if (car.rpm < 0)
      car.rpm = 0;
  }

  // UPPER REV LIMIT.

  if (car.rpm > 6500)
    car.rpm = 6500;

  // CHECK GEARS.

  if (car.autoFlag)
  { // AUTOMATIC

    if (car.rpm < 2000)
    {
      h = car.gear;
      if (h > 1)
      {
        r = &gearRatio[h << 1];
        h = car.rpm;
        h = multdiv(h, r[0], r[1]);
        h = multdiv(h, r[-1], r[-2]);
        car.rpm = h;
        car.gear--;
        car.gearBlock--;
      }
    }
    else if (car.rpm > 5200)
    {
      h = car.gear;
      if (h < 4)
      {
        r = &gearRatio[h << 1];
        h = car.rpm;
        h = multdiv(h, r[0], r[1]);
        h = multdiv(h, r[3], r[2]);
        car.rpm = h;
        car.gear++;
        car.gearBlock++;
      }
    }
  }
  else
  { // MANUAL GEAR SHIFT

    if (car.ShiftFlag || ManualShift)
    {
      char newgear;

      if (car.ShiftFlag && (car.ShiftFlag != car.gear))
      {
        if (car.ShiftFlag == 5)
          car.ShiftFlag = 0;
        newgear = car.ShiftFlag;
        car.ShiftFlag = 0;
      }
      if (ManualShift)
      {
        newgear = ManualShift;
        ManualShift = 0;
      }

      r = &gearRatio[car.gear << 1];
      q = &gearRatio[newgear << 1];
      h = multdiv(car.rpm, r[0], r[1]);
      if (newgear == 0)
        h = car.rpm;
      else
        h = multdiv(h, q[1], q[0]);

      if (newgear < car.gear)
        // DOWNSHIFT: ENGINE SLOWS DOWN.
        car.rpm = (h + car.rpm) >> 1;
      else // UPSHIFT: NO SLOW DOWN.
        car.rpm = h;

      car.gear = newgear;
      car.gearBlock = GearMRect + newgear;
    }
  }

  r = &gearRatio[car.gear << 1];
  car.mph = multdiv(car.rpm, r[0], r[1]);
  car.wheelSpeed = multdiv(car.mph, TopSpeed, TopMPH);

  car.BrakeFlag = FALSE;

  if (EngineOn)
  {
    EnginePitch = multdiv(car.rpm, 1935, TopRPM);
    StartEngine();
  }

#undef TopSpeed
#undef TopMPH
#undef TopRPM
#undef MaxPower
#undef MaxAcceleration
#undef MaxDeceleration
}

static char
CheckCollision(void)
{
  unsigned char i;
  unsigned int* b;
  s_object* obj;
  s_priority* p;
  s_vehicle* vh;
  char NoCollision;
  unsigned int x1, z1, x2, z2, min, max, newX;
  int x, y, z, streetlength, carspeed;
  int crashX, crashY, crashZ, dx, dz, dbx, dbz;
  long a1, a2;

  if (AccidentFlag)
    return (FALSE);

  // DIMENSIONS OF THE CAR CRASH BOX.

  crashX = 30 + (car.mph >> 3);
  crashY = 30 + (car.mph >> 3);
  crashZ = 40 + (car.mph / 3);

  // COLLISION WITH A CAR ?

  p = PrioList;

  if (!LapMode)
  {
    for (i = VisibleObjects + 1; --i;)
    {
      obj = (p++)->obj;

      if ((obj->class != c_car) && (obj->large != 94 + 1))
        continue;

      if (!obj->clip3D)
        continue;

      x = rot1(obj->eyeX, obj->eyeZ, yawSIN, yawCOS);
      z = rot2(obj->eyeX, obj->eyeZ, yawSIN, yawCOS);
      y = rot1(obj->eyeY, z, pitchSIN, pitchCOS);
      z = rot2(obj->eyeY, z, pitchSIN, pitchCOS);

      if (z > crashZ)
        // IGNORE CARS FAR AWAY.
        continue;

      if (abs(x) > crashX)
        continue;

      if (abs(y) > crashY)
        continue;

      if (obj->large == 94 + 1)
      { // NAUGHTY, YOU HIT A COW.

        MakeSound(fx_moo);
        return (FALSE);
      }

      // WHICH CAR ?

      vh = vehicle;
      NoCollision = FALSE;

      for (i = NumOfVehicles + 1; --i;)
      {
        if (vh->obj == obj)
        { // THAT'S THE CAR.

          a1 = mult(car.vx, vh->dx) + mult(car.vy, vh->dy) + mult(car.vz, vh->dz);

          if (a1 > 0L)
          { // BOTH CARS IN SAME DIRECTION.

            streetlength = (int)Sqrt(mult(vh->dx, vh->dx) + mult(vh->dy, vh->dy) + mult(vh->dz, vh->dz));
            carspeed = multdiv(streetlength, vh->speed, MaxRoadPos);

            if (carspeed > car.speed)
            { // CAR CAME FROM BEHIND !

              NoCollision = TRUE;
            }
          }
          else
          { // BOTH CARS IN OPPOSITE DIRECTION.

            if (car.speed > 0)
            {
              vh->speed = -vh->speed;
              vh->OnStreet = FALSE;
            }
          }

          break;
        }
        vh++;
      }

      if (NoCollision)
        break;

      // SET IMPULSE VECTOR.

      car.ImpulseX = -(car.vx);
      car.ImpulseY = -(car.vy);
      car.ImpulseZ = -(car.vz);

      car.ImpulseFlag = ImpulseDuration;

      // RESET TO SAVE POSITION.

      car.preciseX += (car.ImpulseX << 1);
      car.preciseY += (car.ImpulseY << 1);
      car.preciseZ += (car.ImpulseZ << 1);

      car.newX = (unsigned int)(car.preciseX >> 3);
      car.newY = (int)(car.preciseY >> 3);
      car.newZ = (unsigned int)(car.preciseZ >> 3);

      // CANCEL SPEED VECTOR

      car.speed = 0;

      if (car.mph > SpeedLimit)
      {
        car.deltaPitch = 0;
        car.deltaRoll = 0;
        SpinFlag = FALSE;
        AccidentFlag = AccidentDelay;
        car.throttle = 0;
        EnginePitch = 1;

        MakeSound(s_crash);
      }
      else
      {
        MakeSound(s_clonk);
      }

      return (TRUE);
    }
  }

  // COLLISION WITH AN OBJECT ?

  p = PrioList;

  if (CollisionFlag)
  {
    for (i = VisibleObjects + 1; --i;)
    {
      obj = (p++)->obj;
      if (obj->collision)
      {
        obj->collision = FALSE;

        // SET IMPULSE VECTOR.

        car.ImpulseX = -(car.vx);
        car.ImpulseY = -(car.vy);
        car.ImpulseZ = -(car.vz);

        car.ImpulseFlag = ImpulseDuration;

        // RESET TO SAVE POSITION.

        car.preciseX += (car.ImpulseX << 1);
        car.preciseY += (car.ImpulseY << 1);
        car.preciseZ += (car.ImpulseZ << 1);

        car.newX = (unsigned int)(car.preciseX >> 3);
        car.newY = (int)(car.preciseY >> 3);
        car.newZ = (unsigned int)(car.preciseZ >> 3);

        // CANCEL SPEED VECTOR

        car.speed = 0;

        if (car.mph > SpeedLimit)
        {
          car.deltaPitch = 0;
          car.deltaRoll = 0;
          SpinFlag = FALSE;
          AccidentFlag = AccidentDelay;
          car.throttle = 1;
          EnginePitch = 0;
          MakeSound(s_crash);
        }
        else
        {
          MakeSound(s_clonk);
        }

        return (TRUE);
      }
    }
  }

  // COLLISION WITH BARRIERS ?

  if (OffRoadOut)
  {
    b = (unsigned int*)Barriers;

    // EXAMINE CAR MOVEMENT VECTOR.

    if (car.mx < car.newX)
    {
      x1 = car.mx;
      x2 = car.newX;
    }
    else
    {
      x1 = car.newX;
      x2 = car.mx;
    }

    if (car.mz < car.newZ)
    {
      z1 = car.mz;
      z2 = car.newZ;
    }
    else
    {
      z1 = car.newZ;
      z2 = car.mz;
    }

    dx = car.newX - car.mx;
    dz = car.newZ - car.mz;

    for (i = 0; i < NumOfBarriers; i++, b += 4)
    {
      // CAR RECTANGLE AND BARRIER RECTANGLE OVERLAP ?

      // CHECK IN X-DIMENSION.

      if (b[0] < b[2])
      {
        min = b[0];
        max = b[2];
      }
      else
      {
        min = b[2];
        max = b[0];
      }

      if (x1 > max || x2 < min)
        continue;

      // CHECK IN Z-DIMENSION.

      if (b[1] < b[3])
      {
        min = b[1];
        max = b[3];
      }
      else
      {
        min = b[3];
        max = b[1];
      }

      if (z1 > max || z2 < min)
        continue;

      dbx = b[2] - b[0];
      dbz = b[3] - b[1];

      a1 = mult(b[1] - car.mz, dx) - mult(b[0] - car.mx, dz);
      a2 = mult(dz, dbx) - mult(dx, dbz);

      if (a2)
      {
        newX = b[0] + (unsigned int)(a1 * dbx / a2);

        if (x1 <= newX && newX <= x2)
        {
          // COLLISION WITH A BARRIER !

          // CLEAR SPIN.

          car.deltaPitch = 0;
          car.deltaRoll = 0;

          // SET IMPULSE VECTOR.

          car.ImpulseX = -car.vx;
          car.ImpulseY = -car.vy;
          car.ImpulseZ = -car.vz;
          car.ImpulseFlag = ImpulseDuration;

          // SAVE POSITION.

          car.preciseX += (car.ImpulseX << 1);
          car.preciseY += (car.ImpulseY << 1);
          car.preciseZ += (car.ImpulseZ << 1);

          car.newX = (unsigned int)(car.preciseX >> 3);
          car.newY = (int)(car.preciseY >> 3);
          car.newZ = (unsigned int)(car.preciseZ >> 3);

          AccidentFlag = AccidentDelay;
          car.throttle = 1;
          EnginePitch = 0;
          MakeSound(s_crash);
          return (TRUE);
        }
      }
    }
  }

  return (FALSE);
}

static void
GetNearestStreetSegments(void)
{
  int i, d, *p;
  s_track* tr;
  int buf[5 * 2 * 3], distance;
  int dx, dz, *nearest;
  s_track *end, *start;

  // SET TRACK POINTER

  tr = NextStreet - 2;

  // MAKE SURE TO STAY INSIDE THE TRACK ARRAY

  if (OnTrack == 0)
  {
    start = track1;
    end = End1;
    if (tr < track1)
      tr = End1 - (int)(track1 - tr);
  }
  else
  {
    start = track2;
    end = End2;
    if (tr < track2)
    {
      tr = Junct1 - 1;
      start = track1;
      end = End1;
    }
  }

  p = buf;

  // GET RAW COORDINATES.

  for (i = 0; i < 5; i++)
  {
    *p++ = tr->x1 - car.newX;
    *p++ = tr->y1 - car.newY;
    *p++ = tr->z1 - car.newZ;

    *p++ = tr->x2 - car.newX;
    *p++ = tr->y2 - car.newY;
    *p++ = tr->z2 - car.newZ;

    // CHECK AGAINST BOUNDARIES.

    if (++tr >= end)
    {
      if (OnTrack == 1)
      {
        tr = Junct2;
        end = End1;
      }
      else
        tr = start;
    }
  }

  // ROTATE COORDINATES OF FACE1 AND FACE2.

  RotXZ(2 * 5, cosY, sinY, buf, buf);
  RotYZ(2 * 5, cosP, sinP, buf, buf);
  RotXY(2 * 5, cosR, sinR, buf, buf);

  p = buf + 2 * 3;
  distance = MaxInt;

  for (i = 1; i < 3; i++)
  {
    d = abs(p[0] + p[3]) + abs(p[1] + p[4]) + abs(p[2] + p[5]);

    // FIND NEAREST 2 STREET SEGMENTS.

    if (d < distance)
    {
      nearest = p;
      distance = d;
    }

    p += 2 * 3;
  }

  movewords(nearest - 2 * 3, StreetCoor, 6 * 3);

  // 2D-NORMALVECTOR OF CONNECTION LINE BETWEEN FACE1 AND FACE2.

  nx = StreetCoor[3 * 3 + 2] - StreetCoor[2 * 3 + 2];
  nz = StreetCoor[2 * 3 + 0] - StreetCoor[3 * 3 + 0];

  // FLAG INDICATES ORIENTATION OF FACE1.

  dx = StreetCoor[0 * 3 + 0] - StreetCoor[2 * 3 + 0];
  dz = StreetCoor[0 * 3 + 2] - StreetCoor[2 * 3 + 2];

  OnSameSide = (mult(dx, nx) + mult(dz, nz) > 0);

  // SET SURFACE VECTORS OF FACE1 AND FACE2.

  dx11 = StreetCoor[1 * 3 + 0] - StreetCoor[0 * 3 + 0];
  dy11 = StreetCoor[1 * 3 + 1] - StreetCoor[0 * 3 + 1];
  dz11 = StreetCoor[1 * 3 + 2] - StreetCoor[0 * 3 + 2];

  dx12 = StreetCoor[2 * 3 + 0] - StreetCoor[0 * 3 + 0];
  dy12 = StreetCoor[2 * 3 + 1] - StreetCoor[0 * 3 + 1];
  dz12 = StreetCoor[2 * 3 + 2] - StreetCoor[0 * 3 + 2];

  dx13 = StreetCoor[1 * 3 + 0] - StreetCoor[3 * 3 + 0];
  dy13 = StreetCoor[1 * 3 + 1] - StreetCoor[3 * 3 + 1];
  dz13 = StreetCoor[1 * 3 + 2] - StreetCoor[3 * 3 + 2];

  dx14 = StreetCoor[2 * 3 + 0] - StreetCoor[3 * 3 + 0];
  dy14 = StreetCoor[2 * 3 + 1] - StreetCoor[3 * 3 + 1];
  dz14 = StreetCoor[2 * 3 + 2] - StreetCoor[3 * 3 + 2];

  dx21 = StreetCoor[5 * 3 + 0] - StreetCoor[4 * 3 + 0];
  dy21 = StreetCoor[5 * 3 + 1] - StreetCoor[4 * 3 + 1];
  dz21 = StreetCoor[5 * 3 + 2] - StreetCoor[4 * 3 + 2];

  dx22 = StreetCoor[2 * 3 + 0] - StreetCoor[4 * 3 + 0];
  dy22 = StreetCoor[2 * 3 + 1] - StreetCoor[4 * 3 + 1];
  dz22 = StreetCoor[2 * 3 + 2] - StreetCoor[4 * 3 + 2];

  dx23 = StreetCoor[2 * 3 + 0] - StreetCoor[3 * 3 + 0];
  dy23 = StreetCoor[2 * 3 + 1] - StreetCoor[3 * 3 + 1];
  dz23 = StreetCoor[2 * 3 + 2] - StreetCoor[3 * 3 + 2];

  dx24 = StreetCoor[5 * 3 + 0] - StreetCoor[3 * 3 + 0];
  dy24 = StreetCoor[5 * 3 + 1] - StreetCoor[3 * 3 + 1];
  dz24 = StreetCoor[5 * 3 + 2] - StreetCoor[3 * 3 + 2];
}

static int
CompDistance(int x, int y, int z, char set1)
{
  char flag;
  int s1x1, s1y1, s1z1, s1x2, s1y2, s1z2;
  int s2x1, s2y1, s2z1, s2x2, s2y2, s2z2;
  int x1, y1, z1, dx1, dy1, dz1, dx2, dy2, dz2;
  int dx, dz;
  long a1, a2, b1, b2;

  // COMPUTE HEIGHT OVER STREET SEGMENTS.

  switch (set1)
  {
    case 3:
      s1x1 = -dx12;
      s1y1 = -dy12;
      s1z1 = -dz12;
      s1x2 = -dx14;
      s1y2 = -dy14;
      s1z2 = -dz14;
      s2x1 = -dx22;
      s2y1 = -dy22;
      s2z1 = -dz22;
      s2x2 = -dx23;
      s2y2 = -dy23;
      s2z2 = -dz23;
      break;

    case 2:
      s1x1 = -dx11;
      s1y1 = -dy11;
      s1z1 = -dz11;
      s1x2 = -dx13;
      s1y2 = -dy13;
      s1z2 = -dz13;
      s2x1 = -dx21;
      s2y1 = -dy21;
      s2z1 = -dz21;
      s2x2 = -dx24;
      s2y2 = -dy24;
      s2z2 = -dz24;
      break;

    case 1:
      s1x1 = dx11;
      s1y1 = dy11;
      s1z1 = dz11;
      s1x2 = dx12;
      s1y2 = dy12;
      s1z2 = dz12;
      s2x1 = dx21;
      s2y1 = dy21;
      s2z1 = dz21;
      s2x2 = dx22;
      s2y2 = dy22;
      s2z2 = dz22;
      break;

    case 0:
      s1x1 = dx13;
      s1y1 = dy13;
      s1z1 = dz13;
      s1x2 = dx14;
      s1y2 = dy14;
      s1z2 = dz14;
      s2x1 = dx23;
      s2y1 = dy23;
      s2z1 = dz23;
      s2x2 = dx24;
      s2y2 = dy24;
      s2z2 = dz24;
      break;
  }

  dx = x - StreetCoor[2 * 3 + 0];
  dz = z - StreetCoor[2 * 3 + 2];

  flag = ((mult(dx, nx) + mult(dz, nz)) > 0);

  if (flag == OnSameSide)
  {
    x1 = StreetCoor[0 * 3 + 0];
    y1 = StreetCoor[0 * 3 + 1];
    z1 = StreetCoor[0 * 3 + 2];

    dx1 = s1x1;
    dx2 = s1x2;
    dy1 = s1y1;
    dy2 = s1y2;
    dz1 = s1z1;
    dz2 = s1z2;
  }
  else
  {
    x1 = StreetCoor[4 * 3 + 0];
    y1 = StreetCoor[4 * 3 + 1];
    z1 = StreetCoor[4 * 3 + 2];

    dx1 = s2x1;
    dx2 = s2x2;
    dy1 = s2y1;
    dy2 = s2y2;
    dz1 = s2z1;
    dz2 = s2z2;
  }

  dx = x - x1;
  dz = z - z1;

  a1 = (mult(dz, dx2) - mult(dx, dz2)) >> 2;
  a2 = (mult(dz1, dx2) - mult(dx1, dz2)) >> 2;
  b1 = (mult(dz, dx1) - mult(dx, dz1)) >> 2;
  b2 = (mult(dz2, dx1) - mult(dx2, dz1)) >> 2;

  if (!a2 || !b2)
    return (0);

  y -= (y1 + (int)(a1 * dy1 / a2) + (int)(b1 * dy2 / b2));

  return (y);
}

static int
CompHeight(int x, int y, int z)
{
  int x1, y1, z1, dx1, dy1, dz1, dx2, dy2, dz2;
  int dx, dz;
  long a1, a2, b1, b2;

  // COMPUTE HEIGHT OVER OBJECT SURFACE.

  x1 = ObjFace[0];
  y1 = ObjFace[1];
  z1 = ObjFace[2];

  dx1 = ObjFace[3] - x1;
  dx2 = ObjFace[6] - x1;
  dy1 = ObjFace[4] - y1;
  dy2 = ObjFace[7] - y1;
  dz1 = ObjFace[5] - z1;
  dz2 = ObjFace[8] - z1;

  dx = x - x1;
  dz = z - z1;

  a1 = (mult(dz, dx2) - mult(dx, dz2)) >> 1;
  a2 = (mult(dz1, dx2) - mult(dx1, dz2)) >> 1;
  b1 = (mult(dz, dx1) - mult(dx, dz1)) >> 1;
  b2 = (mult(dz2, dx1) - mult(dx2, dz1)) >> 1;

  if (!a2 || !b2)
    return (0);

  y -= y1 + (int)(a1 * dy1 / a2) + (int)(b1 * dy2 / b2);

  return (y);
}

static void
OnTheRoad(void)
{
  status = s_OnTheRoad;

  // DETERMINE NEW POSITION OF NEAREST STREET SEGMENTS.

  GetNearestStreetSegments();

  // COMPUTE HEIGHT ABOVE THE STREET SURFACE FOR ALL CONNECTION POINTS.

#if (0)
  {
    int h1, h2, h3, h4;

    /*
    h1 = CompDistance(x1,y1,z1,0);
    h2 = CompDistance(x1,y1,z1,1);
    height1 = (h1 < h2) ? h1 : h2;
    h1 = CompDistance(x2,y2,z2,0);
    h2 = CompDistance(x2,y2,z2,1);
    height2 = (h1 < h2) ? h1 : h2;
    h1 = CompDistance(x3,y3,z3,0);
    h2 = CompDistance(x3,y3,z3,1);
    height3 = (h1 < h2) ? h1 : h2;
    h1 = CompDistance(x4,y4,z4,0);
    h2 = CompDistance(x4,y4,z4,1);
    height4 = (h1 < h2) ? h1 : h2;
    */
    h1 = CompDistance(x1, y1, z1, 0);
    h2 = CompDistance(x1, y1, z1, 1);
    h3 = CompDistance(x1, y1, z1, 2);
    h4 = CompDistance(x1, y1, z1, 3);
    h1 = (h1 < h2) ? h1 : h2;
    h3 = (h3 < h4) ? h3 : h4;
    height1 = (h1 < h3) ? h1 : h3;
    h1 = CompDistance(x2, y2, z2, 0);
    h2 = CompDistance(x2, y2, z2, 1);
    h3 = CompDistance(x2, y2, z2, 2);
    h4 = CompDistance(x2, y2, z2, 3);
    h1 = (h1 < h2) ? h1 : h2;
    h3 = (h3 < h4) ? h3 : h4;
    height2 = (h1 < h3) ? h1 : h3;
    h1 = CompDistance(x3, y3, z3, 0);
    h2 = CompDistance(x3, y3, z3, 1);
    h3 = CompDistance(x3, y3, z3, 2);
    h4 = CompDistance(x3, y3, z3, 3);
    h1 = (h1 < h2) ? h1 : h2;
    h3 = (h3 < h4) ? h3 : h4;
    height3 = (h1 < h3) ? h1 : h3;
    h1 = CompDistance(x4, y4, z4, 0);
    h2 = CompDistance(x4, y4, z4, 1);
    h3 = CompDistance(x4, y4, z4, 2);
    h4 = CompDistance(x4, y4, z4, 3);
    h1 = (h1 < h2) ? h1 : h2;
    h3 = (h3 < h4) ? h3 : h4;
    height4 = (h1 < h3) ? h1 : h3;
  }
#else

  height1 = (CompDistance(x1, y1, z1, 0) + CompDistance(x1, y1, z1, 1)) / 2;
  height2 = (CompDistance(x2, y2, z2, 0) + CompDistance(x2, y2, z2, 1)) / 2;
  height3 = (CompDistance(x3, y3, z3, 0) + CompDistance(x3, y3, z3, 1)) / 2;
  height4 = (CompDistance(x4, y4, z4, 0) + CompDistance(x4, y4, z4, 1)) / 2;
  /*
  height1  = CompDistance(x1,y1,z1,0);
  height1 += CompDistance(x1,y1,z1,1);
  height1 += CompDistance(x1,y1,z1,2);
  height1 += CompDistance(x1,y1,z1,3);
  height1 /= 4;
  height2  = CompDistance(x2,y2,z2,0);
  height2 += CompDistance(x2,y2,z2,1);
  height2 += CompDistance(x2,y2,z2,2);
  height2 += CompDistance(x2,y2,z2,3);
  height2 /= 4;
  height3  = CompDistance(x3,y3,z3,0);
  height3 += CompDistance(x3,y3,z3,1);
  height3 += CompDistance(x3,y3,z3,2);
  height3 += CompDistance(x3,y3,z3,3);
  height3 /= 4;
  height4  = CompDistance(x4,y4,z4,0);
  height4 += CompDistance(x4,y4,z4,1);
  height4 += CompDistance(x4,y4,z4,2);
  height4 += CompDistance(x4,y4,z4,3);
  height4 /= 4;
  */
#endif
}

static void
OnTheGreen(void)
{
  int k[4 * 3];

  status = s_Offroad;

  // ROTATE/TRANSLATE WHEEL COORDINATES.

  k[0] = x1;
  k[1] = y1;
  k[2] = z1;
  k[3] = x2;
  k[4] = y2;
  k[5] = z2;
  k[6] = x3;
  k[7] = y3;
  k[8] = z3;
  k[9] = x4;
  k[10] = y4;
  k[11] = z4;

  RotYZ(4, cosP, -sinP, k, k);
  RotXY(4, cosR, -sinR, k, k);

  // COMPUTE HEIGHT OF ALL WHEELS ABOVE THE GROUND.

  height1 = car.newY + k[1];
  height2 = car.newY + k[4];
  height3 = car.newY + k[7];
  height4 = car.newY + k[10];
}

static void
CarOnObject(void)
{
  int Correction;

  status = s_OnObject;

  // COMPUTE HEIGHT OF ALL WHEELS ABOVE THE GROUND.

  Correction = rotate(CarHeight - 5, cosP);
  Correction = rotate(Correction, cosR);

  height1 = CompHeight(x1, y1, z1) - Correction;
  height2 = CompHeight(x2, y2, z2) - Correction;
  height3 = CompHeight(x3, y3, z3) - Correction;
  height4 = CompHeight(x4, y4, z4) - Correction;
}

static void
InteractionModel(void)
{
  char flags,     // WHEEL FLAGS
    wheelsOnRoad; // NUMBER OF WHEELS ON THE ROAD.

  int pitchBalance,
    rollBalance,
    averageHeight, // JUST WHAT THE NAME SAYS.
    deltaPitch,    // DELTA PITCH ANGLE.
    deltaRoll,     // DELTA ROLL  ANGLE.
    fall,
    h1;

  // CHECK CAR'S HEIGHT RELATIVE TO THE ROAD.

  car.WheelFlags = wheelsOnRoad = 0;

  if (height1 < RoadTolerance)
  {
    car.WheelFlags |= OnRoad1;
    wheelsOnRoad++;
  }
  if (height2 < RoadTolerance)
  {
    car.WheelFlags |= OnRoad2;
    wheelsOnRoad++;
  }
  if (height3 < RoadTolerance)
  {
    car.WheelFlags |= OnRoad3;
    wheelsOnRoad++;
  }
  if (height4 < RoadTolerance)
  {
    car.WheelFlags |= OnRoad4;
    wheelsOnRoad++;
  }
  flags = car.WheelFlags;

  // CHECK CAR'S BALANCE RELATIVE TO THE STREET SURFACE.

  pitchBalance = ((height1 - height3) + (height2 - height4));
  rollBalance = ((height1 - height2) + (height3 - height4));

  // COMPUTE AVERAGE HEIGHT.

  averageHeight = (height1 + height2 + height3 + height4) >> 2;

  if (wheelsOnRoad >= 3 || flags == Diagonal1 || flags == Diagonal2)
  { // CAR IS COMPLETELY ON THE ROAD.

    car.deltaPitch = 0;
    car.deltaRoll = 0;

    if (rollBalance)
    { // ADJUST CAR'S ROLL ANGLE.

      deltaRoll = arcsin(rollBalance, 4 * CarWidth);
      car.newRoll -= deltaRoll;
    }

    if (pitchBalance)
    { // ADJUST CAR'S PITCH ANGLE.

      deltaPitch = arcsin(pitchBalance, 4 * CarLength);
      TurnPitch(-deltaPitch >> 1);
    }

    if (averageHeight < 0)
    {
      // COMPUTE NORMAL VECTOR

      SetNormalVector();

      car.preciseX += multdiv(CarNormX, averageHeight, -NormLength >> 3);
      car.preciseY += multdiv(CarNormY, averageHeight, -NormLength >> 3);
      car.preciseZ += multdiv(CarNormZ, averageHeight, -NormLength >> 3);

      car.newX = (unsigned int)(car.preciseX >> 3);
      car.newY = (int)(car.preciseY >> 3);
      car.newZ = (unsigned int)(car.preciseZ >> 3);

      // IMPULSE TOO STRONG ?

      if (averageHeight < -5 || (cosP < 0) || (cosR < 0))
      { // SOUND EFFECT: CLONK

        if (car.GravityY > FallLimit)
        {
          MakeSound(s_crash);

          AccidentFlag = AccidentDelay;
          car.throttle = 0;
          EnginePitch = 0;
          StartEngine();
          car.ImpulseFlag = ImpulseDuration;
          car.ImpulseX = car.vx;
          car.ImpulseY = car.vy;
          car.ImpulseZ = car.vz;
        }
        else
          MakeSound(s_clonk);
      }
    }

    if (!CarInLoop)
      car.GravityY = 0;
    else
    {
      if (abs(car.newPitch) > 4 * 80)
      {
        fall = gravityConstant;
        fall -= multdiv(car.mph, gravityConstant, 70);
        car.GravityY += fall;
      }
      else
        car.GravityY = 0;
    }
  }
  else
  {
    if (flags)
    { // SOME OF THE WHEELS ARE ON THE STREET.

      if (flags == LeftSide || flags == RightSide)
      { // ADJUST CAR'S ROLL ANGLE.

        h1 = -arcsin(rollBalance, 4 * CarWidth) / 4;
        car.newRoll += h1;
        car.deltaRoll = h1 >> 1;
      }

      if (flags == FrontSide)
      { // ADJUST CAR'S PITCH ANGLE.

        // HAS TO BE FAIRLY HIGH TO GET THROUGH THE LOOP !

        h1 = -arcsin(pitchBalance, 4 * CarLength) / 4;
        TurnPitch(h1);
        car.deltaPitch = h1 >> 1;

        MakeSound(s_clonk);
      }

      if (flags == RearSide)
      { // ADJUST CAR'S PITCH ANGLE.

        h1 = -arcsin(pitchBalance, 4 * CarLength) / 6;
        car.deltaPitch = h1;
      }
    }
    else
    { // CAR IS FLYING

      car.GravityY += gravityConstant;
    }
  }

  // SPECIAL CASE : SUPPRESS VERY SMALL ROLL ANGLES.

  if ((!car.deltaRoll) && (abs(car.newRoll) < 3))
    car.newRoll = 0;
}

void
CarModel(void)
{
  if (ReturnFlag)
  { // CAR IS SET BACK TO THE RETURN POINT.
    return;
  }

  PlayerControl(); // READ STEERING INPUT.
  CarEngine();     // SIMULATE ENGINE AND DRIVETRAIN.
  CarMotion();     // MOVE CAR.

  if (CheckOffRoad)
  { // CAR IS ON THE ROAD

    OnTheRoad();
  }
  else
  { // CAR IS OFF ROAD

    if (!OnObject)
    { // CAR IS ON THE GREEN

      OnTheGreen();
    }
    else
    { // CAR IS PROBABLY ON A OBJECT

      CarOnObject();
    }
  }

  // CHECK FOR COLLISION. MOVE CAR OUT OF OBJECT,
  // IF NECESSARY.

  if (CheckCollision())
  {
    RewindRecord();
    MoveCar();
    FindAllVisibleObjects();
    CompAllObjects();
  }

  InteractionModel();
}
