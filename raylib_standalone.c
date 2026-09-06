/*
 * raylib_standalone.c
 *
 * The UserFunctions that CLIPS calls when it creates an environment, for the
 * clips binary this repository builds.
 */

#include "clips.h"

void RaylibUserFunctions(Environment *);

void UserFunctions(Environment *env)
{
	RaylibUserFunctions(env);
}
