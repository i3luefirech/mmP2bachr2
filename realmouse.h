//
// Created by bluefire on 05/06/2018.
//

#ifndef MMP2BACHR2_MOUSE_H
#define MMP2BACHR2_MOUSE_H

#include <mutex>

class realmouse {
private:
    long xpos;
    long ypos;
    bool leftclick;
    bool rightclick;
    short mousewheel;
    DWORD timestamp;
    HCURSOR cursor;
public:
    realmouse();

    void run();

#ifdef __MINGW32__
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

    void setpos(int x, int y);

    void doleftclick();

    void dorightclick();

    void doscrollup();

    void doscrolldown();

    void gopos(int x, int y, int delta);

    void dogopos(int x, int y, int delta);

    bool isrunning();

    static std::mutex mtx;

    void GetDesktopResolution(int &horizontal, int &vertical);

    void hiderealmouse(bool hide);
};
extern realmouse * Mainmouse;

#endif //MMP2BACHR2_MOUSE_H
