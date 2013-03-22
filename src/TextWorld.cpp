/*
 * TextWorld.cpp
 *
 *  Created on: Mar 16, 2013
 *      Author: irigi
 */


#include <stdio.h>
#include <math.h>
#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>
#include "TextWorld.h"
#include "TextDraw.h"

#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <ctype.h>
#include <fstream>
#include "PathResolver.h"
#include "creatures/Creature.h"
#include "creatures/CreatureAgent.h"
#include "creatures/c2eCreature.h"
#include "Catalogue.h"

#include <boost/format.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

#include <unistd.h>  /* only for sleep() */

int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}


void textWorld::tests() {
	//executeBootstrap(false);
	PhysicalCreature *norn = newNorn();
	assert(norn);

	norn->consoleOutput();

	for(int i = 0; i < 10; i++)
		norn->tick();

	norn->consoleOutput();

	wattron(console->activeWin(), COLOR_PAIR(2));

	int ch = 0, x = 5, y = 5;
	int state = 1;
	long int n = 0, game_speed = 10000;
    while ( true ) {
    	//mvprintw(y, x, " ");

    	n++;

    	if(kbhit()) {
    		ch = getch();

    		if(ch == 'q')
    		    break;

    		switch(ch) {
    		case KEY_LEFT:
    				x--;
    				break;
    		case KEY_RIGHT:
    				x++;
    				break;
    		case KEY_UP:
    				y--;
    				break;
    		case KEY_DOWN:
    				y++;
    				break;
    		case KEY_F(1):
    			console->switchWin(1);
    			state = 1;
    			break;
    		case KEY_F(2):
    			console->switchWin(2);
    			state = 2;
    			break;
    		case KEY_F(3):
				console->switchWin(4);
    			state = 3;
    			break;
    		case KEY_F(4):
				console->switchWin(4);
    			state = 4;
    			break;
    		case KEY_F(5):
				console->switchWin(4);
    			state = 5;
    			break;
    		case KEY_F(6):
				console->switchWin(4);
    			state = 6;
    			break;
    		case KEY_F(7):
				console->switchWin(4);
    			state = 7;
    			break;
    		case KEY_F(8):
				console->switchWin(4);
    			state = 6;
    			break;

    		case '-' :
    			game_speed = int(round(game_speed*1.1));
    			break;
    		case '+' :
    			game_speed = int(round(game_speed*0.95));
    			break;
    		default:
    			break;
    		}
    	}

    	if(state == 1) {
    		mvwprintw(console->activeWin(), y, x, "X");
    	}

    	if(n % game_speed == 0 && state == 2) {
    		norn->getCreature()->drawNornChemicalsWindow(console->activeWin());
    	}

    	if(n % game_speed == 0 && state == 3) {
    		norn->getCreature()->drawNornEmitterWindow(console->activeWin(), y);
    	}
    	if(n % game_speed == 0 && state == 4) {
    		norn->getCreature()->drawNornReceptorWindow(console->activeWin(), y);
    	}
    	if(n % game_speed == 0 && state == 5) {
    		norn->getCreature()->drawNornReactionWindow(console->activeWin(), x);
    	}
    	if(n % game_speed == 0 && state == 6) {
    		norn->getCreature()->drawNornDrivesWindow(console->activeWin(), x);
    	}


    	if(n % game_speed == 0) {
    		 norn->tick();
    	}

    	wrefresh(console->activeWin());
    }

    //mvprintw(5,5,"%d\n", data_directories.size());
    //refresh();
    //getchar();

}

textWorld::textWorld() {
	console = new textWindow();
	console->initColors();

	// read directories of current path as working directories
	fs::path p(boost::filesystem::current_path());
	fs::directory_iterator fsend;
	for (fs::directory_iterator d(p); d != fsend; ++d) {
		if (fs::is_directory(*d))
			data_directories.push_back(*d);
	}

	std::sort(data_directories.begin(), data_directories.end());
}

textWorld::~textWorld() {
	delete console;
}

shared_ptr<genomeFile> textWorld::loadGenome(std::string &genefile) {
	std::vector<std::string> possibles = findFiles("/Genetics/", genefile + ".gen");
	if (possibles.empty()) return shared_ptr<genomeFile>();
	genefile = possibles[(int)((float)possibles.size() * (rand() / (RAND_MAX + 1.0)))];

	shared_ptr<genomeFile> p(new genomeFile());
	std::ifstream gfile(genefile.c_str(), std::ios::binary);
	assert(gfile.is_open());
	gfile >> std::noskipws;
	gfile >> *(p.get());

	return p;
}

// monikers are shortcuts of genome, human readable hash, basically
void textWorld::newMoniker(shared_ptr<genomeFile> g, std::string genefile, AgentRef agent) {
//	std::string d = history.newMoniker(g);
//	world.history.getMoniker(d).addEvent(2, "", genefile);
//	world.history.getMoniker(d).moveToAgent(agent);
}

