/*

Author: Matthew Minnett
Date: 2022-03-31
Purpose:
1.	To create a unique simple point and click game utilizing multiple classes/objects
2.	To utilize a dynamic memory data structure class to control game objects
3.	To apply the pointer data type and address operators appropriately
4.	To integrate textures and sprites into a DirectX application

*/


#include <string>
#include <sstream>
#include "MyProject.h"

using namespace std;
using namespace DirectX;

float RandFloat() { return float(rand()) / float(RAND_MAX); }

//----------------------------------------------------------------------------------------------
// Constructor
MyProject::MyProject(HINSTANCE hInstance) : DirectXClass(hInstance)
{
	DisplayFPS(true);

	// Set everything to starting values
	currentState = eGameStates::START;

	// Obstacle settings
	obstacleLevel = 1; // Difficulty of obstacles
	obstacleSpeed = 1;
	timeToNextObstacle = 3;
	obstacleTime = 3;

	// Item settings
	itemCombo = 100; // Score gained from item starts at 100
	itemLevel = 1;
	itemDespawn = 5; // items despawn every 5 seconds

	// General play settings
	score = 0;
	lives = 2;
	elapsedTime = 0;
	gracePeriod = 0;
	gameOverTime = 1; // Player must stare at their defeat for at least 1 second
	timeToNextChange = 15; // How many seconds until difficulty increase/item spawn
	scoreForExtraLife = 10000; // Score needed for extra life starts at 10,000
	koalaColor = Color(1, 1, 1);

	// Setting x-pos for each vine
	vineX[0] = 131;
	vineX[1] = 283;
	vineX[2] = 435;
	vineX[3] = 588;
	vineX[4] = 740;
	vineX[5] = 893;
	currentVine = 2; // player starts on 3rd vine in array
}

//----------------------------------------------------------------------------------------------
//	Called by the game loop to render a single frame
void MyProject::Render(void)
{
	if (currentState == eGameStates::START)
	{
		// We are in the main menu
		startTex.Draw(DeviceContext, BackBuffer, 0, 0);
	}
	else if (currentState == eGameStates::PLAYING)
	{
		// We are playing our game

		// Draw Background First
		backgroundTex.Draw(DeviceContext, BackBuffer, 0, 0);

		// Render our sprites
		spriteBatch->Begin(SpriteSortMode_BackToFront, GetBlendState()->NonPremultiplied());
		{
			koalaSprite.Draw(spriteBatch);

			for (int i = 0; i < itemSprites.GetSpriteCount(); i++)
			{
				itemSprites.GetSprite(i).Draw(spriteBatch);
			}

			for (int i = 0; i < snakeSprites.GetSpriteCount(); i++)
			{
				snakeSprites.GetSprite(i).Draw(spriteBatch);
			}
			for (int i = 0; i < dartSprites.GetSpriteCount(); i++)
			{
				dartSprites.GetSprite(i).Draw(spriteBatch);
			}
			for (int i = 0; i < fireSprites.GetSpriteCount(); i++)
			{
				fireSprites.GetSprite(i).Draw(spriteBatch);
			}
			for (int i = 0; i < rockSprites.GetSpriteCount(); i++)
			{
				rockSprites.GetSprite(i).Draw(spriteBatch);
			}
		}
		spriteBatch->End();

		// Lava is in front of sprites
		lavaTex.Draw(DeviceContext, BackBuffer, 0, 768 - lavaTex.GetHeight());

		DisplayUI(); // UI displays above lava
	}
	else if (currentState == eGameStates::OVER)
	{
		// Game Over!
		GameOver();
	}
}

//----------------------------------------------------------------------------------------------
//	Called every frame to update objects.
//	deltaTime: how much time in seconds has elapsed since the last frame
void MyProject::Update(float deltaTime)
{
	if (currentState == eGameStates::PLAYING) // While we are PLAYING
	{
		CheckForCollisions(); // Every frame check to see if player has collided with obstacle/item

		elapsedTime += deltaTime; // Add to elapsed time
	
		if (score >= scoreForExtraLife)
		{
			lives += 1; // Add an extra life if score is equal to or greater than or equal to scoreForExtraLife
			scoreForExtraLife = scoreForExtraLife * 2.5; // scoreForExtraLife is multiplied by 2.5
		}

		if (gracePeriod > 0) // If the player is damaged, then gracePeriod will be greater than 0
		{
			gracePeriod -= deltaTime; // Grace period counts down to 0
			
			if (gracePeriod <= 0) 
			{
				koalaSprite.SetColor(Color(1, 1, 1)); // Set koala back to normal colour once grace period is over
			}
		}

		UpdateLevel(deltaTime); // Check to see if item level and obstacle level should be added to

		UpdateObstacles(); // Move obstacles

		AddObstacles(deltaTime); // Add new obstacles

		RemoveObstacles(); // Remove off-screen obstacles
	}
	else if (currentState == eGameStates::OVER)
	{
		gameOverTime -= deltaTime; // Game over time counts down
	}
}


