#include <string.h>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#define RAYGUIAPI static
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "raylib_glue.h"

static Color GlueColor(int r, int g, int b, int a)
{
	return (Color){ (unsigned char) r, (unsigned char) g, (unsigned char) b, (unsigned char) a };
}

static void GlueUnpackColor(Color c, int *r, int *g, int *b, int *a)
{
	*r = c.r;
	*g = c.g;
	*b = c.b;
	*a = c.a;
}

static Texture2D GlueTexture(long long id, int width, int height, int mipmaps, int format)
{
	Texture2D texture;

	texture.id = (unsigned int) id;
	texture.width = width;
	texture.height = height;
	texture.mipmaps = mipmaps;
	texture.format = format;
	return texture;
}

static RenderTexture2D GlueRenderTexture(long long id,
                                         long long textureId, int textureWidth, int textureHeight,
                                         int textureMipmaps, int textureFormat,
                                         long long depthId, int depthWidth, int depthHeight,
                                         int depthMipmaps, int depthFormat)
{
	RenderTexture2D target;

	target.id = (unsigned int) id;
	target.texture = GlueTexture(textureId, textureWidth, textureHeight, textureMipmaps, textureFormat);
	target.depth = GlueTexture(depthId, depthWidth, depthHeight, depthMipmaps, depthFormat);
	return target;
}

static Camera2D GlueCamera2D(double offsetX, double offsetY,
                             double targetX, double targetY,
                             double rotation, double zoom)
{
	Camera2D camera = { 0 };

	camera.offset = (Vector2){ (float) offsetX, (float) offsetY };
	camera.target = (Vector2){ (float) targetX, (float) targetY };
	camera.rotation = (float) rotation;
	camera.zoom = (float) zoom;
	return camera;
}

static Rectangle GlueRectangle(double x, double y, double width, double height)
{
	return (Rectangle){ (float) x, (float) y, (float) width, (float) height };
}

/* ------------------------------------------------------------------------ */
/* Window and frame                                                         */
/* ------------------------------------------------------------------------ */

void RaylibGlueInitWindow(int width, int height, const char *title)
{
	InitWindow(width, height, title);
}

bool RaylibGlueWindowShouldClose(void)
{
	return WindowShouldClose();
}

void RaylibGlueCloseWindow(void)
{
	CloseWindow();
}

bool RaylibGlueIsWindowReady(void)
{
	return IsWindowReady();
}

void RaylibGlueShutdown(void)
{
	if (IsWindowReady())
	{
		CloseWindow();
	}
}

void RaylibGlueBeginDrawing(void)
{
	BeginDrawing();
}

void RaylibGlueEndDrawing(void)
{
	EndDrawing();
}

void RaylibGlueClearBackground(int r, int g, int b, int a)
{
	ClearBackground(GlueColor(r, g, b, a));
}

void RaylibGlueSetTargetFPS(int fps)
{
	SetTargetFPS(fps);
}

double RaylibGlueGetFrameTime(void)
{
	return GetFrameTime();
}

void RaylibGlueDrawFPS(int x, int y)
{
	DrawFPS(x, y);
}

void RaylibGluePollInputEvents(void)
{
	PollInputEvents();
}

int RaylibGlueGetRenderWidth(void)
{
	return GetRenderWidth();
}

int RaylibGlueGetRenderHeight(void)
{
	return GetRenderHeight();
}

int RaylibGlueGetScreenWidth(void)
{
	return GetScreenWidth();
}

int RaylibGlueGetScreenHeight(void)
{
	return GetScreenHeight();
}

/* ------------------------------------------------------------------------ */
/* Colours                                                                  */
/* ------------------------------------------------------------------------ */