std::string textWorld::generateMoniker(std::string basename) {
	// TODO: is there a better way to handle this? incoming basename is from catalogue files..
	if (basename.size() != 4) {
		std::cout << "World::generateMoniker got passed '" << basename << "' as a basename which isn't 4 characters, so ignoring it" << std::endl;
		basename = "xxxx";
	}

	std::string x = basename;
	for (unsigned int i = 0; i < 4; i++) {
		unsigned int n = (unsigned int) (0xfffff * (rand() / (RAND_MAX + 1.0)));
		x = x + "-" + boost::str(boost::format("%05x") % n);
	}

	return x;
}

std::vector<std::string> textWorld::findFiles(std::string dir, std::string wild) {
	std::vector<std::string> possibles;

	// Go backwards, so we find files in more 'modern' directories first..
	for (int i = data_directories.size() - 1; i != -1; i--) {
		fs::path p = data_directories[i];
		std::string r = (p / fs::path(dir)).string();
		std::vector<std::string> results = findByWildcard(r, wild);
		possibles.insert(possibles.end(), results.begin(), results.end()); // merge results
	}

	return possibles;
}

void textWorld::executeBootstrap(bool switcher) {
	// TODO: this code is possibly wrong with multiple bootstrap directories
	std::multimap<std::string, fs::path> bootstraps;

	for (std::vector<fs::path>::iterator i = data_directories.begin(); i != data_directories.end(); i++) {
		assert(fs::exists(*i));
		assert(fs::is_directory(*i));

		fs::path b(*i / "/Bootstrap/");
		if (fs::exists(b) && fs::is_directory(b)) {
			fs::directory_iterator fsend;
			// iterate through each bootstrap directory
			for (fs::directory_iterator d(b); d != fsend; ++d) {
				if (fs::exists(*d) && fs::is_directory(*d)) {
					std::string s = d->path().leaf();	// XXX: .string() removed
					// TODO: cvillage has switcher code in 'Startup', so i included it here too
					if (s == "000 Switcher" || s == "Startup") {
						if (!switcher) continue;
					} else {
						if (switcher) continue;
					}

					bootstraps.insert(std::pair<std::string, fs::path>(s, *d));
				}
			}
		}
	}

	for (std::multimap<std::string, fs::path>::iterator i = bootstraps.begin(); i != bootstraps.end(); i++) {
		executeBootstrap(i->second);
	}
}

void textWorld::executeBootstrap(fs::path p) {
	if (!fs::is_directory(p)) {
		//executeInitScript(p);
		return;
	}

	std::vector<fs::path> scripts;

	fs::directory_iterator fsend;
	for (fs::directory_iterator d(p); d != fsend; ++d) {
		if ((!fs::is_directory(*d)) && (fs::extension(*d) == ".cos"))
			scripts.push_back(*d);
	}

	std::sort(scripts.begin(), scripts.end());
	for (std::vector<fs::path>::iterator i = scripts.begin(); i != scripts.end(); i++)
		//executeInitScript(*i);
		;
}

void textWorld::initCatalogue() {
	assert(data_directories.size() > 0);
	for (std::vector<fs::path>::iterator i = data_directories.begin(); i != data_directories.end(); i++) {
		assert(fs::exists(*i));
		assert(fs::is_directory(*i));

		fs::path c(*i / "/Catalogue/");
		if (fs::exists(c) && fs::is_directory(c))
			catalogue.initFrom(c);
	}
}

PhysicalCreature* textWorld::newNorn() {
	std::string genomefile = "*";
	shared_ptr<genomeFile> genome;
	try {
		genome = textworld.loadGenome(genomefile);
	} catch (creaturesException &e) {
		fprintf(stderr, "Couldn't load genome file: %s", e.prettyPrint().c_str() );
		return NULL;
	}

	if (!genome) {
		return NULL;
	}

	PhysicalCreature *a = new PhysicalCreature();

	int sex = 1 + (int) (2.0 * (rand() / (RAND_MAX + 1.0)));
	c2eCreature *c;
	try {
		c = new c2eCreature(genome, (sex == 2), 0, a);
	} catch (creaturesException &e) {
		delete a;
		fprintf(stderr, "Couldn't create creature: %s\n", e.prettyPrint().c_str());
		return NULL;
	}

	a->setCreature(c);
	a->finishInit();

	// if you make this work for c2e, you should probably set sane attributes here?

	a->genome_slots[0] = genome;
	//textworld.newMoniker(genome, genomefile, a);
	//world.history.getMoniker(world.history.findMoniker(genome)).moveToCreature(a);

	// TODO: set it dreaming

	c->born();

	return a;
}

void textWorld::makeNewEgg() {
	std::string eggscript;
	/* create the egg obj */
	eggscript = boost::str(boost::format("new: simp eggs 8 %d 2000 0\n") % ((rand() % 6) * 8));
	/* set the pose */
	eggscript += "pose 3\n";
	/* set the correct class/attributes */

	/* set the gender */
	eggscript += "setv obv1 0\n";
	/* start the clock */
	eggscript += "tick 2400\n";

	//std::string err = engine.executeNetwork(eggscript);
	//if (err.size())
	//	QMessageBox::warning(this, tr("Couldn't create egg:"), err.c_str());
}

const std::string textWorld::gametype = "c3";

textWorld textworld;
