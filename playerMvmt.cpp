/*Portions of this code are originally from Lazy Foo' Productions
(http://lazyfoo.net/)*/

//Using SDL, SDL_image, standard IO, vectors, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>

//The dimensions of the level
const int LEVEL_WIDTH = 1513;
const int LEVEL_HEIGHT = 480;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Deallocates texture
		void free();
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL);

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The player sprite that moves on screen
class Player
{
    public:
		//The dimensions of the player
		static const int PLAYER_WIDTH = 30;
		static const int PLAYER_HEIGHT = 50;

		//Maximum axis velocity of the player
		static const int PLAYER_VEL = 3;

		//Initializes the variables
		Player();

		//Takes key presses and adjusts the player's velocity
		void handleEvent( SDL_Event& e );

		//Moves the player
		void move();

		//Shows the player on the screen relative to the camera
		void render( int camX, int camY, int walkFrame, std::string direction);

		//Position accessors
		int getPosX();
		int getPosY();

    private:
		//The X and Y offsets of the player
		int mPosX, mPosY;

		//The velocity of the player
		int mVelX, mVelY;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture samusRight;
LTexture samusLeft;
LTexture samusStanding;
LTexture background;

SDL_Rect samusRightClips[10];
SDL_Rect samusLeftClips[10];
SDL_Rect samusStandingClips[2];

int WALK_RIGHT = 0;
int WALK_LEFT = 0;


LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 106, 147, 113 ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}



void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}




void LTexture::render( int x, int y, SDL_Rect* clip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Player::Player()
{
    //Initialize the offsets
    mPosX = 0;
    mPosY = 390;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Player::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {	
			//no y movement at this time
            //case SDLK_UP: mVelY -= PLAYER_VEL; break;
            //case SDLK_DOWN: mVelY += PLAYER_VEL; break;
            case SDLK_LEFT: mVelX -= PLAYER_VEL; break;
            case SDLK_RIGHT: mVelX += PLAYER_VEL; break;

        }
    }
	

    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            //case SDLK_UP: mVelY += PLAYER_VEL; break;
            //case SDLK_DOWN: mVelY -= PLAYER_VEL; break;
            case SDLK_LEFT: mVelX += PLAYER_VEL; break;
            case SDLK_RIGHT: mVelX -= PLAYER_VEL; break;
				
        }
    }
		
}

void Player::move()
{
    //Move the player left or right
    mPosX += mVelX;

    //If the player went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + PLAYER_WIDTH > LEVEL_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
    }

    //Move the player up or down
    mPosY += mVelY;

    //If the player went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + PLAYER_HEIGHT > LEVEL_HEIGHT ) )
    {
        //Move back
        mPosY -= mVelY;
    }
	
}

void Player::render( int camX, int camY, int walkFrame, std::string direction)
{
    //Show the player relative to the camera
	if (direction == "right")
		samusRight.render(mPosX - camX, mPosY - camY, &samusRightClips[walkFrame]);
	else if (direction == "left")
		samusLeft.render(mPosX - camX, mPosY - camY, &samusLeftClips[walkFrame]);
	else if (direction == "stand")
		samusStanding.render(mPosX - camX, mPosY - camY, &samusStandingClips[1]);
	
}

int Player::getPosX()
{
	return mPosX;
}

