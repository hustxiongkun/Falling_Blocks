//////////////////////////////////////////////////////////////////////////
// Tools.h
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

// texture wrapper class
class LTexture
{
public:
	// initializes variables
	LTexture();

	// deallocates memory
	~LTexture();

	// loads image at specified path
	bool loadFromFile(SDL_Renderer* renderer, std::string path);

	// creates image from font string
	bool loadFromRenderedText(SDL_Renderer* renderer, std::string textureText, SDL_Color textColor);

	// deallocates texture
	void freeTexture();

	// set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	// set blending
	void setBlendMode(SDL_BlendMode blending);

	// set alpha modulation
	void setAlpha(Uint8 alpha);

	// set text texture font
	void setFont(std::string font, int size);

	// renders texture at given point
	void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	// gets image dimensions
	int getWidth();
	int getHeight();

private:
	// the actual hardware texture
	SDL_Texture* mTexture;

	// font for text texture
	TTF_Font* mFont;

	// image dimensions
	int mWidth;
	int mHeight;
};

LTexture::LTexture(){
	// initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture(){
	// deallocate
	freeTexture();
}

bool LTexture::loadFromFile(SDL_Renderer* renderer, std::string path)
{
	// get rid of preexisting texture
	freeTexture();

	// the final texture
	SDL_Texture* newTexture = NULL;

	// load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		// color key image
		//SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		// create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		} else {
			// get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	// return success
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(SDL_Renderer* renderer, std::string textureText, SDL_Color textColor) {
	// get rid of preexisting texture
	if (mTexture != NULL){
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}

	// render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(mFont, textureText.c_str(), textColor);
	if (textSurface == NULL){
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	} else {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (mTexture == NULL) {
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		// get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	// return success
	return mTexture != NULL;
}

void LTexture::freeTexture(){
	// free texture if it exists
	if (mTexture != NULL){
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
	if (mFont != NULL) {
		TTF_CloseFont(mFont);
		mFont = NULL;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
	// modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending){
	// set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	// modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::setFont(std::string font, int size) {
	if (mFont != NULL) {
		TTF_CloseFont(mFont);
	}
	mFont = TTF_OpenFont(font.c_str(), size);
}

void LTexture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
	// set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	// set clip rendering dimensions
	if (clip != NULL){
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// render to screen
	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth(){
	return mWidth;
}

int LTexture::getHeight(){
	return mHeight;
}