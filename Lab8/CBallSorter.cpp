#include "CBallSorter.h"

#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <unistd.h>

CBallSorter::CBallSorter() {
    hue = 100;
    hue_up = 100;

    sat = 100;
    sat_up = 100;

    val = 100;
    val_up = 100;

    config_mode = false;
    auto_mode = true;

    colour[0] = "Green";
    colour[1] = "Blue";
    colour[2] = "Pink";
    colour[3] = "White";
    colour[4] = "Configuring";

    ball_counts["Green"] = 0;
    ball_counts["Blue"] = 0;
    ball_counts["Pink"] = 0;
    ball_counts["White"] = 0;

    colour_index = 0;

    calls = 0;

    last_colour = "Initial";

    // only if not multithreaded
    _vid.open(0);
    cvui::init(CANVAS_NAME);
}

void CBallSorter::gpio() {
}

// here is where the server stuff happens
void CBallSorter::update() {
    server.get_cmd(cmds);
    if (cmds.size() > 0) {
      // Process different commands received by the server
      for (int i = 0; i < cmds.size(); i++) {
        if (cmds.at(i) == "im") {
            //server.send_string("Image");
        }
        else if (cmds.at(i) == "mode") {
          auto_mode = !auto_mode;
        }
        else if (cmds.at(i) == "green") {
            sortBall("Green");
        }
        else if (cmds.at(i) == "blue") {
            sortBall("Blue");
        }
        else if (cmds.at(i) == "mode?") {
            std::string reply = auto_mode ? "Auto" : "Manual";
            server.send_string(reply);
        }
        else if (cmds.at(i) == "counts") {
            std::string reply = to_string(ball_counts["Green"]) + " " + to_string(ball_counts["Blue"]) + " " + to_string(ball_counts["Pink"]) + " " + to_string(ball_counts["White"]);
            server.send_string(reply);
        }
      }
    }
}

void CBallSorter::draw() {
    if (_vid.isOpened()) {
        Mat frame;
        _vid >> frame;

        // Update server image with the latest camera image
        if (!frame.empty()) server.set_txim(frame);

        cvui::window(frame, 0, 0, 210, 470, "Control Interface");
        cvui::trackbar(frame, 10, 10, 150, &hue, 0, 255);
        cvui::text(frame, 60, 60, "Hue LB");
        cvui::trackbar(frame, 10, 70, 150, &hue_up, 0, 255);
        cvui::text(frame, 60, 120, "Hue UB");

        cvui::trackbar(frame, 10, 130, 150, &sat, 0, 255);
        cvui::text(frame, 45, 180, "Saturation LB");
        cvui::trackbar(frame, 10, 190, 150, &sat_up, 0, 255);
        cvui::text(frame, 45, 240, "Saturation UB");

        cvui::trackbar(frame, 10, 250, 150, &val, 0, 255);
        cvui::text(frame, 55, 300, "Value LB");
        cvui::trackbar(frame, 10, 310, 150, &val_up, 0, 255);
        cvui::text(frame, 55, 360, "Value UB");

        cvui::checkbox(frame, 10, 380, "Config Mode", &config_mode, 255);

        if (config_mode) {
            if (cvui::button(frame, 10, 410, "HSV setting for: " + colour[colour_index])) {
                colour_index = colour_index == 4 ? 0 : colour_index + 1;
            }
        } else if (auto_mode) {
            colour_index = colour_index == 3 ? 0 : colour_index + 1;
        }

        string autoMode = auto_mode ? "Auto" : "Manual";
        if ((cvui::button(frame, 10, 440, "Mode: " + autoMode)) || auto_button.get_button(AUTO_BUTTON, _ccontrol)) {
            auto_mode = !auto_mode;
        }

        if (cvui::button(frame, 125, 440, "Reset")) sortBall("Initial");

        int auto_led = auto_mode ? 1 : 0;
        gpio_helper(CControl::DIGITAL, AUTO_LED, true, auto_led);

        if (auto_mode) {
            string found_colour = findColour(frame, colour[colour_index], config_mode, Scalar(hue, sat, val), Scalar(hue_up, sat_up, val_up));
            cout << "Found Colour:" << found_colour << ".\n";

            if (found_colour != "Nothing" && found_colour != "") sortBall(found_colour);
        } else {
            if ((cvui::button(frame, 10, 410, colour[colour_index] + " bin")) || colour_button.get_button(COLOUR_BUTTON, _ccontrol)) {
                colour_index = colour_index == 3 ? 0 : colour_index + 1;
            }

            if ((cvui::button(frame, 125, 410, "Release")) || release_button.get_button(RELEASE_BUTTON, _ccontrol)) {
                sortBall(colour[colour_index]);
            }
        }

        if (!frame.empty()) {
            cvui::update();
            imshow(CANVAS_NAME, frame);
        }
    }
}

