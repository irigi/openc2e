/*
 *  Creature.cpp
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

#include "Creature.h"
#include "oldCreature.h"
#include "c2eCreature.h"
#include "CreatureAgent.h"
//#include "World.h"
#include "Catalogue.h"
#include "c2eBrain.h"
#include "oldBrain.h"
#include <stdio.h>
#include "openc2e.h"

const bool debug = false;

Creature::Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) {
	assert(g);
	genome = g;

	female = is_female;
	genus = 0; // TODO: really, we shouldn't do this, and should instead later assert that a genus was set
	variant = _variant;
	stage = baby;

	parent = a;
	assert(parent);
	parentagent = dynamic_cast<IrigiAgent *>(parent);
	assert(parentagent);
	
	alive = true; // ?
	asleep = false; // ?
	dreaming = false; // ?
	tickage = false;
	zombie = false;

	age = ticks = 0;

	attn = decn = -1;

	for (unsigned int i = 0; i < 5; i++)
		tintinfo[i] = 128;
}

Creature::~Creature() {
}

void Creature::finishInit() {
	processGenes();
}

bool Creature::shouldProcessGene(gene *g) {
	geneFlags &flags = g->header.flags;

	// non-expressed genes are to be ignored
	if (flags.notexpressed) return false;

	// gender-specific genes are only to be processed if they are of this 
	if (flags.femaleonly && !female) return false;
	if (flags.maleonly && female) return false;

	// obviously we only switch on at the stage in question
	if (g->header.switchontime != stage) return false;

	// TODO: header.variant?
	
	return true;
}

void Creature::processGenes() {
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if(debug) {
			printf("  gene %s: %s (Ex:%s F:%s M:%s Del:%s Dup:%s, Sta:%d, %d %d %d)\n", (*i)->typeName(), (*i)->name(),
					BOOL_S(!(*i)->header.flags.notexpressed),
					BOOL_S((*i)->header.flags.femaleonly),
					BOOL_S((*i)->header.flags.maleonly),
					BOOL_S((*i)->header.flags.delable),
					BOOL_S((*i)->header.flags.dupable),
					int((*i)->header.switchontime),
					(*i)->header.generation, (*i)->header.mutweighting, (*i)->header.variant);
		}

		if (shouldProcessGene(*i)) addGene(*i);
	}
}

void oldCreature::processGenes() {
	brain->processGenes();
	Creature::processGenes();
}

void c2Creature::processGenes() {
	oldCreature::processGenes();

	for (std::vector<shared_ptr<c2Organ> >::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->processGenes();
	}
}

void c2eCreature::processGenes() {
	// brain must be processed first (to create loci etc)
	// organs should be processed last, because new ones will be created by normal processGenes()

	brain->processGenes();
	Creature::processGenes();
	for (std::vector<shared_ptr<c2eOrgan> >::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->processGenes();
	}
}

void Creature::addGene(gene *g) {
	if (typeid(*g) == typeid(creatureInstinctGene)) {
		unprocessedinstincts.push_back((creatureInstinctGene *)g);
	} else if (typeid(*g) == typeid(creatureGenusGene)) {
		// TODO: mmh, genus changes after setup shouldn't be valid
		genus = ((creatureGenusGene *)g)->genus;
		parentagent->genus = genus + 1;
	} else if (typeid(*g) == typeid(creaturePigmentGene)) {
		creaturePigmentGene &p = *((creaturePigmentGene *)g);
		// TODO: we don't sanity-check
		tintinfo[p.color] = p.amount;
	} else if (typeid(*g) == typeid(creaturePigmentBleedGene)) {
		creaturePigmentBleedGene &p = *((creaturePigmentBleedGene *)g);
		tintinfo[3] = p.rotation;
		tintinfo[4] = p.swap;
	}
}

void Creature::ageCreature() {
	if (stage >= senile) {
		die();
		return;
	} //previously we just returned
	
	stage = (lifestage)((int)stage + 1);
	processGenes();

	assert(parent);
	parent->creatureAged();
#ifndef _CREATURE_STANDALONE	
	// XXX world.history.getMoniker(world.history.findMoniker(genome)).addEvent(4, "", ""); // aged event
#endif
}

void Creature::setAsleep(bool a) {
	// TODO: skeletalcreature might need to close eyes? or should that just be done during the skeletal update?
	if (!a && dreaming)
		setDreaming(false);
	asleep = a;
}

void Creature::setDreaming(bool d) {
	if (d && !asleep)
		setAsleep(true);
	dreaming = d;
}

void Creature::born() {
	parent->creatureBorn();

	// TODO: life event?
#ifndef _CREATURE_STANDALONE
	// XXX world.history.getMoniker(world.history.findMoniker(genome)).wasBorn();
	// XXX world.history.getMoniker(world.history.findMoniker(genome)).addEvent(3, "", ""); // born event, parents..
#endif

	tickage = true;
}

void Creature::die() {
	parent->creatureDied();

	// TODO: life event?
#ifndef _CREATURE_STANDALONE
	// XXX world.history.getMoniker(world.history.findMoniker(genome)).hasDied();
	// XXX world.history.getMoniker(world.history.findMoniker(genome)).addEvent(7, "", ""); // died event
#endif
	// TODO: disable brain/biochemistry updates
	
	// force die script
	parentagent->stopScript();
	parentagent->queueScript(72); // Death script in c1, c2 and c2e
	
	// skeletalcreature eyes, also? see setAsleep comment
	alive = false;
}

void Creature::tick() {
	ticks++;

	if (!alive) return;

	if (tickage) age++;
}

/*
 * oldCreature contains the shared elements of C1 of C2 (creatures are mostly identical in both games)
 */
