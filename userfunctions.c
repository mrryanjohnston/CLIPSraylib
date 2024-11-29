   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*            CLIPS Version 6.40  07/30/16             */
   /*                                                     */
   /*                USER FUNCTIONS MODULE                */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.24: Created file to seperate UserFunctions and     */
/*            EnvUserFunctions from main.c.                  */
/*                                                           */
/*      6.30: Removed conditional code for unsupported       */
/*            compilers/operating systems (IBM_MCW,          */
/*            MAC_MCW, and IBM_TBC).                         */
/*                                                           */
/*            Removed use of void pointers for specific      */
/*            data structures.                               */
/*                                                           */
/*************************************************************/

/***************************************************************************/
/*                                                                         */
/* Permission is hereby granted, free of charge, to any person obtaining   */
/* a copy of this software and associated documentation files (the         */
/* "Software"), to deal in the Software without restriction, including     */
/* without limitation the rights to use, copy, modify, merge, publish,     */
/* distribute, and/or sell copies of the Software, and to permit persons   */
/* to whom the Software is furnished to do so.                             */
/*                                                                         */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS */
/* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF              */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT   */
/* OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY  */
/* CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES */
/* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN   */
/* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF */
/* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.          */
/*                                                                         */
/***************************************************************************/

#include "clips.h"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

void UserFunctions(Environment *);

void RaylibInitWindow(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	long long x, y;
	UDFValue theArg;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	x = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	y = theArg.integerValue->contents;

	UDFNextArgument(context,STRING_BIT,&theArg);
	InitWindow(x, y, theArg.lexemeValue->contents);
}

void RaylibWindowShouldClose(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	if (WindowShouldClose())
	{
		returnValue->lexemeValue = TrueSymbol(theEnv);
	}
	else
	{
		returnValue->lexemeValue = FalseSymbol(theEnv);
	}
}

void RaylibBeginDrawing(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	BeginDrawing();
}

void str_to_color(const char *str, Color *color)
{
	if (strncmp("LIGHTGRAY", str, 9) == 0) {
		*color = LIGHTGRAY;
	} else if (strncmp("GRAY", str, 4) == 0) {
		*color = GRAY;
	} else if (strncmp("DARKGRAY", str, 8) == 0) {
		*color = DARKGRAY;
	} else if (strncmp("YELLOW", str, 6) == 0) {
		*color = YELLOW;
	} else if (strncmp("GOLD", str, 4) == 0) {
		*color = GOLD;
	} else if (strncmp("ORANGE", str, 6) == 0) {
		*color = YELLOW;
	} else if (strncmp("PINK", str, 4) == 0) {
		*color = PINK;
	} else if (strncmp("RED", str, 3) == 0) {
		*color = RED;
	} else if (strncmp("MAROON", str, 6) == 0) {
		*color = MAROON;
	} else if (strncmp("GREEN", str, 5) == 0) {
		*color = GREEN;
	} else if (strncmp("LIME", str, 4) == 0) {
		*color = LIME;
	} else if (strncmp("DARKGREEN", str, 9) == 0) {
		*color = DARKGREEN;
	} else if (strncmp("SKYBLUE", str, 7) == 0) {
		*color = SKYBLUE;
	} else if (strncmp("BLUE", str, 4) == 0) {
		*color = BLUE;
	} else if (strncmp("DARKBLUE", str, 8) == 0) {
		*color = DARKBLUE;
	} else if (strncmp("PURPLE", str, 6) == 0) {
		*color = PURPLE;
	} else if (strncmp("VIOLET", str, 6) == 0) {
		*color = VIOLET;
	} else if (strncmp("DARKPURPLE", str, 10) == 0) {
		*color = DARKPURPLE;
	} else if (strncmp("BEIGE", str, 5) == 0) {
		*color = BEIGE;
	} else if (strncmp("BROWN", str, 5) == 0) {
		*color = BROWN;
	} else if (strncmp("DARKBROWN", str, 9) == 0) {
		*color = DARKBROWN;
	} else if (strncmp("WHITE", str, 5) == 0) {
		*color = WHITE;
	} else if (strncmp("BLACK", str, 5) == 0) {
		*color = BLACK;
	} else if (strncmp("MAGENTA", str, 7) == 0) {
		*color = MAGENTA;
	} else if (strncmp("RAYWHITE", str, 8) == 0) {
		*color = RAYWHITE;
	} else {
		*color = BLACK;
	}
}