void CBallSorter::sortBall(string colour) {
    // 1000 steps = 25mm
    float mm_per_step = 0.025;
    int steps = 0;
    int dir = 0;
    bool move_servo = true;

    // Starting at Initial Position
    if (last_colour == "Initial") {
        if (colour == "Green") {
            // 75mm right
            steps = 75.0 / mm_per_step;
            dir = 1;
        }

        if (colour == "Blue") {
            // 25mm right
            steps = 25.0 / mm_per_step;
            dir = 1;
        }

        if (colour == "Pink") {
            // 25mm left
            steps = 25.0 / mm_per_step;
            dir = 0;
        }

        if (colour == "White") {
            // 75mm left
            steps = 75.0 / mm_per_step;
            dir = 0;
        }
    }

    // Starting at Green
    if (last_colour == "Green") {
        if (colour == "Blue") {
            // 48mm left
            steps = 48.0 / mm_per_step;
            dir = 0;
        }

        if (colour == "Pink") {
            // 100mm left
            steps = 100.0 / mm_per_step;
            dir = 0;
        }

        if (colour == "White") {
            // 150mm left
            steps = 150.0 / mm_per_step;
            dir = 0;
        }

        if (colour == "Initial") {
            // 75mm left
            steps = 75.0 / mm_per_step;
            dir = 0;
            move_servo = false;
        }
    }

    // Starting at Blue
    if (last_colour == "Blue") {
        if (colour == "Green") {
            // 48mm right
            steps = 48.0 / mm_per_step;
            dir = 1;
        }

        if (colour == "Pink") {
            // 48mm left
            steps = 48.0 / mm_per_step;
            dir = 0;
        }

        if (colour == "White") {
            // 100mm left
            steps = 100.0 / mm_per_step;
            dir = 0;
        }

        if (colour == "Initial") {
            // 25mm left
            steps = 25.0 / mm_per_step;
            dir = 0;
            move_servo = false;
        }
    }

    // Starting at Pink
    if (last_colour == "Pink") {
        if (colour == "Green") {
            // 100mm right
            steps = 100.0 / mm_per_step;
            dir = 1;
        }

        if (colour == "Blue") {
            // 48mm right
            steps = 48.0 / mm_per_step;
            dir = 1;
        }

        if (colour == "White") {
            // 48mm left
            steps = 48.0 / mm_per_step;
            dir = 0;
        }

        if (colour == "Initial") {
            // 25mm right
            steps = 25.0 / mm_per_step;
            dir = 1;
            move_servo = false;
        }
    }

    // Starting at White
    if (last_colour == "White") {
        if (colour == "Green") {
            // 150mm right
            steps = 150.0 / mm_per_step;
            dir = 1;
        }

        if (colour == "Blue") {
            // 100mm right
            steps = 100.0 / mm_per_step;
            dir = 1;
        }

        if (colour == "Pink") {
            // 48mm right
            steps = 48.0 / mm_per_step;
            dir = 1;
        }

        if (colour == "Initial") {
            // 75mm right
            steps = 75.0 / mm_per_step;
            dir = 1;
            move_servo = false;
        }
    }

    int green = colour == "Green" ? 1 : 0;
    int blue = colour == "Blue" ? 1 : 0;
    int pink = colour == "Pink" ? 1 : 0;
    int white = colour == "White" ? 1 : 0;

    gpio_helper(CControl::DIGITAL, GREEN_LED, true, green);
    gpio_helper(CControl::DIGITAL, BLUE_LED, true, blue);
    gpio_helper(CControl::DIGITAL, PINK_LED, true, pink);
    gpio_helper(CControl::DIGITAL, WHITE_LED, true, white);

    cout << steps << "\n";
    gpio_helper(CControl::STEPPER, -1, true, steps, false, !dir);

    if (move_servo) {
        int servo_pos = 90;
        usleep(100000);
        gpio_helper(CControl::SERVO, SERVO_PIN, true, servo_pos);
        usleep(100000);
        servo_pos = 0;
        gpio_helper(CControl::SERVO, SERVO_PIN, true, servo_pos);
    }

    if (colour != "Initial") ball_counts[colour] = ball_counts[colour] + 1;
    last_colour = colour;
}

string CBallSorter::findColour(const Mat &canvas, string colour, bool config_mode, Scalar lb, Scalar ub) {
    string found_colour = "";

    if (colour == "Green") {
        lb = Scalar(4, 79, 98);
        ub = Scalar(79, 255, 255);
    }

    if (colour == "Blue") {
        lb = Scalar(96, 68, 135);
        ub = Scalar(114, 255, 225);
    }

    if (colour == "Pink") {
        lb = Scalar(112, 0, 206);
        ub = Scalar(246, 225, 255);
    }

    if (colour == "White") {
        lb = Scalar(0, 0, 255);
        ub = Scalar(0, 0, 255);
    }

    Mat frame;

    cvtColor(canvas, frame, COLOR_BGR2HSV);
    inRange(frame, lb, ub, frame);

    erode(frame, frame, Mat());
    dilate(frame, frame, Mat());

    vector<Vec4i> hierarchy;
    vector<vector<Point>> contours;

    findContours(frame, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.size() != 0) {
        Rect max_rect = boundingRect(contours.at(0));

        for (unsigned int i = 0; i < contours.size(); i++) {
            Rect r = boundingRect(contours.at(i));
            if (r.area() > max_rect.area()) {
                max_rect = r;
            }
        }

        if (colour == "Green" && max_rect.area() > 100000) {
            found_colour = "Green";
            calls = 0;
        } else if (colour == "Blue" && max_rect.area() > 80000) {
            found_colour = "Blue";
            calls = 0;
        } else if (colour == "Pink" && max_rect.area() > 115000) {
            found_colour = "Pink";
            calls = 0;
        } else if (colour == "White" && max_rect.area() > 50000) {
            found_colour = "White";
            calls = 0;
        } else {
            calls ++;
        }

        if (calls >= 4) {
            found_colour = "Nothing";
        }

        if (config_mode) {
            rectangle(frame, max_rect, Scalar(255,255,0));
            cout << max_rect.area() << "\n";
            imshow("Eroded & Dilated", frame);
        }
    }
    return found_colour;
}