oldCreature::oldCreature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) : Creature(g, is_female, _variant, a) {
	biochemticks = 0;
	halflives = 0;
	
	for (unsigned int i = 0; i < 8; i++) floatingloci[i] = 0;
	for (unsigned int i = 0; i < 7; i++) lifestageloci[i] = 0;
	for (unsigned int i = 0; i < 8; i++) involaction[i] = 0;
	for (unsigned int i = 0; i < 256; i++) chemicals[i] = 0;
	
	for (unsigned int i = 0; i < 8; i++) involactionlatency[i] = 0;
	
	muscleenergy = 0;
	fertile = pregnant = receptive = 0;
	dead = 0;

	brain = 0; // just in case
}

c1Creature::c1Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) : oldCreature(g, is_female, _variant, a) {
	assert(g->getVersion() == 1);

	for (unsigned int i = 0; i < 6; i++) senses[i] = 0;
	for (unsigned int i = 0; i < 8; i++) gaitloci[i] = 0;
	for (unsigned int i = 0; i < 16; i++) drives[i] = 0;
	
	// TODO: chosenagents size

	brain = new oldBrain(this);
	finishInit();
	brain->init();
}

c2Creature::c2Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) : oldCreature(g, is_female, _variant, a) {
	assert(g->getVersion() == 2);

	for (unsigned int i = 0; i < 14; i++) senses[i] = 0;
	for (unsigned int i = 0; i < 16; i++) gaitloci[i] = 0;
	for (unsigned int i = 0; i < 17; i++) drives[i] = 0;

	mutationchance = 0; mutationdegree = 0;

	// TODO: chosenagents size

	brain = new oldBrain(this);
	finishInit();
	brain->init();	
}