//----------------------------------------------------------------------------------------------
//	Window message handler which intercepts messages from the Windows OS. The programmer can check
//	which message occurred and take appropriate actions
LRESULT MyProject::ProcessWindowMessages(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONUP:		// Left mouse button let up, set the boolean variable and get the mouse coordinates
		buttonDownLeft = false;
		mousePos.x = (float)GET_X_LPARAM(lParam);
		mousePos.y = (float)GET_Y_LPARAM(lParam);
		if (currentState == eGameStates::START)
		{
			currentState = eGameStates::PLAYING; // If left click on start screen, play game
		}
		else if (currentState == eGameStates::PLAYING)
		{
			Move(); // If left click in-game, move koala sprite
		}
		else if (currentState == eGameStates::OVER && gameOverTime <= 0)
		{
			Reset(); // If left click on game over, and game over timer is less than or equal to 0, reset
		}
		break;
	case WM_LBUTTONDOWN:	// Left mouse button down, set the boolean variable and get the mouse coordinates
		buttonDownLeft = true;
		mousePos.x = (float)GET_X_LPARAM(lParam);
		mousePos.y = (float)GET_Y_LPARAM(lParam);
		break;
	case WM_KEYUP:		// check for VK_???? and perform an action based on a specific keyboard key being let up
		if (wParam >= '0' && wParam <= '4')		// setting the screen refesh rate setting keys 0 - 4
			presentInterval = wParam - '0';
		break;
	case WM_KEYDOWN:
		break;
	}

	// Let the base class handle remaining messages, THIS IS REQUIRED as all messages should be handled appropriately
	return DirectXClass::ProcessWindowMessages(msg, wParam, lParam);
}

// -----------------------------------------------------------------------------
// Load all textures
void MyProject::InitalizeTextures()
{
	startTex.Load(D3DDevice, L"..\\Textures\\title.jpg");
	backgroundTex.Load(D3DDevice, L"..\\Textures\\background.jpg");
	lavaTex.Load(D3DDevice, L"..\\Textures\\lava.jpg");
	endTex.Load(D3DDevice, L"..\\Textures\\end.jpg");

	// Player and obstacles
	koalaTex.Load(D3DDevice, L"..\\Textures\\player\\koala_jones.png");
	rockTex.Load(D3DDevice, L"..\\Textures\\obstacles\\rock.png");
	fireTex.Load(D3DDevice, L"..\\Textures\\obstacles\\fireball.png");
	dartTex.Load(D3DDevice, L"..\\Textures\\obstacles\\poison_dart.png");
	snakeTex.Load(D3DDevice, L"..\\Textures\\obstacles\\snake.png");

	// Items
	orangeTex.Load(D3DDevice, L"..\\Textures\\items\\item1.png");
	pearTex.Load(D3DDevice, L"..\\Textures\\items\\item2.png");
	appleTex.Load(D3DDevice, L"..\\Textures\\items\\item3.png");
	bananaTex.Load(D3DDevice, L"..\\Textures\\items\\item4.png");
	chaliceTex.Load(D3DDevice, L"..\\Textures\\items\\item5.png");
	necklaceTex.Load(D3DDevice, L"..\\Textures\\items\\item6.png");
	tripowerTex.Load(D3DDevice, L"..\\Textures\\items\\item7.png");
	idolTex.Load(D3DDevice, L"..\\Textures\\items\\item8.png");
}

