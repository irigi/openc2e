/*
 *  Creature.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
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

#ifndef __CREATURE_H
#define __CREATURE_H

#include "AgentRef.h"
#include "genome.h"

#include <deque>
#include <ncurses.h>

class IrigiAgent;
class CreatureAgent;
class Creature;

enum ChemicalsByNumber {
	CHEM_ATP = 35,
	CHEM_ADP = 36,
	CHEM_INJURY = 127
};

namespace boost {
	template <typename T> class shared_ptr;
}

class Creature {
protected:
	CreatureAgent *parent;
	IrigiAgent *parentagent;
	boost::shared_ptr<genomeFile> genome;
	
	// non-specific bits
	unsigned short genus;
	unsigned int variant;
	bool female;
	
	// state
	bool alive, asleep, dreaming, tickage;
	bool zombie;

	unsigned int ticks;
	unsigned int age; // in ticks
	lifestage stage;

	AgentRef attention;
	int attn, decn;
	
	std::vector<AgentRef> chosenagents;
	bool agentInSight(AgentRef a);
	void chooseAgents();
	virtual AgentRef selectRepresentativeAgent(int type, std::vector<AgentRef> possibles) { return AgentRef(); } // TODO: make pure virtual?

	// linguistic stuff

	// to-be-processed instincts
	std::deque<creatureInstinctGene *> unprocessedinstincts;

	// conscious flag? brain/motor enabled flags? flags for each 'faculty'?
	
	unsigned short tintinfo[5]; // red, green, blue, rotation, swap

	virtual void processGenes();
	virtual void addGene(gene *);
	
	Creature(boost::shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);
	void finishInit();

public:
	virtual ~Creature();
	virtual void tick();
	virtual void consoleOutput() { }

	virtual void ageCreature();
	lifestage getStage() { return stage; }

	void setAsleep(bool asleep);
	bool isAsleep() { return asleep; }
	void setDreaming(bool dreaming);
	bool isDreaming() { return dreaming; }
	bool isFemale() { return female; }
	bool isAlive() { return alive; }
	void setZombie(bool z) { zombie = z; }
	bool isZombie() { return zombie; }
	unsigned int getAge() { return age; }
	boost::shared_ptr<genomeFile> getGenome() { return genome; }

	unsigned short getGenus() { return genus; }
	unsigned int getVariant() { return variant; }
	unsigned short getTint(unsigned int id) { return tintinfo[id]; }

	unsigned int getNoUnprocessedInstincts() { return unprocessedinstincts.size(); }

	unsigned int getNoCategories() { return chosenagents.size(); }
	AgentRef getChosenAgentForCategory(unsigned int cat) { assert(cat < chosenagents.size()); return chosenagents[cat]; }
	AgentRef getAttentionFocus() { return attention; }
	int getAttentionId() { return attn; }
	int getDecisionId() { return decn; }

	virtual unsigned int getGait() = 0;
	
	virtual void drawNornChemicalsWindow(WINDOW * win) {};
	virtual void drawNornEmitterWindow(WINDOW * win, int &pos) {};
	virtual void drawNornReceptorWindow(WINDOW * win, int &pos) {};
	virtual void drawNornReactionWindow(WINDOW * win, int &pos) {};
	virtual void drawNornDrivesWindow(WINDOW * win, int &pos) {};
	virtual void drawNornBrainWindow(WINDOW * win, int &posx, int &posy, int &posu, int &posv) {};

	void born();
	void die();
	
	bool shouldProcessGene(gene *);
};

#endif

/* vim: set noet: */
