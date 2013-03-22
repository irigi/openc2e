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

class Creature;

// this should really be a singleton class, but I don't know how to do that now
// so I'm keeping it like this
class textWindow {
public:
	textWindow();
	~textWindow();
	void initColors();
	static char * intprtkey(int ch);
	void switchWin(int no);
	WINDOW * activeWin();
	void resetActiveWin();

	void drawNornChemicalsWindow(WINDOW * win, Creature * norn);

private:
	WINDOW * mainwin;
	WINDOW * F1win;
	WINDOW * F2win;
	WINDOW * F3win1, * F3win2;
	WINDOW * activewin;
	int oldcur;

};

#endif /* TEXTDRAW_H_ */
