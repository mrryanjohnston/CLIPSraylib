#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "clips.h"
#include "raylib_glue.h"

void RaylibUserFunctions(Environment *);
void RaylibShutdown(Environment *);

/* ------------------------------------------------------------------------ */
/* Reading arguments                                                        */
/* ------------------------------------------------------------------------ */

static double NumberValue(UDFValue *value)
{
	if (value->header->type == INTEGER_TYPE)
	{
		return (double) value->integerValue->contents;
	}
	return value->floatValue->contents;
}

static bool NextNumber(UDFContext *context, double *out)
{
	UDFValue theArg;

	if (!UDFNextArgument(context, NUMBER_BITS, &theArg))
	{
		return false;
	}
	*out = NumberValue(&theArg);
	return true;
}

static bool NextInteger(UDFContext *context, long long *out)
{
	UDFValue theArg;

	if (!UDFNextArgument(context, INTEGER_BIT, &theArg))
	{
		return false;
	}
	*out = theArg.integerValue->contents;
	return true;
}

static bool NextInt(UDFContext *context, int *out)
{
	long long value;

	if (!NextInteger(context, &value))
	{
		return false;
	}
	*out = (int) value;
	return true;
}

static bool NextString(UDFContext *context, const char **out)
{
	UDFValue theArg;

	if (!UDFNextArgument(context, STRING_BIT, &theArg))
	{
		return false;
	}
	*out = theArg.lexemeValue->contents;
	return true;
}

static void Fail(Environment *theEnv, UDFContext *context, const char *message)
{
	Writeln(theEnv, message);
	UDFThrowError(context);
}

/* A colour given as one value: a multifield of four integers (r g b a) or a
 * symbol naming one of raylib's colours. */
static bool ColorFromValue(Environment *theEnv, UDFContext *context, const char *udfName,
                           UDFValue *value, int *r, int *g, int *b, int *a)
{
	char message[160];

	if (value->header->type == MULTIFIELD_TYPE)
	{
		Multifield *mf = value->multifieldValue;
		size_t i;

		if (mf->length != 4)
		{
			snprintf(message, sizeof(message), "%s's multifield arg must have exactly 4 elements", udfName);
			Fail(theEnv, context, message);
			return false;
		}
		for (i = 0; i < 4; i++)
		{
			if (mf->contents[i].header->type != INTEGER_TYPE)
			{
				snprintf(message, sizeof(message), "%s's multifield arg must hold 4 integers", udfName);
				Fail(theEnv, context, message);
				return false;
			}
		}
		*r = (int) mf->contents[0].integerValue->contents;
		*g = (int) mf->contents[1].integerValue->contents;
		*b = (int) mf->contents[2].integerValue->contents;
		*a = (int) mf->contents[3].integerValue->contents;
		return true;
	}

	if (value->header->type == SYMBOL_TYPE)
	{
		RaylibGlueColorByName(value->lexemeValue->contents, r, g, b, a);
		return true;
	}

	snprintf(message, sizeof(message), "%s's colour arg must be a multifield or symbol", udfName);
	Fail(theEnv, context, message);
	return false;
}

/* The colour at the end of a drawing call. With compactCount arguments in
 * all, the colour is the one value ColorFromValue reads; with fullCount, it
 * is four integers. */
static bool ColorArgs(Environment *theEnv, UDFContext *context, const char *udfName,
                      unsigned int compactCount, unsigned int fullCount,
                      int *r, int *g, int *b, int *a)
{
	UDFValue theArg;
	unsigned int count = UDFArgumentCount(context);
	char message[160];

	if (count == compactCount)
	{
		if (!UDFNextArgument(context, MULTIFIELD_BIT | SYMBOL_BIT, &theArg))
		{
			return false;
		}
		return ColorFromValue(theEnv, context, udfName, &theArg, r, g, b, a);
	}

	if (count == fullCount)
	{
		return NextInt(context, r) && NextInt(context, g) && NextInt(context, b) && NextInt(context, a);
	}

	snprintf(message, sizeof(message), "%s must have either %u or %u arguments", udfName, compactCount, fullCount);
	Fail(theEnv, context, message);
	return false;
}

/* ------------------------------------------------------------------------ */
/* Writing results                                                          */
/* ------------------------------------------------------------------------ */

static void SetBoolean(Environment *theEnv, UDFValue *returnValue, bool value)
{
	returnValue->lexemeValue = value ? TrueSymbol(theEnv) : FalseSymbol(theEnv);
}

static void SetColor(Environment *theEnv, UDFValue *returnValue, int r, int g, int b, int a)
{
	MultifieldBuilder *mb = CreateMultifieldBuilder(theEnv, 4);

	MBAppendInteger(mb, r);
	MBAppendInteger(mb, g);
	MBAppendInteger(mb, b);
	MBAppendInteger(mb, a);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

static void SetPair(Environment *theEnv, UDFValue *returnValue, double x, double y)
{
	MultifieldBuilder *mb = CreateMultifieldBuilder(theEnv, 2);

	MBAppendFloat(mb, x);
	MBAppendFloat(mb, y);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

/* ------------------------------------------------------------------------ */
/* Window and frame                                                         */
/* ------------------------------------------------------------------------ */

static void RaylibInitWindow(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long width, height;
	const char *title;

	if (!NextInteger(context, &width) || !NextInteger(context, &height) || !NextString(context, &title))
	{
		return;
	}
	RaylibGlueInitWindow((int) width, (int) height, title);
}

static void RaylibWindowShouldClose(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	SetBoolean(theEnv, returnValue, RaylibGlueWindowShouldClose());
}

static void RaylibBeginDrawing(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	RaylibGlueBeginDrawing();
}

static void RaylibEndDrawing(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	RaylibGlueEndDrawing();
}

static void RaylibCloseWindow(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	RaylibGlueCloseWindow();
}

static void RaylibClearBackground(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	UDFValue theArg;
	int r, g, b, a;

	if (!UDFNextArgument(context, MULTIFIELD_BIT | SYMBOL_BIT, &theArg))
	{
		return;
	}
	if (!ColorFromValue(theEnv, context, "raylib-clear-background", &theArg, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueClearBackground(r, g, b, a);
}

static void RaylibSetTargetFPS(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long fps;

	if (NextInteger(context, &fps))
	{
		RaylibGlueSetTargetFPS((int) fps);
	}
}

static void RaylibGetFrameTime(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	returnValue->floatValue = CreateFloat(theEnv, RaylibGlueGetFrameTime());
}

static void RaylibDrawFPS(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long x, y;

	if (NextInteger(context, &x) && NextInteger(context, &y))
	{
		RaylibGlueDrawFPS((int) x, (int) y);
	}
}

static void RaylibPollInputEvents(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	RaylibGluePollInputEvents();
}

static void RaylibGetRenderHeight(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, RaylibGlueGetRenderHeight());
}

static void RaylibGetRenderWidth(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, RaylibGlueGetRenderWidth());
}

static void RaylibGetScreenHeight(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, RaylibGlueGetScreenHeight());
}

static void RaylibGetScreenWidth(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, RaylibGlueGetScreenWidth());
}