c2eCreature::c2eCreature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) : Creature(g, is_female, _variant, a) {
	assert(g->getVersion() == 3);

	for (unsigned int i = 0; i < 256; i++) chemicals[i] = 0.0f;

	// initialise loci
	for (unsigned int i = 0; i < 7; i++) lifestageloci[i] = 0.0f;
	muscleenergy = 0.0f;
	for (unsigned int i = 0; i < 32; i++) floatingloci[i] = 0.0f;
	fertile = pregnant = ovulate = receptive = chanceofmutation = degreeofmutation = 0.0f;
	dead = 0.0f;
	for (unsigned int i = 0; i < 8; i++) involaction[i] = 0.0f;
	for (unsigned int i = 0; i < 16; i++) gaitloci[i] = 0.0f;
	for (unsigned int i = 0; i < 14; i++) senses[i] = 0.0f;
	for (unsigned int i = 0; i < 20; i++) drives[i] = 0.0f;

	for (unsigned int i = 0; i < 8; i++) involactionlatency[i] = 0;

	halflives = 0;

	if (!catalogue.hasTag("Action Script To Neuron Mappings"))
		throw creaturesException("c2eCreature was unable to read the 'Action Script To Neuron Mappings' catalogue tag");
	const std::vector<std::string> &mappinginfotag = catalogue.getTag("Action Script To Neuron Mappings");
	for (std::vector<std::string>::const_iterator i = mappinginfotag.begin(); i != mappinginfotag.end(); i++)
		mappinginfo.push_back(atoi(i->c_str()));

	// TODO: should we really hard-code this?
	chosenagents.resize(40);

	brain = new c2eBrain(this);
	finishInit();
	brain->init();
}

unsigned int c1Creature::getGait() {
	unsigned int gait = 0;

	for (unsigned int i = 1; i < 8; i++)
		if (gaitloci[i] > gaitloci[gait])
			gait = i;

	return gait;
}

unsigned int c2Creature::getGait() {
	unsigned int gait = 0;

	for (unsigned int i = 1; i < 16; i++)
		if (gaitloci[i] > gaitloci[gait])
			gait = i;

	return gait;
}

unsigned int c2eCreature::getGait() {
	unsigned int gait = 0;

	for (unsigned int i = 1; i < 16; i++)
		if (gaitloci[i] > gaitloci[gait])
			gait = i;

	return gait;
}

void c1Creature::tick() {
	// TODO: should we tick some things even if dead?
	if (!alive) return;

	senses[0] = 255; // always-on
	senses[1] = (asleep ? 255 : 0); // asleep
	senses[2] = 0; // air coldness (TODO)
	senses[3] = 0; // air hotness (TODO)
	senses[4] = 0; // light level (TODO)
	senses[5] = 0; // crowdedness (TODO)	

	tickBrain();
	tickBiochemistry();

	// lifestage checks
	for (unsigned int i = 0; i < 7; i++) {
		if ((lifestageloci[i] != 0) && (stage == (lifestage)i))
			ageCreature();
	}

	if (dead != 0) die();

	Creature::tick();
}

void c2Creature::tick() {
	// TODO: should we tick some things even if dead?
	if (!alive) return;

	senses[0] = 255; // always-on
	senses[1] = (asleep ? 255 : 0); // asleep
	senses[2] = 0; // air coldness (TODO)
	senses[3] = 0; // air hotness (TODO)
	senses[4] = 0; // light level (TODO)
	senses[5] = 0; // crowdedness (TODO)
	senses[6] = 0; // radiation (TODO)
	senses[7] = 0; // time of day (TODO)
	senses[8] = 0; // season (TODO)
	senses[9] = 255; // air quality (TODO)
	senses[10] = 0; // slope up (TODO)
	senses[11] = 0; // slope down (TODO)
	senses[12] = 0; // wind towards (TODO)
	senses[13] = 0; // wind behind (TODO)

	tickBrain();
	// TODO: update brain organ every 0.4ms (ie: when brain is processed)!
	tickBiochemistry();
	
	// lifestage checks
	for (unsigned int i = 0; i < 7; i++) {
		if ((lifestageloci[i] != 0) && (stage == (lifestage)i))
			ageCreature();
	}

	if (dead != 0) die();

	Creature::tick();
}

