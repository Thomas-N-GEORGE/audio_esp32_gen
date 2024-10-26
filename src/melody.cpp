#include "melody.h"

Melody::Melody(std::list<float> notes)
{
	this->notes = notes;
}

Melody::~Melody()
{
}

float Melody::rewind()
{
	iter = this->notes.begin();
	return *iter;
}

float Melody::getNextNote()
{
	if (++iter == this->notes.end())
		return this->rewind();
	return *iter;
}
