//
// Created by bluefire on 05/06/2018.
//
#ifdef __APPLE__
#endif  // end apple

#ifdef __linux__
#include <linux/input-event-codes.h>
#include <unistd.h>
#include <iostream>

#endif  // end linux

#ifdef __MINGW32__
#include <windows.h>
#include <conio.h>
#include <iostream>

#endif  // end windows

#include "realmouse.h"

using namespace std;

struct xydelta {
    int x;
    int y;
    int delta;
};

std::mutex realmouse::mtx;           // mutex for critical section

#ifdef __MINGW32__
extern DWORD CALLBACK Thread1(LPVOID pVoid);
#endif  // end windows

realmouse::realmouse() {
    this->xpos = 0;
    this->ypos = 0;
    this->leftclick = false;
    this->rightclick = false;
    this->mousewheel = 0;
    this->timestamp = 0;
#ifdef __MINGW32__
    this->cursor = GetCursor();
#endif  // end windows
#ifdef __linux__
    this->cursor = 0;
#endif  // end linux
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

#ifdef __MINGW32__
DWORD realmouse::getTimestamp() const {
    return timestamp;
}
#endif  // end windows
#ifdef __linux__
unsigned long realmouse::getTimestamp() const {
    return timestamp;
}
#endif  // end linux

void realmouse::run() {
#ifdef __MINGW32__
    MSG msg;
    while (0 < GetMessage(&msg, nullptr, 0, 0)) {
        DispatchMessage(&msg);
    }
#endif  // end windows
#ifdef __linux__
    extern int hook;
    struct input_event ie;
    while(read(hook, &ie, sizeof(struct input_event))) {
       this->LowLevelMouse(ie);
    }
#endif  // end linux
    std::cout << "quitting now..." <<  std::endl;
}

#ifdef __MINGW32__
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

void realmouse::GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right;
    vertical = desktop.bottom;
}


/**
 * setpos<br>
 * set the real cursor directly to this x/y position
 *
 * @param x int with x position
 * @param y int with y position
 */
void realmouse::setpos(int x, int y) {
    SetCursorPos(x,y);
}

/**
 * doleftclick<br>
 * do a real left click at current cursor position
 */
void realmouse::doleftclick() {
    POINT pos_cursor;
    HWND hwnd_outra_win;
    GetCursorPos(&pos_cursor);
    hwnd_outra_win = WindowFromPoint(pos_cursor);
    SendMessage(hwnd_outra_win,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(pos_cursor.x,pos_cursor.y));
    SendMessage(hwnd_outra_win,WM_LBUTTONUP,0,MAKELPARAM(pos_cursor.x,pos_cursor.y));
}

/**
 * dorightclick<br>
 * do a real right click at current cursor position
 */
void realmouse::dorightclick() {
    POINT pos_cursor;
    HWND hwnd_outra_win;
    GetCursorPos(&pos_cursor);
    hwnd_outra_win = WindowFromPoint(pos_cursor);
    SendMessage(hwnd_outra_win,WM_RBUTTONDOWN,MK_RBUTTON,MAKELPARAM(pos_cursor.x,pos_cursor.y));
    SendMessage(hwnd_outra_win,WM_RBUTTONUP,0,MAKELPARAM(pos_cursor.x,pos_cursor.y));
}

/**
 * doleftclick<br>
 * do a real scroll up at current cursor position
 */
void realmouse::doscrollup() {
    INPUT in;
    in.type = INPUT_MOUSE;
    in.mi.dx = 0;
    in.mi.dy = 0;
    in.mi.dwFlags = MOUSEEVENTF_WHEEL;
    in.mi.time = 0;
    in.mi.dwExtraInfo = 0;
    in.mi.mouseData = WHEEL_DELTA;
    SendInput(1,&in,sizeof(in));
}

/**
 * doscrolldown<br>
 * do a real scroll down at current cursor position
 */
void realmouse::doscrolldown() {
    INPUT in;
    in.type = INPUT_MOUSE;
    in.mi.dx = 0;
    in.mi.dy = 0;
    in.mi.dwFlags = MOUSEEVENTF_WHEEL;
    in.mi.time = 0;
    in.mi.dwExtraInfo = 0;
    in.mi.mouseData = (DWORD)-1;
    SendInput(1,&in,sizeof(in));
}

/**
 * gopos<br>
 * drive the real cursor this x/y position
 *
 * @param x int with x position
 * @param y int with y position
 * @param delta how many pixel per 10ms
 */
