//
// Created by bluefire on 05/06/2018.
//

#ifndef MMP2BACHR2_MOUSE_H
#define MMP2BACHR2_MOUSE_H

#include <mutex>
#ifdef __linux__
#include <linux/input.h>
#endif  // end linux

class realmouse {
private:
    long xpos;
    long ypos;
    bool leftclick;
    bool rightclick;
    short mousewheel;
#ifdef __MINGW32__
    DWORD timestamp;
    HCURSOR cursor;
#endif  // end windows
#ifdef __linux__
    unsigned long timestamp;
    int cursor;
#endif  // end linux
public:
    realmouse();

    void run();

#ifdef __MINGW32__
    void LowLevelMouse(int nCode, WPARAM wParam, LPARAM lParam);
#endif  // end windows
#ifdef __linux__
    void LowLevelMouse(struct input_event ie);
#endif  // end linux

    long getXpos() const;

    long getYpos() const;

    bool isLeftclick() const;

    bool isRightclick() const;

    short getMouswheel() const;

#ifdef __MINGW32__
    DWORD getTimestamp() const;
#endif  // end windows
#ifdef __linux__
    unsigned long getTimestamp() const;
#endif  // end windows

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
