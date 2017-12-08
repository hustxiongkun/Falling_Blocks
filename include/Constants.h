//////////////////////////////////////////////////////////////////////////
// Constants.h
//////////////////////////////////////////////////////////////////////////

#pragma once

// constant data
// window setting
const int WINDOW_WIDTH = 300;
const int WINDOW_HEIGHT = 400;
const char* WINDOW_CAPTION = "Falling Blocks";

// game setting
const int FRAMES_PER_SECOND = 60;
const int FRAME_RATE = 1000/FRAMES_PER_SECOND;

// force speed
const int INITIAL_SPEED = 60;
const int SPEED_CHANGE = 12;
const int SLIDE_TIME = 15;
// score
const int POINTS_PER_LINE = 500;
const int POINTS_PER_LEVEL = 6000;
const int LEVEL_NUMS = 5;
// square related setting
const int SQUARES_PER_ROW = 10;
const int SQUARE_MEDIAN = 10;

// game level background coordinate
const int LEVEL_ONE_X = 0;
const int LEVEL_ONE_Y = 0;
const int LEVEL_TWO_X = 300;
const int LEVEL_TWO_Y = 0;
const int LEVEL_THREE_X = 600;
const int LEVEL_THREE_Y = 0;
const int LEVEL_FOUR_X = 0;
const int LEVEL_FOUR_Y = 396;
const int LEVEL_FIVE_X = 300;
const int LEVEL_FIVE_Y = 396;
// squares coordinate
const int SQUARE_START_X = 600;
const int SQUARE_START_Y = 400;

// text coordinate for level, score and needed score
const int LEVEL_RECT_X = 40;
const int LEVEL_RECT_Y = 320;
const int SCORE_RECT_X = 40;
const int SCORE_RECT_Y = 340;
const int NEEDED_SCORE_RECT_X = 40;
const int NEEDED_SCORE_RECT_Y = 360;
// block start coordinate and next block circle coordinate
const int BLOCK_START_X = 150;
const int BLOCK_START_Y = 60;
const int NEXT_BLOCK_CIRCLE_X = 210;
const int NEXT_BLOCK_CIRCLE_Y = 340;
// game area
const int GAME_AREA_LEFT = 50;
const int GAME_AREA_RIGHT = 250;
const int GAME_AREA_BOTTOM = 300;