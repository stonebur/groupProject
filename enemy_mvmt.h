class Enemy
{
public:
	//The dimensions of the player
	static const int ENEMY_WIDTH = 30;
	static const int ENEMY_HEIGHT = 50;

	SDL_Rect collisionBox;

	//Maximum axis velocity of the player
	static const int ENEMY_VEL = 1;

	//Initializes the variables
	Enemy();

    //Moves the player
	void move(SDL_Rect& collision);

	//Shows the player on the screen relative to the camera
	void render(int camX, int camY);


private:
	//The X and Y offsets of the player
	int mPosX, mPosY;

	//The velocity of the player
	int mVelX, mVelY;

	
};

const int CENTER_PATH = 500;

Enemy::Enemy()
{
	
	mPosX = CENTER_PATH;
	mPosY = GROUND_LEVEL - 4;

	//Initialize the velocity
	mVelX = ENEMY_VEL;
	mVelY = 0;

	collisionBox.w = ENEMY_WIDTH;
	collisionBox.h = ENEMY_HEIGHT;
	
}

void Enemy::move(SDL_Rect& collision)
{
	
	if (mPosX > CENTER_PATH + 100)
		mVelX = -1 * ENEMY_VEL;
	else if (mPosX < CENTER_PATH - 100)
		mVelX = ENEMY_VEL;
	
	if (checkCollision(collisionBox, collision))
	{
		mPosX -= mVelX;
		mPosY -= mVelY;
		collisionBox.x = mPosX;
		collisionBox.y = mPosY;
	}
	else
	{
		mPosX += mVelX;
		mPosY += mVelY;
		collisionBox.x = mPosX;
		collisionBox.y = mPosY;
	}
	
	


}

void Enemy::render(int camX, int camY)
{
	enemy.render(mPosX - camX, mPosY - camY);
}