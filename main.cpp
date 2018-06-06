#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif  // end apple

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/input.h>
#include <fcntl.h>
#include <X11/Xlib.h>

#define MOUSEFILE "/dev/input/mouse0"
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

realmouse * Mainmouse = nullptr;

#ifdef __MINGW32__
HINSTANCE hInst = nullptr;
HHOOK hook = nullptr;

/**
 * DllMain <br>
 * get HINSTANCE on DLL process attack message
 *
 * @param hInstance
 * @param reason
 * @param reserved
 * @return
 */
int WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            hInst = hInstance;
            DisableThreadLibraryCalls(hInstance);
        break;default:
            break;
    }
    return 1;
}
/**
 * LowLevelMouseProc<br>
 * callback function which calls original function of the Mouse object
 *
 * @param nCode
 * @param wParam
 * @param lParam
 * @return
 */
LRESULT CALLBACK LowLevelMouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
    Mainmouse->LowLevelMouse(nCode,wParam,lParam);
    return (::CallNextHookEx(hook,nCode,wParam,lParam));
}

/**
 * Thread <br>
 * callback function for threading.
 * Firsat load the mouseeventhook.
 * 2nd run the Thread funtion.
 * Unhook Mousevents when finished
 *
 * @param pVoid
 * @return
 */
DWORD CALLBACK Thread(LPVOID pVoid)
{
    // activate hook
    hook=SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInst, 0);

    Mainmouse->run();

    UnhookWindowsHookEx(hook);
    return 0;
}

/**
 * Thread1 <br>
 * callback function for threading.
 * used for gopos
 *
 * @param pVoid
 * @return
 */
DWORD CALLBACK Thread1(LPVOID pVoid)
{
    int x,y,delta;
    xydelta *xyd = (xydelta*)pVoid;
    x = xyd->x;
    y = xyd->y;
    delta = xyd->delta;
    free(xyd);
    cout << "soll -1 x: " << x << " y: " << y << endl;
    Mainmouse->dogopos(x,y,delta);
    return 0;
}
#endif  // end windows

#ifdef __linux__
int hook = 0;

void *Thread(void *pVoid) {

    if((hook = open(MOUSEFILE, O_RDONLY)) == -1) {
        std::cout << "can not open mousefile..." <<  std::endl;
        return -1;
    }

    struct thread_data *my_data;
    my_data = (struct thread_data *) pVoid;

    Mainmouse->run();

    close(hook);

    pthread_exit(nullptr);
}
#endif  // end linux

/**
 * Main <br>
 * Used for PoC.
 * Creates Mouse Object.
 * Creates needed thread.
 * test code mouse functionality and print monitoring from real mouse.
 *
 * @return
 */
int main()
{
    int count = 0;
    // create realmouse object
    Mainmouse = new realmouse();
    // run realmouse object
#ifdef __MINGW32__
    Sleep(5000);
    DWORD threadId;
    CloseHandle(CreateThread(nullptr, 0, Thread, nullptr, 0, &threadId));
    while (!kbhit()){
        if(Mainmouse->getMouswheel()<0) {
            std::cout << "scroll down" << std::endl;
            Mainmouse->decrementOpenMousewheelActions(1,Mainmouse->getMouswheel());
        } else if (Mainmouse->getMouswheel()>0) {
            std::cout << "scroll up" << std::endl;
            Mainmouse->decrementOpenMousewheelActions(1,Mainmouse->getMouswheel());
        }
        if(count%2500==0){
            std::cout << "Monitordata Mouse: (" << std::endl;
            std::cout << "x:" << Mainmouse->getXpos() << std::endl;
            std::cout << "y:" << Mainmouse->getYpos() << std::endl;
            std::cout << "lc:" << Mainmouse->isLeftclick() << std::endl;
            std::cout << "rc:" << Mainmouse->isRightclick() << std::endl;
            std::cout << ")" << std::endl;
        }
        int horizontal;
        int vertical;
        switch(count++){
            case 0:
                Mainmouse->GetDesktopResolution(horizontal, vertical);
                std::cout << "setpos(" << horizontal/2 << "," << vertical/2 << ")" << std::endl;
                Mainmouse->setpos(horizontal/2,vertical/2);
                break;
            case 500:
                std::cout << "doleftclick()" << std::endl;
                Mainmouse->doleftclick();
                break;
            case 1000:
                std::cout << "dorightclick()" << std::endl;
                Mainmouse->dorightclick();
                break;
            case 2000:
                std::cout << "doscrollup()" << std::endl;
                Mainmouse->setpos((horizontal/2)-50,vertical/2);
                Mainmouse->doleftclick();
                Mainmouse->doscrollup();
                break;
            case 3000:
                std::cout << "doscrolldown()" << std::endl;
                Mainmouse->doscrolldown();
                break;
            case 4000:
                std::cout << "gopos(200,400,32)" << std::endl;
                Mainmouse->gopos(200,400,8);
                break;
            case 8000:
                std::cout << "gopos(600,550,32)" << std::endl;
                Mainmouse->gopos(600,550,8);
                break;
            case 12000:
                std::cout << "hiderealmouse(true)" << std::endl;
                Mainmouse->hiderealmouse(true);
                break;
            case 14000:
                std::cout << "hiderealmouse(false)" << std::endl;
                Mainmouse->hiderealmouse(false);
                break;
            case 20000:
                std::cout << "reset tests..." << std::endl;
                count=0;
                break;
            default:
                break;
        }
        Sleep(1);
    };
    PostThreadMessage(threadId, WM_QUIT, 0, 0);
    Sleep(1000);
#endif  // end windows
#ifdef __linux__
    pthread_create(&threads[i], nullptr, Thread, (void *)i);
#endif  // end linux
    // free realmouse object
    free(Mainmouse);

    return 0;
}