void RaylibClearBackground(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	Color color;
	UDFValue theArg;

	UDFNextArgument(context,SYMBOL_BIT,&theArg);
	str_to_color(theArg.lexemeValue->contents, &color);
	ClearBackground(color);
}

void RaylibDrawText(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	const char *theText;
	long long x, y, fontSize;
	Color color;
	UDFValue theArg;

	UDFNextArgument(context,STRING_BIT,&theArg);
	theText = theArg.lexemeValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	x = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	y = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	fontSize = theArg.integerValue->contents;

	UDFNextArgument(context,SYMBOL_BIT,&theArg);
	str_to_color(theArg.lexemeValue->contents, &color);

	DrawText(theText, x, y, fontSize, color);
}

void RaylibEndDrawing(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	EndDrawing();
}

void RaylibCloseWindow(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	CloseWindow();
}

void RaylibDrawCircle(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	long long centerx, centery;
	float radius;
	Color color;
	UDFValue theArg;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	centerx = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	centery = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			radius = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			radius = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,SYMBOL_BIT,&theArg);
	str_to_color(theArg.lexemeValue->contents, &color);

	DrawCircle(centerx, centery, radius, color);
}

void RaylibDrawCircleLines(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	long long centerx, centery;
	float radius;
	Color color;
	UDFValue theArg;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	centerx = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	centery = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			radius = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			radius = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,SYMBOL_BIT,&theArg);
	str_to_color(theArg.lexemeValue->contents, &color);

	DrawCircleLines(centerx, centery, radius, color);
}


void RaylibDrawRectangle(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	long long positionx, positiony, width, height;
	Color color;
	UDFValue theArg;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	positionx = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	positiony = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	width = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	height = theArg.integerValue->contents;

	UDFNextArgument(context,SYMBOL_BIT,&theArg);
	str_to_color(theArg.lexemeValue->contents, &color);

	DrawRectangle(positionx, positiony, width, height, color);
}

void RaylibDrawRectangleLines(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	long long positionx, positiony, width, height;
	Color color;
	UDFValue theArg;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	positionx = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	positiony = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	width = theArg.integerValue->contents;

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	height = theArg.integerValue->contents;

	UDFNextArgument(context,SYMBOL_BIT,&theArg);
	str_to_color(theArg.lexemeValue->contents, &color);

	DrawRectangleLines(positionx, positiony, width, height, color);
}

void RaylibDrawRectangleRounded(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	float x, y, width, height, roundness;
	long long segments;
	Color color;
	UDFValue theArg;

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			x = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			x = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	y = theArg.integerValue->contents;
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			y = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			y = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			width = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			width = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			height = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			height = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			roundness = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			roundness = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	segments = theArg.integerValue->contents;

	UDFNextArgument(context,SYMBOL_BIT,&theArg);
	str_to_color(theArg.lexemeValue->contents, &color);

	Rectangle rec = { x, y, width, height };

	DrawRectangleRounded(rec, roundness, segments, color);
}

void RaylibDrawRectangleRoundedLines(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	float x, y, width, height, roundness, lineThickness;
	long long segments;
	Color color;
	UDFValue theArg;

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			x = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			x = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	y = theArg.integerValue->contents;
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			y = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			y = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			width = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			width = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			height = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			height = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			roundness = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			roundness = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,INTEGER_BIT,&theArg);
	segments = theArg.integerValue->contents;

	UDFNextArgument(context,NUMBER_BITS,&theArg);
	switch (theArg.header->type) {
		case INTEGER_TYPE:
			lineThickness = 1.0f * theArg.integerValue->contents;
			break;
		case FLOAT_TYPE:
			lineThickness = theArg.floatValue->contents;
			break;
	}

	UDFNextArgument(context,SYMBOL_BIT,&theArg);
	str_to_color(theArg.lexemeValue->contents, &color);

	Rectangle rec = { x, y, width, height };

	//DrawRectangleRoundedLines(rec, roundness, segments, lineThickness, color);
	DrawRectangleRoundedLines(rec, roundness, segments, color);
}

void RaylibGetRenderHeight(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, GetRenderHeight());
}

void RaylibGetRenderWidth(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, GetRenderWidth());
}

