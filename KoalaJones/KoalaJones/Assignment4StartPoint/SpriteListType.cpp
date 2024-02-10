#include "SpriteListType.h"

//-----------------------------------------------
// Initialize member variables
SpriteListType::SpriteListType()
{
	listCount = 0;
	listCapacity = 0;
	spriteList = NULL;		// Dynamic array allocated for the sprites.  Address is stored in sprite list
}

//-----------------------------------------------/
// Set the capacity of the list and dynamically 
// allocated the required amount of memory.
void SpriteListType::SetCapacity(int capacity)
{
	listCount = capacity;
	spriteList = new SpriteType[capacity];		// Dynamic array allocated for the sprites.  Address is stored in sprite list
}

//-----------------------------------------------/
// Grows the list to accomodate additional sprites
void SpriteListType::GrowList()
{
	SpriteType* oldList = spriteList;	// create another pointer to point to the memory for the list

	spriteList = new SpriteType[listCapacity + LIST_GROWTH_AMOUNT];		// allocate more memory for the list, adding more elements to the array

	for (int i = 0; i < listCapacity; i++)		// copying the old list to the new list so we can delete the old list
	{
		spriteList[i] = oldList[i];				// copying the elements over one by one
	}

	delete[] oldList;							// deleting the old list, frees up the memory to be used by something else
	listCapacity += LIST_GROWTH_AMOUNT;			// increase the list capacity as we just increased it
}

//-----------------------------------------------/
// Adds a Sprite to the list
void SpriteListType::Add(SpriteType sprite)
{
	if (listCount == listCapacity)
	{
		GrowList();
	}

	spriteList[listCount] = sprite;
	listCount++;
}


//-----------------------------------------------/
// Removes sprite at index itemIndex from the list
void SpriteListType::Remove(int itemIndex)
{
	for (int i = itemIndex; i < listCount - 1; i++)	// removing item by copying over it
		spriteList[i] = spriteList[i + 1];

	listCount--;						// decrease the listCount
}

//-----------------------------------------------/
// Resets list
void SpriteListType::RemoveAll()
{
	listCount = 0;
	listCapacity = 0;
	spriteList = NULL;
}
