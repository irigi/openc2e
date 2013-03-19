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


// the whole universe
class textWorld {
public:
	textWorld();
	~textWorld();

	shared_ptr<genomeFile> loadGenome(std::string &genefile);
	void newMoniker(shared_ptr<genomeFile> g, std::string genefile, AgentRef agent);
	std::string generateMoniker(std::string basename);
	std::vector<std::string> findFiles(std::string dir, std::string wild);
	void executeBootstrap(bool switcher);
	void executeBootstrap(fs::path p);

	void makeNewEgg();
	void newNorn();

	void tests();

	bool quitting, saving, paused;
	std::list<boost::shared_ptr<IrigiAgent> > agents;

	textWindow * console;

	int timeofday, dayofseason, season, year;
	std::vector<boost::filesystem::path> data_directories;

	static const std::string gametype;
private:

};

extern textWorld textworld;

#endif /* TEXTWORLD_H_ */
