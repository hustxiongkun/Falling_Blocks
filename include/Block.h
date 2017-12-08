//////////////////////////////////////////////////////////////////////////
// Block.h
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "../include/Square.h"

// class for Block
class Block
{
public:
	// constructor
	Block();
	Block(int x, int y, LTexture* texture, BlockTypes type);

	// set position of squares
	void setupSquares(int x, int y);

	// draw block
	void draw(SDL_Renderer* renderer);

	// move block
	void move(Direction dir);

	// rotate block
	void rotate();

	// get rotate block position
	int* getRotatePosition();

	// get squares
	// we will save square pointer in vector and can not deallocate
	// it in destructor, so we deallocate by get the pointer
	Square** getSquares();

	~Block();

private:
	// center position
	int mCenterX;
	int mCenterY;

	// squares
	Square* mSquares[4];
	BlockTypes mBlockType;
public:
	// clips for squares
	static SDL_Rect sBlockClips[BLOCK_TOTAL];
};

// init static clips
SDL_Rect Block::sBlockClips[BLOCK_TOTAL];

Block::Block()
{
}

Block::~Block()
{
}

Block::Block(int x, int y, LTexture* texture, BlockTypes type):
	mCenterX(x),mCenterY(y),mBlockType(type){
	for (int i = 0; i < 4; i++) {
		mSquares[i] = new Square(x, y, texture, &sBlockClips[type]);
	}
	// set squares position
	setupSquares(x, y);
}

void Block::setupSquares(int x, int y) {
	mCenterX = x;
	mCenterY = y;
	switch (mBlockType)
	{
	case SQUARE_BLOCK:
		// [0][1]
		// [2][3]
		mSquares[0]->setCenter(x - SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[1]->setCenter(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[2]->setCenter(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		mSquares[3]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		break;
	case L_BLOCK:
		// [0]
		// [1]
		// [2][3]
		mSquares[0]->setCenter(x - SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[1]->setCenter(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		mSquares[2]->setCenter(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN*3);
		mSquares[3]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN*3);
		break;
	case BACKWORDS_L_BLOCK:
		//    [0]
		//    [1]
		// [2][3]
		mSquares[0]->setCenter(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[1]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		mSquares[2]->setCenter(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN*3);
		mSquares[3]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN*3);
		break;
	case T_BLOCK:
		//    [0]
		// [1][2][3]
		mSquares[0]->setCenter(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[1]->setCenter(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		mSquares[2]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		mSquares[3]->setCenter(x + SQUARE_MEDIAN*3, y + SQUARE_MEDIAN);
		break;
	case S_BLOCK:
		//    [0][1]
		// [2][3]
		mSquares[0]->setCenter(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[1]->setCenter(x + SQUARE_MEDIAN*3, y - SQUARE_MEDIAN);
		mSquares[2]->setCenter(x - SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		mSquares[3]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		break;
	case BACKWARDS_S_BLOCK:
		// [0][1]
		//    [2][3]
		mSquares[0]->setCenter(x - SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[1]->setCenter(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[2]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		mSquares[3]->setCenter(x + SQUARE_MEDIAN*3, y + SQUARE_MEDIAN);
		break;
	case STRAIGHT_BLOCK:
		// [0]
		// [1]
		// [2]
		// [3]
		mSquares[0]->setCenter(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN*3);
		mSquares[1]->setCenter(x + SQUARE_MEDIAN, y - SQUARE_MEDIAN);
		mSquares[2]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN);
		mSquares[3]->setCenter(x + SQUARE_MEDIAN, y + SQUARE_MEDIAN*3);
		break;
	default:
		break;
	}
}

void Block::draw(SDL_Renderer* renderer) {
	for (int i = 0; i < 4; i++) {
		mSquares[i]->draw(renderer);
	}
}

void Block::move(Direction dir) {
	int distance = SQUARE_MEDIAN * 2;
	// move block center
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
	// move squares
	for (int i = 0; i < 4; i++) {
		mSquares[i]->move(dir);
	}
}

void Block::rotate() {
	int x1, x2, y1, y2;
	for (int i = 0; i < 4; i++) {
		// get square position
		x1 = mSquares[i]->getCenterX();
		y1 = mSquares[i]->getCenterY();
		// origin
		x1 -= mCenterX;
		y1 -= mCenterY;

		// rotate
		x2 = -y1 + mCenterX;
		y2 = x1 + mCenterY;
		mSquares[i]->setCenter(x2, y2);
	}
}

int* Block::getRotatePosition() {
	int* temp = new int[8];
	int x1, x2, y1, y2;
	for (int i = 0; i < 4; i++) {
		// get square position
		x1 = mSquares[i]->getCenterX();
		y1 = mSquares[i]->getCenterY();
		// origin
		x1 -= mCenterX;
		y1 -= mCenterY;

		// rotate
		x2 = -y1 + mCenterX;
		y2 = x1 + mCenterY;
		temp[i * 2] = x2;
		temp[i * 2 + 1] = y2;
	}
	return temp;
}

Square** Block::getSquares() {
	return mSquares;
}