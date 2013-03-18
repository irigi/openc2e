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

class IrigiAgent;
class textWindow;


// the whole universe
class textWorld {
public:
	textWorld();
	~textWorld();

	void tests();

	bool quitting, saving, paused;
	std::list<boost::shared_ptr<IrigiAgent> > agents;

	textWindow * console;

	static const std::string gametype;
private:

};

extern textWorld textworld;

#endif /* TEXTWORLD_H_ */