void RaylibGetScreenHeight(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, GetScreenHeight());
}

void RaylibGetScreenWidth(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, GetScreenWidth());
}

void RaylibGetMousePosition(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	Vector2 v;
	MultifieldBuilder *mb = CreateMultifieldBuilder(theEnv, 0);
	v = GetMousePosition();
	MBAppendFloat(mb, v.x);
	MBAppendFloat(mb, v.y);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

void RaylibGetMouseDelta(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	Vector2 v;
	MultifieldBuilder *mb = CreateMultifieldBuilder(theEnv, 0);
	v = GetMouseDelta();
	MBAppendFloat(mb, v.x);
	MBAppendFloat(mb, v.y);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

void RaylibIsMouseButtonPressed(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		if (
			IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
			IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ||
			IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) ||
			IsMouseButtonPressed(MOUSE_BUTTON_SIDE) ||
			IsMouseButtonPressed(MOUSE_BUTTON_EXTRA) ||
			IsMouseButtonPressed(MOUSE_BUTTON_FORWARD) ||
			IsMouseButtonPressed(MOUSE_BUTTON_BACK)
		)
		{
			returnValue->lexemeValue = TrueSymbol(theEnv);
		}
		else
		{
			returnValue->lexemeValue = FalseSymbol(theEnv);
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			if (strncmp("MOUSE_BUTTON_LEFT", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_RIGHT", theArg.lexemeValue->contents, 18) == 0)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_MIDDLE", theArg.lexemeValue->contents, 19) == 0)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_SIDE", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_SIDE))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_EXTRA", theArg.lexemeValue->contents, 18) == 0)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_EXTRA))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_FORWARD", theArg.lexemeValue->contents, 20) == 0)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_FORWARD))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_BACK", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_BACK))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			{
				returnValue->lexemeValue = FalseSymbol(theEnv);
			}
		}
	}

}

