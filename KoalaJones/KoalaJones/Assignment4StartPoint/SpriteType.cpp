//----------------------------------------------------------------------------------------
// Implementation file for an advanced sprite class capable of rotation, scaling and colouring.
// The class can also modify it's pivot point and texture region.
//----------------------------------------------------------------------------------------

#include "SpriteType.h"

// -----------------------------------------------------------------------------
// default sprite constructor
SpriteType::SpriteType()
{
	position = Vector2(0, 0);	// set the initial position to 0,0
	pTexture = NULL;			// set the texture
	color = Color(DirectX::Colors::White.v);	// default colour to the original sprite colours

	rotation = 0;				// set the rotaion to 0
	scale = 1;					// set scale to 1 which is the actual/natural size of the texure
	hasSwapped = false;

	pivot = UpperLeft;			// default the pivot to 0,0
	SetPivot(pivot);			// use the pivot to set the origin point

	layer = 0.0f;				// default the draw layer

	textureRegion.left = 0;		// default the texture extents
	textureRegion.top = 0;
	textureRegion.bottom = 0;
	textureRegion.right = 0;
}

// -----------------------------------------------------------------------------
// initialize the sprite
void SpriteType::Initialize(TextureType* pTex, Vector2 pos, float rotationInDegrees, int rotDelta, float inScale, Color inColor, float inLayer)
{
	pTexture = pTex;	// set the texture for the sprite to use
	position = pos;		// set it's drawing position

	rotation = DegreesToRadians(rotationInDegrees);		// setting the rotation, scale, colour and layer
	scale = inScale;
	color = inColor;
	layer = inLayer;

	if (pTexture)		// if we have a texture set the region values to the full extent of the texture
	{
		textureRegion.left = 0;
		textureRegion.top = 0;
		textureRegion.bottom = pTex->GetHeight();
		textureRegion.right = pTex->GetWidth();
	}
}

// -----------------------------------------------------------------------------
// draw - Requires pBatch->Begin() to be called prior to this
void SpriteType::Draw(SpriteBatch* pBatch)
{
	if (pTexture)		// if there is a texture for this sprite go ahead and draw it
		pBatch->Draw(pTexture->GetResourceView(), position, &textureRegion, color, rotation, origin, scale, DirectX::SpriteEffects_None, layer);	// DirectX sprite batch call to draw the sprite
}

// -----------------------------------------------------------------------------
// set the pivot of the sprite
void SpriteType::SetPivot(SpriteType::Pivot inPivot)
{
	pivot = inPivot;

	if (pTexture != NULL)
	{
		switch (pivot)
		{
		case SpriteType::UpperLeft:
			origin = Vector2(float(textureRegion.left), float(textureRegion.top));
			break;
		case SpriteType::UpperRight:
			origin = Vector2(float(textureRegion.right), float(textureRegion.top));
			break;
		case SpriteType::Center:
			origin = Vector2(float(textureRegion.right - textureRegion.left) / 2.0f, float(textureRegion.bottom - textureRegion.top) / 2.0f);
			break;
		case SpriteType::CenterLeft:
			origin = Vector2(float(textureRegion.left), float(textureRegion.bottom - textureRegion.top) / 2.0f);
			break;
		case SpriteType::CenterRight:
			origin = Vector2(float(textureRegion.right), float(textureRegion.bottom - textureRegion.top) / 2.0f);
			break;
		case SpriteType::LowerRight:
			origin = Vector2(float(textureRegion.right), float(textureRegion.bottom));
			break;
		case SpriteType::LowerLeft:
			origin = Vector2(float(textureRegion.left), float(textureRegion.bottom));
			break;
		}
	}
}

// -----------------------------------------------------------------------------
// Sets the sprite to show only a portion of the texture
//
void SpriteType::SetTextureRegion(int left, int top, int right, int bottom)
{
	if (pTexture != NULL)
	{
		left = Clamp(left, 0, pTexture->GetWidth());		// verifying the values are within the texture region
		top = Clamp(top, 0, pTexture->GetHeight());
		right = Clamp(right, 0, pTexture->GetWidth());
		bottom = Clamp(bottom, 0, pTexture->GetHeight());
	}

	textureRegion.left = left;	// texture values can safely be set now that we know the bounds are within the textures range
	textureRegion.right = right;
	textureRegion.top = top;
	textureRegion.bottom = bottom;

	SetPivot(pivot);		// reset the pivot based on the new texture region
}

// -----------------------------------------------------------------------------
// clamp an integer to either the min or max value if the value is outside the min max range
// used to ensure texture areas are within the texture size
int SpriteType::Clamp(int value, int min, int max)
{
	if (value < min)	// if value is below minimum set it to 0
		value = min;

	if (value > max)	// if value is above maximum set it to maximum
		value = max;

	return value;		// return the value
}

//----------------------------------------------------------------------------------------------------------------
// Checks to see if the point is inside the sprite bounding box
bool SpriteType::PointCollision(Vector2 point)
{
	int left = position.x - ((int)textureRegion.right >> 1) * scale;
	int right = left + textureRegion.right * scale;
	int top = position.y - ((int)textureRegion.bottom >> 1) * scale;
	int bottom = top + textureRegion.bottom * scale;

	if (point.x >= left && point.x <= right && point.y >= top && point.y <= bottom)
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------------------------------------------
// Checks to see if two sprites have collided
bool SpriteType::SpriteCollision(SpriteType& toCollideWith)
{
	Vector2 topLeft, topRight, bottomLeft, bottomRight;

	topLeft.x = position.x - (pTexture->GetWidth() / 2);
	topLeft.y = position.y - (pTexture->GetHeight() / 2);

	topRight.x = position.x + (pTexture->GetWidth() / 2);
	topRight.y = position.y - (pTexture->GetHeight() / 2);

	bottomLeft.x = position.x - (pTexture->GetWidth() / 2);
	bottomLeft.y = position.y + (pTexture->GetHeight() / 2);

	bottomRight.x = position.x + (pTexture->GetWidth() / 2);
	bottomRight.y = position.y + (pTexture->GetHeight() / 2);

	if (toCollideWith.PointCollision(topLeft) == true ||
		toCollideWith.PointCollision(topRight) == true ||
		toCollideWith.PointCollision(bottomLeft) == true ||
		toCollideWith.PointCollision(bottomRight) == true)
	{
		return true;
	}
	return false;
}