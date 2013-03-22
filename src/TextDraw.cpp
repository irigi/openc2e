/*
 * TextDraw.cpp
 *
 *  Created on: Mar 18, 2013
 *      Author: irigi
 */


#include <stdio.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include "TextDraw.h"
#include "PathResolver.h"
#include "creatures/Creature.h"
#include "creatures/CreatureAgent.h"
#include "creatures/c2eCreature.h"
#include "Catalogue.h"

#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <ctype.h>

//bool curses_started = false;

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);

	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

textWindow::textWindow() {
	/* Init curses */
    if ( (mainwin = initscr()) == NULL) {
        perror("error initialising ncurses");
        exit(EXIT_FAILURE);
    }

    F1win = create_newwin(LINES,COLS,0,0);
    F2win = create_newwin(LINES,COLS,0,0);
    F3win1 = create_newwin(LINES,COLS,0,0);
    F3win2 = create_newwin(LINES,COLS/2,0,COLS/2);

    activewin = F1win;
    noecho();
    keypad(mainwin, TRUE);
    cbreak(); //raw(); /* ctrl-C is not interpreted etc */
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    oldcur = curs_set(0);
}

textWindow::~textWindow() {
    delwin(mainwin);
    delwin(F1win);
    delwin(F2win);
    delwin(F3win1);
    delwin(F3win2);
    endwin();
    refresh();
}

void textWindow::switchWin(int no) {
	switch(no) {
	case 1:
		activewin = F1win;
		touchwin(activewin);
		resetActiveWin();
		redrawwin(activewin);
		break;
	case 2:
		activewin = F2win;
		touchwin(activewin);
		resetActiveWin();
		redrawwin(activewin);
		break;
	case 3:
		activewin = F3win1;
		touchwin(activewin);
		resetActiveWin();
		redrawwin(activewin);
		break;
	case 4:
		activewin = F3win2;
		touchwin(activewin);
		resetActiveWin();
		redrawwin(activewin);
		break;
	}

}

WINDOW * textWindow::activeWin() {
	return activewin;
}

void textWindow::resetActiveWin() {
	for(int i = 0; i < this->activeWin()->_maxx; i++) {
		for(int j = 0; j < this->activeWin()->_maxy; j++) {
			mvwprintw(activeWin(), j, i, " ");
		}
	}

	box(this->activeWin(), 0 , 0);
	redrawwin(activewin);
}


void textWindow::initColors() {
    start_color();

    if ( has_colors() && COLOR_PAIRS >= 14 ) {

    	int n = 1;

    	init_pair(1,  COLOR_WHITE,   COLOR_BLACK);
    	init_pair(2,  COLOR_RED,     COLOR_BLACK);
    	init_pair(3,  COLOR_GREEN,   COLOR_BLACK);
    	init_pair(4,  COLOR_YELLOW,  COLOR_BLACK);
    	init_pair(5,  COLOR_BLUE,    COLOR_BLACK);
    	init_pair(6,  COLOR_MAGENTA, COLOR_BLACK);
    	init_pair(7,  COLOR_CYAN,    COLOR_BLACK);
    	init_pair(8,  COLOR_BLUE,    COLOR_WHITE);
    	init_pair(9,  COLOR_WHITE,   COLOR_RED);
    	init_pair(10, COLOR_BLACK,   COLOR_GREEN);
    	init_pair(11, COLOR_BLUE,    COLOR_YELLOW);
    	init_pair(12, COLOR_WHITE,   COLOR_BLUE);
    	init_pair(13, COLOR_WHITE,   COLOR_MAGENTA);
    	init_pair(14, COLOR_BLACK,   COLOR_CYAN);
    }
}

struct keydesc {
    int  code;
    char name[20];
};


char * textWindow::intprtkey(int ch) {

    static struct keydesc keys[] = {
    		{ KEY_UP,        "Up arrow"        },
			{ KEY_DOWN,      "Down arrow"      },
			{ KEY_LEFT,      "Left arrow"      },
			{ KEY_RIGHT,     "Right arrow"     },
			{ KEY_HOME,      "Home"            },
			{ KEY_END,       "End"             },
			{ KEY_BACKSPACE, "Backspace"       },
			{ KEY_IC,        "Insert"          },
			{ KEY_DC,        "Delete"          },
			{ KEY_NPAGE,     "Page down"       },
			{ KEY_PPAGE,     "Page up"         },
			{ KEY_F(1),      "Function key 1"  },
			{ KEY_F(2),      "Function key 2"  },
			{ KEY_F(3),      "Function key 3"  },
			{ KEY_F(4),      "Function key 4"  },
			{ KEY_F(5),      "Function key 5"  },
			{ KEY_F(6),      "Function key 6"  },
			{ KEY_F(7),      "Function key 7"  },
			{ KEY_F(8),      "Function key 8"  },
			{ KEY_F(9),      "Function key 9"  },
			{ KEY_F(10),     "Function key 10" },
			{ KEY_F(11),     "Function key 11" },
			{ KEY_F(12),     "Function key 12" },
			{ -1,            "<unsupported>"   }
    };
    static char keych[2] = {0};

    if ( isprint(ch) && !(ch & KEY_CODE_YES)) {

	keych[0] = ch;
	return keych;
    }
    else {

	int n = 0;

	do {
	    if ( keys[n].code == ch )
		return keys[n].name;
	    n++;
	} while ( keys[n].code != -1 );

	return keys[n].name;
    }

    return NULL;        /*  We shouldn't get here  */
}

