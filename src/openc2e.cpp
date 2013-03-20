/*
 *  openc2e.cpp
 *  openc2e
 *
 *  Created by Jan Olsina on Wed Mar 20 2013.
 *  Copyright (c) 2013 Jan Olsina. All rights reserved.
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

char * BOOL_S(bool s)  {
	if(s)
		return const_cast<char *>("T");

	return const_cast<char *>("F");
};

/* vim: set noet: */