// -----------------------------------------------------------------------------
// Initalize all sprites, including obstacles and items for sprite lists
void MyProject::InitalizeSprites()
{
	spriteBatch = new DirectX::SpriteBatch(DeviceContext);

	koalaSprite.Initialize(&koalaTex, Vector2(vineX[currentVine], 768/2), 0, 0, 1, koalaColor, 0);
	koalaSprite.SetPivot(SpriteType::Pivot::CenterLeft);

	AddRocks();
	AddFire();
	AddDart();
	AddSnake();

	AddItems();
}

// -----------------------------------------------------------------------------
// Prints out elapsed time, current score, current lives, as well as list info
void MyProject::DisplayUI()
{
	wostringstream message;
	wstring messageOut;

	message << L"Time: " << elapsedTime;
	messageOut = message.str();
	font.PrintMessage(0, 700, messageOut.c_str(), FC_BLACK);

	message.str(L"");

	message << L"Score: " << score;
	messageOut = message.str();
	font.PrintMessage(0, 720, messageOut.c_str(), FC_BLACK);

	message.str(L"");

	message << L"Lives: " << lives;
	messageOut = message.str();
	font.PrintMessage(0, 740, messageOut.c_str(), FC_BLACK);

	if (obstacleLevel == 1) // When obstacle level is 1, print rock list sprite count and capacity
	{
		message.str(L"");

		message << L"Rocks: " << rockSprites.GetSpriteCount();
		messageOut = message.str();
		font.PrintMessage(200, 700, messageOut.c_str(), FC_BLACK);

		message.str(L"");

		message << L"Rock Capacity: " << rockSprites.GetCapacity();
		messageOut = message.str();
		font.PrintMessage(200, 720, messageOut.c_str(), FC_BLACK);
	}

	else if (obstacleLevel == 2) // When obstacle level is 2, print fire list sprite count and capacity
	{
		message.str(L"");

		message << L"FireBalls: " << fireSprites.GetSpriteCount();
		messageOut = message.str();
		font.PrintMessage(200, 700, messageOut.c_str(), FC_BLACK);

		message.str(L"");

		message << L"FireBall Capacity: " << fireSprites.GetCapacity();
		messageOut = message.str();
		font.PrintMessage(200, 720, messageOut.c_str(), FC_BLACK);
	}

	else if (obstacleLevel == 3) // When obstacle level is 3, print dart list sprite count and capacity
	{
		message.str(L"");

		message << L"PoisonDarts: " << dartSprites.GetSpriteCount();
		messageOut = message.str();
		font.PrintMessage(200, 700, messageOut.c_str(), FC_BLACK);

		message.str(L"");

		message << L"PoisonDart Capacity: " << dartSprites.GetCapacity();
		messageOut = message.str();
		font.PrintMessage(200, 720, messageOut.c_str(), FC_BLACK);
	}

	else if (obstacleLevel == 4) // When obstacle level is 4, print snake list sprite count and capacity
	{
		message.str(L"");

		message << L"Snakes: " << snakeSprites.GetSpriteCount();
		messageOut = message.str();
		font.PrintMessage(200, 700, messageOut.c_str(), FC_BLACK);

		message.str(L"");

		message << L"Snake Capacity: " << snakeSprites.GetCapacity();
		messageOut = message.str();
		font.PrintMessage(200, 720, messageOut.c_str(), FC_BLACK);
	}
}

// -----------------------------------------------------------------------------
// Displays game over screen, with elapsed time and final score
void MyProject::GameOver()
{
	endTex.Draw(DeviceContext, BackBuffer, 0, 0);

	wostringstream message;
	wstring messageOut;

	message << L"Time Survived: " << elapsedTime;
	messageOut = message.str();
	font.PrintMessage(400, 384, messageOut.c_str(), Color(1,1,1));

	message.str(L"");

	message << L"Final Score: " << score;
	messageOut = message.str();
	font.PrintMessage(400, 404, messageOut.c_str(), Color(1, 1, 1));
}

// -----------------------------------------------------------------------------
// Resets to starting values
void MyProject::Reset()
{
	// Set everything to starting values
	currentState = eGameStates::START;

	// Obstacle settings
	obstacleLevel = 1;
	obstacleSpeed = 1;
	timeToNextObstacle = 3;
	obstacleTime = 3;

	// Item settings
	itemCombo = 100;
	itemLevel = 1;
	itemDespawn = 5;

	// General play settings
	score = 0;
	lives = 2;
	elapsedTime = 0;
	gracePeriod = 0;
	gameOverTime = 1;
	timeToNextChange = 15;
	scoreForExtraLife = 10000;
	koalaColor = Color(1, 1, 1);
	currentVine = 2;

	// Remove all obstacle and item sprites
	rockSprites.RemoveAll();
	fireSprites.RemoveAll();
	dartSprites.RemoveAll();
	snakeSprites.RemoveAll();
	itemSprites.RemoveAll();

	// Re-initalize sprites
	InitalizeSprites();
}

