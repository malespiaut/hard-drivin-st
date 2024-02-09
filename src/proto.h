#pragma once

#include "struct.h"

/*

----------------------------------------------------------------------

 MODULE...............	prototype.h
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes the prototype
                        definitions of all global module functions.

 DATE OF CREATION.....	December 4th 1989
 LAST CHANGE..........	December 25th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 2.0 for the IBM PC

----------------------------------------------------------------------

*/

// MODULE...........Markus'Graphic.s

void screenmode(int mode);              // Set appropriate screenmode.
void syncup(void);                      // Switch to new visible screen.
void Clear(void);                       // Clear dummy screen.
void PreShifter(s_BitImage* shape);     // Preshift block image.
void Convert(void* addr, int x, int y); // Convert picture with given width and height.
void Convert2(void* addr);              // Convert shape.
void Convert3(void* addr);              // Convert block.
void Uncomp(void* addr);                // Uncompress picture.
void RealToDummy(void);                 // Copy content of dummy to real screen.
int Pal(int no, int rgb);               // Set new color in VGA palette.
void PalOn(void);                       // Use new VGA palette.

// MODULE...........setup.asm

void SetVecs(void);                 // Set sound and timer vectors.
void SetAudioVector(void);          // Set tune vector.
void ClearVecs(void);               // Restore old vectors.
void InitTune(int speed, int tune); // Start given tune.
int RefreshTune(void);              // Play the tune.
int CheckKeyboard(void);            // Poll the keyboard.
int CheckSteeringKeys(void);
void ResetKeys(void);

// MODULE...........system.s

void EmptyFunction(void);     // Empty interrupt server.
void DivZero(void);           // Division overflow handler.
void Vblank(void);            // 'Vertical Blank' routine.
void CalibrateJoystick(void); // Call once to find center positions.

// MODULE...........comp.s

void movebytes(void* s, void* d, int num); // Move count bytes from source to destination.
void movewords(void* s, void* d, int num); // Move count words from source to destination.
int abs(int n);                            // Absolute value of n. 16 bit.
long labs(long n);                         // Absolute value of n. 32 bit.
long mult(int a, int b);                   // 32 bit result of a * b.
int multdiv(int a, int b, int c);          // Compute a*b/c with 32 bit intermediate result.
int ratio(int a, int b, int c);            // Same as multdiv() plus rounding and overflow check.
int rotate(int a, int b);                  // Compute (a*b) >> 14.
int rot1(int x, int y, int SIN, int COS);  // x' = x * COS - y * SIN;
int rot2(int x, int y, int SIN, int COS);  // y' = x * SIN + y * COS;
int* RotXY(int count, int cos, int sin, int* dest, int* source);
int* RotXZ(int count, int cos, int sin, // Rotation of a [x,y,z] array with count elements.
           int* dest,
           int* source);
int* RotYZ(int count, int cos, int sin, int* dest, int* source);
int sinus(int arg);                        // Returns sine in the range -16384..16384.
int cosinus(int arg);                      // Returns cosine in the range -16384..16384.
int arcsin(int a, int b);                  // Returns arcsin(a/b) in the range -4*90..4*90.
void project(int x, int y, int z, int* p); // Project 3D coordinate into {p[0], p[1]}.
int* ProjectSmall(int n, int* d, int* s);  // Project n {x,y,z} tupel starting at address s and store them at address d.
char IsVisible(int i, int normal, int* v); // Compute visibility of surface, using the surface normal.
void AddBCD(long bcd, long* value);        // Add 32 bit BCD number to value.
void SubBCD(long bcd, long* value);        // Subtract 32 bit BCD number to value.
void IncTime(void);                        // Increment time for 1/100 second.
void DecCountDown(void);                   // Decrement countdown.
void DecOffRoad(void);                     // Decrement offroad time.
void AddTime(long offset, long* time);     // Increment given time variable.

// MODULE...........print.s

void PrintDigit(char digit, int x, int offset); // Print single digit at {x,y}.
void PrintString(int x, int y, char* string);   // Print string of large characters.
                                                // STRINGS HAVE TO BE TERMINATED WITH '.'
void TypeString(int x, int y, char* string);    // Print string of small characters.
                                                // STRINGS HAVE TO BE TERMINATED WITH '#'
void SetTextColor(int color);                   // New text color for both fonts.

// MODULE...........solid.s

