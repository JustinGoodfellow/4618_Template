#include "stdafx.h"
#include "CBase4618.h"

void CBase4618::update(void)
{

}

void CBase4618::draw(void)
{

}

void CBase4618::run(void)
{
	while (1)
	{
		update();
		draw();
		if (cv::waitKey)
		{
			break;
		}
	}
	return;
}
