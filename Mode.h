#ifndef MODE_H
#define MODE_H

enum status {
ACTIVE,
ANNOTATING
};

class Mode {

public:

	void init();
	void update();
	void render();
	void processInput();

};


#endif