// -----------------------------------------------------------------------------
// Add rocks to the rock sprite list, get a random vine position
void MyProject::AddRocks()
{
	if (currentState == eGameStates::PLAYING)
	{
		SpriteType newSprite;

		Vector2 startPos;

		switch (rand() % 6) // Gets a random vine position for x
		{
		case 0: // Vine 1
			startPos.x = vineX[0];
			startPos.y = 0;
			break;
		case 1: // Vine 2
			startPos.x = vineX[1];
			startPos.y = 0;
			break;
		case 2: // Vine 3
			startPos.x = vineX[2];
			startPos.y = 0;
			break;
		case 3: // Vine 4
			startPos.x = vineX[3];
			startPos.y = 0;
			break;
		case 4: // Vine 5
			startPos.x = vineX[4];
			startPos.y = 0;
			break;
		case 5: // Vine 6
			startPos.x = vineX[5];
			startPos.y = 0;
			break;
		}

		newSprite.Initialize(&rockTex, startPos, 0, 0, 1, Color(1, 1, 1), 0);
		newSprite.SetPivot(SpriteType::Pivot::Center);

		newSprite.startPoint = startPos;
		newSprite.endPoint = Vector2(startPos.x, 768); // Endpoint is bottom of screen

		newSprite.direction = newSprite.endPoint - newSprite.startPoint;
		newSprite.direction.Normalize();

		newSprite.speed = rand() % int(obstacleSpeed) + 1; // speed that they fall is random

		rockSprites.Add(newSprite); // Add to rock sprite list
	}
}

// -----------------------------------------------------------------------------
// Add fire balls to the fire sprite list, at a random vine position
void MyProject::AddFire()
{
	if (currentState == eGameStates::PLAYING)
	{
		SpriteType newSprite;

		Vector2 startPos;

		switch (rand() % 6) // Get random vine pos for x, y is at bottom of screen
		{
		case 0: // Vine 1
			startPos.x = vineX[0];
			startPos.y = 768;
			break;
		case 1: // Vine 2
			startPos.x = vineX[1];
			startPos.y = 768;
			break;
		case 2: // Vine 3
			startPos.x = vineX[2];
			startPos.y = 768;
			break;
		case 3: // Vine 4
			startPos.x = vineX[3];
			startPos.y = 768;
			break;
		case 4: // Vine 5
			startPos.x = vineX[4];
			startPos.y = 768;
			break;
		case 5: // Vine 6
			startPos.x = vineX[5];
			startPos.y = 768;
			break;
		}

		newSprite.Initialize(&fireTex, startPos, 0, 0, 1, Color(1, 1, 1), 0);
		newSprite.SetPivot(SpriteType::Pivot::Center);

		newSprite.startPoint = startPos;
		newSprite.endPoint = Vector2(startPos.x, 0); // End point is top of screen

		newSprite.direction = newSprite.endPoint - newSprite.startPoint;
		newSprite.direction.Normalize();

		newSprite.speed = obstacleSpeed + 1;

		fireSprites.Add(newSprite); // Add to fire sprite list
	}
}

// -----------------------------------------------------------------------------
// Add darts to the dart sprite list, at a random y position
void MyProject::AddDart()
{
	if (currentState == eGameStates::PLAYING)
	{
		SpriteType newSprite;

		Vector2 startPos;
		int startX = rand() % 2 + 1; // Start at left side of screen, or right?

		if (startX == 1) // Spawn left
		{
			startPos.x = 0;
		}
		else // Spawn right
		{
			startPos.x = 1024;
		}
		startPos.y = rand() % (768 - lavaTex.GetHeight() - 50) + 50; // Random y-pos

		newSprite.Initialize(&dartTex, startPos, 0, 0, 1, Color(1, 1, 1), 0);

		newSprite.startPoint = startPos;
		if (startX == 1) // If start on the left
		{
			newSprite.SetRotation(180); // Rotate to face the right
			newSprite.SetPivot(SpriteType::Pivot::CenterRight); // Set pivot to be further back so collision with player sprite feels better
			newSprite.endPoint = Vector2(1100, startPos.y); // End on right side of screen
		}
		else // else, start on right
		{
			newSprite.endPoint = Vector2(-100, startPos.y); // endpoint is left of screen
			newSprite.SetPivot(SpriteType::Pivot::Center);
		}

		newSprite.direction = newSprite.endPoint - newSprite.startPoint;
		newSprite.direction.Normalize();

		newSprite.speed = obstacleSpeed + 1;

		dartSprites.Add(newSprite); // Add to dart sprite list
	}
}

