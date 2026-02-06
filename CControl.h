#include "Serial.h"

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