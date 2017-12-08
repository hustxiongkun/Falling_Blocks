//////////////////////////////////////////////////////////////////////////
// Square.h
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "../include/Constants.h"
#include "../include/Tools.h"

// class for square
class Square
{
public:
	// constructor
	Square();
	Square(int x, int y, LTexture* texture, SDL_Rect* clip);

	// draw square
	void draw(SDL_Renderer* render);

	// move square
	void move(Direction dir);

	// getter
	int getCenterX();
	int getCenterY();

	// setter
	void setCenterX(int x);
	void setCenterY(int y);
	void setCenter(int x, int y);

	~Square();

private:
	// position of center
	int mCenterX;
	int mCenterY;

	// texture for square
	LTexture* mTexture;
	// clip render
	SDL_Rect* mClip;
};

Square::Square()
{
}

Square::~Square()
{
}

Square::Square(int x, int y, LTexture* texture, SDL_Rect* clip):
	mCenterX(x),mCenterY(y),mTexture(texture),mClip(clip){
}

void Square::draw(SDL_Renderer* renderer) {
	mTexture->render(renderer, mCenterX-SQUARE_MEDIAN, mCenterY-SQUARE_MEDIAN, mClip);
}

void Square::move(Direction dir) {
	int distance = SQUARE_MEDIAN * 2;
	switch (dir)
	{
	case LEFT:
		mCenterX -= distance;
		break;
	case RIGHT:
		mCenterX += distance;
		break;
	case DOWN:
		mCenterY += distance;
		break;
	default:
		break;
	}
}

int Square::getCenterX() {
	return mCenterX;
}

int Square::getCenterY() {
	return mCenterY;
}

void Square::setCenterX(int x) {
	mCenterX = x;
}

void Square::setCenterY(int y) {
	mCenterY = y;
}

void Square::setCenter(int x, int y) {
	mCenterX = x;
	mCenterY = y;
}