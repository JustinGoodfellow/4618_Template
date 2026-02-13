#include "stdafx.h"
#include "CPong.h"
#include "cvui.h"

CPong::CPong(cv::Size size, int port)
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);
	control.init_com(port);
	_canvas = cv::Mat::zeros(size, CV_8UC3);
	cvui::init("Pong");
	_gui = cv::Point(25, 25);
	_ball = cv::Point((_canvas.cols / 2), (_canvas.rows / 2));
	_velocity = cv::Point2f(1.f, -0.3f);
	_input = 50.f;
	_paddle1 = _canvas.rows / 2;
	_paddle2 = _canvas.rows / 2;
}

CPong::CPong() : CPong(cv::Size(500, 500), 3)
{
}

CPong::~CPong()
{
}

void CPong::gpio(void)
{

}

void CPong::update(void)
{

}

void CPong::draw(void)
{

}