void RaylibIsMouseButtonDown(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		if (
			IsMouseButtonDown(MOUSE_BUTTON_LEFT) ||
			IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ||
			IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) ||
			IsMouseButtonDown(MOUSE_BUTTON_SIDE) ||
			IsMouseButtonDown(MOUSE_BUTTON_EXTRA) ||
			IsMouseButtonDown(MOUSE_BUTTON_FORWARD) ||
			IsMouseButtonDown(MOUSE_BUTTON_BACK)
		)
		{
			returnValue->lexemeValue = TrueSymbol(theEnv);
		}
		else
		{
			returnValue->lexemeValue = FalseSymbol(theEnv);
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			if (strncmp("MOUSE_BUTTON_LEFT", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_RIGHT", theArg.lexemeValue->contents, 18) == 0)
			{
				if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_MIDDLE", theArg.lexemeValue->contents, 19) == 0)
			{
				if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_SIDE", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonDown(MOUSE_BUTTON_SIDE))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_EXTRA", theArg.lexemeValue->contents, 18) == 0)
			{
				if (IsMouseButtonDown(MOUSE_BUTTON_EXTRA))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_FORWARD", theArg.lexemeValue->contents, 20) == 0)
			{
				if (IsMouseButtonDown(MOUSE_BUTTON_FORWARD))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_BACK", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonDown(MOUSE_BUTTON_BACK))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			{
				returnValue->lexemeValue = FalseSymbol(theEnv);
			}
		}
	}
}

void RaylibIsMouseButtonReleased(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		if (
			IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ||
			IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) ||
			IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) ||
			IsMouseButtonReleased(MOUSE_BUTTON_SIDE) ||
			IsMouseButtonReleased(MOUSE_BUTTON_EXTRA) ||
			IsMouseButtonReleased(MOUSE_BUTTON_FORWARD) ||
			IsMouseButtonReleased(MOUSE_BUTTON_BACK)
		)
		{
			returnValue->lexemeValue = TrueSymbol(theEnv);
		}
		else
		{
			returnValue->lexemeValue = FalseSymbol(theEnv);
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			if (strncmp("MOUSE_BUTTON_LEFT", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_RIGHT", theArg.lexemeValue->contents, 18) == 0)
			{
				if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_MIDDLE", theArg.lexemeValue->contents, 19) == 0)
			{
				if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_SIDE", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonReleased(MOUSE_BUTTON_SIDE))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_EXTRA", theArg.lexemeValue->contents, 18) == 0)
			{
				if (IsMouseButtonReleased(MOUSE_BUTTON_EXTRA))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_FORWARD", theArg.lexemeValue->contents, 20) == 0)
			{
				if (IsMouseButtonReleased(MOUSE_BUTTON_FORWARD))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_BACK", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonReleased(MOUSE_BUTTON_BACK))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			{
				returnValue->lexemeValue = FalseSymbol(theEnv);
			}
		}
	}
}

void RaylibIsMouseButtonUp(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		if (
			IsMouseButtonUp(MOUSE_BUTTON_LEFT) ||
			IsMouseButtonUp(MOUSE_BUTTON_RIGHT) ||
			IsMouseButtonUp(MOUSE_BUTTON_MIDDLE) ||
			IsMouseButtonUp(MOUSE_BUTTON_SIDE) ||
			IsMouseButtonUp(MOUSE_BUTTON_EXTRA) ||
			IsMouseButtonUp(MOUSE_BUTTON_FORWARD) ||
			IsMouseButtonUp(MOUSE_BUTTON_BACK)
		)
		{
			returnValue->lexemeValue = TrueSymbol(theEnv);
		}
		else
		{
			returnValue->lexemeValue = FalseSymbol(theEnv);
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			if (strncmp("MOUSE_BUTTON_LEFT", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonUp(MOUSE_BUTTON_LEFT))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_RIGHT", theArg.lexemeValue->contents, 18) == 0)
			{
				if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_MIDDLE", theArg.lexemeValue->contents, 19) == 0)
			{
				if (IsMouseButtonUp(MOUSE_BUTTON_MIDDLE))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_SIDE", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonUp(MOUSE_BUTTON_SIDE))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_EXTRA", theArg.lexemeValue->contents, 18) == 0)
			{
				if (IsMouseButtonUp(MOUSE_BUTTON_EXTRA))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_FORWARD", theArg.lexemeValue->contents, 20) == 0)
			{
				if (IsMouseButtonUp(MOUSE_BUTTON_FORWARD))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			if (strncmp("MOUSE_BUTTON_BACK", theArg.lexemeValue->contents, 17) == 0)
			{
				if (IsMouseButtonUp(MOUSE_BUTTON_BACK))
				{
					returnValue->lexemeValue = TrueSymbol(theEnv);
				}
				else
				{
					returnValue->lexemeValue = FalseSymbol(theEnv);
				}
			}
			else
			{
				returnValue->lexemeValue = FalseSymbol(theEnv);
			}
		}
	}
}
const int KEYS[] =
{
	KEY_APOSTROPHE,
	KEY_COMMA,
	KEY_MINUS,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_ZERO,
	KEY_ONE,
	KEY_TWO,
	KEY_THREE,
	KEY_FOUR,
	KEY_FIVE,
	KEY_SIX,
	KEY_SEVEN,
	KEY_EIGHT,
	KEY_NINE,
	KEY_SEMICOLON,
	KEY_EQUAL,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_LEFT_BRACKET,
	KEY_BACKSLASH,
	KEY_RIGHT_BRACKET,
	KEY_GRAVE,
	KEY_SPACE,
	KEY_ESCAPE,
	KEY_ENTER,
	KEY_TAB,
	KEY_BACKSPACE,
	KEY_INSERT,
	KEY_DELETE,
	KEY_RIGHT,
	KEY_LEFT,
	KEY_DOWN,
	KEY_UP,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_HOME,
	KEY_END,
	KEY_CAPS_LOCK,
	KEY_SCROLL_LOCK,
	KEY_NUM_LOCK,
	KEY_PRINT_SCREEN,
	KEY_PAUSE,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_F,
	KEY_LEFT_SHIFT,
	KEY_LEFT_CONTROL,
	KEY_LEFT_ALT,
	KEY_LEFT_SUPER,
	KEY_RIGHT_SHIFT,
	KEY_RIGHT_CONTROL,
	KEY_RIGHT_ALT,
	KEY_RIGHT_SUPER,
	KEY_KB_MENU,
	KEY_KP_0,
	KEY_KP_1,
	KEY_KP_2,
	KEY_KP_3,
	KEY_KP_4,
	KEY_KP_5,
	KEY_KP_6,
	KEY_KP_7,
	KEY_KP_8,
	KEY_KP_9,
	KEY_KP_DECIMAL,
	KEY_KP_DIVIDE,
	KEY_KP_MULTIPLY,
	KEY_KP_SUBTRACT,
	KEY_KP_ADD,
	KEY_KP_ENTER,
	KEY_KP_EQUAL,
	KEY_BACK,
	KEY_MENU,
	KEY_VOLUME_UP,
	KEY_VOLUME_DOWN
};
const char *KEY_STRINGS[] =
{
	"KEY_APOSTROPHE",
	"KEY_COMMA",
	"KEY_MINUS",
	"KEY_PERIOD",
	"KEY_SLASH",
	"KEY_ZERO",
	"KEY_ONE",
	"KEY_TWO",
	"KEY_THREE",
	"KEY_FOUR",
	"KEY_FIVE",
	"KEY_SIX",
	"KEY_SEVEN",
	"KEY_EIGHT",
	"KEY_NINE",
	"KEY_SEMICOLON",
	"KEY_EQUAL",
	"KEY_A",
	"KEY_B",
	"KEY_C",
	"KEY_D",
	"KEY_E",
	"KEY_F",
	"KEY_G",
	"KEY_H",
	"KEY_I",
	"KEY_J",
	"KEY_K",
	"KEY_L",
	"KEY_M",
	"KEY_N",
	"KEY_O",
	"KEY_P",
	"KEY_Q",
	"KEY_R",
	"KEY_S",
	"KEY_T",
	"KEY_U",
	"KEY_V",
	"KEY_W",
	"KEY_X",
	"KEY_Y",
	"KEY_Z",
	"KEY_LEFT_BRACKET",
	"KEY_BACKSLASH",
	"KEY_RIGHT_BRACKET",
	"KEY_GRAVE",
	"KEY_SPACE",
	"KEY_ESCAPE",
	"KEY_ENTER",
	"KEY_TAB",
	"KEY_BACKSPACE",
	"KEY_INSERT",
	"KEY_DELETE",
	"KEY_RIGHT",
	"KEY_LEFT",
	"KEY_DOWN",
	"KEY_UP",
	"KEY_PAGE_UP",
	"KEY_PAGE_DOWN",
	"KEY_HOME",
	"KEY_END",
	"KEY_CAPS_LOCK",
	"KEY_SCROLL_LOCK",
	"KEY_NUM_LOCK",
	"KEY_PRINT_SCREEN",
	"KEY_PAUSE",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_F",
	"KEY_LEFT_SHIFT",
	"KEY_LEFT_CONTROL",
	"KEY_LEFT_ALT",
	"KEY_LEFT_SUPER",
	"KEY_RIGHT_SHIFT",
	"KEY_RIGHT_CONTROL",
	"KEY_RIGHT_ALT",
	"KEY_RIGHT_SUPER",
	"KEY_KB_MENU",
	"KEY_KP_0",
	"KEY_KP_1",
	"KEY_KP_2",
	"KEY_KP_3",
	"KEY_KP_4",
	"KEY_KP_5",
	"KEY_KP_6",
	"KEY_KP_7",
	"KEY_KP_8",
	"KEY_KP_9",
	"KEY_KP_DECIMAL",
	"KEY_KP_DIVIDE",
	"KEY_KP_MULTIPLY",
	"KEY_KP_SUBTRACT",
	"KEY_KP_ADD",
	"KEY_KP_ENTER",
	"KEY_KP_EQUAL",
	"KEY_BACK",
	"KEY_MENU",
	"KEY_VOLUME_UP",
	"KEY_VOLUME_DOWN"
};
void RaylibIsKeyPressed(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		for (int x=0; x < 109; x++)
		{
			if (IsKeyPressed(KEYS[x])) {
				returnValue->lexemeValue = TrueSymbol(theEnv);
				return;
			}
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			for (int x=0; x < 109; x++)
			{
				if (strncmp(KEY_STRINGS[x], theArg.lexemeValue->contents, strlen(KEY_STRINGS[x])) == 0 &&
					IsKeyPressed(KEYS[x])) {
					returnValue->lexemeValue = TrueSymbol(theEnv);
					return;
				}
			}
		}
	}
	returnValue->lexemeValue = FalseSymbol(theEnv);
}

void RaylibIsKeyPressedRepeat(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		for (int x=0; x < 109; x++)
		{
			if (IsKeyPressedRepeat(KEYS[x]))
			{
				returnValue->lexemeValue = TrueSymbol(theEnv);
				return;
			}
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			for (int x=0; x < 109; x++)
			{
				if (strncmp(KEY_STRINGS[x], theArg.lexemeValue->contents, strlen(KEY_STRINGS[x])) == 0 &&
					IsKeyPressedRepeat(KEYS[x])) {
					returnValue->lexemeValue = TrueSymbol(theEnv);
					return;
				}
			}
		}
	}
	returnValue->lexemeValue = FalseSymbol(theEnv);
}

void RaylibIsKeyDown(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		for (int x=0; x < 109; x++)
		{
			if (IsKeyDown(KEYS[x])) {
				returnValue->lexemeValue = TrueSymbol(theEnv);
				return;
			}
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			for (int x=0; x < 109; x++)
			{
				if (strncmp(KEY_STRINGS[x], theArg.lexemeValue->contents, strlen(KEY_STRINGS[x])) == 0 &&
					IsKeyDown(KEYS[x])) {
					returnValue->lexemeValue = TrueSymbol(theEnv);
					return;
				}
			}
		}
	}
	returnValue->lexemeValue = FalseSymbol(theEnv);
}