/* ------------------------------------------------------------------------ */
/* Colours and raymath                                                      */
/* ------------------------------------------------------------------------ */

static void RaylibFade(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	UDFValue theArg;
	int r, g, b, a;
	double alpha;
	unsigned int count = UDFArgumentCount(context);

	if (count == 2)
	{
		if (!UDFNextArgument(context, SYMBOL_BIT, &theArg))
		{
			return;
		}
		RaylibGlueColorByName(theArg.lexemeValue->contents, &r, &g, &b, &a);
	}
	else if (count == 5)
	{
		if (!NextInt(context, &r) || !NextInt(context, &g) || !NextInt(context, &b) || !NextInt(context, &a))
		{
			return;
		}
	}
	else
	{
		Fail(theEnv, context, "raylib-fade must have either 2 or 5 arguments");
		return;
	}

	if (!NextNumber(context, &alpha))
	{
		return;
	}
	RaylibGlueFade(r, g, b, a, alpha, &r, &g, &b, &a);
	SetColor(theEnv, returnValue, r, g, b, a);
}

static void RaylibColorLerp(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	UDFValue theArg;
	int r1, g1, b1, a1, r2, g2, b2, a2;
	double factor;

	if (!UDFNextArgument(context, MULTIFIELD_BIT | SYMBOL_BIT, &theArg) ||
	    !ColorFromValue(theEnv, context, "raylib-color-lerp", &theArg, &r1, &g1, &b1, &a1))
	{
		return;
	}
	if (!UDFNextArgument(context, MULTIFIELD_BIT | SYMBOL_BIT, &theArg) ||
	    !ColorFromValue(theEnv, context, "raylib-color-lerp", &theArg, &r2, &g2, &b2, &a2))
	{
		return;
	}
	if (!NextNumber(context, &factor))
	{
		return;
	}
	RaylibGlueColorLerp(r1, g1, b1, a1, r2, g2, b2, a2, factor, &r1, &g1, &b1, &a1);
	SetColor(theEnv, returnValue, r1, g1, b1, a1);
}

static void RaylibGetColor(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long hexValue;
	int r, g, b, a;

	if (!NextInteger(context, &hexValue))
	{
		return;
	}
	RaylibGlueGetColor(hexValue, &r, &g, &b, &a);
	SetColor(theEnv, returnValue, r, g, b, a);
}

static void RaylibLerp(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double start, end, amount;

	if (NextNumber(context, &start) && NextNumber(context, &end) && NextNumber(context, &amount))
	{
		returnValue->floatValue = CreateFloat(theEnv, RaylibGlueLerp(start, end, amount));
	}
}

static void RaylibClamp(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double value, min, max;

	if (NextNumber(context, &value) && NextNumber(context, &min) && NextNumber(context, &max))
	{
		returnValue->floatValue = CreateFloat(theEnv, RaylibGlueClamp(value, min, max));
	}
}

/* ------------------------------------------------------------------------ */
/* Text                                                                     */
/* ------------------------------------------------------------------------ */

static void RaylibDrawText(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	const char *text;
	long long x, y, fontSize;
	int r, g, b, a;

	if (!NextString(context, &text) || !NextInteger(context, &x) || !NextInteger(context, &y) ||
	    !NextInteger(context, &fontSize))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-text", 5, 8, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawText(text, (int) x, (int) y, (int) fontSize, r, g, b, a);
}

static void RaylibMeasureText(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	const char *text;
	long long fontSize;

	if (NextString(context, &text) && NextInteger(context, &fontSize))
	{
		returnValue->integerValue = CreateInteger(theEnv, RaylibGlueMeasureText(text, (int) fontSize));
	}
}

/* ------------------------------------------------------------------------ */
/* Shapes                                                                   */
/* ------------------------------------------------------------------------ */

static void RaylibDrawCircle(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	UDFValue theArg;
	long long centerX, centerY;
	double radius;
	int r, g, b, a;

	if (!NextInteger(context, &centerX) || !NextInteger(context, &centerY) || !NextNumber(context, &radius))
	{
		return;
	}
	if (!UDFNextArgument(context, SYMBOL_BIT, &theArg))
	{
		return;
	}
	RaylibGlueColorByName(theArg.lexemeValue->contents, &r, &g, &b, &a);
	RaylibGlueDrawCircle((int) centerX, (int) centerY, radius, r, g, b, a);
}

static void RaylibDrawCircleLines(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	UDFValue theArg;
	long long centerX, centerY;
	double radius;
	int r, g, b, a;

	if (!NextInteger(context, &centerX) || !NextInteger(context, &centerY) || !NextNumber(context, &radius))
	{
		return;
	}
	if (!UDFNextArgument(context, SYMBOL_BIT, &theArg))
	{
		return;
	}
	RaylibGlueColorByName(theArg.lexemeValue->contents, &r, &g, &b, &a);
	RaylibGlueDrawCircleLines((int) centerX, (int) centerY, radius, r, g, b, a);
}

static void RaylibDrawCircleV(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double centerX, centerY, radius;
	int r, g, b, a;

	if (!NextNumber(context, &centerX) || !NextNumber(context, &centerY) || !NextNumber(context, &radius))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-circle-v", 4, 7, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawCircleV(centerX, centerY, radius, r, g, b, a);
}

static void RaylibDrawLine(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long startX, startY, endX, endY;
	int r, g, b, a;

	if (!NextInteger(context, &startX) || !NextInteger(context, &startY) ||
	    !NextInteger(context, &endX) || !NextInteger(context, &endY))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-line", 5, 8, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawLine((int) startX, (int) startY, (int) endX, (int) endY, r, g, b, a);
}

