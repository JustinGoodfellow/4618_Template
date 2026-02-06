#pragma once
#include "CControl.h"

#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

class CBase4618
{
private:
	CControl control;
	cv::Mat _canvas;
	virtual void update(void);
	virtual void draw(void);
public:
	virtual void run(void);
};

