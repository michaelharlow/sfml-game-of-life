#pragma once

#include <SFML/Graphics.hpp>

class timer
{
	public:
		timer();
		~timer();
		void update();
		void update_frames();
		float get_delta_time();
		float get_fps();
		float* get_frames();

	private:
		float delta_time;
		float fps;
		sf::Clock clock;
		float frames[100];
};

