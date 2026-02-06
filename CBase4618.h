#pragma once
#include "CControl.h"

class CBase4618
{
protected:
	CControl control;
	cv::Mat _canvas;
	std::atomic<bool> _running{ true };
	virtual void gpio(void);
	void gpioLoop(void);
	virtual void update(void);
	void updateLoop();
	virtual void draw(void);
public:
	void run(void);
};