void InitBitMask(void);                                                  // Initializes the BitMask array.
void BlankScreen(void);                                                  // Set car window to black (color 0).
void Box(int x1, int y1, int x2, int y2);                                // Solidfill DrawBox routine.
void WideBox(int y1, int y2);                                            // Same as Box( MinX, y1, MaxX, y2).
void SysToVid(int x1, int y1, int x2, int y2, void* source, void* dest); // NO CLIPPING AND RANGE CHECK !!.
void VidToVid(int x1, int y1, int x2, int y2, void* source, void* dest); // NO CLIPPING AND RANGE CHECK !!.
void CopyBlock(int sx1, int sy1, int sx2, int sy2, int dx1, int dy1,     // BitBlock copy routine.
               void* source,
               void* dest); // NO CLIPPING AND RANGE CHECK !!.
void Polygon(int edges);    // Clip and draw Polygon.

// MODULE...........bitmap.s

void BlendIn(int count, int* source); // Blend new picture into visible screen

// MODULE...........line.s

void NewColor(int color);   // Set new color 0..15 for vector routines.
void SolidColor(int color); // Set new color 0..50 for solid fill routines.
void MoveTo(int x, int y);  // Set vector cursor to {x,y}.
void DrawTo(int x, int y);  // Draw vector to {x,y}.
void Plot(int x, int y);    // Plot point at {x,y}.

// MODULE...........ibm.c

void SystemSpeed(void);                                         // Determines the CPU speed.
void CenterMouse(void);                                         // Center mouse pointer position.
void ClearButtons(void);                                        // Clear all mouse and joystick buttons.
void StartEngine(void);                                         // Enable engine sound.
void StopEngine(void);                                          // Disbale engine sound.
void EnableMouse(void);                                         // Just what the name says.
void DisableMouse(void);                                        // Just what the name says.
int SystemInit(void);                                           // System specific initialization.
void SystemExit(char* message);                                 // System specific house keeping before quit.
void SetScreen(void* screenaddr);                               // screenaddr points to the new screen RAM area.
void StopSound(void);                                           // Stop running sound.
void MakeSound(char sound);                                     // Produce given sound effect.
void CreditsScreen(void);                                       // Show credits during boot.
void WaitSwitch(void);                                          // Wait until next SWITCH.
void Switch(void);                                              // Switch between screen Buffer1 and Buffer2.
int Random(void);                                               // Random nummer generator.
void CopySmallScreen(int y1, int y2, void* source, void* dest); // Copy window screen.
int FileRead(char* name, void* addr);                           // Read the file "name" into the buffer at addr.
int FixedRead(char* name, void* addr, long size);               // Read size bytes of "name" into the buffer at addr.
void FileWrite(char* name, void* addr, long size);              // Read the file "name" into the buffer at addr.
int ReadShapeFile(s_BitImage** shape,
                  char* FileName); // Read a shape file.
int ReadBlockFile(s_BitImage** shape,
                  char* FileName); // Read a block file.
void ReadDigiFiles(void);          // Just what the name says.
int LoadSoundModule(void);         // Load the sound engine.
void WaitSync(void);               // Wait for the next vertical sync interrupt.
int KeyPressed(void);              // Return keycode if a key is pressed.
int SetNewColor(int no, int col);  // Set color no to new code col. Return old.
void SetColorPalette(int* addr);   // Install new color palette at adress addr.
void InitColors(void);             // Initialize the hatched colors.
void GetResolution(void);          // Get screen resolution and number of colors.
void* GetPieceOfChunk(long size);  // Allocate one buffer of given size.

// MODULE...........shape.c

void DrawBitBlock(int x, int y, s_BitImage* shape); // Draw image at given position.
void DrawShape(int x, int y, s_BitImage* shape);    // Draw shape at a given position.

// MODULE...........misc.c

