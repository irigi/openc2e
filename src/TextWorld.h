/*
 * TextWorld.h
 *
 *  Created on: Mar 16, 2013
 *      Author: irigi
 */

#ifndef TEXTWORLD_H_
#define TEXTWORLD_H_

#include <ncurses.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <istream>
#include <boost/filesystem/path.hpp>
#include <list>
#include "exceptions.h"
#include "creatures/Creature.h"

#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

#include "AgentRef.h"
#include "alloc_count.h"
#include <set>

using boost::shared_ptr;
namespace fs = boost::filesystem;

class IrigiAgent;
class textWindow;
class genomeFile;
class PhysicalCreature;

// one text field of the world
class textField {
public:
	textField();
	void initNeighbours(textField * n, textField *e, textField *w, textField *s, textField *u, textField *d);
	void initRiver();
	void initRockWall();
	void initWoodenWall();
	void initAir();
	void initGrass();

	int SymbolStyle() { return this->symbol_style; }
	int Symbol() { return this->symbol; }
	bool Passable() { return this->passable; }
	float AirQuality() { return air_quality; }
	textField *North() { return north ; }
	textField *East() { return east ; }
	textField *West() { return west ; }
	textField *South() { return south ; }
	textField *Up() { return up ; }
	textField *Down() { return down; }


private:
	std::vector<IrigiAgent> agents;
	textField * north, *east, *west, *south, *up, *down;
	bool passable;
	float air_quality;
	int symbol;
	int symbol_style;
};


// the whole universe
class textWorld {
public:
	textWorld(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ);
	~textWorld();

	shared_ptr<genomeFile> loadGenome(std::string &genefile);
	void newMoniker(shared_ptr<genomeFile> g, std::string genefile, AgentRef agent);
	std::string generateMoniker(std::string basename);
	std::vector<std::string> findFiles(std::string dir, std::string wild);
	void executeBootstrap(bool switcher);
	void executeBootstrap(fs::path p);
	void initCatalogue();

	void makeNewEgg();
	PhysicalCreature *newNorn();

	void drawWorld(WINDOW *win, int X, int Y, int Z);

	void tests();

	bool quitting, saving, paused;
	std::list<boost::shared_ptr<IrigiAgent> > agents;

	textWindow * console;

	int timeofday, dayofseason, season, year;
	std::vector<boost::filesystem::path> data_directories;

	static const std::string gametype;
private:
	textField **** world;
	int sizeX, sizeY, sizeZ;
};

extern textWorld *textworld;

#endif /* TEXTWORLD_H_ */