// -----------------------------------------------------------------------------
// Add snakes to the snake sprite list, at a random vine position
void MyProject::AddSnake()
{
	if (currentState == eGameStates::PLAYING)
	{
		SpriteType newSprite;
		Color snakeColor;
		Vector2 startPos;
		int startY = rand() % 2 + 1; // Start at bottom or top?

		switch (rand() % 6) // Get random vine position
		{
		case 0: // Vine 1
			startPos.x = vineX[0];
			break;
		case 1: // Vine 2
			startPos.x = vineX[1];
			break;
		case 2: // Vine 3
			startPos.x = vineX[2];
			break;
		case 3: // Vine 4
			startPos.x = vineX[3];
			break;
		case 4: // Vine 5
			startPos.x = vineX[4];
			break;
		case 5: // Vine 6
			startPos.x = vineX[5];
			break;
		}

		if (startY == 1) // Start at top
		{
			startPos.y = 0;
			snakeColor = Color(1, 1, 1); // Regular colour
		}
		else if (startY == 2) // Start at bottom
		{
			startPos.y = 768;
			snakeColor = DirectX::Colors::OrangeRed; // Lava snake!
		}

		newSprite.Initialize(&snakeTex, startPos, 0, 0, 1, snakeColor, 0);
		newSprite.SetPivot(SpriteType::Pivot::Center);

		newSprite.startPoint = startPos;

		if (startY == 1) // If start at top, end at bottom
		{
			newSprite.endPoint = Vector2(startPos.x, (768 - lavaTex.GetHeight() - 20));
		}

		else if (startY == 2) // If start at bottom, end at top
		{
			newSprite.SetRotation(180); // Flip around to face top of screen
			newSprite.SetScale(0.8); // Lava snakes are slightly smaller
			newSprite.endPoint = Vector2(startPos.x, 0);
		}

		newSprite.direction = newSprite.endPoint - newSprite.startPoint;
		newSprite.direction.Normalize();

		newSprite.speed = rand() % int(obstacleSpeed) + 1; // Move at random speeds

		snakeSprites.Add(newSprite); // Add to snake sprite list
	}
}

// -----------------------------------------------------------------------------
// Add items to the item sprite list, at a random vine position
void MyProject::AddItems()
{
	if (currentState == eGameStates::PLAYING)
	{
		SpriteType newSprite;
		Vector2 pos = ItemPos(); // Go to item pos function to get a random position

		switch (itemLevel) // Based on item level, initialize item sprite
		{
		case 1:
			newSprite.Initialize(&orangeTex, pos, 0, 0, 1, Color(1, 1, 1), 0);
			break;
		case 2:
			newSprite.Initialize(&pearTex, pos, 0, 0, 1, Color(1, 1, 1), 0);
			break;
		case 3:
			newSprite.Initialize(&appleTex, pos, 0, 0, 1, Color(1, 1, 1), 0);
			break;
		case 4:
			newSprite.Initialize(&bananaTex, pos, 0, 0, 1, Color(1, 1, 1), 0);
			break;
		case 5:
			newSprite.Initialize(&chaliceTex, pos, 0, 0, 1, Color(1, 1, 1), 0);
			break;
		case 6:
			newSprite.Initialize(&necklaceTex, pos, 0, 0, 1, Color(1, 1, 1), 0);
			break;
		case 7:
			newSprite.Initialize(&tripowerTex, pos, 0, 0, 1, Color(1, 1, 1), 0);
			break;
		case 8:
			newSprite.Initialize(&idolTex, pos, 0, 0, 1, Color(1, 1, 1), 0);
			break;
		}

		newSprite.SetPivot(SpriteType::Pivot::Center);

		itemSprites.Add(newSprite); // Add to item sprite list
	}
}

