#include <iostream>
#include <opencv2/opencv.hpp>

#include "CBallSorter.h"
#include "CControl.h"

using namespace std;
using namespace cv;

int main() {
    //CControl stepper_test;
    //stepper_test.set_data(stepper_test.STEPPER, -1, 1000, 0);

    CBallSorter sorter;
    sorter.run();

    return 0;
}