void c2eCreature::tick() {
	// TODO: should we tick some things even if dead?
	if (!alive) return;

	// TODO: update muscleenergy

	senses[0] = 1.0f; // always-on
	senses[1] = (asleep ? 1.0f : 0.0f); // asleep
	// space for old C2 senses: hotness, coldness, light level
	senses[5] = 0.0f; // crowedness (TODO)
	// space for old C2 senses: radiation, time of day, season
	senses[9] = 1.0f; // air quality (TODO)
	senses[10] = 0.0f; // steepness of upcoming slope (up) (TODO)
	senses[11] = 0.0f; // steepness of upcoming slope (down) (TODO)
	// space for old C2 senses: oncoming wind, wind from behind

	tickBrain();
	tickBiochemistry();

	// lifestage checks
	for (unsigned int i = 0; i < 7; i++) {
		if ((lifestageloci[i] != 0.0f) && (stage == (lifestage)i))
			ageCreature();
	}

	if (dead != 0.0f) die();
	
	Creature::tick();
}

void oldCreature::addGene(gene *g) {
	Creature::addGene(g);
	if (typeid(*g) == typeid(bioInitialConcentrationGene)) {
		// initialise chemical levels
		bioInitialConcentrationGene *b = (bioInitialConcentrationGene *)(g);
		chemicals[b->chemical] = b->quantity;
	} else if (typeid(*g) == typeid(bioHalfLivesGene)) {
		bioHalfLivesGene *d = dynamic_cast<bioHalfLivesGene *>(g);
		assert(d);
		halflives = d;
	}	
}

void c1Creature::addGene(gene *g) {
	oldCreature::addGene(g);

	if (typeid(*g) == typeid(bioReactionGene)) {
		reactions.push_back(shared_ptr<c1Reaction>(new c1Reaction()));
		reactions.back()->init((bioReactionGene *)(g));
	} else if (typeid(*g) == typeid(bioEmitterGene)) {
		emitters.push_back(c1Emitter());
		emitters.back().init((bioEmitterGene *)(g), this);
	} else if (typeid(*g) == typeid(bioReceptorGene)) {
		receptors.push_back(c1Receptor());
		receptors.back().init((bioReceptorGene *)(g), this);
	}
}

void c2Creature::addGene(gene *g) {
	oldCreature::addGene(g);

	if (typeid(*g) == typeid(organGene)) {
		// create organ
		organGene *o = dynamic_cast<organGene *>(g);
		assert(o);
		if (!o->isBrain()) { // TODO: handle brain organ
			organs.push_back(shared_ptr<c2Organ>(new c2Organ(this, o)));
		}
	}
}

void c2eCreature::addGene(gene *g) {
	Creature::addGene(g);

	if (typeid(*g) == typeid(bioInitialConcentrationGene)) {
		// initialise chemical levels
		bioInitialConcentrationGene *b = (bioInitialConcentrationGene *)(g);
		chemicals[b->chemical] = b->quantity / 255.0f; // TODO: correctness unchecked
	} else if (typeid(*g) == typeid(organGene)) {
		// create organ
		organGene *o = dynamic_cast<organGene *>(g);
		assert(o);
		if (!o->isBrain()) { // TODO: handle brain organ
			organs.push_back(shared_ptr<c2eOrgan>(new c2eOrgan(this, o)));
		}
	} else if (typeid(*g) == typeid(bioHalfLivesGene)) {
		bioHalfLivesGene *d = dynamic_cast<bioHalfLivesGene *>(g);
		assert(d);
		halflives = d;
	}
}

void c2eCreature::adjustDrive(unsigned int id, float value) {
	assert(id < 20);
	drives[id] += value;

	if (drives[id] < 0.0f) drives[id] = 0.0f;
	else if (drives[id] > 1.0f) drives[id] = 1.0f;
}

void c2eCreature::consoleOutput() {
	if (!catalogue.hasTag("chemical_names"))
		throw creaturesException("c2eCreature was unable to read the 'chemical_names' catalogue tag");
	const std::vector<std::string> &mappinginfotag = catalogue.getTag("chemical_names");

	printf("Chemical levels\n");

	int k = 0;
	for (std::vector<std::string>::const_iterator i = mappinginfotag.begin(); i != mappinginfotag.end(); i++) {
		printf("%.3f (%s)\n", chemicals[k], i->c_str());
		if(k > 256) break; k++;
	}
	printf("\n");
}