// -----------------------------------------------------------------------------
// Get a random position for item
Vector2 MyProject::ItemPos()
{
	if (currentState == eGameStates::PLAYING)
	{
		Vector2 pos = koalaSprite.GetPosition();

		while (pos.x == koalaSprite.GetPosition().x) // Keep looping until the x of the item pos is not equal to the player's x pos
		{
			switch (rand() % 6) // Random vine
			{
			case 0: // Vine 1
				pos.x = vineX[0];
				break;
			case 1: // Vine 2
				pos.x = vineX[1];
				break;
			case 2: // Vine 3
				pos.x = vineX[2];
				break;
			case 3: // Vine 4
				pos.x = vineX[3];
				break;
			case 4: // Vine 5
				pos.x = vineX[4];
				break;
			case 5: // Vine 6
				pos.x = vineX[5];
				break;
			}
			pos.y = rand() % (768 - lavaTex.GetHeight() - 30) + 30; // Random y
		}
		return pos;
	}
}

// -----------------------------------------------------------------------------
// Move player sprite based on mouse pos
void MyProject::Move()
{
	// If mouse is clicked on the same vine as player, and mouse is clicked above the koala sprite
	if (mousePos.y <= (koalaSprite.GetPosition().y - 50) && mousePos.x >= (koalaSprite.GetPosition().x - 20) && mousePos.x <= (koalaSprite.GetPosition().x + 20))
	{
		Vector2 newPos = koalaSprite.GetPosition(); // Set new pos to koala pos
		newPos.y -= 50; // Move new pos up by 50
		koalaSprite.SetPosition(newPos); // Update koala pos
	}

	// If mouse is clicked on the same vine as player, and mouse is clicked below the koala sprite
	else if (mousePos.y >= (koalaSprite.GetPosition().y + 50) && mousePos.x >= (koalaSprite.GetPosition().x - 20) && mousePos.x <= (koalaSprite.GetPosition().x + 20) && mousePos.y < (768 - lavaTex.GetHeight() - 20))
	{
		Vector2 newPos = koalaSprite.GetPosition();
		newPos.y += 50; // Move new pos down by 50
		koalaSprite.SetPosition(newPos); // Update koala pos
	}

	// If mouse is clicked on the vine to the right of the current vine, and current vine is less than or equal to vine count
	else if (mousePos.x >= (vineX[currentVine + 1] - 20) && mousePos.x <= (vineX[currentVine + 1] + 20) && currentVine <= VINE_COUNT)
	{
		Vector2 newPos = koalaSprite.GetPosition();
		newPos.x = vineX[currentVine + 1]; // new x pos = vine that was clicked on
		koalaSprite.SetPosition(newPos); // Koala sprite moves to new vine
		currentVine++; // Add to currentVine to keep track of vine position
		score += 20; // Movement adds to the score
	}

	// If mouse is clicked on the vine to the left of the current vine, and current vine is greater than 0
	else if (mousePos.x >= (vineX[currentVine - 1] - 20) && mousePos.x <= (vineX[currentVine - 1] + 20) && currentVine > 0)
	{
		Vector2 newPos = koalaSprite.GetPosition();
		newPos.x = vineX[currentVine - 1]; // new x pos = vine that was clicked on
		koalaSprite.SetPosition(newPos); // Update koala position to new vine
		currentVine--; // Subtract from current vine
		score += 20; // Movement adds to score
	}
}