void RaylibIsKeyReleased(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		for (int x=0; x < 109; x++)
		{
			if (IsKeyReleased(KEYS[x])) {
				returnValue->lexemeValue = TrueSymbol(theEnv);
				return;
			}
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			for (int x=0; x < 109; x++)
			{
				if (strncmp(KEY_STRINGS[x], theArg.lexemeValue->contents, strlen(KEY_STRINGS[x])) == 0 &&
					IsKeyReleased(KEYS[x])) {
					returnValue->lexemeValue = TrueSymbol(theEnv);
					return;
				}
			}
		}
	}
	returnValue->lexemeValue = FalseSymbol(theEnv);
}

void RaylibIsKeyUp(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	if (!UDFHasNextArgument(context))
	{
		for (int x=0; x < 109; x++)
		{
			if (IsKeyUp(KEYS[x])) {
				returnValue->lexemeValue = TrueSymbol(theEnv);
				return;
			}
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			UDFNextArgument(context,SYMBOL_BIT,&theArg);
			for (int x=0; x < 109; x++)
			{
				if (strncmp(KEY_STRINGS[x], theArg.lexemeValue->contents, strlen(KEY_STRINGS[x])) == 0 &&
					IsKeyUp(KEYS[x])) {
					returnValue->lexemeValue = TrueSymbol(theEnv);
					return;
				}
			}
		}
	}
	returnValue->lexemeValue = FalseSymbol(theEnv);
}

