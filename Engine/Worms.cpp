#include "Worms.h"

void Worm::Update(float dt)
{
	pos += vel * dt;
	vel += acc * dt;
	acc *= 0.0f;
}

void Worm::Draw(Graphics& gfx)
{
	gfx.PutPixel(pos.x, pos.y, Colors::White);
}
