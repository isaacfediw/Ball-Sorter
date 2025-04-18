#include "CBase4618.h"
#include <thread>
#include <chrono>

//#define MULTI_THREADED

void CBase4618::run() {
#ifdef MULTI_THREADED
	std::thread gpio_thread(&CBase4618::gpio_thread, this);
	std::thread update_thread(&CBase4618::update_thread, this);
	std::thread draw_thread(&CBase4618::draw_thread, this);

	gpio_thread.join();
	update_thread.join();
	draw_thread.join();
#else
	double start;
	double wait;
	double elapsed;
	double freq = cv::getTickFrequency();
	_start_time = cv::getTickCount();
	_start_time2 = cv::getTickCount();

    std::thread t(&CBase4618::server_thread, this);
    std::thread update_thread(&CBase4618::update_thread, this);

    t.detach();
    update_thread.detach();

	do {
		//gpio();
        //update();

		wait = (cv::getTickCount() - _start_time) / freq;
		if (wait > (DRAW_RATE / 1000)) {
			start = cv::getTickCount();
			draw();

			elapsed = (cv::getTickCount() - start) / freq;
			_fps = 1 / (wait + elapsed);

			_start_time = cv::getTickCount();
		}
	} while (!(cv::waitKey(1) == 'q' || _state == END));

    server.stop();
#endif
}

void CBase4618::server_thread() {
    server.start(4618);
}

void CBase4618::gpio_thread() {
	do {
		gpio();
	} while (_state != END);
}

void CBase4618::update_thread() {
	do {
		//auto end_time = std::chrono::system_clock::now() + std::chrono::milliseconds((long) _gamespeed);
		update();
		//std::this_thread::sleep_until(end_time);
	} while (_state != END);
}

void CBase4618::draw_thread() {
	cvui::init(CANVAS_NAME);
    _vid.open(0);
	//_canvas = cv::Mat::zeros(_size.height, _size.width, CV_8UC3); // Initialize the canvas to be empty with the set width and height
	double freq = cv::getTickFrequency();
	double start;
	double elapsed;

	do {
		start = cv::getTickCount();

		auto end_time = std::chrono::system_clock::now() + std::chrono::milliseconds((long) DRAW_RATE - 2);
		draw();
		std::this_thread::sleep_until(end_time);

		elapsed = (cv::getTickCount() - start) / freq;
		_fps = 1 / elapsed;

		if (cv::waitKey(1) == 'q') _state = END;

	} while (_state != END);
}

// Please only make type digital or analog, servo has no function in snake
bool CBase4618::gpio_helper(CControl::type type, int channel, bool write, int& val, bool percent, int dir, int n) {
	if (type == CControl::DIGITAL) {
		if (write) _ccontrol.set_data(type, channel, val); // digital write
		else _ccontrol.get_data(type, channel, val); // digital read
	}

	if (type == CControl::ANALOG) {
		if (write) return false; // analog write not supported

		int percent_value = _ccontrol.get_analog(channel, n, val); // analog read
		if (percent) val = percent_value;
	}

	if (type == CControl::SERVO) {
        if (!write) _ccontrol.get_data(CControl::SERVO, channel, val);
        else _ccontrol.set_data(CControl::SERVO, channel, val);
	}

	if (type == CControl::STEPPER) {
        if (!write) return false; // stepper read not supported

        _ccontrol.set_data(CControl::STEPPER, channel, val, dir);
	}

	return true;
}