void realmouse::gopos(int x, int y , int delta) {
    xydelta *xyd = new xydelta();
    xyd->x = x;
    xyd->y = y;
    xyd->delta = delta;
    cout << "soll - 2 x: " << xyd->x << " y: " << xyd->y << endl;
    DWORD threadId;
    CloseHandle(CreateThread(nullptr, 0, Thread1, (void *)xyd, 0, &threadId));
}

/**
 * gopos<br>
 * drive the real cursor this x/y position
 *
 * @param x int with x position
 * @param y int with y position
 * @param delta ~how many pixel per 100ms
 */
void realmouse::dogopos(int x, int y , int delta) {
    cout << "get mutex lock..." << endl;
    this->mtx.lock();
    cout << "has mutex lock" << endl;
    POINT ptrpoint;
    GetCursorPos(&ptrpoint);
    cout << "soll x: " << x << " y: " << y << endl;
    cout << "ist x: " << ptrpoint.x << " y: " << ptrpoint.y << endl;
    while(ptrpoint.x!=x || ptrpoint.y!=y){

        bool diffxpos;
        int diffx;
        int maxdeltax;

        bool diffypos;
        int diffy;
        int maxdeltay;

        // checke richtung durch pos/neg differenz
        if(ptrpoint.x<x){
            diffxpos = true;
        } else {
            diffxpos = false;
        }
        if(ptrpoint.y<y){
            diffypos = true;
        } else {
            diffypos = false;
        }

        // rechne differenz
        if(diffxpos){
            diffx = x - ptrpoint.x;
        } else {
            diffx = ptrpoint.x - x;
        }
        if(diffypos){
            diffy = y - ptrpoint.y;
        } else {
            diffy = ptrpoint.y - y;
        }

        // checke welche distanz grösser ist und passe maxdelta an
        if(abs(diffx)<abs(diffy)){
            maxdeltax = delta * 1 / 4;
            maxdeltay = delta * 3 / 4;
        } else if(abs(diffx)>abs(diffy)){
            maxdeltax = delta * 3 / 4;
            maxdeltay = delta * 1 / 4;
        } else {
            maxdeltax = delta * 1 / 2;
            maxdeltay = delta * 1 / 2;
        }

        // setze max delta wenn ein wert schon 0
        if(diffx==0){
            maxdeltax = 0;
            maxdeltay = delta;
        }
        if(diffy==0){
            maxdeltax = delta;
            maxdeltay = 0;
        }

        // passe maxdelta an wenn limit erreicht
        if(maxdeltax < diffx){
            diffx = maxdeltax;
        }
        if(maxdeltay < diffy){
            diffy = maxdeltay;
        }

        int newposx = 0;
        int newposy = 0;

        // rechne neue position
        if(diffxpos){
            newposx = ptrpoint.x+diffx;
        } else {
            newposx = ptrpoint.x-diffx;
        }
        if(diffypos){
            newposy = ptrpoint.y+diffy;
        } else {
            newposy = ptrpoint.y-diffy;
        }

        // setze neue position
        this->setpos(newposx,newposy);
        Sleep(10);
        GetCursorPos(&ptrpoint);
        cout << "ist x: " << ptrpoint.x << " y: " << ptrpoint.y << endl;
    }
    this->mtx.unlock();
}

void realmouse::hiderealmouse(bool hide) {
    POINT pos_cursor;
    HWND hwnd_outra_win;
    GetCursorPos(&pos_cursor);
    hwnd_outra_win = WindowFromPoint(pos_cursor);
    if(hide) {
        SetCapture(hwnd_outra_win);
        while(ShowCursor(false)>=0);
    } else {
        SetCapture(hwnd_outra_win);
        ShowCursor(hide);
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
                std::cout << "ABS_X... " << value << std::endl;
                this->xpos = value;
                break;
            case ABS_Y:
                std::cout << "ABS_Y... " << value << std::endl;
                this->ypos = value;
                break;
            case ABS_WHEEL:
                std::cout << "ABS/REL_WHEEL..." <<  std::endl;
                break;
            case BTN_LEFT:
                std::cout << "BTN_LEFT..." <<  std::endl;
                break;
            case BTN_RIGHT:
                std::cout << "BTN_RIGHT..." <<  std::endl;
                break;
            case BTN_MIDDLE:
                std::cout << "BTN_MIDDLE..." <<  std::endl;
                break;
            default:
                break;
        }
    }
}
#endif  // end linux
