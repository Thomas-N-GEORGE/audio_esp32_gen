#include "melody.h"

Melody::Melody(std::list<float> notes)
{
	this->noteList = notes;
}

Melody::~Melody()
{
}

float Melody::rewind()
{
	iter = this->noteList.begin();
	return *iter;
}

float Melody::getNextNote()
{
	if (++iter == this->noteList.end())
		return this->rewind();
	return *iter;
}
