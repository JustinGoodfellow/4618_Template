#pragma once
#include "CBase4618.h"

/**
*
* @brief Etch-a-Sketch program
*
* This class implements an Etch-a-Sketch program
*
* @author Justin Goodfellow
* 
*/
class CSketch : public CBase4618
{

public:
	/** @brief Constructor for CSketch class
	*
	*@param size The size of the canvas for the Etch-a-Sketch
	*@param port The COM port number for communication with the control board
	*/
	CSketch(cv::Size size, int port);
	CSketch(void);
	~CSketch(void);
private:
	cv::Point _canvas; ///< The current position of the "pen" on the canvas
	cv::Point _gui; ///< The position of the GUI elements on the canvas
	cv::Point2f _input; ///< The current input values from the control board for x and y directions
	//int _xcanvas, _ycanvas;
	//int _xgui, _ygui;
	//float _xinput, _yinput;
	int _shake; ///< The current input value from the accelerometer for shake detection
	int _reset; ///< Flag to indicate whether the canvas should be reset
	int _colour_change; ///< Flag to indicate whether the colour should be changed
	bool _last_button; ///< The last state of the colour change button
	cv::Scalar _colour; ///< The current colour of the "pen" on the canvas
	void gpio(void); ///< Method to read inputs from the control board and update internal state
	void update(void); ///< Method to update the position and state of the Etch-a-Sketch
	void draw(void); ///< Method to draw on the canvas based on the current state
};