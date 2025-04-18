#pragma once
#include "CBase4618.h"
#include "CButton.h"

using namespace std;
using namespace cv;


class CBallSorter : public CBase4618 {
    public:
        CBallSorter();

    private:
        int calls;

        int hue;
        int hue_up;

        int sat;
        int sat_up;

        int val;
        int val_up;

        map<string, int> ball_counts;

        vector<string> cmds;

        bool config_mode;
        bool auto_mode;

        CButton auto_button;
        CButton colour_button;
        CButton release_button;

        Mat last_image;

        string colour[5];
        int colour_index;

        string last_colour;

        string findColour(const Mat& canvas, string colour, bool config_mode, Scalar lb, Scalar ub);
        void sortBall(string colour);

        void gpio();
        void update();
        void draw();
};