static const struct { const char *name; Color color; } GlueNamedColors[] =
{
	{ "LIGHTGRAY",  LIGHTGRAY },
	{ "GRAY",       GRAY },
	{ "DARKGRAY",   DARKGRAY },
	{ "YELLOW",     YELLOW },
	{ "GOLD",       GOLD },
	{ "ORANGE",     ORANGE },
	{ "PINK",       PINK },
	{ "RED",        RED },
	{ "MAROON",     MAROON },
	{ "GREEN",      GREEN },
	{ "LIME",       LIME },
	{ "DARKGREEN",  DARKGREEN },
	{ "SKYBLUE",    SKYBLUE },
	{ "BLUE",       BLUE },
	{ "DARKBLUE",   DARKBLUE },
	{ "PURPLE",     PURPLE },
	{ "VIOLET",     VIOLET },
	{ "DARKPURPLE", DARKPURPLE },
	{ "BEIGE",      BEIGE },
	{ "BROWN",      BROWN },
	{ "DARKBROWN",  DARKBROWN },
	{ "WHITE",      WHITE },
	{ "BLACK",      BLACK },
	{ "MAGENTA",    MAGENTA },
	{ "RAYWHITE",   RAYWHITE },
	{ "BLANK",      BLANK },
};

bool RaylibGlueColorByName(const char *name, int *r, int *g, int *b, int *a)
{
	size_t i;

	for (i = 0; i < sizeof(GlueNamedColors) / sizeof(GlueNamedColors[0]); i++)
	{
		if (strcmp(GlueNamedColors[i].name, name) == 0)
		{
			GlueUnpackColor(GlueNamedColors[i].color, r, g, b, a);
			return true;
		}
	}
	GlueUnpackColor(BLACK, r, g, b, a);
	return false;
}

void RaylibGlueFade(int r, int g, int b, int a, double alpha,
                    int *outR, int *outG, int *outB, int *outA)
{
	GlueUnpackColor(Fade(GlueColor(r, g, b, a), (float) alpha), outR, outG, outB, outA);
}

void RaylibGlueColorLerp(int r1, int g1, int b1, int a1,
                         int r2, int g2, int b2, int a2, double factor,
                         int *outR, int *outG, int *outB, int *outA)
{
	GlueUnpackColor(ColorLerp(GlueColor(r1, g1, b1, a1), GlueColor(r2, g2, b2, a2), (float) factor),
	                outR, outG, outB, outA);
}

void RaylibGlueGetColor(long long hexValue, int *r, int *g, int *b, int *a)
{
	GlueUnpackColor(GetColor((unsigned int) hexValue), r, g, b, a);
}

/* ------------------------------------------------------------------------ */
/* raymath                                                                  */
/* ------------------------------------------------------------------------ */

double RaylibGlueLerp(double start, double end, double amount)
{
	return Lerp((float) start, (float) end, (float) amount);
}

double RaylibGlueClamp(double value, double min, double max)
{
	return Clamp((float) value, (float) min, (float) max);
}

/* ------------------------------------------------------------------------ */
/* Text                                                                     */
/* ------------------------------------------------------------------------ */

void RaylibGlueDrawText(const char *text, int x, int y, int fontSize,
                        int r, int g, int b, int a)
{
	DrawText(text, x, y, fontSize, GlueColor(r, g, b, a));
}

int RaylibGlueMeasureText(const char *text, int fontSize)
{
	return MeasureText(text, fontSize);
}

/* ------------------------------------------------------------------------ */
/* Shapes                                                                   */
/* ------------------------------------------------------------------------ */

void RaylibGlueDrawCircle(int centerX, int centerY, double radius,
                          int r, int g, int b, int a)
{
	DrawCircle(centerX, centerY, (float) radius, GlueColor(r, g, b, a));
}

void RaylibGlueDrawCircleLines(int centerX, int centerY, double radius,
                               int r, int g, int b, int a)
{
	DrawCircleLines(centerX, centerY, (float) radius, GlueColor(r, g, b, a));
}

void RaylibGlueDrawCircleV(double centerX, double centerY, double radius,
                           int r, int g, int b, int a)
{
	DrawCircleV((Vector2){ (float) centerX, (float) centerY }, (float) radius, GlueColor(r, g, b, a));
}