static void RaylibDrawLineV(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double startX, startY, endX, endY;
	int r, g, b, a;

	if (!NextNumber(context, &startX) || !NextNumber(context, &startY) ||
	    !NextNumber(context, &endX) || !NextNumber(context, &endY))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-line-v", 5, 8, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawLineV(startX, startY, endX, endY, r, g, b, a);
}

static void RaylibDrawRectangle(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long x, y, width, height;
	int r, g, b, a;

	if (!NextInteger(context, &x) || !NextInteger(context, &y) ||
	    !NextInteger(context, &width) || !NextInteger(context, &height))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-rectangle", 5, 8, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawRectangle((int) x, (int) y, (int) width, (int) height, r, g, b, a);
}

static void RaylibDrawRectangleLines(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x, y, width, height;
	int r, g, b, a;

	if (!NextNumber(context, &x) || !NextNumber(context, &y) ||
	    !NextNumber(context, &width) || !NextNumber(context, &height))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-rectangle-lines", 5, 8, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawRectangleLines((int) x, (int) y, (int) width, (int) height, r, g, b, a);
}

/* The three rounded-rectangle calls read the same leading arguments. */
static bool RoundedRectangleArgs(UDFContext *context,
                                 double *x, double *y, double *width, double *height,
                                 double *roundness, long long *segments)
{
	return NextNumber(context, x) && NextNumber(context, y) &&
	       NextNumber(context, width) && NextNumber(context, height) &&
	       NextNumber(context, roundness) && NextInteger(context, segments);
}

static void RaylibDrawRectangleRounded(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x, y, width, height, roundness;
	long long segments;
	int r, g, b, a;

	if (!RoundedRectangleArgs(context, &x, &y, &width, &height, &roundness, &segments))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-rectangle-rounded", 7, 10, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawRectangleRounded(x, y, width, height, roundness, (int) segments, r, g, b, a);
}

static void RaylibDrawRectangleRoundedLines(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x, y, width, height, roundness;
	long long segments;
	int r, g, b, a;

	if (!RoundedRectangleArgs(context, &x, &y, &width, &height, &roundness, &segments))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-rectangle-rounded-lines", 7, 10, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawRectangleRoundedLines(x, y, width, height, roundness, (int) segments, r, g, b, a);
}

static void RaylibDrawRectangleRoundedLinesEx(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x, y, width, height, roundness, lineThick;
	long long segments;
	int r, g, b, a;

	if (!RoundedRectangleArgs(context, &x, &y, &width, &height, &roundness, &segments) ||
	    !NextNumber(context, &lineThick))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-rectangle-rounded-lines-ex", 8, 11, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawRectangleRoundedLinesEx(x, y, width, height, roundness, (int) segments, lineThick, r, g, b, a);
}

static void RaylibDrawPlane(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double centerX, centerY, centerZ, sizeX, sizeY;
	int r, g, b, a;

	if (!NextNumber(context, &centerX) || !NextNumber(context, &centerY) || !NextNumber(context, &centerZ) ||
	    !NextNumber(context, &sizeX) || !NextNumber(context, &sizeY))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-plane", 6, 9, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawPlane(centerX, centerY, centerZ, sizeX, sizeY, r, g, b, a);
}

static void RaylibDrawCube(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x, y, z, width, height, length;
	int r, g, b, a;

	if (!NextNumber(context, &x) || !NextNumber(context, &y) || !NextNumber(context, &z) ||
	    !NextNumber(context, &width) || !NextNumber(context, &height) || !NextNumber(context, &length))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-cube", 7, 10, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawCube(x, y, z, width, height, length, r, g, b, a);
}

/* ------------------------------------------------------------------------ */
/* Collisions                                                               */
/* ------------------------------------------------------------------------ */

static bool RectangleArgs(UDFContext *context, double *x, double *y, double *width, double *height)
{
	return NextNumber(context, x) && NextNumber(context, y) &&
	       NextNumber(context, width) && NextNumber(context, height);
}

static void RaylibCheckCollisionRecs(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x1, y1, w1, h1, x2, y2, w2, h2;

	if (RectangleArgs(context, &x1, &y1, &w1, &h1) && RectangleArgs(context, &x2, &y2, &w2, &h2))
	{
		SetBoolean(theEnv, returnValue, RaylibGlueCheckCollisionRecs(x1, y1, w1, h1, x2, y2, w2, h2));
	}
}

static void RaylibCheckCollisionPointRec(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double px, py, x, y, w, h;

	if (NextNumber(context, &px) && NextNumber(context, &py) && RectangleArgs(context, &x, &y, &w, &h))
	{
		SetBoolean(theEnv, returnValue, RaylibGlueCheckCollisionPointRec(px, py, x, y, w, h));
	}
}

static void RaylibGetCollisionRec(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x1, y1, w1, h1, x2, y2, w2, h2, x, y, w, h;
	MultifieldBuilder *mb;

	if (!RectangleArgs(context, &x1, &y1, &w1, &h1) || !RectangleArgs(context, &x2, &y2, &w2, &h2))
	{
		return;
	}
	RaylibGlueGetCollisionRec(x1, y1, w1, h1, x2, y2, w2, h2, &x, &y, &w, &h);

	mb = CreateMultifieldBuilder(theEnv, 4);
	MBAppendFloat(mb, x);
	MBAppendFloat(mb, y);
	MBAppendFloat(mb, w);
	MBAppendFloat(mb, h);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

/* ------------------------------------------------------------------------ */
/* Mouse                                                                    */
/* ------------------------------------------------------------------------ */

/* With no arguments, whether any button is in the state; with arguments,
 * each a MOUSE_BUTTON_* symbol, the state of the last one named. */
static void MouseButtonState(Environment *theEnv, UDFContext *context, UDFValue *returnValue,
                             bool (*query)(int))
{
	UDFValue theArg;
	int i, button;
	bool result = false;

	if (!UDFHasNextArgument(context))
	{
		for (i = 0; i < RaylibGlueMouseButtonCount(); i++)
		{
			if (query(RaylibGlueMouseButtonAt(i)))
			{
				result = true;
				break;
			}
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			if (!UDFNextArgument(context, SYMBOL_BIT, &theArg))
			{
				return;
			}
			button = RaylibGlueMouseButtonByName(theArg.lexemeValue->contents);
			result = (button >= 0) && query(button);
		}
	}
	SetBoolean(theEnv, returnValue, result);
}

static void RaylibIsMouseButtonPressed(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	MouseButtonState(theEnv, context, returnValue, RaylibGlueIsMouseButtonPressed);
}

static void RaylibIsMouseButtonDown(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	MouseButtonState(theEnv, context, returnValue, RaylibGlueIsMouseButtonDown);
}

static void RaylibIsMouseButtonReleased(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	MouseButtonState(theEnv, context, returnValue, RaylibGlueIsMouseButtonReleased);
}

static void RaylibIsMouseButtonUp(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	MouseButtonState(theEnv, context, returnValue, RaylibGlueIsMouseButtonUp);
}

static void RaylibGetMousePosition(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x, y;

	RaylibGlueGetMousePosition(&x, &y);
	SetPair(theEnv, returnValue, x, y);
}

static void RaylibGetMouseDelta(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x, y;

	RaylibGlueGetMouseDelta(&x, &y);
	SetPair(theEnv, returnValue, x, y);
}

static void RaylibGetMouseX(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, RaylibGlueGetMouseX());
}

