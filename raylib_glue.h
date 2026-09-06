/*
 * raylib_glue.h
 *
 * The private boundary between the CLIPS side of CLIPSraylib and the raylib
 * side. That keeps raylib's and the CLIPS
 * core's declarations of GetFileName out of the same translation unit.
 */

#ifndef RAYLIB_GLUE_H
#define RAYLIB_GLUE_H

#include <stdbool.h>

/* Window and frame. */
void   RaylibGlueInitWindow(int width, int height, const char *title);
bool   RaylibGlueWindowShouldClose(void);
void   RaylibGlueCloseWindow(void);
bool   RaylibGlueIsWindowReady(void);
void   RaylibGlueShutdown(void);
void   RaylibGlueBeginDrawing(void);
void   RaylibGlueEndDrawing(void);
void   RaylibGlueClearBackground(int r, int g, int b, int a);
void   RaylibGlueSetTargetFPS(int fps);
double RaylibGlueGetFrameTime(void);
void   RaylibGlueDrawFPS(int x, int y);
void   RaylibGluePollInputEvents(void);
int    RaylibGlueGetRenderWidth(void);
int    RaylibGlueGetRenderHeight(void);
int    RaylibGlueGetScreenWidth(void);
int    RaylibGlueGetScreenHeight(void);

/* Colours. RaylibGlueColorByName knows raylib's named colours (RED, RAYWHITE,
 * ...); an unknown name gives BLACK and returns false. */
bool   RaylibGlueColorByName(const char *name, int *r, int *g, int *b, int *a);
void   RaylibGlueFade(int r, int g, int b, int a, double alpha,
                      int *outR, int *outG, int *outB, int *outA);
void   RaylibGlueColorLerp(int r1, int g1, int b1, int a1,
                           int r2, int g2, int b2, int a2, double factor,
                           int *outR, int *outG, int *outB, int *outA);
void   RaylibGlueGetColor(long long hexValue, int *r, int *g, int *b, int *a);

/* raymath. */
double RaylibGlueLerp(double start, double end, double amount);
double RaylibGlueClamp(double value, double min, double max);

/* Text. */
void   RaylibGlueDrawText(const char *text, int x, int y, int fontSize,
                          int r, int g, int b, int a);
int    RaylibGlueMeasureText(const char *text, int fontSize);

/* Shapes. */
void   RaylibGlueDrawCircle(int centerX, int centerY, double radius,
                            int r, int g, int b, int a);
void   RaylibGlueDrawCircleLines(int centerX, int centerY, double radius,
                                 int r, int g, int b, int a);
void   RaylibGlueDrawCircleV(double centerX, double centerY, double radius,
                             int r, int g, int b, int a);
void   RaylibGlueDrawLine(int startX, int startY, int endX, int endY,
                          int r, int g, int b, int a);
void   RaylibGlueDrawLineV(double startX, double startY, double endX, double endY,
                           int r, int g, int b, int a);
void   RaylibGlueDrawRectangle(int x, int y, int width, int height,
                               int r, int g, int b, int a);
void   RaylibGlueDrawRectangleLines(int x, int y, int width, int height,
                                    int r, int g, int b, int a);
void   RaylibGlueDrawRectangleRounded(double x, double y, double width, double height,
                                      double roundness, int segments,
                                      int r, int g, int b, int a);
void   RaylibGlueDrawRectangleRoundedLines(double x, double y, double width, double height,
                                           double roundness, int segments,
                                           int r, int g, int b, int a);
void   RaylibGlueDrawRectangleRoundedLinesEx(double x, double y, double width, double height,
                                             double roundness, int segments, double lineThick,
                                             int r, int g, int b, int a);
void   RaylibGlueDrawPlane(double centerX, double centerY, double centerZ,
                           double sizeX, double sizeY,
                           int r, int g, int b, int a);
void   RaylibGlueDrawCube(double x, double y, double z,
                          double width, double height, double length,
                          int r, int g, int b, int a);

/* Collisions. */
bool   RaylibGlueCheckCollisionRecs(double x1, double y1, double w1, double h1,
                                    double x2, double y2, double w2, double h2);
bool   RaylibGlueCheckCollisionPointRec(double px, double py,
                                        double x, double y, double w, double h);
void   RaylibGlueGetCollisionRec(double x1, double y1, double w1, double h1,
                                 double x2, double y2, double w2, double h2,
                                 double *outX, double *outY, double *outW, double *outH);

/* Mouse. RaylibGlueMouseButtonByName maps MOUSE_BUTTON_LEFT and the rest to
 * raylib's button codes, or -1 for a name it does not know. The table of
 * buttons is exposed for the "any button" case. */