void RaylibGlueDrawLine(int startX, int startY, int endX, int endY,
                        int r, int g, int b, int a)
{
	DrawLine(startX, startY, endX, endY, GlueColor(r, g, b, a));
}

void RaylibGlueDrawLineV(double startX, double startY, double endX, double endY,
                         int r, int g, int b, int a)
{
	DrawLineV((Vector2){ (float) startX, (float) startY },
	          (Vector2){ (float) endX, (float) endY },
	          GlueColor(r, g, b, a));
}

void RaylibGlueDrawRectangle(int x, int y, int width, int height,
                             int r, int g, int b, int a)
{
	DrawRectangle(x, y, width, height, GlueColor(r, g, b, a));
}

void RaylibGlueDrawRectangleLines(int x, int y, int width, int height,
                                  int r, int g, int b, int a)
{
	DrawRectangleLines(x, y, width, height, GlueColor(r, g, b, a));
}

void RaylibGlueDrawRectangleRounded(double x, double y, double width, double height,
                                    double roundness, int segments,
                                    int r, int g, int b, int a)
{
	DrawRectangleRounded(GlueRectangle(x, y, width, height), (float) roundness, segments,
	                     GlueColor(r, g, b, a));
}

void RaylibGlueDrawRectangleRoundedLines(double x, double y, double width, double height,
                                         double roundness, int segments,
                                         int r, int g, int b, int a)
{
	DrawRectangleRoundedLines(GlueRectangle(x, y, width, height), (float) roundness, segments,
	                          GlueColor(r, g, b, a));
}

void RaylibGlueDrawRectangleRoundedLinesEx(double x, double y, double width, double height,
                                           double roundness, int segments, double lineThick,
                                           int r, int g, int b, int a)
{
	DrawRectangleRoundedLinesEx(GlueRectangle(x, y, width, height), (float) roundness, segments,
	                            (float) lineThick, GlueColor(r, g, b, a));
}

void RaylibGlueDrawPlane(double centerX, double centerY, double centerZ,
                         double sizeX, double sizeY,
                         int r, int g, int b, int a)
{
	DrawPlane((Vector3){ (float) centerX, (float) centerY, (float) centerZ },
	          (Vector2){ (float) sizeX, (float) sizeY },
	          GlueColor(r, g, b, a));
}

void RaylibGlueDrawCube(double x, double y, double z,
                        double width, double height, double length,
                        int r, int g, int b, int a)
{
	DrawCube((Vector3){ (float) x, (float) y, (float) z },
	         (float) width, (float) height, (float) length,
	         GlueColor(r, g, b, a));
}

/* ------------------------------------------------------------------------ */
/* Collisions                                                               */
/* ------------------------------------------------------------------------ */

bool RaylibGlueCheckCollisionRecs(double x1, double y1, double w1, double h1,
                                  double x2, double y2, double w2, double h2)
{
	return CheckCollisionRecs(GlueRectangle(x1, y1, w1, h1), GlueRectangle(x2, y2, w2, h2));
}

bool RaylibGlueCheckCollisionPointRec(double px, double py,
                                      double x, double y, double w, double h)
{
	return CheckCollisionPointRec((Vector2){ (float) px, (float) py }, GlueRectangle(x, y, w, h));
}

void RaylibGlueGetCollisionRec(double x1, double y1, double w1, double h1,
                               double x2, double y2, double w2, double h2,
                               double *outX, double *outY, double *outW, double *outH)
{
	Rectangle collision = GetCollisionRec(GlueRectangle(x1, y1, w1, h1), GlueRectangle(x2, y2, w2, h2));

	*outX = collision.x;
	*outY = collision.y;
	*outW = collision.width;
	*outH = collision.height;
}

/* ------------------------------------------------------------------------ */
/* Mouse                                                                    */
/* ------------------------------------------------------------------------ */

