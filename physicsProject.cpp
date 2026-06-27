#include <chrono>
#include <thread>
#include <iostream>

#include<string>
#include<vector>
#include<conio.h>

using namespace std;

int main()
{
    int targetFps = 30;
    chrono::duration<float> targetFrameTime(1.0f / targetFps);

    while (true) {
        auto frameStart = chrono::steady_clock::now();

        //PHYS WORK

        //ELAPSED TIME
        auto workEnd = chrono::steady_clock::now();
        chrono::duration<float> workTime = workEnd - frameStart;

        //FPS CONTROL 
        chrono::duration<float> sleepTime = targetFrameTime - workTime;

        if (sleepTime.count() > 0) {
            auto sleepDuration = chrono::duration_cast<chrono::milliseconds>(sleepTime);
            this_thread::sleep_for(sleepDuration);
        }

        auto frameEnd = chrono::steady_clock::now();
        chrono::duration<float> totalFrameTime = frameEnd - frameStart;

        cout << "FPS: " << 1.0f / totalFrameTime.count() << endl;
    }

    return 0;
}