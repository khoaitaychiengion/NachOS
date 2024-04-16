#include "stable.h"

STable::STable()
{	
	this->bm = new BitMap(MAX_SEMAPHORE);
	
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{
		this->semTab[i] = NULL;
	}
}

STable::~STable()
{
	if(this->bm)
	{
		delete this->bm;
		this->bm = NULL;
	}
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if(this->semTab[i])
		{
			delete this->semTab[i];
			this->semTab[i] = NULL;
		}
	}
	
}

int STable::Create(char *name, int init)
{

	// Check whether semaphore survived
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if(bm->Test(i))
		{
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				return -1;
			}
		}
	}

	// Find free slot in semaphore table
	int id = this->FindFreeSlot();
	
	if (id < 0)
	{
		return -1;
	}
    else {
        // Create new semaphore and add to semaphore table
        this->semTab[id] = new Sem(name, init);
    }
    return 0;
}


int STable::FindFreeSlot()
{
	return this->bm->Find();
}
