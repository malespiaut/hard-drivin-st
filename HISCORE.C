

	#include	"proto.h"






/*

----------------------------------------------------------------------

 MODULE...............	hiscore.c
 PROJECT..............	HARDDRIVING game program

 DESCRIPTION..........	This module includes all functions
 			for the hiscore table.

 DATE OF CREATION.....	April 26th 1989
 LAST CHANGE..........	May 2th 1989
 AUTHOR...............	Juergen Friedrich
 LANGUAGE.............	Turbo C 1.0 for the Atari ST

----------------------------------------------------------------------

*/







/*		LOCAL VARIABLES :
		=================
*/



	static	char	EndFlag;	/* Flag indicates name finished.	*/
	static	char	cursor;		/* Character position.			*/
	static	char	*CharPos;	/* Position in HiscoreList.		*/

	static	int	choosen;	/* ASCII code of choosen character.	*/
	static	int	TableX1,
			TableX2,
			TableX3;	/* Score Table start positions.		*/



/*		FUNCTIONS :
		===========
*/





/*

**********************************************************************
********  H I G H   S C O R E  ***************************************
**********************************************************************

*/



static	void near	EditString( s_score *sc);
static	void near	PrintTable( int num);
static	void near	GetNewName( int num, s_score *sc);
static	void near	ShowNewScoreList( int player);
static	void near	ChampTable( s_score *champ);
static	void near	GetChampName( s_score *champ);







static	void near	EditString( s_score *sc)

{       extern	char	ExitWait, CharNum, SteerSelect;
	extern	int	button, SteerX;
		char	ch;

	if (SteerSelect != 4)
		choosen = multdiv(SteerX,29,641);
	else
	{	if (CharNum <  0)	CharNum =  0;
		if (CharNum > 28)	CharNum = 28;
		choosen = CharNum;
	}

	if (choosen < 27)
	{	if (choosen == 26)
			ch = ' ';
		else
			ch = 'A' + choosen;
		CharPos[cursor]  = ch;
		sc->name[cursor] = ch;
	}

	if (button || ExitWait)
	{	ClearButtons();

		if (choosen >= 28)
		{	CharPos[cursor]  = ' ';
			sc->name[cursor] = ' ';
			EndFlag = TRUE;
			return;
		}

		if (choosen < 27)
		{	if (++cursor > 25)
			{	cursor = 25;
			}
		}
		else
		{	CharPos[cursor]  = ' ';
			sc->name[cursor] = ' ';
			if (--cursor < 0)
				cursor = 0;
			CharPos[cursor]  = ' ';
			sc->name[cursor] = ' ';
		}
	}
}








static	void near	PrintTable( int num)

{	extern	 char		ScoreList[];
	extern	 int		CharWidth, CharHeight, LineHeight, SteerX;
	extern	 int		MinX, OrgX, OrgY;
	register char		*p;
	register int		x, y, BoxY, BoxX;

	SetTextColor(white);
	PrintString( TableX1, 0, "ENTER YOUR NAME TO THE HIGH SCORES.");

	SetTextColor(yellow);
	PrintString( TableX2, LineHeight+2, "USE MOUSE OR KEYS TO SELECT CHARACTER.");

	p  = ScoreList;
	y  = LineHeight + LineHeight + LineHeight;

	for (x = 0; x < 10; x++)
	{	SetTextColor( (x != num) ? darkblue : white);
		PrintString( 1, y, p);
		p += 39;
		y += LineHeight;
	}

	NewColor(red);

	x = (4 + cursor) * CharWidth - OrgX - 1;
	y = (3 + num) * LineHeight - OrgY + 8;

	MoveTo( x, y);
	DrawTo( x+CharWidth, y);

	y = 14 * LineHeight;
	SetTextColor(green);
	PrintString( TableX3, y, "ABCDEFGHIJKLMNOPQRSTUVWXYZ [\\].");

	x  = MinX + CharWidth * (TableX3 + choosen) - 2;
	y -= OrgY + 1;
	BoxX = CharWidth + 2;
	BoxY = CharHeight + 1;

	if (choosen == 28)
		BoxX += CharWidth;

	NewColor(red);
	MoveTo(x,y);		DrawTo(x+BoxX,y);
	DrawTo(x+BoxX,y+BoxY);	DrawTo(x,y+BoxY);
	DrawTo(x,y);
}







static	void near	GetNewName( int num, s_score *sc)

{       extern	char	CharNum;
	extern	int	CharWidth, ScreenX;

	TableX1 = (ScreenX - 34 * CharWidth) / (2 * CharWidth);
	TableX2 = (ScreenX - 38 * CharWidth) / (2 * CharWidth);
	TableX3 = (ScreenX - 29 * CharWidth) / (2 * CharWidth);

	EndFlag = FALSE;
	cursor  = 0;
	CharNum = 0;

	while (!EndFlag)
	{	EditString( sc);
		GetPlayerInput();
		BlankScreen();
		PrintTable( num);
		PrintCockpit();
		Switch();
	}
}







void	ScoreUpdate( void)