static void RaylibGetMouseY(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	returnValue->integerValue = CreateInteger(theEnv, RaylibGlueGetMouseY());
}

static void RaylibGetMouseWheelMove(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	returnValue->floatValue = CreateFloat(theEnv, RaylibGlueGetMouseWheelMove());
}

/* ------------------------------------------------------------------------ */
/* Keyboard                                                                 */
/* ------------------------------------------------------------------------ */

/* With no arguments, whether any key is in the state; with arguments, each a
 * KEY_* symbol, whether any of those is. */
static void KeyState(Environment *theEnv, UDFContext *context, UDFValue *returnValue,
                     bool (*query)(int))
{
	UDFValue theArg;
	int i, key;
	bool result = false;

	if (!UDFHasNextArgument(context))
	{
		for (i = 0; i < RaylibGlueKeyCount(); i++)
		{
			if (query(RaylibGlueKeyAt(i)))
			{
				result = true;
				break;
			}
		}
	}
	else
	{
		while (UDFHasNextArgument(context))
		{
			if (!UDFNextArgument(context, SYMBOL_BIT, &theArg))
			{
				return;
			}
			key = RaylibGlueKeyByName(theArg.lexemeValue->contents);
			if (key != 0 && query(key))
			{
				result = true;
				break;
			}
		}
	}
	SetBoolean(theEnv, returnValue, result);
}

static void RaylibIsKeyPressed(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	KeyState(theEnv, context, returnValue, RaylibGlueIsKeyPressed);
}

static void RaylibIsKeyPressedRepeat(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	KeyState(theEnv, context, returnValue, RaylibGlueIsKeyPressedRepeat);
}

static void RaylibIsKeyDown(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	KeyState(theEnv, context, returnValue, RaylibGlueIsKeyDown);
}

static void RaylibIsKeyReleased(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	KeyState(theEnv, context, returnValue, RaylibGlueIsKeyReleased);
}

static void RaylibIsKeyUp(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	KeyState(theEnv, context, returnValue, RaylibGlueIsKeyUp);
}

/* ------------------------------------------------------------------------ */
/* Cameras                                                                  */
/* ------------------------------------------------------------------------ */

/* An optional number: the next argument if there is one, else the default. */
static bool OptionalNumber(UDFContext *context, double *out, double fallback)
{
	if (!UDFHasNextArgument(context))
	{
		*out = fallback;
		return true;
	}
	return NextNumber(context, out);
}

static bool OptionalInteger(UDFContext *context, long long *out, long long fallback)
{
	if (!UDFHasNextArgument(context))
	{
		*out = fallback;
		return true;
	}
	return NextInteger(context, out);
}

static void RaylibBeginMode2D(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long offsetX, offsetY, targetX, targetY;
	double rotation, zoom;

	if (!OptionalInteger(context, &offsetX, 0) || !OptionalInteger(context, &offsetY, 0) ||
	    !OptionalInteger(context, &targetX, 0) || !OptionalInteger(context, &targetY, 0) ||
	    !OptionalNumber(context, &rotation, 0.0) || !OptionalNumber(context, &zoom, 1.0))
	{
		return;
	}
	RaylibGlueBeginMode2D((double) offsetX, (double) offsetY, (double) targetX, (double) targetY, rotation, zoom);
}

static void RaylibEndMode2D(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	RaylibGlueEndMode2D();
}

static void RaylibBeginMode3D(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double position[3], target[3], up[3], fovy;
	long long projection;
	int i;

	for (i = 0; i < 3; i++)
	{
		if (!OptionalNumber(context, &position[i], 0.0)) return;
	}
	for (i = 0; i < 3; i++)
	{
		if (!OptionalNumber(context, &target[i], 0.0)) return;
	}
	for (i = 0; i < 3; i++)
	{
		if (!OptionalNumber(context, &up[i], 0.0)) return;
	}
	if (!OptionalNumber(context, &fovy, 0.0) || !OptionalInteger(context, &projection, 0))
	{
		return;
	}
	RaylibGlueBeginMode3D(position[0], position[1], position[2],
	                      target[0], target[1], target[2],
	                      up[0], up[1], up[2],
	                      fovy, (int) projection);
}

static void RaylibEndMode3D(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	RaylibGlueEndMode3D();
}

/* A point and a 2D camera: point x y, camera offset x y, target x y,
 * rotation, zoom. Every one is optional. The registered argument list stops
 * at rotation, so the zoom defaults to 1.0 rather than to a degenerate 0.0. */
static bool CameraPointArgs(UDFContext *context, double *px, double *py,
                            double *offsetX, double *offsetY, double *targetX, double *targetY,
                            double *rotation, double *zoom)
{
	return OptionalNumber(context, px, 0.0) && OptionalNumber(context, py, 0.0) &&
	       OptionalNumber(context, offsetX, 0.0) && OptionalNumber(context, offsetY, 0.0) &&
	       OptionalNumber(context, targetX, 0.0) && OptionalNumber(context, targetY, 0.0) &&
	       OptionalNumber(context, rotation, 0.0) && OptionalNumber(context, zoom, 1.0);
}