void c2Creature::consoleOutput() {}

void c1Creature::consoleOutput() {}


void c2eCreature::drawNornChemicalsWindow(WINDOW * win) {
	if (!catalogue.hasTag("chemical_names"))
		throw creaturesException("c2eCreature was unable to read the 'chemical_names' catalogue tag");
	const std::vector<std::string> &mappinginfotag = catalogue.getTag("chemical_names");


	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3, 2, "Chemical levels:");
	wattroff(win, A_BOLD | A_UNDERLINE);

	int k = -1; const int wrap = 35, spac = 25;
	for (std::vector<std::string>::const_iterator i = mappinginfotag.begin(); i != mappinginfotag.end(); i++) {
		if(k > 256 || k < 0 || 2 + (k/wrap)*spac > COLS) {
			k++;
			continue;
		}

		mvwprintw(win, 5 + k % wrap, 2 + (k/wrap)*spac,
				"%3d %s", int(round(100*getChemical(k+1))), i->c_str());

		k++;
	}
}

void c2eCreature::drawNornEmitterWindow(WINDOW * win, int &pos) {
	if (!catalogue.hasTag("chemical_names"))
		throw creaturesException("c2eCreature was unable to read the 'chemical_names' catalogue tag");
	const std::vector<std::string> &mappinginfotag = catalogue.getTag("chemical_names");

	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3, 2, "Emitters:");
	wattroff(win, A_BOLD | A_UNDERLINE);

	int organs = noOrgans();
	const int wrap = 30, spac = 25;

	int emitterCount = 0;
	for(int i = 0; i < organs; i++) {
		emitterCount += getOrgan(i)->getEmitterCount();
	}

	if(pos >= emitterCount - wrap) pos = emitterCount - wrap;
	if(pos < 0) pos = 0;

	int k = 0;
	for(int i = 0; i < organs; i++) {
		boost::shared_ptr<c2eOrgan>  organ = getOrgan(i);
		int emitters = organ->getEmitterCount();
		for(int j = 0; j < emitters; j++) {
			if(k > 256 || k < 0 || ((k - pos)/wrap)*spac >= 1 || k - int(pos) < 0) {
				k++;
				continue;
			}

			assert(organ->getEmitter(j).data);
			assert(organ->getEmitter(j).locus);
			assert((*organ->getEmitter(j).data).chemical < mappinginfotag.size() );

			mvwprintw(win, 5 + (k-pos) % wrap, 2 + ((k-pos)/wrap)*spac,
				"%25s O%02d (%3d %2d-%2d-%2d %3f)",
				mappinginfotag.at(int((*organ->getEmitter(j).data).chemical)).c_str(),
				i,
				(*organ->getEmitter(j).data).threshold,
				(*organ->getEmitter(j).data).organ,
				(*organ->getEmitter(j).data).tissue,
				(*organ->getEmitter(j).data).locus,
				*organ->getEmitter(j).locus);

			k++;
		}
	}
}

