#pragma once

#include "SpriteType.h"

class SpriteListType
{
private:
	static const int LIST_GROWTH_AMOUNT = 6;

	int listCount;
	int listCapacity;
	SpriteType* spriteList;

	void GrowList(); // Grows list by growth amount

public:
	int GetSpriteCount() const { return listCount; } // Returns number of sprites in the list
	void SetCapacity(int capacity); // Set the capacity of the list
	int GetCapacity() const { return listCapacity; } // Returns current capacity of the list

	// Return a REFERENCE to a sprite
	SpriteType& GetSprite(int spriteIndex) { return spriteList[spriteIndex]; }

	void Add(SpriteType sprite); // Add sprite to list
	void Remove(int index); // Remove sprite from list
	void RemoveAll(); // Reset the list

	SpriteListType(); // Constructor
};
