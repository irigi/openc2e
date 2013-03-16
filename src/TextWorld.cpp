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



bool curses_started = false;

void text_world() {
	startCurses();
}

void endCurses() {
   if (curses_started && !isendwin())
      endwin();
}


void startCurses() {
   if (curses_started) {
      refresh();
   }
   else {
      initscr();
      cbreak();
      noecho();
      intrflush(stdscr, false);
      keypad(stdscr, true);
      atexit(endCurses);
      curses_started = true;
   }
}
