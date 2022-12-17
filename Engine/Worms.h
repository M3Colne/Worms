#pragma once

#include "Graphics.h"

struct Worm
{
	Vec2 pos{};
	Vec2 vel{};
	Vec2 acc{};
	
	void Update(float dt);
	void Draw(Graphics& gfx);
};