// -----------------------------------------------------------------------------
// Check to see if koala sprite collides any with items or obstacles
void MyProject::CheckForCollisions()
{
	for (int i = 0; i < rockSprites.GetSpriteCount(); i++)
	{
		if (rockSprites.GetSprite(i).SpriteCollision(koalaSprite) && gracePeriod <= 0) // If collision and invulnerability period is 0
		{
			rockSprites.Remove(i); // Remove sprite
			lives--; // Lose a life
			gracePeriod = 1; // Give a second of invulnerability
			koalaSprite.SetColor(Color(1, 0, 0)); // Koala turns red
			Vector2 newPos = koalaSprite.GetPosition();
			newPos.y += 20; // Move koala down a bit
			koalaSprite.SetPosition(newPos);
			if (lives < 0)
			{
				currentState = eGameStates::OVER; // When lives are less than 0, game over!
			}
		}
	}
	
	// Same effects as rock collision
	for (int i = 0; i < fireSprites.GetSpriteCount(); i++)
	{
		if (fireSprites.GetSprite(i).SpriteCollision(koalaSprite) && gracePeriod <= 0)
		{
			fireSprites.Remove(i);
			lives--;
			gracePeriod = 1;
			koalaSprite.SetColor(Color(1, 0, 0));
			Vector2 newPos = koalaSprite.GetPosition();
			newPos.y -= 20; // Koala moves up a bit, rather than down
			koalaSprite.SetPosition(newPos);
			if (lives < 0)
			{
				currentState = eGameStates::OVER;
			}
		}
	}

	// Same effects as rock collision
	for (int i = 0; i < dartSprites.GetSpriteCount(); i++)
	{
		if (dartSprites.GetSprite(i).SpriteCollision(koalaSprite) && gracePeriod <= 0)
		{
			dartSprites.Remove(i);
			lives--;
			gracePeriod = 1;
			koalaSprite.SetColor(Color(1, 0, 0));
			Vector2 newPos = koalaSprite.GetPosition();
			newPos.y += 20;
			koalaSprite.SetPosition(newPos);
			if (lives < 0)
			{
				currentState = eGameStates::OVER;
			}
		}
	}

	// Same effects as rock collision
	for (int i = 0; i < snakeSprites.GetSpriteCount(); i++)
	{
		if (snakeSprites.GetSprite(i).SpriteCollision(koalaSprite) && gracePeriod <= 0)
		{
			snakeSprites.Remove(i);
			lives--;
			gracePeriod = 1;
			koalaSprite.SetColor(Color(1, 0, 0));
			Vector2 newPos = koalaSprite.GetPosition();
			newPos.y += 20;
			koalaSprite.SetPosition(newPos);
			if (lives < 0)
			{
				currentState = eGameStates::OVER;
			}
		}
	}

	for (int i = 0; i < itemSprites.GetSpriteCount(); i++)
	{
		if (itemSprites.GetSprite(i).SpriteCollision(koalaSprite)) // If collision
		{
			itemSprites.Remove(i); // Remove item
			score += itemCombo; // Add current itemCombo score to score
			itemCombo = itemCombo * 2; // Double current itemCombo
		}
	}
}

// -----------------------------------------------------------------------------
// Updates obstacle and item levels
void MyProject::UpdateLevel(float deltaTime)
{
	if (elapsedTime >= timeToNextChange && elapsedTime < timeToNextChange + 15) // If timeToNextChange has been reached
	{
		if (itemLevel > 8) // If itemLevel is greater than 8
		{
			itemLevel = 1; // Set level back to 1 (orange/item1)
			itemCombo = 100; // Reset combo (how much score gained from item)
		}

		// Spawn new item
		AddItems();
		itemLevel++; // Item level goes up by 1

		itemDespawn = 5; // Despawn timer is reset
		timeToNextChange += 15; // 15 seconds is added to the timer

		if (obstacleLevel <= 4)
		{
			obstacleLevel++; // Obstacle level goes up until it reaches 4,
		}
		else if (obstacleLevel >= 5)
		{
			obstacleSpeed += 0.5; // at which point obstacle speed is added to instead
		}
	}

	if (itemSprites.GetSpriteCount() > 0) // If there is an item on screen
	{
		itemDespawn -= deltaTime; // despawn timer counts down
		if (itemDespawn <= 0) // if less than or equal to 0,
		{
			itemSprites.Remove(itemLevel); // remove item
			itemCombo = 100; // reset score gained from item
			itemLevel = 1; // reset item level
		}
	}
}

