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

LRESULT CALLBACK LowLevelMouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
    Mainmouse->LowLevelMouse(nCode,wParam,lParam);
    return (::CallNextHookEx(hook,nCode,wParam,lParam));
}

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
                // Todo startposition
                Mainmouse->setpos(0,0);
                break;
            case 50000:
                // Todo leftclick
                Mainmouse->doleftclick();
                break;
            case 100000:
                // Todo rightclick
                Mainmouse->dorightclick();
                break;
            case 200000:
                // Todo scroll up
                Mainmouse->doscrollup();
                break;
            case 300000:
                // Todo scroll down
                Mainmouse->doscrolldown();
                break;
            case 400000:
                // Todo move mouse
                Mainmouse->gopos(0,0);
                break;
            case 500000:
                // Todo move mouse
                Mainmouse->gopos(0,0);
                break;
            case 600000:
                // Todo move mouse
                Mainmouse->gopos(0,0);
                break;
            case 700000:
                // Todo move mouse
                Mainmouse->gopos(0,0);
                break;
            case 800000:
                // Todo move mouse
                Mainmouse->gopos(0,0);
                break;
            case 900000:
                // Todo move mouse
                Mainmouse->gopos(0,0);
                break;
            default:
                break;
        }
        if(Mainmouse->getMouswheel()<0) {
            Mainmouse->decrementOpenMousewheelActions(1,Mainmouse->getMouswheel());
        } else if (Mainmouse->getMouswheel()>0) {
            Mainmouse->decrementOpenMousewheelActions(1,Mainmouse->getMouswheel());
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