void RaylibBeginMode2D(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	long long offsetx, offsety, targetx, targety;
	float rotation, zoom;
	UDFValue theArg;

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,INTEGER_BIT,&theArg);
		offsetx = theArg.integerValue->contents;
	}
	else
	{
		offsetx = 0;
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,INTEGER_BIT,&theArg);
		offsety = theArg.integerValue->contents;
	}
	else
	{
		offsety = 0;
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,INTEGER_BIT,&theArg);
		targetx = theArg.integerValue->contents;
	}
	else
	{
		targetx = 0;
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,INTEGER_BIT,&theArg);
		targety = theArg.integerValue->contents;
	}
	else
	{
		targety = 0;
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,FLOAT_BIT,&theArg);
		rotation = theArg.floatValue->contents;
	}
	else
	{
		rotation = 0.0f;
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,FLOAT_BIT,&theArg);
		zoom = theArg.floatValue->contents;
	}
	else
	{
		zoom = 1.0f;
	}

	Camera2D camera = { 0 };
	camera.offset = (Vector2){ offsetx, offsety };
	camera.target = (Vector2){ targetx, targety };
	camera.rotation = rotation;
	camera.zoom = zoom;

	BeginMode2D(camera);
}

void RaylibEndMode2D(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	EndMode2D();
}


void RaylibSetTargetFPS(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	UDFValue theArg;

	UDFNextArgument(context,INTEGER_BIT,&theArg);

	SetTargetFPS(theArg.integerValue->contents);
}

void RaylibGetFrameTime(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	returnValue->floatValue = CreateFloat(theEnv, GetFrameTime());
}

