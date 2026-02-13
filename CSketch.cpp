#include "stdafx.h"
#include "CSketch.h"
#include "cvui.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>

std::mutex _reset_mutex;
std::mutex _colour_mutex;

CSketch::CSketch(cv::Size size = cv::Size(500, 500), int port = 3)
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
	_colour_change = 0;
	_reset = 0;
	_shake = 0;
	_last_button = false;
	_xgui = 10;
	_ygui = 10;
}

CSketch::CSketch(void) : CSketch(cv::Size(500, 500), 3)
{
}

CSketch::~CSketch(void)
{
	cv::destroyWindow("Etch-a-Sketch");
}

void CSketch::gpio(void)
{
	control.get_analog(JOYX_CHAN, _xinput);
	control.get_analog(JOYY_CHAN, _yinput);
	control.get_data(ANALOG, ACC_X_CHAN, _shake);

	if (control.get_button(BUTTON1_CHAN))
	{
		std::lock_guard<std::mutex> lock(_reset_mutex);
		_reset = 1;
	}
	{
		std::lock_guard<std::mutex> lock(_colour_mutex);
		bool colour_button = control.get_button(BUTTON2_CHAN);
		if (colour_button && !_last_button)
		{
			_colour_change = 1;
		}
		_last_button = colour_button;
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
	_xcanvas += static_cast<int>((_xinput - 50.f) / 10.f);
	_ycanvas -= static_cast<int>((_yinput - 50.f) / 10.f);

	_xcanvas = (std::max)(0, (std::min)(_xcanvas, _canvas.cols - 1));
	_ycanvas = (std::max)(0, (std::min)(_ycanvas, _canvas.rows - 1));

	if (_colour_change == 1)
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
		{
			std::lock_guard<std::mutex> lock(_colour_mutex);
			_colour_change = 0;
		}
	}
		if (_shake > 3500)
		{
			_reset = 1;
		}
}

void CSketch::draw(void)
{
		if (!_reset)
		{
			cv::Point point(_xcanvas, _ycanvas);
			cv::circle(_canvas, point, 3, _colour, -1);
		}
		else if (_reset)
		{
			std::lock_guard<std::mutex> lock(_reset_mutex);
			_canvas = cv::Mat::zeros(_canvas.size(), CV_8UC3);
			_reset = 0;
		}
		cvui::context("Etch-a-Sketch");
		if (cvui::button(_canvas, _xgui, _ygui, "Reset"))
		{
			_canvas = cv::Mat::zeros(_canvas.size(), CV_8UC3);
		}
		cvui::context("Etch-a-Sketch");
		if (cvui::button(_canvas, _xgui, _ygui + 40, "Exit"))
		{
			_running = false;
		}
	cvui::update();
	cv::imshow("Etch-a-Sketch", _canvas);
	Sleep(1);
}