//
// Created by bluefire on 05/06/2018.
//
#ifdef __APPLE__
#endif  // end apple

#ifdef __linux__
#endif  // end linux

#ifdef __MINGW32__ || __MINGW64__
#include <windows.h>
#include <conio.h>
#include <iostream>
#endif  // end windows

#include "realmouse.h"

using namespace std;

realmouse::realmouse() {
    this->xpos = 0;
    this->ypos = 0;
    this->leftclick = false;
    this->rightclick = false;
    this->mousewheel = 0;
    this->timestamp = 0;
}

void realmouse::decrementOpenMousewheelActions(int amount, int direction) {
    if(direction<0){
        this->mousewheel += amount;
    } else {
        this->mousewheel -= amount;
    }
}

long realmouse::getXpos() const {
    return xpos;
}

long realmouse::getYpos() const {
    return ypos;
}

bool realmouse::isLeftclick() const {
    return leftclick;
}

bool realmouse::isRightclick() const {
    return rightclick;
}

short realmouse::getMouswheel() const {
    return mousewheel;
}

DWORD realmouse::getTimestamp() const {
    return timestamp;
}

void realmouse::run() {
#ifdef __MINGW32__ || __MINGW64__
    MSG msg;
    while (0 < GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }
#endif  // end windows
#ifdef __linux__
    struct input_event ie;
    while(read(hook, &ie, sizeof(struct input_event))) {
       this->LowLevelMouse(ie);
    }
#endif  // end linux
    std::cout << "quitting now..." <<  std::endl;
}

#ifdef __MINGW32__ || __MINGW64__
void realmouse::LowLevelMouse(int nCode, WPARAM wParam, LPARAM lParam) {

    MSLLHOOKSTRUCT * ptrlParam = (MSLLHOOKSTRUCT *)lParam;

    DWORD fullvalue = (DWORD)ptrlParam->mouseData;
    WORD wheelvalue = HIWORD(fullvalue);

    switch(wParam) {
        case  WM_LBUTTONDOWN:
            this->leftclick = true;
            break;
        case  WM_LBUTTONUP:
            this->leftclick = false;
            break;
        case WM_MOUSEWHEEL:
            if(wheelvalue==65416) {
                --this->mousewheel;
            } else if (wheelvalue==120) {
                ++this->mousewheel;
            } else {
                this->mousewheel = 0;
            }
            break;
        case  WM_RBUTTONDOWN:
            this->rightclick = true;
            break;
        case  WM_RBUTTONUP:
            this->rightclick = false;
            break;
        case  WM_MOUSEMOVE:
            this->xpos = ptrlParam->pt.x;
            this->ypos = ptrlParam->pt.y;
            break;
        default:
            std::cout << "not implemented TODO (" << wParam << ")" << std::endl;
            break;
    }
}
#endif  // end windows

#ifdef __linux__
void realmouse::LowLevelMouse(struct input_event ie) {
    int type = ie.type;
    int code = ie.code;
    int value = ie.value;
    if(EV_KEY == type || EV_ABS == type || EV_REL == type) {
        switch(code){
            case ABS_X:
                this->xpos = ptrlParam->pt.x;
                break;
            case ABS_Y:
                this->ypos = ptrlParam->pt.y;
                break;
            case ABS_WHEEL:
                break;
            case REL_WHEEL:
                break;
            case BTN_LEFT:
                break;
            case BTN_RIGHT:
                break;
            case BTN_MIDDLE:
                break;
            default:
                break;
        }
    }
}
#endif  // end linux