// -----------------------------------------------------------------------------
// Updates obstacle positions
void MyProject::UpdateObstacles()
{
	for (int i = 0; i < rockSprites.GetSpriteCount(); i++)
	{
		SpriteType& currentSprite = rockSprites.GetSprite(i);

		Vector2 curPos = currentSprite.GetPosition();

		curPos += (currentSprite.direction * currentSprite.speed);

		float distToTargetX = abs(currentSprite.endPoint.x - curPos.x); 
		float distToTargetY = abs(currentSprite.endPoint.y - curPos.y);

		currentSprite.SetRotation(currentSprite.GetRotation() + currentSprite.speed);

		currentSprite.SetPosition(curPos);
	}

	for (int i = 0; i < fireSprites.GetSpriteCount(); i++)
	{
		SpriteType& currentSprite = fireSprites.GetSprite(i);

		Vector2 curPos = currentSprite.GetPosition();

		curPos += (currentSprite.direction * currentSprite.speed);

		float distToTargetX = abs(currentSprite.endPoint.x - curPos.x);
		float distToTargetY = abs(currentSprite.endPoint.y - curPos.y);

		currentSprite.SetPosition(curPos);
	}

	for (int i = 0; i < dartSprites.GetSpriteCount(); i++)
	{
		SpriteType& currentSprite = dartSprites.GetSprite(i);

		Vector2 curPos = currentSprite.GetPosition();

		curPos += (currentSprite.direction * currentSprite.speed);

		float distToTargetX = abs(currentSprite.endPoint.x - curPos.x); 
		float distToTargetY = abs(currentSprite.endPoint.y - curPos.y);

		currentSprite.SetPosition(curPos);
	}

	for (int i = 0; i < snakeSprites.GetSpriteCount(); i++)
	{
		SpriteType& currentSprite = snakeSprites.GetSprite(i);

		Vector2 curPos = currentSprite.GetPosition();

		curPos += (currentSprite.direction * currentSprite.speed);

		float distToTargetX = abs(currentSprite.endPoint.x - curPos.x);
		float distToTargetY = abs(currentSprite.endPoint.y - curPos.y);

		if (distToTargetX < 5 && distToTargetY < 5 && currentSprite.hasSwapped == false)
		{
			// Swapping start and end point
			Vector2 temp = currentSprite.endPoint;
			currentSprite.endPoint = currentSprite.startPoint;
			currentSprite.startPoint = temp;

			currentSprite.SetRotation(currentSprite.GetRotation() + 180); // Rotate around to face other direction

			currentSprite.direction = currentSprite.endPoint - currentSprite.startPoint;
			currentSprite.direction.Normalize();

			currentSprite.hasSwapped = true; // Set hasSwapped to true, don't swap start and end again
		}

		currentSprite.SetPosition(curPos);
	}
}

// -----------------------------------------------------------------------------
// Add new obstacles
void MyProject::AddObstacles(float deltaTime)
{
	int toSpawn = rand() % obstacleLevel; // Choose a random obstacle to spawn in

	obstacleTime -= deltaTime; // Obstacle time counts down

	if (obstacleTime <= 0) // When obstacle time reaches 0,
	{
		obstacleTime = rand() % timeToNextObstacle + 0.5; // Obstacle time is set to new random value

		switch (toSpawn) // Spawn in obstacle that corresponds to toSpawn's value
		{
		case ROCK:
			AddRocks();
			break;
		case FIRE:
			AddFire();
			break;
		case DART:
			AddDart();
			break;
		case SNAKE:
			AddSnake();
			break;
		default:
			break;
		}
	}
}

// -----------------------------------------------------------------------------
// Remove obstacles if they go off-screen
void MyProject::RemoveObstacles()
{
	for (int i = 0; i < rockSprites.GetSpriteCount(); i++)
	{
		SpriteType& currentSprite = rockSprites.GetSprite(i);

		Vector2 curPos = currentSprite.GetPosition();

		if (curPos.y > 768)
		{
			rockSprites.Remove(i);
		}
	}

	for (int i = 0; i < fireSprites.GetSpriteCount(); i++)
	{
		SpriteType& currentSprite = fireSprites.GetSprite(i);

		Vector2 curPos = currentSprite.GetPosition();

		if (curPos.y < -100)
		{
			fireSprites.Remove(i);
		}
	}

	for (int i = 0; i < dartSprites.GetSpriteCount(); i++)
	{
		SpriteType& currentSprite = dartSprites.GetSprite(i);

		Vector2 curPos = currentSprite.GetPosition();

		if (curPos.x > 1124 || curPos.x < -100)
		{
			dartSprites.Remove(i);
		}
	}

	for (int i = 0; i < snakeSprites.GetSpriteCount(); i++)
	{
		SpriteType& currentSprite = snakeSprites.GetSprite(i);

		Vector2 curPos = currentSprite.GetPosition();

		if (curPos.y < -100 || curPos.y > 768)
		{
			snakeSprites.Remove(i);
		}
	}
}