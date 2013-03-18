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

#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <ctype.h>



bool curses_started = false;

void text_world() {
	textWindow * console = new textWindow();
	console->initColors();

/*
	for (int n = 1; n <= 13; n++) {
	    color_set(n, NULL);
	    mvaddstr(3, 3+n, "X");
	    //addch('X');
	}

    refresh();
  */

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

	delete console;
}

textWindow::textWindow() {
	/* Init curses */
    if ( (mainwin = initscr()) == NULL ) {
        perror("error initialising ncurses");
        exit(EXIT_FAILURE);
    }

    noecho();
    keypad(mainwin, TRUE);
    oldcur = curs_set(0);
}

textWindow::~textWindow() {
    delwin(mainwin);
    endwin();
    refresh();
}

void textWindow::initColors() {
    start_color();

    if ( has_colors() && COLOR_PAIRS >= 13 ) {

    	int n = 1;


    	/*  Initialize a bunch of colour pairs, where:

	        init_pair(pair number, foreground, background);

	    	specifies the pair.                                  */

    	init_pair(1,  COLOR_RED,     COLOR_BLACK);
    	init_pair(2,  COLOR_GREEN,   COLOR_BLACK);
    	init_pair(3,  COLOR_YELLOW,  COLOR_BLACK);
    	init_pair(4,  COLOR_BLUE,    COLOR_BLACK);
    	init_pair(5,  COLOR_MAGENTA, COLOR_BLACK);
    	init_pair(6,  COLOR_CYAN,    COLOR_BLACK);
    	init_pair(7,  COLOR_BLUE,    COLOR_WHITE);
    	init_pair(8,  COLOR_WHITE,   COLOR_RED);
    	init_pair(9,  COLOR_BLACK,   COLOR_GREEN);
    	init_pair(10, COLOR_BLUE,    COLOR_YELLOW);
    	init_pair(11, COLOR_WHITE,   COLOR_BLUE);
    	init_pair(12, COLOR_WHITE,   COLOR_MAGENTA);
    	init_pair(13, COLOR_BLACK,   COLOR_CYAN);

    }
}

/*  Struct to hold keycode/keyname information  */

struct keydesc {
    int  code;
    char name[20];
};


/*  Returns a string describing a character passed to it  */

char * textWindow::intprtkey(int ch) {

    /*  Define a selection of keys we will handle  */

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

	/*  If a printable character  */

	keych[0] = ch;
	return keych;
    }
    else {

	/*  Non-printable, so loop through our array of structs  */

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


const std::string textWorld::gametype = "c3";
