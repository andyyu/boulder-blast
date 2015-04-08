#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
#include <sstream>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Agent;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    int loadLevel();
    int getActorID(int x, int y);
    void addPoints(int pointAmt);
    int getPoints();
    int getLevel();
    void incLevel();
    Actor* getActor(int x, int y);
    list <Actor*> getAllActors(int x, int y);
    void addActor(Actor* actor);
    void setDisplayText();
    
    //bool canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy) const;
    bool canBoulderMoveTo(int x, int y);
    //bool isPlayerColocatedWith(int x, int y) const;
    bool damageSomething(Actor* a, unsigned int damageAmt);
    //bool swallowSwallowable(Actor* a);
    bool existsClearShotToPlayer(int x, int y, int dx, int dy);
    //bool doFactoryCensus(int x, int y, int distance, int& count);
    //Actor* getColocatedStealable(int x, int y) const;
    //void restorePlayerHealth();
    //void increaseAmmo();
    bool anyJewels();
    void decJewels();
    void setLevelFinished();
    void applyBonusPoints();
    int getBonus();
    int getPlayerHP();
    void setPlayerHP(int hp);
    int getPlayerAmmo();
    void setPlayerAmmo(int ammo);

private:
    list <Actor*> actorlist;
    int playerhp = 100;
    int level;
    int jewels = 0;
    int points = 0;
    int playerammo = 20;
    int bonuspoints = 1000;
    bool levelFinished = false;
};

#endif // STUDENTWORLD_H_
