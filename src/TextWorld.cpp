/*
 * TextWorld.cpp
 *
 *  Created on: Mar 16, 2013
 *      Author: irigi
 */


#include <stdio.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include "TextWorld.h"
#include "TextDraw.h"

#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <ctype.h>


void textWorld::tests() {
	int ch = 0, x = 5, y = 5;
    while ( (ch = getch()) != 'q' ) {

    	/*  Delete the old response line, and print a new one  */

    	//deleteln();
    	//mvprintw(7, 10, "You pressed: 0x%x (%s)", ch, textWindow::intprtkey(ch));

    	mvprintw(x, y, " ");

    	if(ch == KEY_LEFT)
    		y--;
    	if(ch == KEY_RIGHT)
    	    y++;
    	if(ch == KEY_UP)
    	    x--;
    	if(ch == KEY_DOWN)
    	    x++;

    	mvprintw(x, y, "X");
    	refresh();
    }

}

textWorld::textWorld() {
	console = new textWindow();
	console->initColors();
}

textWorld::~textWorld() {
	delete console;
}

const std::string textWorld::gametype = "c3";

textWorld textworld;
