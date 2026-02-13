#pragma once

#include "Serial.h"

#define JOYX_CHAN 11
#define JOYY_CHAN 4
#define BUTTON1_CHAN 33
#define BUTTON2_CHAN 32
#define GRN_LED_CHAN 38
#define BLU_LED_CHAN 37
#define RED_LED_CHAN 39
#define ACC_X_CHAN 5
#define ACC_Y_CHAN 6
#define ACC_Z_CHAN 7
#define SERVO_CHAN 0
#define SERVO_MIN_ANGLE 1
#define SERVO_MAX_ANGLE 180

enum type {
	DIGITAL = 0,
	ANALOG = 1,
	SERVO = 2
};
/**
*
* @brief Sends and receives data to/from the control board.
*
* This class is used to initialize the COM port and communicate with the control board.
*
* @author Justin Goodfellow
*
*/
class CControl {
private:
	Serial _com; ///< Serial port object for communication

	std::atomic<bool> connected{ false };
	std::mutex com_mutex;

	std::map<int, int> _last_raw_state;
	std::map<int, int> _stable_state;
	std::map<int, std::chrono::steady_clock::time_point> _last_change_time;

public:
	CControl();
	~CControl();

	/** @brief Initializes the COM port for communication.
	*
	*@param comport The number of the COM port to initialize
	*@return nothing to return
	*/
	void init_com(int comport = -1);

	/** @brief Gets digital or analog data from the control board.
	*
	*@param type The type of data to get (digital or analog)
	*@param channel The channel number to get data from
	*@param result Reference to store the retrieved data
	*@return boolean indicating success or failure
	*/
	bool get_data(int type, int channel, int& result);

	/** @brief Sets digital or analog data on the control board.
	* 
	*@param type The type of data to set (digital or analog)
	*@param channel The channel number to set data on
	*@param val The value to set
	*@return boolean indicating success or failure
	*/
	bool set_data(int type, int channel, int val);

	/** @brief Gets analog data as a %
	* 
	*@param channel The channel number to get data from
	*@param result Reference to store the retrieved data
	*@return boolean indicating success or failure
	*/
	bool get_analog(int channel, float& result);

	/** @brief Gets the state of a button
	*
	*@param channel The channel number of the button
	*@return boolean indicating if the button is pressed
	*/
	bool get_button(int channel);
};