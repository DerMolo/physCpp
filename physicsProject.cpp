#include <chrono>
#include <thread>
#include <iostream>

#include <unordered_map>
#include<string>
#include<vector>
#include<conio.h>

#include <numbers>

using namespace std;

//GLOBAL VARS 

const int colSize = 50;
const int rowSize = 20;
const float spatialUnit = 0.5; //meters 

//GLOBAL CLOCK 
float worldTime = 0.0;
const float worldTick = 0.016;

float lowerBound_Y = rowSize - (rowSize-1 + rowSize) * spatialUnit; 
float upperBound_Y = rowSize - (rowSize)*spatialUnit; 

float leftBound_X = 1;
float rightBound_X = (colSize - 1) * spatialUnit;

struct Particle {
    float mass; 

    float velX; 
    float velY; 

    float accX; 
    float accY; 

    float coordX; 
    float coordY;

    int posX; 
    int posY; 
    
    //global forces 
    float gravity = -9.8; 
    float dragY;
    float dragX; 

    Particle(int x, int y, float m) {
         mass = m;

         velX = -5000; 
         velY = 0;

         accX = 0;
         accY = 0;

         dragY = 0; 
         dragX = 0; 

         posX = x; 
         posY = y;

         coordX = posX * spatialUnit;
         coordY = rowSize - (posY + rowSize) * spatialUnit;
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

void debugParticle(Particle*& speck) {
    //debugging trajectory 
    cout << "\033[" << 3 << ";" << colSize + 2 << "H" << "PARTICLE DATA: ";
 
    cout << "\033[" << 5 << ";" << colSize + 2 << "H" << "VELOCITY: " << speck->velX << ", " << speck->velY << "  ";
    cout << "\033[" << 6 << ";" << colSize + 2 << "H" << "ACCEL   : " << speck->accX << ", " << speck->accY << "  ";

    cout << "\033[" << 7 << ";" << colSize + 2 << "H" << "TERM POS: " << speck->posX << ", " << speck->posY << "  ";
    cout << "\033[" << 8 << ";" << colSize + 2 << "H" << "COORD POS: " << speck->coordX << "," << speck->coordY << "  ";
}

void coordToPos(Particle*& speck) {
    speck->posX = speck->coordX / spatialUnit; //rendered position
    speck->posY = (rowSize - speck->coordY) / spatialUnit - rowSize;
}

int posToFlatInd(Particle*& speck) {
    return speck->posY * colSize + speck->posX;
}

void renderWorld(char* world, vector<Particle*> tempParts) { 

    /*
    current objective: 
   * apply a force to every particle in the scene. (ensure proper trajectory) 
   * ensure particles don't cross border '#' (basic collisions) 
   * 
   * later objective: 
   * handle all particle-based collisions 
    */

    worldTime += worldTick; 

    float dampeningFactor = 0.5; 

    //each spatial unit is .5m

    //sequential calculations doesn't account for collision chains (if A->B & B->C, B never registers collision w/ C *and vice versa) 

    unordered_map<int, Particle*> particleTracker; //neccessary for tracking collisions

    vector<pair<Particle*, Particle*>> ParticleContacts; 
    vector<Particle*> BoundaryContacts; 

    int debugIndex = 0;
    int targetIndex = 0;

    cout << "\033[" << 4 << ";" << colSize + 2 << "H" << "NUMBER OF PARTICLES : "<<tempParts.size();

    for (auto speck : tempParts) { //calculcating intial trajectory & detecting collisions 

        Particle* tempPart = speck; 
        //clearing previous position: 

        int flatInd = posToFlatInd(speck);
        int tempFlatInd = flatInd; 

        //calculating new position: 
        //euler's update rule: 

        speck->velY += (speck->accY + speck->gravity + speck->dragY) * worldTick; 
        speck->velX += (speck->accX + speck->dragX) * worldTick;
        
        speck->coordX += speck->velX * worldTick; //spatially accurate 
        speck->coordY += speck->velY * worldTick;

        //converting spatial coordinate to terminal coordinate 

        coordToPos(speck);

        flatInd = speck->posY * colSize + speck->posX;

        //detecting collisions
        if (particleTracker.find(flatInd) == particleTracker.end()) {
            //if (world[flatInd] != '#') {
            bool withinBounds = (speck->coordX < rightBound_X && speck->coordX > leftBound_X) && (speck->coordY < upperBound_Y && speck->coordY > lowerBound_Y);
            if(withinBounds){
                particleTracker[flatInd] = speck;
                if(debugIndex == targetIndex) {
                    debugParticle(speck);
                    cout << "\033[" << 9 << ";" << colSize + 2 << "H" << string(50, ' ');
                    cout << "\033[" << 10 << ";" << colSize + 2 << "H" << string(50, ' ');
                    cout << "\033[" << 11 << ";" << colSize + 2 << "H" << " RIGHT BOUND " << rightBound_X << "  " << "LEFT BOUND: "<<leftBound_X;
                    cout << "\033[" << 12 << ";" << colSize + 2 << "H" << " world[flatInd] " << world[flatInd] << "   " << " world[tempFlatInd]: " << world[tempFlatInd] << "   ";
                    cout << "";
                }
            }
            else {
                particleTracker[tempFlatInd] = speck;
                BoundaryContacts.push_back(speck);

                if(debugIndex == targetIndex){
                    debugParticle(speck);
                    cout << "\033[" << 9 << ";" << colSize + 2 << "H" << " BOUNDARY CONTACT " << "  " << speck->coordX << "," << speck->coordY << "  ";
                    cout << "\033[" << 10 << ";" << colSize + 2 << "H" << " LOWER BOUND " << lowerBound_Y << "  ";
                    cout << "\033[" << 11 << ";" << colSize + 2 << "H" << " RIGHT BOUND " << rightBound_X << "  ";
                    cout << "\033[" << 12 << ";" << colSize + 2 << "H" << " world[flatInd] " << world[flatInd] << "   " << " world[tempFlatInd]: " << world[tempFlatInd] << "   ";
                    cout << "\033[" << 13 << ";" << colSize + 2 << "H" << " flatInd: " << flatInd << "   " << " tempFlatInd: " << tempFlatInd << "   ";
                    cout << "";
                }

            }
        }
        else 
            ParticleContacts.push_back({ speck,particleTracker[flatInd] });

        if(debugIndex == targetIndex)
            debugParticle(speck);

        if (tempFlatInd != flatInd) {//clears path if particle has shifted positions 
            world[tempFlatInd] = '.';

            int tempPosX = tempFlatInd % colSize; 
            int tempPosY = tempFlatInd / colSize; 

            world[flatInd] = '@';
            cout << "\033[" << tempPosY + 2 << ";" << tempPosX + 1 << "H" << world[tempFlatInd];
        }

        debugIndex++;
    }

    debugIndex = 0;
    //handling wall collisions
    for(auto speck: BoundaryContacts){

        float normalX = 0;
        float normalY = 0; 

        if (speck->posX >= colSize - 1)
            normalX = -1;
        else if (speck->posX <= 0)
            normalX = 1;

        if (speck->posY >= rowSize - 1)
            normalY = 1;
        else if (speck->posY <= 0)
            normalY = -1;
        
        speck->velX += -(1 + dampeningFactor) * (speck->velX * normalX) * normalX;
        speck->velY += -(1 + dampeningFactor) * (speck->velY * normalY) * normalY;

        //distance = (vx * nx) + (vy * ny)
        //radius = 1m

        float radius = 1.0; 
        float depth = 0.0;

        // Calculate penetration based on which normal was triggered
        if (normalX == -1.0) {
            depth = (speck->coordX + radius) - rightBound_X;
        }
        else if (normalX == 1.0) {
            depth = radius - speck->coordX; // Assuming left spatial bound is 0
        }

        if (normalY == -1.0) {
            depth = (speck->coordY + radius) - lowerBound_Y;
        }
        else if (normalY == 1.0) {
            depth = radius - speck->coordY; // Assuming top spatial bound is 0
        }

        // Apply correction (pushing out along the normal)
        if (depth > 0.0) {
            speck->coordX += depth * normalX;
            speck->coordY += depth * normalY;
        }

        //translating to terminal-based coordinate
        coordToPos(speck);

        if(debugIndex == targetIndex)
            debugParticle(speck);

        debugIndex++;
    }

    //handling particle-to-particle collisions 
    for (auto speckPair : ParticleContacts) {
        Particle* A = speckPair.first; 
        Particle* B = speckPair.second;

        float Dx = B->coordX - A->coordX;
        float Dy = B->coordY - A->coordY; 
        float length = sqrt(Dx * Dx + Dy * Dy);

        float normalX = Dx / length; 
        float normalY = Dy / length; 

        float vABx = A->velX - B->velX;
        float vABy = A->velY - B->velY;

        float impulseX = ((-1 + dampeningFactor) * (vABx * normalX))/( 1/A->mass + 1/B->mass);
        float impulseY = ((-1 + dampeningFactor) * (vABy * normalY)) / (1 / A->mass + 1 / B->mass);

        A->velX += (impulseX / A->mass) * normalX;
        B->velX += (impulseX / B->mass) * normalX;

        A->velY += (impulseY / A->mass) * normalY;
        B->velY += (impulseY / B->mass) * normalY;

        float overlap = (1 + 1) - length; //radiusA + radiusB - length
        float correction = overlap / 2.0;

        A->coordX -= correction * normalX; 
        A->coordY -= correction * normalY; 

        B->coordX += correction * normalX;
        B->coordY += correction * normalY;

        //translating to terminal-based coordinate
        coordToPos(A);
        coordToPos(B);
    }

    //drawing complete particle positions
    for (auto speck : tempParts) {
        int worldInd = posToFlatInd(speck);
        cout << "\033[" << speck->posY + 2 << ";" << speck->posX + 1 << "H" << world[worldInd];
    }
}

void cleanDebug() {
    for(int i = 3;i<=8;i++)
        cout << "\033[" << i << ";" << colSize + 2 << "H" << string(30, ' ');

    int base = rowSize + 3;

    for(int i = 0;i<=6;i++)
        cout << "\033[" << base + i<< ";" << 1 << "H" << string(50, ' ');

}

void clearParticleDebug() {
    for (int i = 3; i <= 8; i++)
        cout << "\033[" << i << ";" << colSize + 2 << "H" << string(30, ' ');
}

int main()
{
    int targetFps = 60;
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
    basic objective: 
    allowing the user to place particles (then run the simulation) 

    later objective: 
    - allowing users to select pre-packaged particle shapes and insert them into the scene 
    - allowing the user to apply certain scenarios like: vortex, windiness, explosion, 
    */

    printWorld(*world); 

    cout << "\033[" << rowSize + 3 << ";" << 1 << "H" << "TO FINALIZE SETUP, PRESS 'E'";
    cout << "\033[" << rowSize + 4 << ";" << 1 << "H" << "TO PLACE DOWN A PARTICLE, PRESS 'ENTER'";

    int userX = colSize/2;
    int userY = rowSize/2; 

    vector<Particle*> spawnedParticles; 

    char userIn ;

    //int debugIndex = 16; 
    //unordered_map<int, pair<Particle*,int>> particleTracker; 

    unordered_map<int, Particle*> particleTracker;

    int flatInd = userY * colSize + userX;

    while (true) { //SETUP PHASE 

        int trueUserX = userX + 1; 
        int trueUserY = userY + 2; // terminal offset + world offset 

        if (_kbhit) {
            userIn = _getch();
            if (userIn == '\r') {
                //offsetting userY by 3 to account for world's position & terminal indexing 
                cout << "\033[" << trueUserY << ";" << trueUserX  << "H" << '@';

                if (particleTracker.find(flatInd) == particleTracker.end()) { //creating particle

                    world[userY][userX] = '@';
                    //rowSize - (userY + rowSize) : converting from terminal-based coordinates to 

                    Particle* temp = new Particle(userX, userY, 1.0);
                    spawnedParticles.push_back(temp);
                    
                    particleTracker[flatInd] = temp; //tracking particle 
                } //spawning overlapping particles is prohibited
            }

            if (tolower(userIn) == 'e')  //exiting setup phase 
                break;

            if (tolower(userIn) == '\b' && particleTracker.find(flatInd) != particleTracker.end()) { //backspace to delete select particles 
                Particle* target = particleTracker[flatInd];

                particleTracker.erase(flatInd); 

                //linear search 
                for (auto i = spawnedParticles.begin(); i != spawnedParticles.end(); i++) {
                    if (*i == target) {
                        spawnedParticles.erase(i);
                        break;
                    }
                }

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
                Particle* temp = particleTracker[flatInd]; 

                debugParticle(temp);
            }
            else //clear the previous debug info 
                clearParticleDebug();

            // "\033[s" to save cursor position *only for windows terminals 
            // "\033[u" to restore cursor position 

            cout << "\033[" << trueUserY << ";" << trueUserX << "H" << "\033[s";
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

    cleanDebug();

    int frameCount = 0;

    while (true) {
        auto frameStart = chrono::steady_clock::now();

        //PHYS WORK

        renderWorld(*world, spawnedParticles);

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

        frameCount++; 
        cout << "\033[1;1H" << "FPS: " << 1.0f / totalFrameTime.count() << "  " << frameCount << endl;
    }

    return 0;
}