int    RaylibGlueMouseButtonByName(const char *name);
int    RaylibGlueMouseButtonCount(void);
int    RaylibGlueMouseButtonAt(int index);
bool   RaylibGlueIsMouseButtonPressed(int button);
bool   RaylibGlueIsMouseButtonDown(int button);
bool   RaylibGlueIsMouseButtonReleased(int button);
bool   RaylibGlueIsMouseButtonUp(int button);
void   RaylibGlueGetMousePosition(double *x, double *y);
void   RaylibGlueGetMouseDelta(double *x, double *y);
int    RaylibGlueGetMouseX(void);
int    RaylibGlueGetMouseY(void);
double RaylibGlueGetMouseWheelMove(void);

/* Keyboard. RaylibGlueKeyByName maps KEY_A and the rest to raylib's key
 * codes, or 0 (KEY_NULL) for a name it does not know. */
int    RaylibGlueKeyByName(const char *name);
int    RaylibGlueKeyCount(void);
int    RaylibGlueKeyAt(int index);
bool   RaylibGlueIsKeyPressed(int key);
bool   RaylibGlueIsKeyPressedRepeat(int key);
bool   RaylibGlueIsKeyDown(int key);
bool   RaylibGlueIsKeyReleased(int key);
bool   RaylibGlueIsKeyUp(int key);

/* Cameras. */
void   RaylibGlueBeginMode2D(double offsetX, double offsetY,
                             double targetX, double targetY,
                             double rotation, double zoom);
void   RaylibGlueEndMode2D(void);
void   RaylibGlueBeginMode3D(double positionX, double positionY, double positionZ,
                             double targetX, double targetY, double targetZ,
                             double upX, double upY, double upZ,
                             double fovy, int projection);
void   RaylibGlueEndMode3D(void);
void   RaylibGlueGetScreenToWorld2D(double screenX, double screenY,
                                    double offsetX, double offsetY,
                                    double targetX, double targetY,
                                    double rotation, double zoom,
                                    double *outX, double *outY);
void   RaylibGlueGetWorldToScreen2D(double worldX, double worldY,
                                    double offsetX, double offsetY,
                                    double targetX, double targetY,
                                    double rotation, double zoom,
                                    double *outX, double *outY);

/* raygui. */
double RaylibGlueGuiSliderBar(double x, double y, double width, double height,
                              const char *textLeft, const char *textRight,
                              double value, double minValue, double maxValue);
bool   RaylibGlueGuiCheckBox(double x, double y, double width, double height,
                             const char *text, bool checked);

/* Textures. */
void   RaylibGlueLoadTexture(const char *fileName,
                             long long *id, int *width, int *height,
                             int *mipmaps, int *format);
void   RaylibGlueLoadRenderTexture(int width, int height,
                                   long long *id,
                                   long long *textureId, int *textureWidth, int *textureHeight,
                                   int *textureMipmaps, int *textureFormat,
                                   long long *depthId, int *depthWidth, int *depthHeight,
                                   int *depthMipmaps, int *depthFormat);
void   RaylibGlueDrawTexture(long long id, int width, int height, int mipmaps, int format,
                             double x, double y,
                             int r, int g, int b, int a);
void   RaylibGlueDrawTextureEx(long long id, int width, int height, int mipmaps, int format,
                               double x, double y, double rotation, double scale,
                               int r, int g, int b, int a);
void   RaylibGlueDrawTextureRec(long long id, int width, int height, int mipmaps, int format,
                                double sourceX, double sourceY, double sourceW, double sourceH,
                                double x, double y,
                                int r, int g, int b, int a);
void   RaylibGlueDrawTexturePro(long long id, int width, int height, int mipmaps, int format,
                                double sourceX, double sourceY, double sourceW, double sourceH,
                                double destX, double destY, double destW, double destH,
                                double originX, double originY, double rotation,
                                int r, int g, int b, int a);
int    RaylibGlueTextureFilterByName(const char *name);
void   RaylibGlueSetTextureFilter(long long id, int width, int height, int mipmaps, int format,
                                  int filter);
void   RaylibGlueBeginTextureMode(long long id,
                                  long long textureId, int textureWidth, int textureHeight,
                                  int textureMipmaps, int textureFormat,
                                  long long depthId, int depthWidth, int depthHeight,
                                  int depthMipmaps, int depthFormat);
void   RaylibGlueEndTextureMode(void);
void   RaylibGlueUnloadRenderTexture(long long id,
                                     long long textureId, int textureWidth, int textureHeight,
                                     int textureMipmaps, int textureFormat,
                                     long long depthId, int depthWidth, int depthHeight,
                                     int depthMipmaps, int depthFormat);

/* Random numbers. */
void   RaylibGlueSetRandomSeed(long long seed);
int    RaylibGlueGetRandomValue(int min, int max);

#endif /* RAYLIB_GLUE_H */
