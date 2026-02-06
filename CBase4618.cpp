#include "stdafx.h"
#include "CBase4618.h"
#include "cvui.h"

void CBase4618::gpio(void)
{
}

void CBase4618::gpioLoop()
{
	while (_running)
	{
		this->gpio();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void CBase4618::update(void)
{
}

void CBase4618::updateLoop()
{
	while (_running)
	{
		this->update();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void CBase4618::draw(void)
{
}

void CBase4618::run(void)
{
	std::thread gpio_thread(&CBase4618::gpioLoop, this);
	std::thread update_thread(&CBase4618::updateLoop, this);

	while (_running)
	{
		draw();
		char key = (char)cv::waitKey(1);
		if (key == 'q')
			_running = false;
	}
	gpio_thread.join();
	update_thread.join();
}