void GetPlayerInput(void);                             // Check mouse/joystick/keyboard.
void SaveGameFile(void);                               // Save Hiscores, Best Laptime etc.
void ReadScoreList(void);                              // Just what the name says.
void HardDrivingInit(void);                            // Global initializing.
void GameInit(void);                                   // Initialize all variables and set up for a new game.
void FramePoly(int n);                                 // DEBUGGING: Draw polygon as wire frame.
void Insert(int dist, s_object* obj);                  // Insert new visible object into the priority list.
void InitRecord(void);                                 // Initialize car recording.
void RecordCar(void);                                  // Save current car's poition, bearing and so on.
void RewindRecord(void);                               // Delete last record position, in case of an accident.
void RecordRedCar(void);                               // ChampionLap: Record red car's position.
void ReadBlueCar(void);                                // ChampionLap: Get phantom photon's position.
void ReadRedCar(void);                                 // ChampionLap: Get red car's position.
void TimeString(char* p, long time);                   // Copy time into time string.
void SetCar(int segment, int lane, int row, int side); // Set car to that position.
unsigned char SetObjClass(s_object* obj);              // Set attributes of object class n.
s_object* SearchObject(int type);                      // Find first active object of given type.
void SetArrows(unsigned char status);                  // Enable/Disable gridarrows.
void CopyCarPosition(s_object* obj);                   // Copy car's position and orientation to given object.
void PassCheckpoint(void);                             // Car has passed the checkpoint.
void PassFinish(void);                                 // Car has passed the finish.
int StringLength(char* string);                        // Get the length of a string of large characters.
void UpdateTime(void);                                 // Update various timer dependend on frame rate instead clock rate.
void PrintValue(int x, int y, int value);              // Print numeric value at (x,y) using PrintString or TypeString.

// MODULE...........element.c

void SetNearestElement(void); // Set pointer to nearest street segment.
void PrintTrack(void);        // DEBUGGING: Show map of the race track

// MODULE...........attract.c

void ScaleLogo(int factor);    // Scale HARD DRIVIN' logo.
void DrawLogo(void);           // Draw HARD DRIVIN' logo.
void ShowTitle(void);          // Show the title screen with text.
void ShowRestOfLap(void);      // Show all covered control points.
void ChooseTransmission(void); // Just what the name says.
void SetOptions(void);         // Change game options.

// MODULE...........init.c

void InitObjects(void);       // Initialize all objects.
void InitCar(void);           // Initialize car for a new race.
void InitPhantomPhoton(void); // Set first champion ship lap.
void InitControlPoints(void); // Initialize race tracks.

// MODULE...........hiscore.c

void ScoreUpdate(void); // Build new HiScore list.
void LapUpdate(void);   // Enter new Lap Time.

// MODULE...........display.c

void PrintBackground(void);         // Display earth, horizon, sky and the mountains.
void PrintCockpit(void);            // Display instruments, gauges, messages etc.
void PrintAllObjects(void);         // Just what the name says.
void PrintLoop(s_object* obj);      // Display looping.
void PrintSign(s_object* obj);      // Display sign.
void PrintBridge(s_object* obj);    // Display suspension bridge.
void PrintUnderpass(s_object* obj); // Display underpass.
void PrintCar(s_object* obj);       // Display vehicle.
void PrintObject(s_object* obj);    // Display standard object.
void PrintBrokenWindow(void);       // Print broken window.

// MODULE...........compute.c

unsigned int Sqrt(unsigned long n); // Compute square root.
void CompNormal(int* p1,
                int* p2,
                int* p3,
                int* normal);     // Compute normal vector out of given three coordinates.
int CompRadius(s_object* obj);    // Compute objects's radius.
void CompVertices(s_object* obj); // Compute vertices of a given object.
void CompAllObjects(void);        // Just what the name says.

// MODULE...........movement.c

void MoveCar(void);               // Just what the name says.
void FindAllVisibleObjects(void); // Find visible objects and build priority list.

// MODULE...........newfind.c

void FindStreets(void); // Find all visible Streets on track 1 and 2.

// MODULE...........cars.c

void InitVehicles(void);          // Just what the name says.
void SetVehicle(s_vehicle* vh);   // Set vehicle to next position on the track.
void MoveVehicles(void);          // Just what the name says.
void RevertVehicleMovement(void); // Revert all vehicle movement vectors.
void DisableVehicles(void);       // Set vehicles inactive.
void EnableVehicles(void);        // Set vehicles active.

// MODULE...........text.c

void PrintScore(void);       // Print score in the score field.
void PrintTime(void);        // Print time in the time field.
void PrintQualify(void);     // Print qualify time in the proper field.
void PrintMessage(void);     // Print given message.
void PrintCountDown(void);   // Print countdown for the lap.
void PrintOffRoad(void);     // Print countdown for offroad driving.
void PrintHiscoreList(void); // Scroll Hiscore list up.
void PrintChampion(void);    // Show current champion and best time.

// MODULE...........carmodel.c

void CarModel(void); // Compute new car position, angle and check for collisions.
