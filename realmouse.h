//
// Created by bluefire on 05/06/2018.
//

#ifndef MMP2BACHR2_MOUSE_H
#define MMP2BACHR2_MOUSE_H

class realmouse {
private:
    long xpos;
    long ypos;
    bool leftclick;
    bool rightclick;
    short mousewheel;
    DWORD timestamp;
public:
    realmouse();

    void run();

#ifdef __MINGW32__ || __MINGW64__
    void LowLevelMouse(int nCode, WPARAM wParam, LPARAM lParam);
#endif  // end windows
#ifdef __linux__
    void realmouse::LowLevelMouse(struct input_event ie) {
}
#endif  // end linux

    long getXpos() const;

    long getYpos() const;

    bool isLeftclick() const;

    bool isRightclick() const;

    short getMouswheel() const;

    DWORD getTimestamp() const;

    void decrementOpenMousewheelActions(int amount, int direction);
};


#endif //MMP2BACHR2_MOUSE_H
