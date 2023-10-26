#include "timer.h"

timer::timer()
{
	this->delta_time = 0;
	this->fps = 0;
	this->clock = sf::Clock();
	for (int i = 0; i < 100; i++)
	{
		this->frames[i] = 0;
	}
}

timer::~timer()
{
}

void timer::update()
{
	this->delta_time = this->clock.restart().asSeconds();
	this->fps = 1 / this->delta_time;
	this->update_frames();
}

float timer::get_delta_time()
{
	return this->delta_time;
}

float timer::get_fps()
{
	return this->fps;
}

void timer::update_frames()
{
	for (int i = 0; i < 99; i++)
	{
		this->frames[i] = this->frames[i + 1];
	}
	this->frames[99] = this->fps;
}

float* timer::get_frames()
{
	return this->frames;
}