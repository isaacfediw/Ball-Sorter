#pragma once
#include "CControl.h"
#include <opencv2/opencv.hpp>

/**
*
* @brief Allows for simple button communication with the Tiva TM4C123G
*
* @author Isaac Fediw
*
*/
class CButton {
private:
	int _flag; ///<used for button stuff
	int _flag_2; ///<used for button stuff
	double _start_time; ///<used for button stuff

public:
	/** @brief CButton constructor
	*
	* @param none
	* @return none
	*/
	CButton();

	/** @brief Gets the specified debounced button input from the microcontroller
	* @brief Assumes the button is active high
	*
	* @param channel The channel which the input is on
	* @return Returns a bool. (True --> Button pressed) (False --> Button not pressed)
	*/
	bool get_button(int channel, CControl& ccontrol);
};
