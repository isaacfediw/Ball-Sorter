#pragma once

#include <opencv2/opencv.hpp>
#include "CControl.h"
#include "cvui.h"
#include "server.h"

#define S1 17 // S1 on booster pack
#define S2 27 // S2 on booster pack

#define JOY_X 0 // SPI CH0
#define JOY_Y 1 // SPI CH1

#define RGB_BLUE  5
#define RGB_GREEN 6
#define RGB_RED   13

#define DRAW_RATE 23.3

#define CANVAS_NAME "Ball Sorter"

/**<CBase4618.h>
*
* @brief Base class for all ELEX 4618 Labs
*
* @author Isaac Fediw
*
*/
class CBase4618 {
protected:
	CControl _ccontrol; ///<CControl object
	cv::Mat _canvas; ///<Mat object. Used for drawing onto
	cv::Mat _canvas_copy; ///<Copy of the canvas so that it makes a new one every iteration
    cv::VideoCapture _vid;
	cv::Rect _canvas_box; ///<Rectangle the same size as the canvas

	//std::mutex _mtx; ///<Mutex object to protect variables in multi threading

	/**
	* @brief Holds all possible game states
	*/
	enum _gamestate {START = 0, RESET, END, GAMEOVER};
	_gamestate _state; ///<Defines what state the game is in

	double _gamespeed; ///<Holds the speed of the game
	double _fps; ///<Holds the fps of the game
	double _start_time; ///<Used for non blocking delays
	double _start_time2; ///<Used for non blocking delays

	int _score; ///<Holds the player's score

	cv::Size _size; ///<The size of the game window

	cv::Point _gui_position; ///<Top left corner of the gui

	int _blue; ///<Value for blue led on rgb led
	int _green; ///<Value for green on rgb led
	int _red; ///<Value for red led on rgb led

	CServer server;

	/** @brief Performs reads and write operations to interact with the Tiva TM4C123C
	*
	* @param none
	* @return none
	*/
	virtual void gpio() = 0;


	/** @brief Updates the game and gets the current game state
	*
	* @param none
	* @return nothing to return
	*/
	virtual void update() = 0;


	/** @brief Performs all drawing on the image based on the current state of the game
	*
	* @param none
	* @return nothing to return
	*/
	virtual void draw() = 0;

	/** @brief Used to access any gpio pin
	*
	* @param type The type of input (DIGITAL, ANALOG, SERVO, BUTTON)
	* @param channel The channel which the input is on
	* @param write True if writing, False if reading
	* @param result The variable you want the result to be stored in
	* @param percent True if you want the value to be returned as a percentage (for analog only)
	* @param n Number of bit ADC (Ex. 12) Optional if type != ANALOG
	* @return returns a bool stating if the interaction was successful. If type == BUTTON, true means pressed, false means not pressed
	*/
	bool gpio_helper(CControl::type type, int channel, bool write, int& val, bool percent = false, int dir = 1, int n = 12);

public:
	/** @brief Calls gpio(), update(), and draw() in a continuos loop. User can press 'q' to exit.
	*
	* @param none
	* @return nothing to return
	*/
	void run();

private:
	/** @brief the thread for gpio()
	*
	* @param none
	* @return nothing to return
	*/
	 void gpio_thread(/*CBase4618* ptr*/);

	/** @brief the thread for update()
	*
	* @param none
	* @return nothing to return
	*/
	 void update_thread(/*CBase4618* ptr*/);

	/** @brief the thread for draw()
	*
	* @param none
	* @return nothing to return
	*/
	 void draw_thread(/*CBase4618* ptr*/);

	 void server_thread();
};
