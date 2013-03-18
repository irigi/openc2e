/*
 *  main.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed 02 Jun 2004.
 *  Copyright (c) 2004-2008 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005-2008 Bryan Donlan. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "openc2e.h"
#include "version.h"
#include <iostream>
//#include "Engine.h"
//#include "backends/SDLBackend.h"
//#ifdef SDLMIXER_SUPPORT
//#include "backends/SDLMixerBackend.h"
//#endif
//#ifdef OPENAL_SUPPORT
//#include "backends/OpenALBackend.h"
//#endif
//#ifdef QT_SUPPORT
//#include "backends/qtgui/QtBackend.h"
//#endif

//#ifdef _WIN32
//#include <windows.h>
//#endif

#include "TextWorld.h"

// SDL tries stealing main on some platforms, which we don't want.
#undef main

extern "C" int main(int argc, char *argv[]) {

	// trial and error by irigi

	text_world();

	return 0;

}

/* vim: set noet: */
