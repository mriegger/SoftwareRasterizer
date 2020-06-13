/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

#define PERSP_CORRECT

//Screen dimension constants
const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;

////Starts up SDL and creates window
//bool init();
//
////Loads media
//bool loadMedia();

////Frees media and shuts down SDL
//void close();
//
////The window we'll be rendering to
//SDL_Window* gWindow = NULL;
//
////The surface contained by the window
//SDL_Surface* gScreenSurface = NULL;
//
////The image we will load and show on the screen
//SDL_Surface* gHelloWorld = NULL;
//SDL_Renderer* renderer = NULL;

//
//bool init()
//{
//	//Initialization flag
//	bool success = true;
//
//	//Initialize SDL
//	if (SDL_Init(SDL_INIT_VIDEO) < 0)
//	{
//		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
//		success = false;
//	}
//	else
//	{
//		//Create window
//		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//		if (gWindow == NULL)
//		{
//			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
//			success = false;
//		}
//		else
//		{
//			//Get window surface
//			gScreenSurface = SDL_GetWindowSurface(gWindow);
//		}
//	}
//
//	return success;
//}
//
//bool loadMedia()
//{
//	//Loading success flag
//	bool success = true;
//
//	//Load splash image
//	gHelloWorld = SDL_LoadBMP("hello_world.bmp");
//	if (gHelloWorld == NULL)
//	{
//		printf("Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError());
//		success = false;
//	}
//
//	return success;
//}

//void close()
//{
//	//Deallocate surface
//	SDL_FreeSurface(gHelloWorld);
//	gHelloWorld = NULL;
//
//	//Destroy window
//	SDL_DestroyWindow(gWindow);
//	gWindow = NULL;
//
//	//Quit SDL subsystems
//	SDL_Quit();
//}

void ClearSceenToBlack(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
}

typedef float Vec2[2];
typedef float Vec3[3];
typedef unsigned char Rgb[3];

inline float edgeFunction(const Vec3& a, const Vec3& b, const Vec3& c)
{
	return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
}

int main(int argc, char* args[])
{
	SDL_Window* win = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* bitmapTex = NULL;
	SDL_Surface* bitmapSurface = NULL;
	int posX = 100, posY = 100;

	SDL_Init(SDL_INIT_VIDEO);

	win = SDL_CreateWindow("Hello World", posX, posY, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	bitmapSurface = SDL_LoadBMP("img/hello.bmp");
	bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
	SDL_FreeSurface(bitmapSurface);

	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}

		ClearSceenToBlack(renderer);
		SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);

		Vec3 v2 = { -48, -10,  82 };
		Vec3 v1 = { 29, -15,  44 };
		Vec3 v0 = { 13,  34, 114 };
		Vec3 c2 = { 1, 0, 0 };
		Vec3 c1 = { 0, 1, 0 };
		Vec3 c0 = { 0, 0, 1 };

		const uint32_t w = 512;
		const uint32_t h = 512;

		// project triangle onto the screen
		v0[0] /= v0[2], v0[1] /= v0[2];
		v1[0] /= v1[2], v1[1] /= v1[2];
		v2[0] /= v2[2], v2[1] /= v2[2];
		// convert from screen space to NDC then raster (in one go)
		v0[0] = (1 + v0[0]) * 0.5 * w, v0[1] = (1 + v0[1]) * 0.5 * h;
		v1[0] = (1 + v1[0]) * 0.5 * w, v1[1] = (1 + v1[1]) * 0.5 * h;
		v2[0] = (1 + v2[0]) * 0.5 * w, v2[1] = (1 + v2[1]) * 0.5 * h;

#ifdef PERSP_CORRECT 
		// divide vertex-attribute by the vertex z-coordinate
		c0[0] /= v0[2], c0[1] /= v0[2], c0[2] /= v0[2];
		c1[0] /= v1[2], c1[1] /= v1[2], c1[2] /= v1[2];
		c2[0] /= v2[2], c2[1] /= v2[2], c2[2] /= v2[2];
		// pre-compute 1 over z
		v0[2] = 1 / v0[2], v1[2] = 1 / v1[2], v2[2] = 1 / v2[2];
#endif 

		//Rgb* framebuffer = new Rgb[w * h];
		//memset(framebuffer, 0x0, w * h * 3);

		float area = edgeFunction(v0, v1, v2);

		for (uint32_t j = 0; j < h; ++j) 
		{
			for (uint32_t i = 0; i < w; ++i) 
			{
				Vec3 p = { i + 0.5, h - j + 0.5, 0 };
				float w0 = edgeFunction(v1, v2, p);
				float w1 = edgeFunction(v2, v0, p);
				float w2 = edgeFunction(v0, v1, p);
				if (w0 >= 0 && w1 >= 0 && w2 >= 0) 
				{
					w0 /= area;
					w1 /= area;
					w2 /= area;
					float r = w0 * c0[0] + w1 * c1[0] + w2 * c2[0];
					float g = w0 * c0[1] + w1 * c1[1] + w2 * c2[1];
					float b = w0 * c0[2] + w1 * c1[2] + w2 * c2[2];
#ifdef PERSP_CORRECT 
					float z = 1 / (w0 * v0[2] + w1 * v1[2] + w2 * v2[2]);
					// if we use perspective correct interpolation we need to
					// multiply the result of this interpolation by z, the depth
					// of the point on the 3D triangle that the pixel overlaps.
					r *= z, g *= z, b *= z;
#endif 
					SDL_SetRenderDrawColor(renderer, r*255, g*255, b*255, SDL_ALPHA_OPAQUE);
					SDL_RenderDrawPoint(renderer, i, j);
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(bitmapTex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

	SDL_Quit();

	return 0;

	////Start up SDL and create window
	//if (!init())
	//{
	//	printf("Failed to initialize!\n");
	//}
	//elses
	//{
	//	//Load media
	//	if (!loadMedia())
	//	{
	//		printf("Failed to load media!\n");
	//	}
	//	else
	//	{
	//		//Apply the image
	//		SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);


	//		//Update the surface
	//		SDL_UpdateWindowSurface(gWindow);

	//		//Wait 15 seconds
	//		SDL_Delay(15000);
	//	}
	//}

	////Free resources and close SDL
	//close();

	//return 0;
}