static const struct { const char *name; int button; } GlueMouseButtons[] =
{
	{ "MOUSE_BUTTON_LEFT",    MOUSE_BUTTON_LEFT },
	{ "MOUSE_BUTTON_RIGHT",   MOUSE_BUTTON_RIGHT },
	{ "MOUSE_BUTTON_MIDDLE",  MOUSE_BUTTON_MIDDLE },
	{ "MOUSE_BUTTON_SIDE",    MOUSE_BUTTON_SIDE },
	{ "MOUSE_BUTTON_EXTRA",   MOUSE_BUTTON_EXTRA },
	{ "MOUSE_BUTTON_FORWARD", MOUSE_BUTTON_FORWARD },
	{ "MOUSE_BUTTON_BACK",    MOUSE_BUTTON_BACK },
};

int RaylibGlueMouseButtonByName(const char *name)
{
	size_t i;

	for (i = 0; i < sizeof(GlueMouseButtons) / sizeof(GlueMouseButtons[0]); i++)
	{
		if (strcmp(GlueMouseButtons[i].name, name) == 0)
		{
			return GlueMouseButtons[i].button;
		}
	}
	return -1;
}

int RaylibGlueMouseButtonCount(void)
{
	return (int) (sizeof(GlueMouseButtons) / sizeof(GlueMouseButtons[0]));
}

int RaylibGlueMouseButtonAt(int index)
{
	return GlueMouseButtons[index].button;
}

bool RaylibGlueIsMouseButtonPressed(int button)
{
	return IsMouseButtonPressed(button);
}

bool RaylibGlueIsMouseButtonDown(int button)
{
	return IsMouseButtonDown(button);
}

bool RaylibGlueIsMouseButtonReleased(int button)
{
	return IsMouseButtonReleased(button);
}

bool RaylibGlueIsMouseButtonUp(int button)
{
	return IsMouseButtonUp(button);
}

void RaylibGlueGetMousePosition(double *x, double *y)
{
	Vector2 v = GetMousePosition();

	*x = v.x;
	*y = v.y;
}

void RaylibGlueGetMouseDelta(double *x, double *y)
{
	Vector2 v = GetMouseDelta();

	*x = v.x;
	*y = v.y;
}

int RaylibGlueGetMouseX(void)
{
	return GetMouseX();
}

int RaylibGlueGetMouseY(void)
{
	return GetMouseY();
}

double RaylibGlueGetMouseWheelMove(void)
{
	return GetMouseWheelMove();
}

/* ------------------------------------------------------------------------ */
/* Keyboard                                                                 */
/* ------------------------------------------------------------------------ */

