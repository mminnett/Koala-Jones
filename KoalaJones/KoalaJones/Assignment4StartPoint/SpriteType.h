#pragma once
//----------------------------------------------------------------------------------------
// Definition file for an advanced sprite class capable of rotation, scaling and colouring.
// The class can also modify it's pivot point and texture region.
//----------------------------------------------------------------------------------------

#include <DirectX.h>
#include "TextureType.h"

using namespace DirectX;

class SpriteType
{
	public:

		// enumerated type representing the pivot
		enum Pivot { UpperLeft,	UpperRight, Center, CenterLeft, CenterRight, LowerLeft, LowerRight };

		// constructor
		SpriteType();

		// initialize the sprite
		void Initialize(TextureType* pTexture, Vector2 position, float rotationDegrees, int rotDelta, float scale, Color color, float layer);

		// draw 
		void Draw(SpriteBatch* pBatch);

		// get and set the color
		Color GetColor() const { return color; }
		void SetColor(Color c) { color = c; }

		// get and set the position
		Vector2 GetPosition() const { return position; }
		void SetPosition(Vector2 p) { position = p; }

		// get and set the rotation in degrees
		float GetRotation() { return rotation * 180.0f / 3.141592f; }
		void SetRotation(float d) { rotation = d * 3.141592f / 180.0f; }

		// get and set the scale
		float GetScale() const { return scale; }
		void SetScale(float f) { scale = f; }

		// set the pivot of the sprite for rotation purposes
		void SetPivot(Pivot inPivot);

		// set the texture region that you would like to copy
		//  note - Initialize will reset this to the full texture
		void SetTextureRegion(int left, int top, int right, int bottom);
		RECT GetTextureRegion() { return textureRegion; }

		// convert degrees to radians as the DirectX rotation operated on radians not degrees
		float DegreesToRadians(float deg) { return 3.141592f * deg / 180.0f; }

		// clamp an integer to either the min or max value if the value is outside the min max range
		// prevents errors when copying texture regions
		int Clamp(int value, int min, int max);

		// Collisions
		bool PointCollision(Vector2 point);
		bool SpriteCollision(SpriteType& toCollideWith);

		Vector2 startPoint; // Where the sprite starts
		Vector2 endPoint; // Sprite's destination
		Vector2 direction; // Direction for sprite to move

		float speed; // Speed that sprite moves at

		bool hasSwapped; // Bool for snakes, to determine if they have swapped directions already

	private:
		// transformation information
		Vector2			position;
		float			rotation;
		float			scale;
		float			layer;

		// colour & texture
		Color			color;
		TextureType*	pTexture;

		// pivot/origin information
		Pivot			pivot;
		Vector2			origin;

		// the region of the texture we are drawing
		RECT			textureRegion;
};