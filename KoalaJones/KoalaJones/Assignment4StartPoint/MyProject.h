#pragma once

//----------------------------------------------------------------------------------------------
//	Main project class
//	Creates a "MyProject" class which inherits the Directx class to help us initalize DirectX
//  Since we inherit the "DirectXClass" publicly the "MyProject" class has access to all the 
//	"DirectXClass" functionality.
//
//	Now we can add addition code to this class to build on what is provided by the "DirectXClass"
//----------------------------------------------------------------------------------------------
#pragma once

#include <Windowsx.h>
#include <SpriteBatch.h>
#include "DirectX.h"
#include "TextureType.h"
#include "SpriteType.h"
#include "SpriteListType.h"

class MyProject : public DirectXClass
{
	public:
		// constructor
		MyProject(HINSTANCE hInstance);		// Constructor: required to initialize the base class, DirectXClass.

											// Virtual function from DirectX class which we are implementing here
		LRESULT ProcessWindowMessages(UINT msg, WPARAM wParam, LPARAM lParam);		// window message handler
		void Render(void);				// Called by the render loop to render a single frame
		void Update(float deltaTime);	// Called by DirectX framework to allow you to update any scene objects
		
		void InitalizeTextures();
		void InitalizeSprites();

		void DisplayUI(); // Display score, lives, time, obstacle list capacity and sprite count
		void GameOver(); // Display final score and time
		void Reset(); // Return everything to starting values

		// Functions to add obstacles and items to sprite lists
		void AddRocks();
		void AddFire();
		void AddDart();
		void AddSnake();
		void AddItems();
		Vector2 ItemPos(); // Get random position for item

		void Move(); // Player movement
		void CheckForCollisions(); // Player and obstacle/item collision check

		void UpdateLevel(float deltaTime); // Update difficulty/item level
		void UpdateObstacles(); // Update positions of obstacles
		void AddObstacles(float deltaTime); // Add new obstacles to scene
		void RemoveObstacles(); // Remove obstacles from scene

		void setScore(int inScore) { score = inScore; }
		int getScore() const { return score; }
		int getLives() const { return lives; }

	private:
		static enum eGameStates {START, PLAYING, OVER};		// Game State enumerated type

		// sprite batch 
		DirectX::SpriteBatch* spriteBatch;

		// mouse variables
		Vector2 mousePos;				// mouse position
		bool buttonDownLeft = false;	// whether button is down or not

		// Textures Variables
		TextureType startTex; // Starting screen
		TextureType backgroundTex; // Background for game
		TextureType lavaTex; // Foreground for game
		TextureType endTex; // Game over screen

		// Player
		TextureType koalaTex;

		// Obstacles
		TextureType rockTex;
		TextureType fireTex;
		TextureType dartTex;
		TextureType snakeTex;

		// Items
		TextureType orangeTex;
		TextureType pearTex;
		TextureType appleTex;
		TextureType bananaTex;
		TextureType chaliceTex;
		TextureType necklaceTex;
		TextureType tripowerTex;
		TextureType idolTex;

		// Sprites Variables
		SpriteType koalaSprite; // Player

		static enum obstacleType { ROCK, FIRE, DART, SNAKE}; // enum for which obstacle type to spawn

		// Lists for each obstacle type and for items
		SpriteListType rockSprites;
		SpriteListType fireSprites;
		SpriteListType dartSprites;
		SpriteListType snakeSprites;

		SpriteListType itemSprites;

		// Game Play Variables
		static const int VINE_COUNT = 6; // Amount of vines
		eGameStates currentState; // Game state to track the current state (start, play, over)

		int score; // Track score
		int lives; // Track lives
		float gracePeriod; // When hit, brief period of time where player is invulnerable
		float elapsedTime; // Timer to keep track of game's duration
		int vineX[VINE_COUNT]; // Array to store x positions of each vine
		int currentVine; // Tracks which vine the player is currently on
		Color koalaColor; // Color of player sprite (changes on damage)

		int itemCombo; // Score modifier based on how many items have been collected in a row
		int itemLevel; // Tracks which item to spawn next (if item is obtained, itemLevel++)
		float itemDespawn; // Countdown to despawn current item (5 seconds)
		int obstacleLevel; // Number to track current obstacle difficulty (which types will spawn in)
		float obstacleTime; // Countdown to next obstacle spawn
		int timeToNextObstacle; // The time for the obstacleTime countdown to start at.
		float obstacleSpeed; // Speed that obstacles move
		int timeToNextChange; // Seconds until difficulty increase/item spawn
		float gameOverTime; // Seconds until the player can play again (prevents skipping the game over screen accidentally if clicking rapidly)
		int scoreForExtraLife; // Score needed to obtain extra life
};

