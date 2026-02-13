#pragma once
#include "CBase4618.h"

class CPong : public CBase4618
{
public:
	CPong(cv::Size size, int port);
	CPong();
	~CPong();
private:
	cv::Point _gui;
	cv::Point2f _ball;
	cv::Point2f _velocity;
	float _input;
	int _paddle1;
	int _paddle2;
	void gpio(void);
	void update(void);
	void draw(void);
};
