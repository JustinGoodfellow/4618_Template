#pragma once
#include "CBase4618.h"

class CSketch : public CBase4618
{
public:
	CSketch(cv::Size size, int port);
	CSketch(void);
private:
	int _xcanvas, _ycanvas;
	float _xinput, _yinput;
	int _shake;
	int _reset;
	int _colour_button;
	cv::Scalar _colour;
	void gpio(void);
	void update(void);
	void draw(void);
};