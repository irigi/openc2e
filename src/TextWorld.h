/*
 * TextWorld.h
 *
 *  Created on: Mar 16, 2013
 *      Author: ad
 */

#ifndef TEXTWORLD_H_
#define TEXTWORLD_H_

#include <ncurses.h>

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


#endif /* TEXTWORLD_H_ */