void c2eCreature::drawNornReceptorWindow(WINDOW * win, int &pos) {
	if (!catalogue.hasTag("chemical_names"))
		throw creaturesException("c2eCreature was unable to read the 'chemical_names' catalogue tag");
	const std::vector<std::string> &mappinginfotag = catalogue.getTag("chemical_names");

	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3, 2, "Receptors:");
	wattroff(win, A_BOLD | A_UNDERLINE);

	int organs = noOrgans();
	const int wrap = 30, spac = 25;

	int receptorCount = 0;
	for(int i = 0; i < organs; i++) {
		receptorCount += getOrgan(i)->getReceptorCount();
	}

	if(pos >= receptorCount - wrap) pos = receptorCount - wrap;
	if(pos < 0) pos = 0;

	int k = 0;
	for(int i = 0; i < organs; i++) {
		boost::shared_ptr<c2eOrgan>  organ = getOrgan(i);
		int receptors = organ->getReceptorCount();
		for(int j = 0; j < receptors; j++) {
			if(k > 256 || k < 0 || ((k - pos)/wrap)*spac >= 1 || k - int(pos) < 0) {
				k++;
				continue;
			}

			assert(organ->getReceptor(j).data);
			assert(organ->getReceptor(j).locus);
			assert((*organ->getReceptor(j).data).chemical < mappinginfotag.size() );

			mvwprintw(win, 5 + (k-pos) % wrap, 2 + ((k-pos)/wrap)*spac,
				"%25s O%02d %3d %25s %3f",
				mappinginfotag.at(int((*organ->getReceptor(j).data).chemical)).c_str(),
				i,
				(*organ->getReceptor(j).data).threshold,
				organ->getLocusDescription(true,
				(*organ->getReceptor(j).data).organ,
				(*organ->getReceptor(j).data).tissue,
				(*organ->getReceptor(j).data).locus).c_str(),
				*organ->getReceptor(j).locus);

			k++;
		}
	}
}

void c2eCreature::drawNornReactionWindow(WINDOW * win, int &pos) {
	if (!catalogue.hasTag("chemical_names"))
		throw creaturesException("c2eCreature was unable to read the 'chemical_names' catalogue tag");
	const std::vector<std::string> &mappinginfotag = catalogue.getTag("chemical_names");

	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3, 2, "Reactions:");
	wattroff(win, A_BOLD | A_UNDERLINE);

	int organs = noOrgans();
	const int wrap = 30, spac = 25;

	int reactionsCount = 0;
	for(int i = 0; i < organs; i++) {
		reactionsCount += getOrgan(i)->getReactionCount();
	}

	if(pos >= reactionsCount - wrap) pos = reactionsCount - wrap;
	if(pos < 0) pos = 0;

	int k = 0;
	for(int i = 0; i < organs; i++) {
		boost::shared_ptr<c2eOrgan>  organ = getOrgan(i);
		int reactions = organ->getReactionCount();
		for(int j = 0; j < reactions; j++) {
			if(k > 256 || k < 0 || ((k - pos)/wrap)*spac >= 1 || k - int(pos) < 0) {
				k++;
				continue;
			}

			assert(organ->getReaction(j).get()->data);
			assert((*organ->getReaction(j).get()->data).reactant[0] < mappinginfotag.size() );
			assert((*organ->getReaction(j).get()->data).reactant[1] < mappinginfotag.size() );
			assert((*organ->getReaction(j).get()->data).reactant[2] < mappinginfotag.size() );
			assert((*organ->getReaction(j).get()->data).reactant[3] < mappinginfotag.size() );

			mvwprintw(win, 5 + (k-pos) % wrap, 2 + ((k-pos)/wrap)*spac,
				"%d %s + %d %s -> %d %s + %d %s",
				(*organ->getReaction(j).get()->data).quantity[0],
				mappinginfotag.at(int((*organ->getReaction(j)->data).reactant[0])).c_str(),
				(*organ->getReaction(j).get()->data).quantity[1],
				mappinginfotag.at(int((*organ->getReaction(j)->data).reactant[1])).c_str(),
				(*organ->getReaction(j).get()->data).quantity[2],
				mappinginfotag.at(int((*organ->getReaction(j)->data).reactant[2])).c_str(),
				(*organ->getReaction(j).get()->data).quantity[3],
				mappinginfotag.at(int((*organ->getReaction(j)->data).reactant[3])).c_str() );
			wprintw(win, "                                ");

			k++;
		}
	}
	box(win, 0, 0);
}

