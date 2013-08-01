/*
 * Sifteo SDK Example.
 */

#ifndef _GAME_H
#define _GAME_H

#include <sifteo.h>
#include "assets.gen.h"

using namespace Sifteo;

#define NUM_CUBES     3
#define MAP_SIZE 10

extern AssetSlot MainSlot;
    
struct Location {
    int x, y;
};

class GameCube {
public:
    GameCube(CubeID id);

    void init();
    void reset();

    Int2 getRestPosition(Side s);
    int sideConnectionCount();
    bool addSideConnection( unsigned s, Location goal);
    bool removeSideConnection( unsigned s, bool is_active = false );
    
    void animate(float timeStep);
    void draw();
    void HideAll();

    void setLoc(int x,int y);
    void setLoc(Location l,unsigned side,unsigned my_side);

    Float2 velocityFromTilt();
    
    VideoBuffer vid;

    Location loc;
    int getX();
    int getY();
    unsigned up;

    int activeNeighbor;

    void SetActive(unsigned a);
    void initGirl();
    unsigned GetActive();

    void SetHelp(unsigned size);

private:
    unsigned active;

    bool connected[NUM_SIDES];

    TimeTicker portalTicker;
    unsigned numMarkers;
};

class Game {
public:
    Game();

    void loadAssets();
    void title();
    void init();
    void reset();
    void run();
    void cleanup();

    void animate(float dt);\
    void draw();    
    
	static Random random;

    void setGoal(int x,int y);
    Location goal;

private:
    bool forrest_missing;

    CubeSet oldTouching;

    unsigned cur_cube;

    GameCube cube_0, cube_1, cube_2;
    GameCube &getGameCube(unsigned i) {
        switch (i) {
        default:
        case 0: return cube_0;
        case 1: return cube_1;
        case 2: return cube_2;
        };
    }
    
    bool running;

    // Event handlers
    void onTouch(unsigned id);

    void onCubeRefresh(unsigned cid);


    void onNeighborAdd(unsigned c0, unsigned s0, unsigned c1, unsigned s1);
    void onNeighborRemove(unsigned c0, unsigned s0, unsigned c1, unsigned s1);
    void onRestart();
    void onWin();
};

#endif
