#include <chrono>
#include <thread>
#include <iostream>

#include <unordered_map>
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
    
    Force(float accX, float accY, float m, float ang) {
        accelX = accX; 
        accelY = accY; 
        mass = m; 
        angle = ang; 
    }
};

struct Particle {
    float mass; 

    float velX; 
    float velY; 

    float accX; 
    float accY; 

    int posX; 
    int posY; 
    
    //global forces 
    float gravity = -9.8; //accelY, angle applied
    float drag;

    //trajectory as determined by applied forces
    Direction dir; 

    Particle(int x, int y, float m) {
         posX = x; 
         posY = y;
         mass = m;
    }
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


    //each spatial unit is .5m

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

    int userX = colSize/2;
    int userY = rowSize/2; //correcting for offset 

    vector<Particle*> spawnedParticles; 

    char userIn ;

    //int debugIndex = 16; 
    
    unordered_map<int, pair<Particle*,int>> particleTracker; 

    int flatInd = userY * colSize + userX;

    while (true) {

        int trueUserX = userX + 1; 
        int trueUserY = userY + 2; // terminal offset + world offset 

        if (_kbhit) {
            userIn = _getch();
            if (userIn == '\r') {
                //offsetting userY by 3 to account for world's position & terminal indexing 
                cout << "\033[" << trueUserY << ";" << trueUserX  << "H" << '@';

                if (particleTracker.find(flatInd) == particleTracker.end()) { //creating particle

                    world[userY][userX] = '@';
                    Particle* temp = new Particle(userX, userY, 5.0);
                    spawnedParticles.push_back(temp);
                    
                    int vectIndex = spawnedParticles.size() - 1 > 0 ? spawnedParticles.size() - 1 : 0; 

                    particleTracker[flatInd] = { temp , vectIndex }; //tracking particle 
                }
            }

            if (tolower(userIn) == 'e')  //exiting setup phase 
                break;

            if (tolower(userIn) == '\b' && particleTracker.find(flatInd) != particleTracker.end()) { //backspace to delete select particles 
                int vectInd = particleTracker[flatInd].second;
                particleTracker.erase(flatInd); 
                spawnedParticles.erase(spawnedParticles.begin() + vectInd);

                cout << "\033[" << trueUserY << ";" << trueUserX << "H" << '.';
            }

            //user controls
            switch (tolower(userIn))
            {
            case 'w':
                if (!(userY - 1 < 0) && world[userY-1][userX] != '#') {
                    userY--;
                    trueUserY--;
                }
                break;
            case 'a':
                if (!(userX - 1 < 0) && world[userY][userX-1] != '#') {
                    userX--;
                    trueUserX--;
                }
                break;
            case 's':
                if (!(userY + 1 >= rowSize) && world[userY + 1][userX] != '#') {
                    userY++;
                    trueUserY++;
                }
                break;
            case 'd':
                if (!(userX + 1 >= colSize) && world[userY][userX+1] != '#') {
                    userX++;
                    trueUserX++;
                }
                break;
            default:
                break;
            }

            flatInd = userY * colSize + userX;

            if (particleTracker.find(flatInd) != particleTracker.end()) {
                Particle* temp = particleTracker[flatInd].first; 
                cout << "\033[" << 3 << ";" << colSize + 2 << "H" << "PARTICLE DATA: ";
                cout << "\033[" << 4 << ";" << colSize + 2 << "H" << "POSITION: "<<temp->posX<<", "<<temp->posY;
                cout << "\033[" << 5 << ";" << colSize + 2 << "H" << "MASS: "<<temp->mass;
                cout << "\033[" << 6 << ";" << colSize + 2 << "H" << "FLAT INDEX: " << flatInd;

                cout << "\033[" << 7 << ";" << colSize + 2 << "H" << "Position is already reserved!";
            }
            else{ //clear the previous debug info 
                cout << "\033[" << 3 << ";" << colSize + 2 << "H" << string(30, ' ');
                cout << "\033[" << 4 << ";" << colSize + 2 << "H" << string(30, ' ');
                cout << "\033[" << 5 << ";" << colSize + 2 << "H" << string(30, ' ');
                cout << "\033[" << 6 << ";" << colSize + 2 << "H" << string(30, ' ');

                cout << "\033[" << 7 << ";" << colSize + 2 << "H" << string(30, ' ');
            }
            // "\033[s" to save cursor position *only for windows terminals 
            // "\033[u" to restore cursor position 

            cout << "\033[" << trueUserY << ";" << trueUserX << "H" << "\033[s";

            //debugIndex = debugIndex+1 > 22 ? 17 : debugIndex + 1;
        }
        int base = rowSize + 5;
        cout << "\033[" << base << ";" << 1 << "H" << "TRUE POSITION: " << trueUserX << "," << trueUserY << " ";
        cout << "\033[" << base + 1 << ";" << 1 << "H" << "SELECT POSITION: "<<userX <<"," << userY<<" "<<" flatInd: "<<flatInd<<" ";
        cout << "\033[" << base + 2 << ";" << 1 << "H" << "KEY PRESSED : ";
        cout << "\033[" << base + 2 << ";" << 17 << "H" << userIn; 
                           
        cout << "\033[" << base + 3 << ";" << 1 << "H" << "SIZE OF particleTracker: "<<particleTracker.size();
        cout << "\033[" << base + 4 << ";" << 1 << "H" << "SIZE OF spawnedParticles: " << spawnedParticles.size();


        cout << "\033[u"; //restore cursor to user-inputted coordinate
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