void c2eCreature::drawNornDrivesWindow(WINDOW * win, int &pos) {
	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3, 2, "Drives:");
	wattroff(win, A_BOLD | A_UNDERLINE);

	const int wrap = 33, spac = 25;

	for(int i = 0; i < 20; i++) {
		mvwprintw(win, 5 + i % wrap, 2,
						"Drive %2d: %.3f", i, drives[i] );
	}

	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3, spac+2, "Senses:");
	wattroff(win, A_BOLD | A_UNDERLINE);

	for(int i = 0; i < 14; i++) {
		mvwprintw(win, 5 + i % wrap, spac+2,
						"Sense %2d: %.3f", i, senses[i] );
	}

	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3+20, spac+2, "Gait Loci:");
	wattroff(win, A_BOLD | A_UNDERLINE);

	for(int i = 0; i < 16; i++) {
		mvwprintw(win, 5 + 20 + i % wrap, spac+2,
						"Gait %2d: %.3f", i, gaitloci[i] );
	}

	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3, 2*spac+2, "Floating loci:");
	wattroff(win, A_BOLD | A_UNDERLINE);

	for(int i = 0; i < 32; i++) {
		mvwprintw(win, 5 + i % wrap, 2*spac+2,
						"Fl. loc. %2d: %.3f", i, floatingloci[i] );
	}
	box(win, 0, 0);
}

void logdraw(float varf, int ch, int neuronx, int neurony, WINDOW * win, int colpair) {
	// if below threshold, don't draw
	int var;

	if(varf == 0.0f)
		var = 0;
	else
		var = log(varf)/log(2) + 10;

	if(var > 10) {
		wattron(win, COLOR_PAIR(colpair)); wattron(win, A_BOLD | A_BLINK);
		mvwprintw(win, neurony, neuronx, "%c", toupper(ch));
		wattron(win, COLOR_PAIR(1)); wattroff(win, A_BOLD| A_BLINK);
	} else

	switch(var) {
	case 10:
	case 9:
		wattron(win, COLOR_PAIR(colpair)); wattron(win, A_BOLD);
		mvwprintw(win, neurony, neuronx, "%c", toupper(ch));
		wattron(win, COLOR_PAIR(1)); wattroff(win, A_BOLD);
		break;
	case 8:
	case 7:
	case 6:
		wattron(win, COLOR_PAIR(colpair));
		mvwprintw(win, neurony, neuronx, "%c", toupper(ch));
		wattron(win, COLOR_PAIR(1));
		break;
	case 5:
	case 4:
	case 3:
		wattron(win, COLOR_PAIR(colpair));
		mvwprintw(win, neurony, neuronx, "%c", tolower(ch));
		wattron(win, COLOR_PAIR(1));
	case 2:
	case 1:
		mvwprintw(win, neurony, neuronx, "%c", tolower(ch));
	case 0:
	default:
		break;
	}
}