static const struct { const char *name; int key; } GlueKeys[] =
{
	{ "KEY_APOSTROPHE",    KEY_APOSTROPHE },
	{ "KEY_COMMA",         KEY_COMMA },
	{ "KEY_MINUS",         KEY_MINUS },
	{ "KEY_PERIOD",        KEY_PERIOD },
	{ "KEY_SLASH",         KEY_SLASH },
	{ "KEY_ZERO",          KEY_ZERO },
	{ "KEY_ONE",           KEY_ONE },
	{ "KEY_TWO",           KEY_TWO },
	{ "KEY_THREE",         KEY_THREE },
	{ "KEY_FOUR",          KEY_FOUR },
	{ "KEY_FIVE",          KEY_FIVE },
	{ "KEY_SIX",           KEY_SIX },
	{ "KEY_SEVEN",         KEY_SEVEN },
	{ "KEY_EIGHT",         KEY_EIGHT },
	{ "KEY_NINE",          KEY_NINE },
	{ "KEY_SEMICOLON",     KEY_SEMICOLON },
	{ "KEY_EQUAL",         KEY_EQUAL },
	{ "KEY_A",             KEY_A },
	{ "KEY_B",             KEY_B },
	{ "KEY_C",             KEY_C },
	{ "KEY_D",             KEY_D },
	{ "KEY_E",             KEY_E },
	{ "KEY_F",             KEY_F },
	{ "KEY_G",             KEY_G },
	{ "KEY_H",             KEY_H },
	{ "KEY_I",             KEY_I },
	{ "KEY_J",             KEY_J },
	{ "KEY_K",             KEY_K },
	{ "KEY_L",             KEY_L },
	{ "KEY_M",             KEY_M },
	{ "KEY_N",             KEY_N },
	{ "KEY_O",             KEY_O },
	{ "KEY_P",             KEY_P },
	{ "KEY_Q",             KEY_Q },
	{ "KEY_R",             KEY_R },
	{ "KEY_S",             KEY_S },
	{ "KEY_T",             KEY_T },
	{ "KEY_U",             KEY_U },
	{ "KEY_V",             KEY_V },
	{ "KEY_W",             KEY_W },
	{ "KEY_X",             KEY_X },
	{ "KEY_Y",             KEY_Y },
	{ "KEY_Z",             KEY_Z },
	{ "KEY_LEFT_BRACKET",  KEY_LEFT_BRACKET },
	{ "KEY_BACKSLASH",     KEY_BACKSLASH },
	{ "KEY_RIGHT_BRACKET", KEY_RIGHT_BRACKET },
	{ "KEY_GRAVE",         KEY_GRAVE },
	{ "KEY_SPACE",         KEY_SPACE },
	{ "KEY_ESCAPE",        KEY_ESCAPE },
	{ "KEY_ENTER",         KEY_ENTER },
	{ "KEY_TAB",           KEY_TAB },
	{ "KEY_BACKSPACE",     KEY_BACKSPACE },
	{ "KEY_INSERT",        KEY_INSERT },
	{ "KEY_DELETE",        KEY_DELETE },
	{ "KEY_RIGHT",         KEY_RIGHT },
	{ "KEY_LEFT",          KEY_LEFT },
	{ "KEY_DOWN",          KEY_DOWN },
	{ "KEY_UP",            KEY_UP },
	{ "KEY_PAGE_UP",       KEY_PAGE_UP },
	{ "KEY_PAGE_DOWN",     KEY_PAGE_DOWN },
	{ "KEY_HOME",          KEY_HOME },
	{ "KEY_END",           KEY_END },
	{ "KEY_CAPS_LOCK",     KEY_CAPS_LOCK },
	{ "KEY_SCROLL_LOCK",   KEY_SCROLL_LOCK },
	{ "KEY_NUM_LOCK",      KEY_NUM_LOCK },
	{ "KEY_PRINT_SCREEN",  KEY_PRINT_SCREEN },
	{ "KEY_PAUSE",         KEY_PAUSE },
	{ "KEY_F1",            KEY_F1 },
	{ "KEY_F2",            KEY_F2 },
	{ "KEY_F3",            KEY_F3 },
	{ "KEY_F4",            KEY_F4 },
	{ "KEY_F5",            KEY_F5 },
	{ "KEY_F6",            KEY_F6 },
	{ "KEY_F7",            KEY_F7 },
	{ "KEY_F8",            KEY_F8 },
	{ "KEY_F9",            KEY_F9 },
	{ "KEY_F10",           KEY_F10 },
	{ "KEY_F11",           KEY_F11 },
	{ "KEY_F12",           KEY_F12 },
	{ "KEY_LEFT_SHIFT",    KEY_LEFT_SHIFT },
	{ "KEY_LEFT_CONTROL",  KEY_LEFT_CONTROL },
	{ "KEY_LEFT_ALT",      KEY_LEFT_ALT },
	{ "KEY_LEFT_SUPER",    KEY_LEFT_SUPER },
	{ "KEY_RIGHT_SHIFT",   KEY_RIGHT_SHIFT },
	{ "KEY_RIGHT_CONTROL", KEY_RIGHT_CONTROL },
	{ "KEY_RIGHT_ALT",     KEY_RIGHT_ALT },
	{ "KEY_RIGHT_SUPER",   KEY_RIGHT_SUPER },
	{ "KEY_KB_MENU",       KEY_KB_MENU },
	{ "KEY_KP_0",          KEY_KP_0 },
	{ "KEY_KP_1",          KEY_KP_1 },
	{ "KEY_KP_2",          KEY_KP_2 },
	{ "KEY_KP_3",          KEY_KP_3 },
	{ "KEY_KP_4",          KEY_KP_4 },
	{ "KEY_KP_5",          KEY_KP_5 },
	{ "KEY_KP_6",          KEY_KP_6 },
	{ "KEY_KP_7",          KEY_KP_7 },
	{ "KEY_KP_8",          KEY_KP_8 },
	{ "KEY_KP_9",          KEY_KP_9 },
	{ "KEY_KP_DECIMAL",    KEY_KP_DECIMAL },
	{ "KEY_KP_DIVIDE",     KEY_KP_DIVIDE },
	{ "KEY_KP_MULTIPLY",   KEY_KP_MULTIPLY },
	{ "KEY_KP_SUBTRACT",   KEY_KP_SUBTRACT },
	{ "KEY_KP_ADD",        KEY_KP_ADD },
	{ "KEY_KP_ENTER",      KEY_KP_ENTER },
	{ "KEY_KP_EQUAL",      KEY_KP_EQUAL },
	{ "KEY_BACK",          KEY_BACK },
	{ "KEY_MENU",          KEY_MENU },
	{ "KEY_VOLUME_UP",     KEY_VOLUME_UP },
	{ "KEY_VOLUME_DOWN",   KEY_VOLUME_DOWN },
};

