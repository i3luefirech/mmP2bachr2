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

    pthread_exit(NULL);
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
    DWORD threadId;
    CloseHandle(CreateThread(nullptr, 0, Thread, nullptr, 0, &threadId));
    while (!kbhit()){
        switch(count%1000000){
            case 0:
                Mainmouse->setpos(0,0);
                break;
            case 50000:
                Mainmouse->doleftclick();
                break;
            case 100000:
                Mainmouse->dorightclick();
                break;
            case 200000:
                Mainmouse->doscrollup();
                break;
            case 300000:
                Mainmouse->doscrolldown();
                break;
            case 400000:
                Mainmouse->gopos(0,0);
                break;
            case 500000:
                Mainmouse->gopos(0,0);
                break;
            case 600000:
                Mainmouse->gopos(0,0);
                break;
            case 700000:
                Mainmouse->gopos(0,0);
                break;
            case 800000:
                Mainmouse->gopos(0,0);
                break;
            case 900000:
                Mainmouse->gopos(0,0);
                break;
            default:
                break;
        }
        if(Mainmouse->getMouswheel()<0) {
            std::cout << "scroll down" << std::endl;
            Mainmouse->decrementOpenMousewheelActions(1,Mainmouse->getMouswheel());
        } else if (Mainmouse->getMouswheel()>0) {
            std::cout << "scroll up" << std::endl;
            Mainmouse->decrementOpenMousewheelActions(1,Mainmouse->getMouswheel());
        }
        if(count%500000==0){
            std::cout << "Monitordata Mouse: (" << std::endl;
            std::cout << "x:" << Mainmouse->getXpos() << std::endl;
            std::cout << "y:" << Mainmouse->getYpos() << std::endl;
            std::cout << "lc:" << Mainmouse->isLeftclick() << std::endl;
            std::cout << "rc:" << Mainmouse->isRightclick() << std::endl;
            std::cout << ")" << std::endl;
        }
    };
    PostThreadMessage(threadId, WM_QUIT, 0, 0);
    Sleep(1000);
#endif  // end windows
#ifdef __linux__
    pthread_create(&threads[i], NULL, Thread, (void *)i);
#endif  // end linux
    // free realmouse object
    free(Mainmouse);

    return 0;
}