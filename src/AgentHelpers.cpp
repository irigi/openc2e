/*
 *  AgentHelpers.cpp
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


#include "Agent.h"
#include "TextWorld.h"

bool agentIsVisible(IrigiAgent *seeing, IrigiAgent *dest) {
	// TODO: To be replaced by proper check if there is free path between them.
	return pow(seeing->x-dest->x, 2) + pow(seeing->y-dest->y, 2) + pow(seeing->z-dest->z, 2) <= 25;

}

std::vector<boost::shared_ptr<IrigiAgent> > getVisibleList(IrigiAgent *seeing, unsigned char family, unsigned char genus, unsigned short species) {
	std::vector<boost::shared_ptr<IrigiAgent> > agents;

	for (std::list<boost::shared_ptr<IrigiAgent> >::iterator i
			= textworld.agents.begin(); i != textworld.agents.end(); i++) {
		boost::shared_ptr<IrigiAgent> a = (*i);
		if (!a) continue;
		
		// TODO: if owner is a creature, skip stuff with invisible attribute
		
		// verify species/genus/family
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		if (agentIsVisible(seeing, a.get()))
			agents.push_back(a);
	}

	return agents;
}

bool agentsTouching(IrigiAgent *first, IrigiAgent *second) {
	assert(first && second);

	return (first->x == second->x) && (first->y == second->y) && (first->z == second->z);
}

