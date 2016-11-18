#include "GameEngine.h"

Player::Player()
{
	position = VECTOR2D(0, 0);
	facingDirection = "up";
	moveSpeed = 400.0f;
	isPositionUpdated = false;
}

Player::~Player()
{
	
}

void Player::AddFrontAnimation(AnimatedBitmap anim)
{
	frontAnim = anim;
	frontAnim.SetPosition(position);
}

void Player::AddLeftAnimation(AnimatedBitmap anim)
{
	leftAnim = anim;
	leftAnim.SetPosition(position);
}

void Player::AddRightAnimation(AnimatedBitmap anim)
{
	rightAnim = anim;
	rightAnim.SetPosition(position);
}

void Player::AddBackAnimation(AnimatedBitmap anim)
{
	backAnim = anim;
	backAnim.SetPosition(position);
}

void Player::SetPosition(float x, float y)
{
	SetPosition(VECTOR2D(x, y));

}

void Player::SetPosition(VECTOR2D& pos)
{
	position = pos;
	backAnim.SetPosition(pos);
	frontAnim.SetPosition(pos);
	leftAnim.SetPosition(pos);
	rightAnim.SetPosition(pos);
	isPositionUpdated = true;

}

VECTOR2D Player::GetPosition() const
{
	return position;
}

float Player::GetMoveSpeed() const
{
	return moveSpeed;
}

void Player::GetBoundingRect(RECT2D& ret) 
{
	frontAnim.GetBoundingRectangle(ret);
	
}

void Player::SetDirection(string dir)
{
	facingDirection = dir;
}

void Player::Draw(DWORD* vmem, int lpitch32)
{
	if (!isPositionUpdated)
	{
		if (facingDirection == "up")
		{
			RECT2D r;
			backAnim.GetBoundingRectangle(r);
			DrawBitmapWithClipping(vmem, lpitch32, backAnim.GetFrame(0), int(r.GetPosition().x + 0.5f), int(r.GetPosition().y + 0.5f), NULL);
		}
		else if (facingDirection == "down")
		{
			RECT2D r;
			frontAnim.GetBoundingRectangle(r);
			DrawBitmapWithClipping(vmem, lpitch32, frontAnim.GetFrame(0), int(r.GetPosition().x + 0.5f), int(r.GetPosition().y + 0.5f), NULL);
		}
		else if (facingDirection == "left")
		{
			RECT2D r;
			leftAnim.GetBoundingRectangle(r);
			DrawBitmapWithClipping(vmem, lpitch32, leftAnim.GetFrame(0), int(r.GetPosition().x + 0.5f), int(r.GetPosition().y + 0.5f), NULL);
		}
		else if (facingDirection == "right")
		{
			RECT2D r;
			rightAnim.GetBoundingRectangle(r);
			DrawBitmapWithClipping(vmem, lpitch32, rightAnim.GetFrame(0), int(r.GetPosition().x + 0.5f), int(r.GetPosition().y + 0.5f), NULL);
		}

		return;
	}

	if (facingDirection == "up")
	{
		backAnim.Draw(vmem, lpitch32);
	}
	else if (facingDirection == "down")
	{
		frontAnim.Draw(vmem, lpitch32);
	}
	else if (facingDirection == "left")
	{
		leftAnim.Draw(vmem, lpitch32);
	}
	else if (facingDirection == "right")
	{
		rightAnim.Draw(vmem, lpitch32);
	}

	isPositionUpdated = false;
}

World::World()
{
	userInterface = (NULL);
	player = (NULL);
}

void World::SoftwareDraw(DWORD* mem, int lpitch32)
{
	
	if(player)
		player->Draw(mem, lpitch32);

 	vector<AnimatedBitmap>::iterator vIter = collisionObjects.begin();
	for (vIter; vIter < collisionObjects.end(); vIter++)
	{
		vIter->Draw(mem, lpitch32);
	}
}

void World::HardwareDraw(double deltaTime)
{
	if(userInterface)
		userInterface->DrawUI(deltaTime);
}