void RaylibGetScreenToWorld2D(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	float screenx, screeny, offsetx, offsety, targetx, targety, rotation, zoom = 0.0f;
	UDFValue theArg;

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				screenx = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				screenx = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				screeny = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				screeny = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				offsetx = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				offsetx = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				offsety = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				offsety = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				targetx = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				targetx = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				targety = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				targety = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				rotation = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				rotation = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				zoom = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				zoom = theArg.floatValue->contents;
				break;
		}
	}

	Vector2 vector = { offsetx, offsety };
	Camera2D camera = { 0 };
	camera.offset = (Vector2){ offsetx, offsety };
	camera.target = (Vector2){ targetx, targety };
	camera.rotation = rotation;
	camera.zoom = zoom;

	Vector2 screen = GetScreenToWorld2D(vector, camera);

	MultifieldBuilder *mb = CreateMultifieldBuilder(theEnv, 2);
	MBAppendFloat(mb, screen.x);
	MBAppendFloat(mb, screen.y);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

void RaylibGetWorldToScreen2D(
		Environment *theEnv,
		UDFContext *context,
		UDFValue *returnValue)
{
	float screenx, screeny, offsetx, offsety, targetx, targety, rotation, zoom = 0.0f;
	UDFValue theArg;

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				screenx = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				screenx = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				screeny = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				screeny = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				offsetx = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				offsetx = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				offsety = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				offsety = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				targetx = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				targetx = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				targety = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				targety = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				rotation = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				rotation = theArg.floatValue->contents;
				break;
		}
	}

	if (UDFHasNextArgument(context))
	{
		UDFNextArgument(context,NUMBER_BITS,&theArg);
		switch (theArg.header->type) {
			case INTEGER_TYPE:
				zoom = 1.0f * theArg.integerValue->contents;
				break;
			case FLOAT_TYPE:
				zoom = theArg.floatValue->contents;
				break;
		}
	}

	Vector2 vector = { offsetx, offsety };
	Camera2D camera = { 0 };
	camera.offset = (Vector2){ offsetx, offsety };
	camera.target = (Vector2){ targetx, targety };
	camera.rotation = rotation;
	camera.zoom = zoom;

	Vector2 screen = GetWorldToScreen2D(vector, camera);

	MultifieldBuilder *mb = CreateMultifieldBuilder(theEnv, 2);
	MBAppendFloat(mb, screen.x);
	MBAppendFloat(mb, screen.y);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

