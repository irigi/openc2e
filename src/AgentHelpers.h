/*
 *  AgentHelpers.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jul 23 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#ifndef AGENTHELPERS_H
#define AGENTHELPERS_H

#include <vector>

class IrigiAgent;
class AgentRef;

namespace boost {
	template <typename T> class shared_ptr;
}

bool agentIsVisible(IrigiAgent *seeing, IrigiAgent *dest);
std::vector<boost::shared_ptr<IrigiAgent> > getVisibleList(IrigiAgent *seeing, unsigned char family, unsigned char genus, unsigned short species);

bool agentsTouching(IrigiAgent *first, IrigiAgent *second);

#endif

/* vim: set noet: */
