/*
 * TextWorld.h
 *
 *  Created on: Mar 16, 2013
 *      Author: ad
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

void startCurses();
void endCurses();
void text_world();

// this should really be a singleton class, but I don't know how to do that now
// so I'm keeping it like this
class textWindow {
public:
	textWindow();
	~textWindow();
	void initColors();
	static char * intprtkey(int ch);

private:
	WINDOW * mainwin;
	int oldcur;

};

// the whole universe
class textWorld {
public:
	bool quitting, saving, paused;
	std::list<boost::shared_ptr<IrigiAgent> > agents;

	static const std::string gametype;
private:

};

extern textWorld textworld;

#endif /* TEXTWORLD_H_ */