int RaylibGlueKeyByName(const char *name)
{
	size_t i;

	for (i = 0; i < sizeof(GlueKeys) / sizeof(GlueKeys[0]); i++)
	{
		if (strcmp(GlueKeys[i].name, name) == 0)
		{
			return GlueKeys[i].key;
		}
	}
	return KEY_NULL;
}

int RaylibGlueKeyCount(void)
{
	return (int) (sizeof(GlueKeys) / sizeof(GlueKeys[0]));
}

int RaylibGlueKeyAt(int index)
{
	return GlueKeys[index].key;
}

bool RaylibGlueIsKeyPressed(int key)
{
	return IsKeyPressed(key);
}

bool RaylibGlueIsKeyPressedRepeat(int key)
{
	return IsKeyPressedRepeat(key);
}

bool RaylibGlueIsKeyDown(int key)
{
	return IsKeyDown(key);
}

bool RaylibGlueIsKeyReleased(int key)
{
	return IsKeyReleased(key);
}

bool RaylibGlueIsKeyUp(int key)
{
	return IsKeyUp(key);
}

/* ------------------------------------------------------------------------ */
/* Cameras                                                                  */
/* ------------------------------------------------------------------------ */

void RaylibGlueBeginMode2D(double offsetX, double offsetY,
                           double targetX, double targetY,
                           double rotation, double zoom)
{
	BeginMode2D(GlueCamera2D(offsetX, offsetY, targetX, targetY, rotation, zoom));
}

void RaylibGlueEndMode2D(void)
{
	EndMode2D();
}

void RaylibGlueBeginMode3D(double positionX, double positionY, double positionZ,
                           double targetX, double targetY, double targetZ,
                           double upX, double upY, double upZ,
                           double fovy, int projection)
{
	Camera3D camera = { 0 };

	camera.position = (Vector3){ (float) positionX, (float) positionY, (float) positionZ };
	camera.target = (Vector3){ (float) targetX, (float) targetY, (float) targetZ };
	camera.up = (Vector3){ (float) upX, (float) upY, (float) upZ };
	camera.fovy = (float) fovy;
	camera.projection = projection;
	BeginMode3D(camera);
}

