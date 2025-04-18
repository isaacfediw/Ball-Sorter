#include "CButton.h"

using namespace cv;
using namespace std;

CButton::CButton() {
	_flag = false;
    _flag_2 = false;
    _start_time = 0;
}

bool CButton::get_button(int channel, CControl& ccontrol) {
    int result;

    ccontrol.get_data(CControl::DIGITAL, channel, result);

    if (result == 1 && !_flag && !_flag_2) {
        _start_time = cv::getTickCount();
        _flag = true;
    }

    // check 50ms later
    if (_flag && result == 1 && (cv::getTickCount() - _start_time) / cv::getTickFrequency() >= 0.05) {
        _flag = false;
        _flag_2 = true;
        return true;
    }

    if (result == 0) {
        _flag_2 = false;
    }

    return false;
}
