#include "stdafx.h"
#include "CSketch.h"
#include "cvui.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>

std::mutex _mutex;

CSketch::CSketch(cv::Size size, int port)
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);
	control.init_com(port);
	_canvas = cv::Mat::zeros(size, CV_8UC3);
	cvui::init("Etch-a-Sketch");
	_xcanvas = size.width / 2;
	_ycanvas = size.height / 2;
	_xinput = 50.f;
	_yinput = 50.f;
	_colour = cv::Scalar(0, 0, 255);
	_colour_button = 0;
	_reset = 0;
}

CSketch::CSketch(void)
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);
	control.init_com(3);
	cv::Size size(500, 500);
	_canvas = cv::Mat::zeros(size, CV_8UC3);
	cvui::init("Etch-a-Sketch");
	_xcanvas = size.width / 2;
	_ycanvas = size.height / 2;
	_xinput = 50.f;
	_yinput = 50.f;
	_colour = cv::Scalar(0, 0, 255);
	_colour_button = 0;
	_reset = 0;
}

void CSketch::gpio(void)
{
	control.get_analog(JOYX_CHAN, _xinput);
	control.get_analog(JOYY_CHAN, _yinput);
	control.get_data(ANALOG, ACC_Z_CHAN, _shake);
	_colour_button = control.get_button(BUTTON2_CHAN);
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_reset = control.get_button(BUTTON1_CHAN);
	}
	
	if (_colour == cv::Scalar(255, 0, 0))
	{
		control.set_data(DIGITAL, BLU_LED_CHAN, 1);
		control.set_data(DIGITAL, GRN_LED_CHAN, 0);
		control.set_data(DIGITAL, RED_LED_CHAN, 0);
	}
	else if (_colour == cv::Scalar(0, 255, 0))
	{
		control.set_data(DIGITAL, BLU_LED_CHAN, 0);
		control.set_data(DIGITAL, GRN_LED_CHAN, 1);
		control.set_data(DIGITAL, RED_LED_CHAN, 0);
	}
	else if (_colour == cv::Scalar(0, 0, 255))
	{
		control.set_data(DIGITAL, BLU_LED_CHAN, 0);
		control.set_data(DIGITAL, GRN_LED_CHAN, 0);
		control.set_data(DIGITAL, RED_LED_CHAN, 1);
	}
}

void CSketch::update(void)
{
	if (_xinput > 65.f)
		_xcanvas++;
	else if (_xinput < 35.f)
		_xcanvas--;

	if (_yinput > 65.f)
		_ycanvas--;
	else if (_yinput < 35.f)
		_ycanvas++;
	_xcanvas = (std::max)(0, (std::min)(_xcanvas, _canvas.cols - 1));
	_ycanvas = (std::max)(0, (std::min)(_ycanvas, _canvas.rows - 1));

	if (_colour_button == 1)
	{
		if (_colour == cv::Scalar(255, 0, 0))
		{
			_colour = cv::Scalar(0, 255, 0);
		}
		else if (_colour == cv::Scalar(0, 255, 0))
		{
		_colour = cv::Scalar(0, 0, 255);
		}
		else if (_colour == cv::Scalar(0, 0, 255))
		{
			_colour = cv::Scalar(255, 0, 0);
		}
		_colour_button = 0;
	}

	if (_shake > 2000)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_reset = 1;
	}
}

void CSketch::draw(void)
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (!_reset)
		{
			cv::Point point(_xcanvas, _ycanvas);
			cv::circle(_canvas, point, 1, _colour, -1);
		}
		else if (_reset)
		{
			_canvas = cv::Mat::zeros(_canvas.size(), CV_8UC3);
			_reset = 0;
		}
	}
	cvui::update();
	cv::imshow("Etch-a-Sketch", _canvas);
	Sleep(10);
}