/*
 * TextDraw.h
 *
 *  Created on: Mar 18, 2013
 *      Author: irigi
 */

#ifndef TEXTDRAW_H_
#define TEXTDRAW_H_

#include <ncurses.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <istream>
#include <boost/filesystem/path.hpp>
#include <list>
#include "exceptions.h"

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

#endif /* TEXTDRAW_H_ */
