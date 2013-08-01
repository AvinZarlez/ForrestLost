/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "game.h"


GameCube::GameCube(CubeID cube)
    : portalTicker(50),
      numMarkers(0)
{
    vid.attach(cube);
}
      
void GameCube::init()
{
    up = TOP;

    vid.initMode(BG0_SPR_BG1);
}

void GameCube::reset() {
    for (unsigned i=0; i<NUM_SIDES; i++) {
        connected[i] = false;
    }

    SetActive(10);

    SetHelp(99);

    vid.bg0.image(vec(0,0), Playfield, 0);
}

Int2 GameCube::getRestPosition(Side s) {
    // Look up the top-left pixel of the bar for the given side.
    // We use a switch so that the compiler can optimize this
    // however if feels is best.
    switch(s) {
    case TOP: return vec(64 - Bars[0].pixelWidth()/2,0);
    case LEFT: return vec(0, 64 - Bars[1].pixelHeight()/2);
    case BOTTOM: return vec(64 - Bars[2].pixelWidth()/2, 128-Bars[2].pixelHeight());
    case RIGHT: return vec(128-Bars[3].pixelWidth(), 64 - Bars[3].pixelHeight()/2);
    default: return vec(0,0);
    }
}

int GameCube::sideConnectionCount() {
    // how many connections are on this cube?
    int result = 0;
    for(int i=0; i<NUM_SIDES; ++i) {
        if (connected[i]) {
            result++;
        }
    }
    return result;
}

bool GameCube::addSideConnection( unsigned s, Location goal) {
    //Add a connection.

    if (connected[s] == false) {
        connected[s] = true;
        if (sideConnectionCount() == 1) {
            unsigned w = 1, h=1;
            if (loc.x <=0) w=0;
            else if (loc.x >= MAP_SIZE-1) w=2;
            if (loc.y <=0) h=0;
            else if (loc.y >= MAP_SIZE-1) h=2;

            if ((w == 1)&&(h==1)) {
                vid.bg0.image(vec(0,0), Playfield, 1);
            } else if ((w == 1)&&(h==0)) {
                vid.bg0.image(vec(0,0), Playfield, 2);
            } else if ((w == 0)&&(h==1)) {
                vid.bg0.image(vec(0,0), Playfield, 3);
            } else if ((w == 2)&&(h==1)) {
                vid.bg0.image(vec(0,0), Playfield, 4);
            } else if ((w == 1)&&(h==2)) {
                vid.bg0.image(vec(0,0), Playfield, 5);
            } else if ((w == 0)&&(h==0)) {
                vid.bg0.image(vec(0,0), Playfield, 6);
            } else if ((w == 2)&&(h==0)) {
                vid.bg0.image(vec(0,0), Playfield, 7);
            } else if ((w == 0)&&(h==2)) {
                vid.bg0.image(vec(0,0), Playfield, 8);
            } else if ((w == 2)&&(h==2)) {
                vid.bg0.image(vec(0,0), Playfield, 9);
            }


            if ((goal.x == loc.x)&&(goal.y == loc.y)) {
                if (vid.sprites[6].isHidden()) {
                    vid.sprites[6].setImage(Boy); // Had UP
                    vid.sprites[6].move(vec(32+16,32+16));
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

bool GameCube::removeSideConnection( unsigned s, bool is_active) {
    if (!vid.sprites[6].isHidden()) {
        vid.sprites[6].hide();
    }
    if (connected[s] == true) {
        connected[s] = false;
        if (!is_active) {
            if (sideConnectionCount() == 0) {
                active = 10;
                vid.bg0.image(vec(0,0), Playfield, 0);
            }
        }
        return true;
    } else {
        return false;
    }
}

void GameCube::animate(float timeStep)
{
    // for (int t = portalTicker.tick(timeStep); t; t--)
    //     for (int i = 0; i < NUM_SIDES; i++) {
    //     }
}
    

void GameCube::draw()
{
    // for (int i = 0; i < NUM_SIDES; i++) {
    // }
    
}

void GameCube::HideAll()
{
    for (int i = 0; i <= 7; i++) {
        if (!vid.sprites[i].isHidden()) {
            vid.sprites[i].hide();
        }
    }
}

Float2 GameCube::velocityFromTilt()
{
    return vid.physicalAccel().xy() * 0.1f;
}

void GameCube::setLoc(int x,int y) {
    loc.x = x;
    loc.y = y;
}

void GameCube::setLoc(Location l,unsigned their_side,unsigned my_side) {
    unsigned px=0,py=0;

    switch (their_side) {
    default:
    case TOP: py=-1;
        switch (my_side) {
            case BOTTOM: up=TOP; break;
            case LEFT: up=RIGHT; break;
            case RIGHT: up=LEFT; break;
            case TOP: up=BOTTOM; break;
        } break;
    case LEFT: px=-1;
        switch (my_side) {
            case BOTTOM: up=RIGHT; break;
            case LEFT: up=BOTTOM; break;
            case RIGHT: up=TOP; break;
            case TOP: up=LEFT; break;
        } break;
    case BOTTOM: py=1;
        switch (my_side) {
            case BOTTOM: up=BOTTOM; break;
            case LEFT: up=LEFT; break;
            case RIGHT: up=RIGHT; break;
            case TOP: up=TOP; break;
        } break;
    case RIGHT: px=1; 
        switch (my_side) {
            case BOTTOM: up=LEFT; break;
            case LEFT: up=TOP; break;
            case RIGHT: up=BOTTOM; break;
            case TOP: up=RIGHT; break;
        } break;
    };
    loc.x = l.x+px;
    loc.y = l.y+py;

    Rotation r = ROT_NORMAL;
    switch (up) {
        case LEFT: r = ROT_LEFT_90; break;
        case BOTTOM: r = ROT_180; break;
        case RIGHT: r = ROT_RIGHT_90; break;
    };
    vid.setRotation(Rotation(r));

    LOG ("New connection! Location: %d,%d\n\n",getX(),getY());
}

int GameCube::getX() {return loc.x;}
int GameCube::getY() {return loc.y;}

void GameCube::SetActive(unsigned a){
    active = a;
    if (a != 5) {
        if (!vid.sprites[5].isHidden()) {
            vid.sprites[5].hide();
        }
    }
}

void GameCube::initGirl() {
    if (vid.sprites[5].isHidden()) {
        vid.sprites[5].setImage(Girl,0);
        vid.sprites[5].move(vec(32+16,32+16));
    }
}

unsigned GameCube::GetActive(){
    return active;
}

void GameCube::SetHelp(unsigned dist)
{
    if ((dist >= 8)||active==10) {
        if (!vid.sprites[0].isHidden()) {
            vid.sprites[0].hide();
        }
    }
    else {
        if (vid.sprites[0].isHidden()) {
            vid.sprites[0].setImage(Help,floor(dist/2));
            vid.sprites[0].move(vec(0,0));
        }
    }
}