{	extern	 char		DirtyFlag, TimeFlag;
	extern	 long		Score;
	extern	 s_score	HiScoreList[];
	register char		*p, *chPos;
	register int		i, j;
	register s_score	*sc, *sc2;

	TimeFlag = FALSE;

	sc = HiScoreList;
	chPos = ScoreList + 3;

	for (i = 0; i < 10; i++)
	{	if (Score > sc->score)
		{	DirtyFlag = TRUE;

			sc2 = HiScoreList + 9;

			while (sc2 > sc)
			{	sc2[0] = sc2[-1];
				sc2--;
			}

			sc->score = Score;
			CharPos   = chPos;

			p = sc->name;
			for (j = 0; j < 29; j++)
				*p++ = ' ';

			ReadScoreList();
			GetNewName( i, sc);
			break;
		}
		sc++;
		chPos += 39;
	}

	ReadScoreList();
}





/*

**********************************************************************
********  B E S T   L A P   T I M E  *********************************
**********************************************************************

*/







static	void near	ShowNewScoreList( int player)

{	extern	char	ScoreList[];
	extern	int	LineHeight;
		char	*p;
		int	i, j, y;

	for (i = 0; i < 200; i++)
	{	GetPlayerInput();
		y = 10;
		p = ScoreList;
		
		BlankScreen();
		
		SetTextColor(white);
		PrintString( 10, y, "CONGRATULATIONS \014\014\014.");
		
		y += 3 * LineHeight;
		
		for (j = 0; j < 10; j++)
		{	SetTextColor( (j == player) ? white : darkblue);
			PrintString( 1, y, p);
			y += LineHeight;
			p += 39;
		}
		
		PrintCockpit();

		Switch();
	}
}







static	void near	ChampTable( s_score *champ)

{	extern	 char		ScoreList[], BestTime[];
	extern	 int		CharWidth, CharHeight, LineHeight;
	extern	 int		MinX, OrgX, OrgY;
	register int		x, y, BoxY, BoxX;
	
	SetTextColor(white);
	PrintString( TableX1,   LineHeight, "ENTER YOUR NAME AS THE NEW.");
	PrintString( TableX1, 2*LineHeight, "   HARD DRIVIN\013 CHAMPION.");

	SetTextColor(yellow);
	PrintString( TableX2, 4*LineHeight, "MOVE MOUSE TO SELECT CHARACTER.");

	y =  9 * LineHeight;

	SetTextColor(white);
	PrintString(  0, y, "\001.");
	PrintString(  2, y, champ->name);
	PrintString( 30, y, BestTime);

	NewColor(red);

	x  = (2 + cursor) * CharWidth - OrgX - 1;
	y -= OrgY - 9;

	MoveTo( x, y);
	DrawTo( x+CharWidth, y);

	y = 14 * LineHeight;

	SetTextColor(green);
	PrintString( TableX3, y, "ABCDEFGHIJKLMNOPQRSTUVWXYZ [\\].");
	
	x  = MinX + CharWidth * (TableX3 + choosen) - 2;
	y -= OrgY + 1;
	BoxX = CharWidth + 2;
	BoxY = CharHeight + 1;
	
	if (choosen == 28)
		BoxX += CharWidth;

	NewColor(red);
	MoveTo(x,y);		DrawTo(x+BoxX,y);
	DrawTo(x+BoxX,y+BoxY);	DrawTo(x,y+BoxY);
	DrawTo(x,y);
}







static	void near	GetChampName( s_score *champ)

{	extern	 int	CharWidth, ScreenX;
	extern	 char	BestTime[], BestName[];
	extern	 long	Time;
	register char	*p, *d;
	register int	i;

	ClearButtons();

	for (p = champ->name, i = 0; i < 28; i++)
		*p++ = ' ';
	*p++ = '.';

	TimeString( BestTime, Time);

	TableX1 = (ScreenX - 26 * CharWidth) / (2 * CharWidth);
	TableX2 = (ScreenX - 30 * CharWidth) / (2 * CharWidth);
	TableX3 = (ScreenX - 29 * CharWidth) / (2 * CharWidth);

	EndFlag = FALSE;
	cursor  = 0;
	CharPos = champ->name;

	while (!EndFlag)
	{	EditString( champ);
		GetPlayerInput();
		BlankScreen();
		ChampTable( champ);
		PrintCockpit();
		Switch();
	}

	p = champ->name;
	d = BestName;

	for (i = 0; i < cursor; i++)
		*d++ = *p++;
	*d++ = '.';
}







void	LapUpdate( void)

{	extern	 char		DirtyFlag, TimeFlag, QualifyFlag;
	extern	 char		BestTime[], BestName[];
	extern	 int		BestLength;
	extern	 long		Score;
	extern	 s_score	HiScoreList[];
	register int		i;
	register s_score	*sc, *sc2;
		 s_score	NewChamp;


	TimeFlag    = FALSE;
	QualifyFlag = FALSE;
	DirtyFlag   = TRUE;

	GetChampName( &NewChamp);

	sc = HiScoreList;
	
	for (i = 0; i < 10; i++)
	{	if (Score > sc->score)
		{	sc2 = HiScoreList + 9;
			
			while (sc2 > sc)
			{	sc2[0] = sc2[-1]; 
				sc2--;
			}
			
			NewChamp.score = Score;
			*sc = NewChamp;

			ReadScoreList();
			ShowNewScoreList(i);
			
			break;
		}
		sc++;
	}
	
	BestLength = StringLength( BestName);
}