static void RaylibGetScreenToWorld2D(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double px, py, offsetX, offsetY, targetX, targetY, rotation, zoom, x, y;

	if (!CameraPointArgs(context, &px, &py, &offsetX, &offsetY, &targetX, &targetY, &rotation, &zoom))
	{
		return;
	}
	RaylibGlueGetScreenToWorld2D(px, py, offsetX, offsetY, targetX, targetY, rotation, zoom, &x, &y);
	SetPair(theEnv, returnValue, x, y);
}

static void RaylibGetWorldToScreen2D(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double px, py, offsetX, offsetY, targetX, targetY, rotation, zoom, x, y;

	if (!CameraPointArgs(context, &px, &py, &offsetX, &offsetY, &targetX, &targetY, &rotation, &zoom))
	{
		return;
	}
	RaylibGlueGetWorldToScreen2D(px, py, offsetX, offsetY, targetX, targetY, rotation, zoom, &x, &y);
	SetPair(theEnv, returnValue, x, y);
}

/* ------------------------------------------------------------------------ */
/* raygui                                                                   */
/* ------------------------------------------------------------------------ */

static void RaylibGuiSliderBar(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	double x, y, width, height, value, minValue, maxValue;
	const char *textLeft, *textRight;

	if (!RectangleArgs(context, &x, &y, &width, &height) ||
	    !NextString(context, &textLeft) || !NextString(context, &textRight) ||
	    !NextNumber(context, &value) || !NextNumber(context, &minValue) || !NextNumber(context, &maxValue))
	{
		return;
	}
	returnValue->floatValue = CreateFloat(theEnv,
		RaylibGlueGuiSliderBar(x, y, width, height, textLeft, textRight, value, minValue, maxValue));
}

static void RaylibGuiCheckBox(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	UDFValue theArg;
	double x, y, width, height;
	const char *text;
	bool checked;

	if (!RectangleArgs(context, &x, &y, &width, &height) || !NextString(context, &text))
	{
		return;
	}
	if (!UDFNextArgument(context, BOOLEAN_BIT, &theArg))
	{
		return;
	}
	checked = (theArg.lexemeValue == TrueSymbol(theEnv));
	SetBoolean(theEnv, returnValue, RaylibGlueGuiCheckBox(x, y, width, height, text, checked));
}

/* ------------------------------------------------------------------------ */
/* Textures                                                                 */
/* ------------------------------------------------------------------------ */

/* A texture is passed around CLIPS as the five integers LoadTexture returned:
 * id, width, height, mipmaps, format. */
static bool TextureArgs(UDFContext *context, long long *id, int *width, int *height, int *mipmaps, int *format)
{
	return NextInteger(context, id) && NextInt(context, width) && NextInt(context, height) &&
	       NextInt(context, mipmaps) && NextInt(context, format);
}

/* A render texture is its id and then its two textures, eleven integers. */
static bool RenderTextureArgs(UDFContext *context, long long *id,
                              long long *textureId, int *textureWidth, int *textureHeight,
                              int *textureMipmaps, int *textureFormat,
                              long long *depthId, int *depthWidth, int *depthHeight,
                              int *depthMipmaps, int *depthFormat)
{
	return NextInteger(context, id) &&
	       TextureArgs(context, textureId, textureWidth, textureHeight, textureMipmaps, textureFormat) &&
	       TextureArgs(context, depthId, depthWidth, depthHeight, depthMipmaps, depthFormat);
}

static void RaylibLoadTexture(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	const char *fileName;
	long long id;
	int width, height, mipmaps, format;
	MultifieldBuilder *mb;

	if (!NextString(context, &fileName))
	{
		return;
	}
	RaylibGlueLoadTexture(fileName, &id, &width, &height, &mipmaps, &format);

	mb = CreateMultifieldBuilder(theEnv, 5);
	MBAppendInteger(mb, id);
	MBAppendInteger(mb, width);
	MBAppendInteger(mb, height);
	MBAppendInteger(mb, mipmaps);
	MBAppendInteger(mb, format);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

static void RaylibLoadRenderTexture(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long width, height, id, textureId, depthId;
	int textureWidth, textureHeight, textureMipmaps, textureFormat;
	int depthWidth, depthHeight, depthMipmaps, depthFormat;
	MultifieldBuilder *mb;

	if (!NextInteger(context, &width) || !NextInteger(context, &height))
	{
		return;
	}
	RaylibGlueLoadRenderTexture((int) width, (int) height, &id,
	                            &textureId, &textureWidth, &textureHeight, &textureMipmaps, &textureFormat,
	                            &depthId, &depthWidth, &depthHeight, &depthMipmaps, &depthFormat);

	mb = CreateMultifieldBuilder(theEnv, 11);
	MBAppendInteger(mb, id);
	MBAppendInteger(mb, textureId);
	MBAppendInteger(mb, textureWidth);
	MBAppendInteger(mb, textureHeight);
	MBAppendInteger(mb, textureMipmaps);
	MBAppendInteger(mb, textureFormat);
	MBAppendInteger(mb, depthId);
	MBAppendInteger(mb, depthWidth);
	MBAppendInteger(mb, depthHeight);
	MBAppendInteger(mb, depthMipmaps);
	MBAppendInteger(mb, depthFormat);
	returnValue->multifieldValue = MBCreate(mb);
	MBDispose(mb);
}

static void RaylibDrawTexture(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long id;
	int width, height, mipmaps, format, r, g, b, a;
	double x, y;

	if (!TextureArgs(context, &id, &width, &height, &mipmaps, &format) ||
	    !NextNumber(context, &x) || !NextNumber(context, &y))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-texture", 8, 11, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawTexture(id, width, height, mipmaps, format, x, y, r, g, b, a);
}

static void RaylibDrawTextureEx(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long id;
	int width, height, mipmaps, format, r, g, b, a;
	double x, y, rotation, scale;

	if (!TextureArgs(context, &id, &width, &height, &mipmaps, &format) ||
	    !NextNumber(context, &x) || !NextNumber(context, &y) ||
	    !NextNumber(context, &rotation) || !NextNumber(context, &scale))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-texture-ex", 10, 13, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawTextureEx(id, width, height, mipmaps, format, x, y, rotation, scale, r, g, b, a);
}

static void RaylibDrawTextureRec(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long id;
	int width, height, mipmaps, format, r, g, b, a;
	double sourceX, sourceY, sourceW, sourceH, x, y;

	if (!TextureArgs(context, &id, &width, &height, &mipmaps, &format) ||
	    !RectangleArgs(context, &sourceX, &sourceY, &sourceW, &sourceH) ||
	    !NextNumber(context, &x) || !NextNumber(context, &y))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-texture-rec", 12, 15, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawTextureRec(id, width, height, mipmaps, format,
	                         sourceX, sourceY, sourceW, sourceH, x, y, r, g, b, a);
}