/*********************************************************/
/* UserFunctions: Informs the expert system environment  */
/*   of any user defined functions. In the default case, */
/*   there are no user defined functions. To define      */
/*   functions, either this function must be replaced by */
/*   a function with the same name within this file, or  */
/*   this function can be deleted from this file and     */
/*   included in another file.                           */
/*********************************************************/
void UserFunctions(
  Environment *env)
  {
#if MAC_XCD
#pragma unused(env)
#endif
	  AddUDF(env,"raylib-init-window","v",3,3,";l;l;s",RaylibInitWindow,"RaylibInitWindow",NULL);
	  AddUDF(env,"raylib-window-should-close","b",0,0,NULL,RaylibWindowShouldClose,"RaylibWindowShouldClose",NULL);
	  AddUDF(env,"raylib-begin-drawing","v",0,0,NULL,RaylibBeginDrawing,"RaylibBeginDrawing",NULL);
	  AddUDF(env,"raylib-clear-background","v",1,1,";y",RaylibClearBackground,"RaylibClearBackground",NULL);
	  AddUDF(env,"raylib-draw-text","v",5,5,";s;l;l;l;y",RaylibDrawText,"RaylibDrawText",NULL);
	  AddUDF(env,"raylib-end-drawing","v",0,0,NULL,RaylibEndDrawing,"RaylibEndDrawing",NULL);
	  AddUDF(env,"raylib-close-window","v",0,0,NULL,RaylibCloseWindow,"RaylibCloseWindow",NULL);
	  AddUDF(env,"raylib-draw-circle","v",4,4,";l;l;dl;y",RaylibDrawCircle,"RaylibDrawCircle",NULL);
	  AddUDF(env,"raylib-draw-circle-lines","v",4,4,";l;l;dl;y",RaylibDrawCircleLines,"RaylibDrawCircleLines",NULL);
	  AddUDF(env,"raylib-draw-rectangle","v",5,5,";l;l;l;l;y",RaylibDrawRectangle,"RaylibDrawRectangle",NULL);
	  AddUDF(env,"raylib-draw-rectangle-lines","v",5,5,";l;l;l;l;y",RaylibDrawRectangleLines,"RaylibDrawRectangleLines",NULL);
	  AddUDF(env,"raylib-draw-rectangle-rounded","v",7,7,";dl;dl;dl;dl;dl;l;y",RaylibDrawRectangleRounded,"RaylibDrawRectangleRounded",NULL);
	  AddUDF(env,"raylib-draw-rectangle-rounded-lines","v",8,8,";dl;dl;dl;dl;dl;l;dl;y",RaylibDrawRectangleRoundedLines,"RaylibDrawRectangleRoundedLines",NULL);
	  AddUDF(env,"raylib-get-mouse-position","m",0,0,NULL,RaylibGetMousePosition,"RaylibGetMousePosition",NULL);
	  AddUDF(env,"raylib-get-mouse-delta","m",0,0,NULL,RaylibGetMouseDelta,"RaylibGetMouseDelta",NULL);
	  AddUDF(env,"raylib-get-render-height","l",0,0,NULL,RaylibGetRenderHeight,"RaylibGetRenderHeight",NULL);
	  AddUDF(env,"raylib-get-render-width","l",0,0,NULL,RaylibGetRenderWidth,"RaylibGetRenderWidth",NULL);
	  AddUDF(env,"raylib-get-screen-height","l",0,0,NULL,RaylibGetScreenHeight,"RaylibGetScreenHeight",NULL);
	  AddUDF(env,"raylib-get-screen-width","l",0,0,NULL,RaylibGetScreenWidth,"RaylibGetScreenWidth",NULL);

	  AddUDF(env,"raylib-is-mouse-button-pressed","b",0,7,"y",RaylibIsMouseButtonPressed,"RaylibIsMouseButtonPressed",NULL);
	  AddUDF(env,"raylib-is-mouse-button-down","b",0,7,"y",RaylibIsMouseButtonDown,"RaylibIsMouseButtonDown",NULL);
	  AddUDF(env,"raylib-is-mouse-button-up","b",0,7,"y",RaylibIsMouseButtonUp,"RaylibIsMouseButtonUp",NULL);
	  AddUDF(env,"raylib-is-mouse-button-released","b",0,7,"y",RaylibIsMouseButtonReleased,"RaylibIsMouseButtonReleased",NULL);

	  AddUDF(env,"raylib-is-key-pressed","b",0,7,"y",RaylibIsKeyPressed,"RaylibIsKeyPressed",NULL);
	  AddUDF(env,"raylib-is-key-pressed-repeat","b",0,7,"y",RaylibIsKeyPressed,"RaylibIsKeyPressed",NULL);
	  AddUDF(env,"raylib-is-key-down","b",0,7,"y",RaylibIsKeyDown,"RaylibIsKeyDown",NULL);
	  AddUDF(env,"raylib-is-key-up","b",0,7,"y",RaylibIsKeyUp,"RaylibIsKeyUp",NULL);
	  AddUDF(env,"raylib-is-key-released","b",0,7,"y",RaylibIsKeyReleased,"RaylibIsKeyReleased",NULL);

	  AddUDF(env,"raylib-begin-mode-2d","v",0,6,";l;l;l;l;d;d",RaylibBeginMode2D,"RaylibBeginMode2D",NULL);
	  AddUDF(env,"raylib-end-mode-2d","v",0,0,NULL,RaylibEndMode2D,"RaylibEndMode2D",NULL);

	  AddUDF(env,"raylib-set-target-fps","v",1,1,";l",RaylibSetTargetFPS,"RaylibSetTargetFPS",NULL);
	  AddUDF(env,"raylib-get-frame-time","d",0,0,NULL,RaylibGetFrameTime,"RaylibGetFrameTime",NULL);

	  AddUDF(env,"raylib-get-screen-to-world-2d","m",0,7,";dl;dl;dl;dl;dl;dl;dl",RaylibGetScreenToWorld2D,"RaylibGetScreenToWorld2D",NULL);
	  AddUDF(env,"raylib-get-world-to-screen-2d","m",0,7,";dl;dl;dl;dl;dl;dl;dl",RaylibGetWorldToScreen2D,"RaylibGetWorldToScreen2D",NULL);
  }
