#ifndef _DEF_MELODY
#define DEF_MELODY

#include <list>

class Melody
{
private:
	std::list<float> noteList;
	std::list<float>::iterator iter;

public:
	Melody(std::list<float> notes);
	~Melody();

	float rewind();
	float getNextNote();
};

#endif