void RaylibGlueEndMode3D(void)
{
	EndMode3D();
}

void RaylibGlueGetScreenToWorld2D(double screenX, double screenY,
                                  double offsetX, double offsetY,
                                  double targetX, double targetY,
                                  double rotation, double zoom,
                                  double *outX, double *outY)
{
	Vector2 world = GetScreenToWorld2D((Vector2){ (float) screenX, (float) screenY },
	                                   GlueCamera2D(offsetX, offsetY, targetX, targetY, rotation, zoom));

	*outX = world.x;
	*outY = world.y;
}

void RaylibGlueGetWorldToScreen2D(double worldX, double worldY,
                                  double offsetX, double offsetY,
                                  double targetX, double targetY,
                                  double rotation, double zoom,
                                  double *outX, double *outY)
{
	Vector2 screen = GetWorldToScreen2D((Vector2){ (float) worldX, (float) worldY },
	                                    GlueCamera2D(offsetX, offsetY, targetX, targetY, rotation, zoom));

	*outX = screen.x;
	*outY = screen.y;
}

/* ------------------------------------------------------------------------ */
/* raygui                                                                   */
/* ------------------------------------------------------------------------ */

double RaylibGlueGuiSliderBar(double x, double y, double width, double height,
                              const char *textLeft, const char *textRight,
                              double value, double minValue, double maxValue)
{
	float current = (float) value;

	GuiSliderBar(GlueRectangle(x, y, width, height), textLeft, textRight,
	             &current, (float) minValue, (float) maxValue);
	return current;
}

bool RaylibGlueGuiCheckBox(double x, double y, double width, double height,
                           const char *text, bool checked)
{
	bool current = checked;

	GuiCheckBox(GlueRectangle(x, y, width, height), text, &current);
	return current;
}

/* ------------------------------------------------------------------------ */
/* Textures                                                                 */
/* ------------------------------------------------------------------------ */

void RaylibGlueLoadTexture(const char *fileName,
                           long long *id, int *width, int *height,
                           int *mipmaps, int *format)
{
	Texture2D texture = LoadTexture(fileName);

	*id = texture.id;
	*width = texture.width;
	*height = texture.height;
	*mipmaps = texture.mipmaps;
	*format = texture.format;
}

void RaylibGlueLoadRenderTexture(int width, int height,
                                 long long *id,
                                 long long *textureId, int *textureWidth, int *textureHeight,
                                 int *textureMipmaps, int *textureFormat,
                                 long long *depthId, int *depthWidth, int *depthHeight,
                                 int *depthMipmaps, int *depthFormat)
{
	RenderTexture2D target = LoadRenderTexture(width, height);

	*id = target.id;
	*textureId = target.texture.id;
	*textureWidth = target.texture.width;
	*textureHeight = target.texture.height;
	*textureMipmaps = target.texture.mipmaps;
	*textureFormat = target.texture.format;
	*depthId = target.depth.id;
	*depthWidth = target.depth.width;
	*depthHeight = target.depth.height;
	*depthMipmaps = target.depth.mipmaps;
	*depthFormat = target.depth.format;
}

void RaylibGlueDrawTexture(long long id, int width, int height, int mipmaps, int format,
                           double x, double y,
                           int r, int g, int b, int a)
{
	DrawTexture(GlueTexture(id, width, height, mipmaps, format), (int) x, (int) y,
	            GlueColor(r, g, b, a));
}

void RaylibGlueDrawTextureEx(long long id, int width, int height, int mipmaps, int format,
                             double x, double y, double rotation, double scale,
                             int r, int g, int b, int a)
{
	DrawTextureEx(GlueTexture(id, width, height, mipmaps, format),
	              (Vector2){ (float) x, (float) y }, (float) rotation, (float) scale,
	              GlueColor(r, g, b, a));
}