int Player::getPosY()
{
	return mPosY;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//load samus right movement texture
	if( !samusRight.loadFromFile( "samusRight.png" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
	//load samus left movement texture
	if (!samusLeft.loadFromFile("samusLeft.png"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}
	//load samus standing still
	if (!samusStanding.loadFromFile("samusStanding.png"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	//Load background texture
	if( !background.loadFromFile( "smbg.png" ) )
	{
		printf( "Failed to load background texture!\n" );
		success = false;
	}
	else
	{
		//Sprite clips

		//right movement
		samusRightClips[0].x = 0;
		samusRightClips[0].y = 0;
		samusRightClips[0].w = 26;
		samusRightClips[0].h = 37;

		samusRightClips[1].x = 26;
		samusRightClips[1].y = 0;
		samusRightClips[1].w = 26;
		samusRightClips[1].h = 37;

		samusRightClips[2].x = 56;
		samusRightClips[2].y = 0;
		samusRightClips[2].w = 26;
		samusRightClips[2].h = 37;

		samusRightClips[3].x = 86;
		samusRightClips[3].y = 0;
		samusRightClips[3].w = 34;
		samusRightClips[3].h = 37;

		samusRightClips[4].x = 120;
		samusRightClips[4].y = 0;
		samusRightClips[4].w = 26;
		samusRightClips[4].h = 37;

		samusRightClips[5].x = 150;
		samusRightClips[5].y = 0;
		samusRightClips[5].w = 26;
		samusRightClips[5].h = 37;

		samusRightClips[6].x = 175;
		samusRightClips[6].y = 0;
		samusRightClips[6].w = 26;
		samusRightClips[6].h = 37;

		samusRightClips[7].x = 203;
		samusRightClips[7].y = 0;
		samusRightClips[7].w = 26;
		samusRightClips[7].h = 37;

		samusRightClips[8].x = 236;
		samusRightClips[8].y = 0;
		samusRightClips[8].w = 35;
		samusRightClips[8].h = 37;

		samusRightClips[9].x = 275;
		samusRightClips[9].y = 0;
		samusRightClips[9].w = 26;
		samusRightClips[9].h = 37;

		//left movement
		samusLeftClips[0].x = 0;
		samusLeftClips[0].y = 0;
		samusLeftClips[0].w = 34;
		samusLeftClips[0].h = 37;

		samusLeftClips[1].x = 34;
		samusLeftClips[1].y = 0;
		samusLeftClips[1].w = 33;
		samusLeftClips[1].h = 37;

		samusLeftClips[2].x = 67;
		samusLeftClips[2].y = 0;
		samusLeftClips[2].w = 33;
		samusLeftClips[2].h = 37;

		samusLeftClips[3].x = 100;
		samusLeftClips[3].y = 0;
		samusLeftClips[3].w = 35;
		samusLeftClips[3].h = 37;

		samusLeftClips[4].x = 135;
		samusLeftClips[4].y = 0;
		samusLeftClips[4].w = 25;
		samusLeftClips[4].h = 37;

		samusLeftClips[5].x = 160;
		samusLeftClips[5].y = 0;
		samusLeftClips[5].w = 29;
		samusLeftClips[5].h = 37;

		samusLeftClips[6].x = 189;
		samusLeftClips[6].y = 0;
		samusLeftClips[6].w = 31;
		samusLeftClips[6].h = 37;

		samusLeftClips[7].x = 220;
		samusLeftClips[7].y = 0;
		samusLeftClips[7].w = 35;
		samusLeftClips[7].h = 37;

		samusLeftClips[8].x = 255;
		samusLeftClips[8].y = 0;
		samusLeftClips[8].w = 34;
		samusLeftClips[8].h = 37;

		samusLeftClips[9].x = 289;
		samusLeftClips[9].y = 0;
		samusLeftClips[9].w = 28;
		samusLeftClips[9].h = 37;

		//samus standing still
		samusStandingClips[0].x = 0;
		samusStandingClips[0].y = 0;
		samusStandingClips[0].w = 30;
		samusStandingClips[0].h = 37;

		samusStandingClips[1].x = 30;
		samusStandingClips[1].y = 0;
		samusStandingClips[1].w = 30;
		samusStandingClips[1].h = 37;

	}
	
	return success;
}

void close()
{
	//Free loaded images
	samusRight.free();
	background.free();
	samusLeft.free();
	samusStanding.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;


			//Event handler
			SDL_Event e;

			//The player that will be moving around on the screen
			Player p1;

			//The camera area
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			const Uint8* keystates = SDL_GetKeyboardState(NULL);

			//While application is running
			while( !quit )
			{
			
				
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					p1.handleEvent( e );
				}

				bool rMvmt = 0;
				bool lMvmt = 0;
				bool stand = 0;

				if (keystates[SDL_SCANCODE_RIGHT])
				{
					
					if (WALK_RIGHT < 39)
						WALK_RIGHT++;
					else
						WALK_RIGHT = 0;

					rMvmt = 1;
				}
				else if (keystates[SDL_SCANCODE_LEFT])
				{
					if (WALK_LEFT < 39)
						WALK_LEFT++;
					else
						WALK_LEFT = 0;

					lMvmt = 1;
				}
				else 
				{
					stand = 1;
				}
				//Move the player
				p1.move();

				//Center the camera over the player
				camera.x = ( p1.getPosX() + Player::PLAYER_WIDTH / 2 ) - SCREEN_WIDTH / 2;
				camera.y = ( p1.getPosY() + Player::PLAYER_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

				//Keep the camera in bounds
				if( camera.x < 0 )
				{ 
					camera.x = 0;
				}
				if( camera.y < 0 )
				{
					camera.y = 0;
				}
				if( camera.x > LEVEL_WIDTH - camera.w )
				{
					camera.x = LEVEL_WIDTH - camera.w;
				}
				if( camera.y > LEVEL_HEIGHT - camera.h )
				{
					camera.y = LEVEL_HEIGHT - camera.h;
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render background
				background.render( 0, 0, &camera );

				//Render objects
				if(rMvmt)
					p1.render(camera.x, camera.y, WALK_RIGHT / 4, "right");
				else if(lMvmt)
					p1.render(camera.x, camera.y, WALK_LEFT / 4, "left");
				else if(stand)
					p1.render(camera.x, camera.y, NULL, "stand");
				


				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}