static void RaylibDrawTexturePro(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long id;
	int width, height, mipmaps, format, r, g, b, a;
	double sourceX, sourceY, sourceW, sourceH, destX, destY, destW, destH, originX, originY, rotation;

	if (!TextureArgs(context, &id, &width, &height, &mipmaps, &format) ||
	    !RectangleArgs(context, &sourceX, &sourceY, &sourceW, &sourceH) ||
	    !RectangleArgs(context, &destX, &destY, &destW, &destH) ||
	    !NextNumber(context, &originX) || !NextNumber(context, &originY) || !NextNumber(context, &rotation))
	{
		return;
	}
	if (!ColorArgs(theEnv, context, "raylib-draw-texture-pro", 17, 20, &r, &g, &b, &a))
	{
		return;
	}
	RaylibGlueDrawTexturePro(id, width, height, mipmaps, format,
	                         sourceX, sourceY, sourceW, sourceH,
	                         destX, destY, destW, destH,
	                         originX, originY, rotation, r, g, b, a);
}

static void RaylibSetTextureFilter(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	UDFValue theArg;
	long long id;
	int width, height, mipmaps, format, filter;

	if (!TextureArgs(context, &id, &width, &height, &mipmaps, &format))
	{
		return;
	}
	if (!UDFNextArgument(context, SYMBOL_BIT, &theArg))
	{
		return;
	}
	filter = RaylibGlueTextureFilterByName(theArg.lexemeValue->contents);
	if (filter < 0)
	{
		Fail(theEnv, context, "raylib-set-texture-filter's last argument must be TEXTURE_FILTER_POINT, "
		     "TEXTURE_FILTER_BILINEAR, TEXTURE_FILTER_TRILINEAR, TEXTURE_FILTER_ANISOTROPIC_4X, "
		     "TEXTURE_FILTER_ANISOTROPIC_8X, or TEXTURE_FILTER_ANISOTROPIC_16X");
		return;
	}
	RaylibGlueSetTextureFilter(id, width, height, mipmaps, format, filter);
}

static void RaylibBeginTextureMode(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long id, textureId, depthId;
	int textureWidth, textureHeight, textureMipmaps, textureFormat;
	int depthWidth, depthHeight, depthMipmaps, depthFormat;

	if (!RenderTextureArgs(context, &id,
	                       &textureId, &textureWidth, &textureHeight, &textureMipmaps, &textureFormat,
	                       &depthId, &depthWidth, &depthHeight, &depthMipmaps, &depthFormat))
	{
		return;
	}
	RaylibGlueBeginTextureMode(id,
	                           textureId, textureWidth, textureHeight, textureMipmaps, textureFormat,
	                           depthId, depthWidth, depthHeight, depthMipmaps, depthFormat);
}

static void RaylibEndTextureMode(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	RaylibGlueEndTextureMode();
}

static void RaylibUnloadRenderTexture(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long id, textureId, depthId;
	int textureWidth, textureHeight, textureMipmaps, textureFormat;
	int depthWidth, depthHeight, depthMipmaps, depthFormat;

	if (!RenderTextureArgs(context, &id,
	                       &textureId, &textureWidth, &textureHeight, &textureMipmaps, &textureFormat,
	                       &depthId, &depthWidth, &depthHeight, &depthMipmaps, &depthFormat))
	{
		return;
	}
	RaylibGlueUnloadRenderTexture(id,
	                              textureId, textureWidth, textureHeight, textureMipmaps, textureFormat,
	                              depthId, depthWidth, depthHeight, depthMipmaps, depthFormat);
}

/* ------------------------------------------------------------------------ */
/* Random numbers                                                           */
/* ------------------------------------------------------------------------ */

static void RaylibSetRandomSeed(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long seed;

	if (NextInteger(context, &seed))
	{
		RaylibGlueSetRandomSeed(seed);
	}
}

static void RaylibGetRandomValue(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	long long min, max;

	if (NextInteger(context, &min) && NextInteger(context, &max))
	{
		returnValue->integerValue = CreateInteger(theEnv, RaylibGlueGetRandomValue((int) min, (int) max));
	}
}

/* ------------------------------------------------------------------------ */
/* Hex strings, for colours written the way raylib's examples write them    */
/* ------------------------------------------------------------------------ */

static void RaylibHexStringToInt(Environment *theEnv, UDFContext *context, UDFValue *returnValue)
{
	UDFValue theArg;
	const char *str, *p;
	char *end;
	long long value;

	if (!UDFFirstArgument(context, STRING_BIT | SYMBOL_BIT, &theArg))
	{
		Fail(theEnv, context, "raylib-hex-string-to-int's first argument must be a string or symbol");
		return;
	}
	str = theArg.lexemeValue->contents;

	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str += 2;
	}
	if (str[0] == '\0')
	{
		Fail(theEnv, context, "raylib-hex-string-to-int's first argument must not be empty");
		return;
	}
	for (p = str; *p != '\0'; p++)
	{
		if ((*p < '0' || *p > '9') && (*p < 'a' || *p > 'f') && (*p < 'A' || *p > 'F'))
		{
			Fail(theEnv, context, "raylib-hex-string-to-int's first argument must be a hex value");
			return;
		}
	}

	errno = 0;
	value = strtoll(str, &end, 16);
	if (errno == ERANGE || end == str || *end != '\0')
	{
		Fail(theEnv, context, "raylib-hex-string-to-int: Could not convert hex string to integer");
		return;
	}
	returnValue->integerValue = CreateInteger(theEnv, value);
}

/* ------------------------------------------------------------------------ */
/* Registration                                                             */
/* ------------------------------------------------------------------------ */

