/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "game.h"

Random Game::random;

Game::Game()
    : cube_0(0), cube_1(1), cube_2(2),
      running(true)
{}

void Game::title()
{
    //Display title screen
    const float titleTime = 2.0f;

    ScopedAssetLoader loader;
    AssetConfiguration<1> config;
    config.append(MainSlot, GameAssets);
    loader.start(config);

    for (CubeID i = 0; i < NUM_CUBES; ++i) {
        getGameCube(i).vid.initMode(BG0);
    }

    SystemTime titleDeadline = SystemTime::now() + titleTime;
    
    for (CubeID i = 0; i <= NUM_CUBES; ++i) {
        getGameCube(i).vid.bg0.image(vec(0,0), Title); //Display the main title on all screens.
    }

    while (SystemTime::now() < titleDeadline || !loader.isComplete()) {
        //Repeating nothing until time runs out + is loaded.
    }
}

void Game::init()
{   
    for (unsigned i = 0; i < NUM_CUBES; i++)
         getGameCube(i).init();

    Events::neighborAdd.set(&Game::onNeighborAdd, this);
    Events::neighborRemove.set(&Game::onNeighborRemove, this);
    Events::gameMenu.set(&Game::onRestart, this, "« Restart »");

    Events::cubeTouch.set(&Game::onTouch, this);

    reset();
}

void Game::setGoal(int x,int y) {
    goal.x = x;
    goal.y = y;
}

void Game::reset() {

    Random r;

    cur_cube = 0;
    do {
       setGoal(r.randint(0,MAP_SIZE-1),r.randint(0,MAP_SIZE-1));
    } while ((goal.x == 5)&&(goal.y == 5));
    
    forrest_missing = true;

    for (unsigned i = 0; i < NUM_CUBES; i++)
         getGameCube(i).reset();

    getGameCube(cur_cube).SetActive(5);
    getGameCube(cur_cube).initGirl();
    getGameCube(cur_cube).setLoc(5,5);
    getGameCube(cur_cube).vid.bg0.image(vec(0,0), Playfield, 1);
}

void Game::cleanup()
{
    Events::neighborAdd.unset();
    Events::neighborRemove.unset();
    Events::gameMenu.unset();
}

void Game::animate(float dt)
{
     for (unsigned i = 0; i < NUM_CUBES; i++)
         getGameCube(i).animate(dt);
}

void Game::onTouch(unsigned id)
{
    if (CubeID(id).isTouching()&& !oldTouching.test(id)) {
        oldTouching.mark(id);
    }
    else if(!CubeID(id).isTouching() && oldTouching.test(id))
    {
        oldTouching.clear(id);
        if ((forrest_missing))
        {
            if (getGameCube(id).GetActive() < NUM_SIDES) {
                if (getGameCube(id).activeNeighbor == cur_cube) {

                    if ((getGameCube(id).loc.x == goal.x)&&(getGameCube(id).loc.y == goal.y)) {
                        LOG("You won!\n");
                        onWin();
                        return;
                    }

                    int xdir = (getGameCube(id).loc.x - getGameCube(cur_cube).loc.x);
                    int ydir = (getGameCube(id).loc.y - getGameCube(cur_cube).loc.y);

                    const float titleTime = 0.25f;
                    SystemTime titleDeadline = SystemTime::now() + titleTime;

                    while (SystemTime::now() < titleDeadline) {
                        TimeDelta t = titleDeadline - SystemTime::now();

                        if ( xdir < 0 ) {
                            getGameCube(cur_cube).vid.sprites[5].move(vec(
                                int( (48*(t.milliseconds()/(titleTime*1000)))),
                                48
                                ));
                        } else if ( xdir > 0 ) {
                            getGameCube(cur_cube).vid.sprites[5].move(vec(
                                int( 48+48-(48*(t.milliseconds()/(titleTime*1000)))),
                                48
                                ));
                        } else if ( ydir < 0 ) {
                            getGameCube(cur_cube).vid.sprites[5].move(vec(
                                48,
                                int( (48*(t.milliseconds()/(titleTime*1000))))
                                ));
                        } else if ( ydir > 0 ) {
                            getGameCube(cur_cube).vid.sprites[5].move(vec(
                                48,
                                int( 48+48-(48*(t.milliseconds()/(titleTime*1000))))
                                ));
                        }

                        getGameCube(cur_cube).draw();
                        System::paint();
                    }

                    getGameCube(cur_cube).activeNeighbor = id;

                    CubeID c = CubeID(cur_cube);
                    Neighborhood n = Neighborhood(c);
                    getGameCube(cur_cube).SetActive(n.sideOf(id));

                    getGameCube(id).removeSideConnection(getGameCube(id).GetActive(),true);

                    getGameCube(id).SetActive(5);
                    if (getGameCube(id).vid.sprites[5].isHidden()) {
                        getGameCube(id).vid.sprites[5].setImage(Girl,0);
                    }

                    getGameCube(cur_cube).addSideConnection( getGameCube(cur_cube).GetActive() , goal);

                    cur_cube = id;

                    for (unsigned i = 0; i < NUM_CUBES; i++) {
                        getGameCube(i).SetHelp((abs(getGameCube(i).loc.x - goal.x)+abs(getGameCube(i).loc.y - goal.y)));
                    }

                    titleDeadline = SystemTime::now() + titleTime;

                    while (SystemTime::now() < titleDeadline) {
                        TimeDelta t = titleDeadline - SystemTime::now();

                        if ( xdir > 0 ) {
                            getGameCube(cur_cube).vid.sprites[5].move(vec(
                                int(48 - (48*(t.milliseconds()/(titleTime*1000)))),
                                48
                                ));
                        } else if ( xdir < 0 ) {
                            getGameCube(cur_cube).vid.sprites[5].move(vec(
                                int(48 + (48*(t.milliseconds()/(titleTime*1000)))),
                                48
                                ));
                        } else if ( ydir > 0 ) {
                            getGameCube(cur_cube).vid.sprites[5].move(vec(
                                48,
                                int(48 - (48*(t.milliseconds()/(titleTime*1000))))
                                ));
                        } else if ( ydir < 0 ) {
                            getGameCube(cur_cube).vid.sprites[5].move(vec(
                                48,
                                int(48 + (48*(t.milliseconds()/(titleTime*1000))))
                                ));
                        }

                        getGameCube(cur_cube).draw();
                        System::paint();
                    }

                    for (unsigned i = 0; i < NUM_CUBES; i++) {
                        getGameCube(i).SetHelp(99);
                    }

                }
            }
        } else {
            reset();
        }
    }
}

