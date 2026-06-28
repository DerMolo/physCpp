#include <chrono>
#include <thread>
#include <iostream>

#include<string>
#include<vector>
#include<conio.h>

using namespace std;

//GLOBAL VARS 

const int colSize = 50;
const int rowSize = 20;

//GLOBAL CLOCK 
float worldTime = 0.0;

const float worldTick = 0.0002;

enum class Direction{ //may be unnecessary 
    NEUTRAL,//NULL
    LEFT,//180
    RIGHT,//0
    UP,//90
    DOWN,//270
    U_RIGHT,//45
    U_LEFT,//135
    D_RIGHT,//315
    D_LEFT//225
};

struct Force { 
    float accelX; 
    float accelY; 

    float mass; 
    float angle; 

    Direction dir;
};

struct Particle {
    float velX; 
    float velY; 

    float accX; 
    float accY; 

    int posX; 
    int posY; 
    
    //global forces 
    Force gravity;
    Force drag; 

    //trajectory as determined by applied forces
    Direction dir; 
};


void printWorld(char* world) {

    //conversion formula: ind = row * colSize + col

    for (int y = 0; y < rowSize; y++) {
        for (int x = 0; x < colSize; x++)
        {
            string color; 
            int trueInd = y * colSize + x; 
            if (world[trueInd] == '#')
                color = "\033[31m";
            cout << "\033[" << y + 2 << ";" << x + 1 << "H" << color << world[trueInd] << "\033[0m";
        }
    }
}

void renderWorld(vector<Particle> tempParts) { 
    /* 
    should each frame be considered a simulated second? no 
    simulated time should be deterministic 
    */

    /*
    current objective: 
   * apply a force to every particle in the scene. (ensure proper trajectory) 
   * ensure particles don't cross border '#' 
   * 
   * later objective: 
   * handle all collisions 
    */

    for (auto speck : tempParts) {

    }
}

int main()
{
    int targetFps = 30;
    chrono::duration<float> targetFrameTime(1.0f / targetFps);

    char world[rowSize][colSize];

    // populating the world 
    for (int y = 0; y < rowSize; y++) {
        for (int x = 0; x < colSize; x++)
        {
            if(x==0 || x==colSize-1 || y == 0 || y == rowSize-1)
                world[y][x] = '#';
            else 
                world[y][x] = '.';
        }
    }

    /*
    objective: 
    allowing the user to interactively place particles (then run the simulation) 

    later objective: 
    - allowing users to select pre-packaged particle shapes and insert them into the scene 
    - allowing the user to apply certain scenarios like: vortex, windiness, explosions, 
    */

    printWorld(*world); 

    cout << "\033[" << rowSize + 3 << ";" << 1 << "H" << "TO FINALIZE SETUP, PRESS 'E'";
    cout << "\033[" << rowSize + 4 << ";" << 1 << "H" << "TO PLACE DOWN A PARTICLE, PRESS 'ENTER'";
    
    bool complete = false; 

    int userX = colSize/2;
    int userY = rowSize/2;

    vector<Particle> spawnedParticles; 

    char userIn ;
    int debugIndex = 17; 

    while (!complete) {
        if (_kbhit) {
            userIn = _getch();
            if (userIn == '\r') {
                //offsetting userY by 3 to account for world's position & terminal indexing 
                cout << "\033[" << userY + 3 << ";" << userX + 1 << "H" << '@' << endl;

            }
            else if (tolower(userIn) == 'e') {
                complete = true;
                break;
            }

            //user controls
            switch (tolower(userIn))
            {
            case 'w':
                userY++;
                break;
            case 'a':
                userX--;
                break;
            case 's':
                userY--;
                break;
            case 'd':
                userX++;
                break;
            default:
                break;
            }
            // "\033[s" to save cursor position *only for windows terminals 
            // "\033[u" to restore cursor position 

            cout << "\033[" << userY + 3 << ";" << userX + 1 << "H" << "\033[s";

            debugIndex = debugIndex+1 > 22 ? 17 : debugIndex + 1;
        }
        cout << "\033[" << rowSize + 5 << ";" << 1 << "H" << "SELECT POSITION: "<<userX <<", " << userY;
        cout << "\033[" << rowSize + 6 << ";" << 1 << "H" << "KEY PRESSED : ";
        cout << "\033[" << rowSize + 6 << ";" << debugIndex << "H" << userIn; 

        cout << "\033[u";
    };


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

        cout << "\033[1;1H" << "FPS: " << 1.0f / totalFrameTime.count() << endl;
    }

    return 0;
}