void RaylibUserFunctions(Environment *env)
{
	AddUDF(env,"raylib-init-window","v",3,3,";l;l;s",RaylibInitWindow,"RaylibInitWindow",NULL);
	AddUDF(env,"raylib-window-should-close","b",0,0,NULL,RaylibWindowShouldClose,"RaylibWindowShouldClose",NULL);
	AddUDF(env,"raylib-begin-drawing","v",0,0,NULL,RaylibBeginDrawing,"RaylibBeginDrawing",NULL);
	AddUDF(env,"raylib-clear-background","v",1,1,";my",RaylibClearBackground,"RaylibClearBackground",NULL);
	AddUDF(env,"raylib-draw-text","v",5,8,";s;l;l;l;lmy;l;l;l",RaylibDrawText,"RaylibDrawText",NULL);
	AddUDF(env,"raylib-measure-text","l",2,2,";s;l",RaylibMeasureText,"RaylibMeasureText",NULL);
	AddUDF(env,"raylib-end-drawing","v",0,0,NULL,RaylibEndDrawing,"RaylibEndDrawing",NULL);
	AddUDF(env,"raylib-close-window","v",0,0,NULL,RaylibCloseWindow,"RaylibCloseWindow",NULL);
	AddUDF(env,"raylib-draw-circle","v",4,4,";l;l;dl;y",RaylibDrawCircle,"RaylibDrawCircle",NULL);
	AddUDF(env,"raylib-draw-circle-lines","v",4,4,";l;l;dl;y",RaylibDrawCircleLines,"RaylibDrawCircleLines",NULL);
	AddUDF(env,"raylib-draw-circle-v","v",4,7,";dl;dl;dl;lmy;l;l;l",RaylibDrawCircleV,"RaylibDrawCircleV",NULL);
	AddUDF(env,"raylib-draw-line","v",5,8,";l;l;l;l;lmy;l;l;l",RaylibDrawLine,"RaylibDrawLine",NULL);
	AddUDF(env,"raylib-draw-line-v","v",5,8,";dl;dl;dl;dl;lmy;l;l;l",RaylibDrawLineV,"RaylibDrawLineV",NULL);
	AddUDF(env,"raylib-draw-rectangle","v",5,8,";l;l;l;l;dmy;l;l;l",RaylibDrawRectangle,"RaylibDrawRectangle",NULL);
	AddUDF(env,"raylib-draw-rectangle-lines","v",5,8,";dl;dl;dl;dl;lmy;l;l;l",RaylibDrawRectangleLines,"RaylibDrawRectangleLines",NULL);
	AddUDF(env,"raylib-draw-rectangle-rounded","v",7,10,";dl;dl;dl;dl;dl;l;dmy;l;l;l",RaylibDrawRectangleRounded,"RaylibDrawRectangleRounded",NULL);
	AddUDF(env,"raylib-draw-rectangle-rounded-lines","v",7,10,";dl;dl;dl;dl;dl;l;lmy;l;l;l",RaylibDrawRectangleRoundedLines,"RaylibDrawRectangleRoundedLines",NULL);
	AddUDF(env,"raylib-draw-rectangle-rounded-lines-ex","v",8,11,";dl;dl;dl;dl;dl;l;dl;lmy;l;l;l",RaylibDrawRectangleRoundedLinesEx,"RaylibDrawRectangleRoundedLinesEx",NULL);
	AddUDF(env,"raylib-draw-plane","v",6,9,";dl;dl;dl;dl;dl;lmy;l;l;l",RaylibDrawPlane,"RaylibDrawPlane",NULL);
	AddUDF(env,"raylib-draw-cube","v",7,10,";dl;dl;dl;dl;dl;dl;lmy;l;l;l",RaylibDrawCube,"RaylibDrawCube",NULL);
	AddUDF(env,"raylib-draw-fps","v",2,2,";l;l",RaylibDrawFPS,"RaylibDrawFPS",NULL);
	AddUDF(env,"raylib-fade","m",2,5,";ly;dl;l;l;d",RaylibFade,"RaylibFade",NULL);
	AddUDF(env,"raylib-get-mouse-position","m",0,0,NULL,RaylibGetMousePosition,"RaylibGetMousePosition",NULL);
	AddUDF(env,"raylib-get-mouse-delta","m",0,0,NULL,RaylibGetMouseDelta,"RaylibGetMouseDelta",NULL);
	AddUDF(env,"raylib-get-mouse-x","d",0,0,NULL,RaylibGetMouseX,"RaylibGetMouseX",NULL);
	AddUDF(env,"raylib-get-mouse-y","d",0,0,NULL,RaylibGetMouseY,"RaylibGetMouseY",NULL);
	AddUDF(env,"raylib-get-mouse-wheel-move","d",0,0,NULL,RaylibGetMouseWheelMove,"RaylibGetMouseWheelMove",NULL);
	AddUDF(env,"raylib-get-render-height","l",0,0,NULL,RaylibGetRenderHeight,"RaylibGetRenderHeight",NULL);
	AddUDF(env,"raylib-get-render-width","l",0,0,NULL,RaylibGetRenderWidth,"RaylibGetRenderWidth",NULL);
	AddUDF(env,"raylib-get-screen-height","l",0,0,NULL,RaylibGetScreenHeight,"RaylibGetScreenHeight",NULL);
	AddUDF(env,"raylib-get-screen-width","l",0,0,NULL,RaylibGetScreenWidth,"RaylibGetScreenWidth",NULL);

	AddUDF(env,"raylib-is-mouse-button-pressed","b",0,7,"y",RaylibIsMouseButtonPressed,"RaylibIsMouseButtonPressed",NULL);
	AddUDF(env,"raylib-is-mouse-button-down","b",0,7,"y",RaylibIsMouseButtonDown,"RaylibIsMouseButtonDown",NULL);
	AddUDF(env,"raylib-is-mouse-button-up","b",0,7,"y",RaylibIsMouseButtonUp,"RaylibIsMouseButtonUp",NULL);
	AddUDF(env,"raylib-is-mouse-button-released","b",0,7,"y",RaylibIsMouseButtonReleased,"RaylibIsMouseButtonReleased",NULL);

	AddUDF(env,"raylib-is-key-pressed","b",0,7,"y",RaylibIsKeyPressed,"RaylibIsKeyPressed",NULL);
	AddUDF(env,"raylib-is-key-pressed-repeat","b",0,7,"y",RaylibIsKeyPressedRepeat,"RaylibIsKeyPressedRepeat",NULL);
	AddUDF(env,"raylib-is-key-down","b",0,7,"y",RaylibIsKeyDown,"RaylibIsKeyDown",NULL);
	AddUDF(env,"raylib-is-key-up","b",0,7,"y",RaylibIsKeyUp,"RaylibIsKeyUp",NULL);
	AddUDF(env,"raylib-is-key-released","b",0,7,"y",RaylibIsKeyReleased,"RaylibIsKeyReleased",NULL);

	AddUDF(env,"raylib-begin-mode-2d","v",0,6,";l;l;l;l;d;d",RaylibBeginMode2D,"RaylibBeginMode2D",NULL);
	AddUDF(env,"raylib-end-mode-2d","v",0,0,NULL,RaylibEndMode2D,"RaylibEndMode2D",NULL);
	AddUDF(env,"raylib-begin-mode-3d","v",0,11,";dl;dl;dl;dl;dl;dl;dl;dl;dl;d;l",RaylibBeginMode3D,"RaylibBeginMode3D",NULL);
	AddUDF(env,"raylib-end-mode-3d","v",0,0,NULL,RaylibEndMode3D,"RaylibEndMode3D",NULL);

	AddUDF(env,"raylib-set-target-fps","v",1,1,";l",RaylibSetTargetFPS,"RaylibSetTargetFPS",NULL);
	AddUDF(env,"raylib-get-frame-time","d",0,0,NULL,RaylibGetFrameTime,"RaylibGetFrameTime",NULL);

	AddUDF(env,"raylib-get-screen-to-world-2d","m",0,7,";dl;dl;dl;dl;dl;dl;dl",RaylibGetScreenToWorld2D,"RaylibGetScreenToWorld2D",NULL);
	AddUDF(env,"raylib-get-world-to-screen-2d","m",0,7,";dl;dl;dl;dl;dl;dl;dl",RaylibGetWorldToScreen2D,"RaylibGetWorldToScreen2D",NULL);

	AddUDF(env,"raylib-gui-check-box","b",6,6,";dl;dl;dl;dl;s;b",RaylibGuiCheckBox,"RaylibGuiCheckBox",NULL);
	AddUDF(env,"raylib-gui-slider-bar","d",9,9,";dl;dl;dl;dl;s;s;d;d;d",RaylibGuiSliderBar,"RaylibGuiSliderBar",NULL);

	AddUDF(env,"raylib-check-collision-recs","b",8,8,"dl",RaylibCheckCollisionRecs,"RaylibCheckCollisionRecs",NULL);
	AddUDF(env,"raylib-check-collision-point-rec","b",6,6,"dl",RaylibCheckCollisionPointRec,"RaylibCheckCollisionPointRec",NULL);
	AddUDF(env,"raylib-get-collision-rec","m",8,8,"dl",RaylibGetCollisionRec,"RaylibGetCollisionRec",NULL);
	AddUDF(env,"raylib-get-color","m",1,1,"dl",RaylibGetColor,"RaylibGetColor",NULL);

	AddUDF(env,"raylib-color-lerp","m",3,3,";my;my;dl",RaylibColorLerp,"RaylibColorLerp",NULL);

	AddUDF(env,"raylib-clamp","d",3,3,"dl",RaylibClamp,"RaylibClamp",NULL);
	AddUDF(env,"raylib-lerp","d",3,3,"dl",RaylibLerp,"RaylibLerp",NULL);

	AddUDF(env,"raylib-load-texture","m",1,1,"s",RaylibLoadTexture,"RaylibLoadTexture",NULL);
	AddUDF(env,"raylib-load-render-texture","m",2,2,"l",RaylibLoadRenderTexture,"RaylibLoadRenderTexture",NULL);
	AddUDF(env,"raylib-draw-texture","v",8,11,";dl;dl;dl;dl;dl;dl;dl;dlmy;dl;dl;dl",RaylibDrawTexture,"RaylibDrawTexture",NULL);
	AddUDF(env,"raylib-draw-texture-ex","v",10,13,";l;l;l;l;l;dl;dl;dl;dl;dlmy;dl;dl;dl",RaylibDrawTextureEx,"RaylibDrawTextureEx",NULL);
	AddUDF(env,"raylib-draw-texture-rec","v",12,15,";l;l;l;l;l;dl;dl;dl;dl;dl;dl;dlmy;dl;dl;dl",RaylibDrawTextureRec,"RaylibDrawTextureRec",NULL);
	AddUDF(env,"raylib-draw-texture-pro","v",17,20,";l;l;l;l;l;dl;dl;dl;dl;dl;dl;dl;dl;dl;dl;dl;dlmy;dl;dl;dl",RaylibDrawTexturePro,"RaylibDrawTexturePro",NULL);
	AddUDF(env,"raylib-set-texture-filter","v",6,6,";l;l;l;l;l;ly",RaylibSetTextureFilter,"RaylibSetTextureFilter",NULL);
	AddUDF(env,"raylib-begin-texture-mode","v",11,11,"l",RaylibBeginTextureMode,"RaylibBeginTextureMode",NULL);
	AddUDF(env,"raylib-end-texture-mode","v",0,0,NULL,RaylibEndTextureMode,"RaylibEndTextureMode",NULL);

	AddUDF(env,"raylib-set-random-seed","v",1,1,";l",RaylibSetRandomSeed,"RaylibSetRandomSeed",NULL);
	AddUDF(env,"raylib-get-random-value","l",2,2,";l;l",RaylibGetRandomValue,"RaylibGetRandomValue",NULL);

	AddUDF(env,"raylib-hex-string-to-int","l",1,1,"sy",RaylibHexStringToInt,"RaylibHexStringToInt",NULL);

	AddUDF(env,"raylib-poll-input-events","s",0,0,NULL,RaylibPollInputEvents,"RaylibPollInputEvents",NULL);

	AddUDF(env,"raylib-unload-render-texture","s",11,11,NULL,RaylibUnloadRenderTexture,"RaylibUnloadRenderTexture",NULL);
}

/* Closes the window if one is open. */
void RaylibShutdown(Environment *env)
{
	(void) env;
	RaylibGlueShutdown();
}