void Game::onCubeRefresh(unsigned cid) {
    if (getGameCube(cid).GetActive() < 4) {
        if (getGameCube(getGameCube(cid).activeNeighbor).GetActive() == 10) {
            getGameCube(cid).removeSideConnection(getGameCube(cid).GetActive());
        }
    } else {
        getGameCube(cid).SetHelp(99);
    }
}
     
void Game::onNeighborAdd(unsigned c0, unsigned s0, unsigned c1, unsigned s1)
{
    if ((c0 >= NUM_CUBES || c1 >= NUM_CUBES) || !forrest_missing)
        return;

    bool sfx = false;
    if (getGameCube(c1).GetActive() == 10) {
        if ((getGameCube(c0).GetActive()<4)or(getGameCube(c0).GetActive()==5)) {

            int new_side = s0-getGameCube(c0).up;
            while (new_side < 0) { new_side+=NUM_SIDES; }

            getGameCube(c1).setLoc(getGameCube(c0).loc,new_side,s1);

            if ( ((getGameCube(c1).getX() >= 0)&&(getGameCube(c1).getX() < MAP_SIZE)) &&
                 ((getGameCube(c1).getY() >= 0)&&(getGameCube(c1).getY() < MAP_SIZE)) ) {
                sfx |= getGameCube(c1).addSideConnection( s1 , goal);
                getGameCube(c1).SetActive(s1);
                getGameCube(c1).activeNeighbor = c0;
            }


        } 
    }
    else if (getGameCube(c0).GetActive() == 10) {
        if ((getGameCube(c1).GetActive()<4)or(getGameCube(c1).GetActive()==5)) {


            int new_side = s1-getGameCube(c1).up;
            while (new_side < 0) { new_side+=NUM_SIDES; }

            getGameCube(c0).setLoc(getGameCube(c1).loc,new_side,s0);

            if ( ((getGameCube(c0).getX() >= 0)&&(getGameCube(c0).getX() < MAP_SIZE)) &&
                 ((getGameCube(c0).getY() >= 0)&&(getGameCube(c0).getY() < MAP_SIZE)) ) {
                sfx |= getGameCube(c0).addSideConnection( s0 , goal);

                getGameCube(c0).SetActive(s0);
                getGameCube(c0).activeNeighbor = c1;
            }

         
        } 
    }
}

void Game::onNeighborRemove(unsigned c0, unsigned s0, unsigned c1, unsigned s1)
{
    if ((c0 >= NUM_CUBES || c1 >= NUM_CUBES) || !forrest_missing)
        return;

    bool check = false;
    check |= getGameCube(c0).removeSideConnection( s0);
    check |= getGameCube(c1).removeSideConnection( s1);

    for (unsigned i = 0; i < NUM_CUBES; i++) {
        onCubeRefresh(i);
    }

}

void Game::onRestart()
{
    //running = false;
    reset();
}

void Game::onWin()
{
    for (CubeID i = 0; i <= NUM_CUBES; ++i) {
        getGameCube(i).vid.bg0.image(vec(0,0), Victory);
        getGameCube(i).HideAll();
    }

    forrest_missing = false;

}

void Game::draw()
{
    for (unsigned i = 0; i < NUM_CUBES; i++) {
        GameCube &gc = getGameCube(i);
        
        gc.draw();

    }
}

void Game::run()
{
    TimeStep ts;

    while (running) {
        ts.next();
        
        // Real-time for animations
        animate(ts.delta());
        
        // Put stuff on the screen!
        draw();
        System::paint();
    }
}