#pragma once
#include "CControl.h"

/**
*
* @brief Base class for 4618 projects
*
* This class contains functions for handling gpio, updating, drawing, and running the program.
*
* @author Justin Goodfellow
*
*/
class CBase4618
{
protected:
	/** @brief Constructor for CSketch class
	*
	*@param size The size of the canvas for the Etch-a-Sketch
	*@param port The COM port number for communication with the control board
	*/
	CControl control; ///< The control object for communication with the control board
	cv::Mat _canvas; ///< The canvas for drawing
	std::atomic<bool> _running{ true }; ///< Flag to indicate whether the program is running
	/** @brief Function to handle GPIO inputs and outputs
	*
	*@return void
	*/
	virtual void gpio(void);
	/** @brief Creates a thread for gpio() to run in a loop
	*
	*@return void
	*/
	void gpioLoop(void);
	/** @brief Function to handle updating the program state
	*
	*@return void
	*/
	virtual void update(void);
	/** @brief Creates a thread for update() to run in a loop
	*
	*@return void
	*/
	void updateLoop();
	/** @brief Function to handle drawing on the canvas
	*
	*@return void
	*/
	virtual void draw(void);
public:
	/** @brief Runs the main program loop and handles exit conditions
	*
	*@return void
	*/
	void run(void);
};