// http://www.gamewaredevelopment.com/cdn/CDN_more.php?CDN_article_id=108
void c2eCreature::drawNornBrainWindow(WINDOW *win, int &posx, int &posy, int &posu, int &posv) {

	assert(win);

	if(posx >= int(getBrain()->lobes.size())) posx = getBrain()->lobes.size() - 1;
	if(posx < 0) posx = 0;
	int lobeNumber = posx;
	std::string lobeName = "";

	if(posy >= 8) posy = 7;
	if(posy < 0) posy = 0;
	int neuron_var = posy;
	int dendrite_var = posy;
	float threshold = 0.001;

	std::map<c2eNeuron *, std::pair<unsigned int, unsigned int> > neuroncoords;
	c2eBrain * b = getBrain();

	int HighlightedNeuronY = 0, HighlightedNeuronX = 0;

	// draw lobes/neurons
	int lobeno = -1;
	for (std::map<std::string, c2eLobe *>::iterator i = b->lobes.begin(); i != b->lobes.end(); i++) {
		lobeName = i->second->getId();
		lobeno++;

		c2eBrainLobeGene *lobe = i->second->getGene();

		if(lobeno == lobeNumber) {
			wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
			mvwprintw(win, 2, 4, "%s lobe", i->second->getId().c_str());
			wattroff(win, A_BOLD | A_UNDERLINE);
		}

		for (int y = 0; y < lobe->height; y++) {
			for (int x = 0; x < lobe->width; x++) {
				unsigned int neuronid = x + (y * lobe->width);
				c2eNeuron *neuron = i->second->getNeuron(neuronid);

				wattron(win, COLOR_PAIR(1));

				int neuronx = 4+x + lobe->x, neurony = 4+y + lobe->y;
				// it doesn't fit on "screen" like this, refit
				if(neurony >= 40) {
					neuronx = neuronx + 100;
					neurony = neurony - 40;
				}

				// store the centre coordinate for drawing dendrites
				assert(neuroncoords.find(neuron) == neuroncoords.end());
				neuroncoords[neuron] = std::pair<unsigned int, unsigned int>(neuronx, neurony);

				mvwprintw(win, neurony, neuronx, ".");

				// always highlight spare neuron
				if (i->second->getSpareNeuron() == neuronid) {
					// TODO: don't hardcode these names?
					if (i->second->getId() == "attn" || i->second->getId() == "decn" || i->second->getId() == "comb") {

						wattron(win, COLOR_PAIR(5));
					}
				}

				mvwprintw(win, neurony, neuronx, ".");
				wattron(win, COLOR_PAIR(1));

				logdraw(neuron->variables[neuron_var], 'x', neuronx, neurony, win, 2);

				if(lobeno != lobeNumber)
					continue;


				if(posu >= int(lobe->width)) posu = lobe->width - 1;
				if(posu < 0) posu = 0;

				if(posv >= int(lobe->height)) posv = lobe->height - 1;
				if(posv < 0) posv = 0;

				// highlight the selected neuron
				if(posu == x && posv == y) {
					HighlightedNeuronX = neuronx;
					HighlightedNeuronY = neurony;

					wattron(win, COLOR_PAIR(3));
					mvwprintw(win, neurony, neuronx, "X");
					wattron(win, COLOR_PAIR(1));
					continue;
				}

			}
		}
	}

	// draw dendrites
	for (std::vector<c2eTract *>::iterator i = b->tracts.begin(); i != b->tracts.end(); i++) {
		c2eBrainTractGene *tract = (*i)->getGene();
		std::string destlobename = std::string((char *)tract->destlobe, 4);
		if (b->lobes.find(destlobename) == b->lobes.end())
			continue; // can't find the source lobe, so whu? must be a bad tract
		c2eBrainLobeGene *destlobe = b->lobes[destlobename]->getGene();

		for (unsigned int j = 0; j < (*i)->getNoDendrites(); j++) {
			c2eDendrite *dend = (*i)->getDendrite(j);

			bool highlight = true;
			float var = dend->variables[dendrite_var];
			if (threshold == 0.0f) {
				if (var == threshold) highlight = false;
			} else {
				if (var <= threshold) highlight = false;
			}

			assert(neuroncoords.find(dend->source) != neuroncoords.end());
			assert(neuroncoords.find(dend->dest) != neuroncoords.end());

			// highlight source neurons
			if(int(neuroncoords[dend->dest].first) == HighlightedNeuronX &&
					int(neuroncoords[dend->dest].second) == HighlightedNeuronY) {
				if(highlight) wattron(win, COLOR_PAIR(4));
				mvwprintw(win, neuroncoords[dend->source].second, neuroncoords[dend->source].first, "u");
				wattron(win, COLOR_PAIR(1));
			}

			// highlight dest neurons
			if(int(neuroncoords[dend->source].first) == HighlightedNeuronX &&
					int(neuroncoords[dend->source].second) == HighlightedNeuronY) {
				if(highlight) wattron(win, COLOR_PAIR(4));
				mvwprintw(win, neuroncoords[dend->dest].second, neuroncoords[dend->dest].first, "o");
				wattron(win, COLOR_PAIR(1));
			}
		}
	}
}

/* vim: set noet: */