void RaylibGlueDrawTextureRec(long long id, int width, int height, int mipmaps, int format,
                              double sourceX, double sourceY, double sourceW, double sourceH,
                              double x, double y,
                              int r, int g, int b, int a)
{
	DrawTextureRec(GlueTexture(id, width, height, mipmaps, format),
	               GlueRectangle(sourceX, sourceY, sourceW, sourceH),
	               (Vector2){ (float) x, (float) y },
	               GlueColor(r, g, b, a));
}

void RaylibGlueDrawTexturePro(long long id, int width, int height, int mipmaps, int format,
                              double sourceX, double sourceY, double sourceW, double sourceH,
                              double destX, double destY, double destW, double destH,
                              double originX, double originY, double rotation,
                              int r, int g, int b, int a)
{
	DrawTexturePro(GlueTexture(id, width, height, mipmaps, format),
	               GlueRectangle(sourceX, sourceY, sourceW, sourceH),
	               GlueRectangle(destX, destY, destW, destH),
	               (Vector2){ (float) originX, (float) originY }, (float) rotation,
	               GlueColor(r, g, b, a));
}

static const struct { const char *name; int filter; } GlueTextureFilters[] =
{
	{ "TEXTURE_FILTER_POINT",           TEXTURE_FILTER_POINT },
	{ "TEXTURE_FILTER_BILINEAR",        TEXTURE_FILTER_BILINEAR },
	{ "TEXTURE_FILTER_TRILINEAR",       TEXTURE_FILTER_TRILINEAR },
	{ "TEXTURE_FILTER_ANISOTROPIC_4X",  TEXTURE_FILTER_ANISOTROPIC_4X },
	{ "TEXTURE_FILTER_ANISOTROPIC_8X",  TEXTURE_FILTER_ANISOTROPIC_8X },
	{ "TEXTURE_FILTER_ANISOTROPIC_16X", TEXTURE_FILTER_ANISOTROPIC_16X },
};

int RaylibGlueTextureFilterByName(const char *name)
{
	size_t i;

	for (i = 0; i < sizeof(GlueTextureFilters) / sizeof(GlueTextureFilters[0]); i++)
	{
		if (strcmp(GlueTextureFilters[i].name, name) == 0)
		{
			return GlueTextureFilters[i].filter;
		}
	}
	return -1;
}

void RaylibGlueSetTextureFilter(long long id, int width, int height, int mipmaps, int format,
                                int filter)
{
	SetTextureFilter(GlueTexture(id, width, height, mipmaps, format), filter);
}

void RaylibGlueBeginTextureMode(long long id,
                                long long textureId, int textureWidth, int textureHeight,
                                int textureMipmaps, int textureFormat,
                                long long depthId, int depthWidth, int depthHeight,
                                int depthMipmaps, int depthFormat)
{
	BeginTextureMode(GlueRenderTexture(id,
	                                   textureId, textureWidth, textureHeight, textureMipmaps, textureFormat,
	                                   depthId, depthWidth, depthHeight, depthMipmaps, depthFormat));
}

void RaylibGlueEndTextureMode(void)
{
	EndTextureMode();
}

void RaylibGlueUnloadRenderTexture(long long id,
                                   long long textureId, int textureWidth, int textureHeight,
                                   int textureMipmaps, int textureFormat,
                                   long long depthId, int depthWidth, int depthHeight,
                                   int depthMipmaps, int depthFormat)
{
	UnloadRenderTexture(GlueRenderTexture(id,
	                                      textureId, textureWidth, textureHeight, textureMipmaps, textureFormat,
	                                      depthId, depthWidth, depthHeight, depthMipmaps, depthFormat));
}

/* ------------------------------------------------------------------------ */
/* Random numbers                                                           */
/* ------------------------------------------------------------------------ */

void RaylibGlueSetRandomSeed(long long seed)
{
	SetRandomSeed((unsigned int) seed);
}

int RaylibGlueGetRandomValue(int min, int max)
{
	return GetRandomValue(min, max);
}
