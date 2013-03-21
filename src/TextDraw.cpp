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
    F3win = create_newwin(LINES,COLS,0,0);

    activewin = F1win;
    noecho();
    keypad(mainwin, TRUE);
    //raw(); /* ctrl-C is not interpreted etc */
    oldcur = curs_set(0);
}

textWindow::~textWindow() {
    delwin(mainwin);
    delwin(F1win);
    delwin(F2win);
    delwin(F3win);
    endwin();
    refresh();
}

void textWindow::switchWin(int no) {
	switch(no) {
	case 1:
		activewin = F1win;
		touchwin(activewin);
		redrawwin(activewin);
		break;
	case 2:
		activewin = F2win;
		touchwin(activewin);
		redrawwin(activewin);
		break;
	case 3:
		activewin = F3win;
		touchwin(activewin);
		redrawwin(activewin);
		break;
	}

}

WINDOW * textWindow::activeWin() {
	return activewin;
}

void textWindow::drawNornChemicalsWindow(WINDOW * win, Creature * norn) {
	if (!catalogue.hasTag("chemical_names"))
		throw creaturesException("c2eCreature was unable to read the 'chemical_names' catalogue tag");
	const std::vector<std::string> &mappinginfotag = catalogue.getTag("chemical_names");


	wattron(win, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 3, 2, "Chemical levels: ");
	wattroff(win, A_BOLD | A_UNDERLINE);

	int k = -1; const int wrap = 35, spac = 25;
	for (std::vector<std::string>::const_iterator i = mappinginfotag.begin(); i != mappinginfotag.end(); i++) {
		if(k > 256 || k < 0 || 2 + (k/wrap)*spac > COLS) {
			k++;
			continue;
		}

		mvwprintw(win, 5 + k % wrap, 2 + (k/wrap)*spac,
				"%3d %s", int(round(100*norn->getFloatChemical(k+1))), i->c_str());

		k++;
	}
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

