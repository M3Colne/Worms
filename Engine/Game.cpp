/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include <numbers>
#include <algorithm>

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
	std::random_device rn;
	std::mt19937 rng(rn());
	std::uniform_real_distribution<float> urd(0.0f, 2.0f * std::numbers::pi);

	for (auto& worm : worms)
	{
		worm.pos = Vec2(float(rand() % Graphics::ScreenWidth), float(rand() % Graphics::ScreenHeight));
		worm.vel = Vec2(wormSpeed, 0.0f);
		worm.vel.Rotate(urd(rng));
	}
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	const float DT = ft.Mark();
	eta += DT;
	if (eta > 100.0f)
	{
		eta = 0.0f;
	}

	if (wnd.mouse.LeftIsPressed())
	{
		mousePressed = true;
	}
	else
	{
		mousePressed = false;
	}

	UpdateTrails(DT);

	const Vec2 mousePos(float(wnd.mouse.GetPosX()), float(wnd.mouse.GetPosY()));
	for (int i = 0; i < nWorms; i++)
	{
		if (mousePressed)
		{
			worms[i].acc += (mousePos - worms[i].pos).GetNormalized() * 6.0f * wormSpeed;
			CapSpeed(worms[i].vel, 4.5f * wormSpeed);
		}
		else
		{
			worms[i].vel.Rotate((std::numbers::pi / 8.0f) * Noise::Transform01toN1P1(Noise::PerlinNoise_1D(eta + float(i), 20.0f, 100.0f, 8)));
			worms[i].vel = worms[i].vel.GetNormalized() * wormSpeed;
		}

		worms[i].Update(DT);
	}
	ClampToScreen();
}

void Game::UpdateTrails(float dt)
{
	for (int i = 0; i < nWorms; i++)
	{
		trails.push_back(Trail{ .pos = Vei2(worms[i].pos.x, worms[i].pos.y) });
	}

	for (auto i = trails.begin(); i != trails.end(); ++i)
	{
		i->color -= trailDiffusionRate * dt;
	}
	std::erase_if(trails, [](const auto& v) {return v.color <= 0.0f; });
}

void Game::UpdateWormVelocity()
{
	for (auto& worm : worms)
	{
		worm.vel.Rotate(1.5707963f * Noise::Transform01toN1P1(Noise::PerlinNoise_1D(eta)));
	}
}

void Game::ClampToScreen()
{
	for (auto& worm : worms)
	{
		if (worm.pos.x >= Graphics::ScreenWidth)
		{
			worm.pos.x = Graphics::ScreenWidth - 1;
			worm.vel.x *= -1.0f;
		}
		if (worm.pos.x < 0)
		{
			worm.pos.x = 0;
			worm.vel.x *= -1.0f;
		}
		if (worm.pos.y >= Graphics::ScreenHeight)
		{
			worm.pos.y = Graphics::ScreenHeight - 1;
			worm.vel.y *= -1.0f;
		}
		if (worm.pos.y < 0)
		{
			worm.pos.y = 0;
			worm.vel.y *= -1.0f;
		}
	}
}

void Game::CapSpeed(Vec2& v, float maxS)
{
	if (v.GetLength() > maxS)
	{
		v.Normalize()* maxS;
	}
}

void Game::ComposeFrame()
{	
	for (int i = 0; i < nWorms; i++)
	{
		worms[i].Draw(gfx);
	}
	for (auto& trail : trails)
	{
		gfx.PutPixel(trail.pos.x, trail.pos.y, unsigned char(trail.color), unsigned char(trail.color), unsigned char(trail.color));
	}
}
