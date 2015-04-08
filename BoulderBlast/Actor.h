#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class Actor: public GraphObject
{
    
public:
    Actor(int imageID, int startX, int startY, StudentWorld* stw,
          unsigned int hitPoints, Direction startDir);
    virtual void doSomething() = 0;
    StudentWorld* getWorld() const;
    bool isAlive() const;
    void setDead();
    void decHitPoints(unsigned int amt);
    virtual bool allowsAgentColocation() const;
    virtual bool allowsBoulder() const;
    virtual bool countsInFactoryCensus() const;
    virtual bool stopsBullet() const;
    virtual bool isDamageable() const;
    virtual void damage(unsigned int damageAmt);
    //virtual bool bePushedBy(Agent* a, int x, int y);
    //virtual bool isSwallowable() const;
    virtual bool isStealable() const;
    virtual int getHitPoints() const;
    virtual void setHitPoints(int amt);
    virtual bool tryToBeKilled(unsigned int damageAmt);
    
private:
    StudentWorld* stworld;
    bool isalive;
    int hp;
    
};

class Agent: public Actor
{
public:
    Agent(int imageID, int startX, int startY, StudentWorld* stw,
          unsigned int hitPoints, Direction startDir); 
    // Move to the adjacent square in the direction the agent is facing
    // if it is not blocked, and return true.  Return false if the agent
    // can't move.
    bool moveIfPossible();
    
    // Return true if this agent can push boulders (which means it's the
    // player).
    virtual bool canPushBoulders() const;
    
    virtual bool isDamageable() const;
    
    virtual void fire();
    
private:
};

class Player: public Agent
{
public:
    Player(int startX, int startY, StudentWorld* stw, GraphObject::Direction dir = right);
    virtual void doSomething();
    virtual bool canPushBoulders() const;
    
    // Get player's health percentage
    unsigned int getHealthPct() const;
    
    // Get player's amount of ammunition
    unsigned int getAmmo() const;
    
    virtual int shootingSound() const;
    
    // Restore player's health to the full amount.
    void restoreHealth();
    
    // Increase player's amount of ammunition.
    void increaseAmmo();
    void decreaseAmmo();
    
private:
    GraphObject::Direction d;
    StudentWorld* stworld;
    int ammo;
    int lives;
};

class Robot: public Agent
{
public:
    Robot(int imageID, int startX, int startY, StudentWorld* stw,
          unsigned int hitPoints, Direction startDir, unsigned int score);
    virtual bool canPushBoulders() const;
    virtual int shootingSound() const;
    // Does this robot shoot?
    virtual bool isShootingRobot() const;
    int getScore() const;
    virtual void damage(unsigned int damageAmt);
    int getTicks();
    virtual bool stopsBullet() const;
    virtual bool isDamageable() const;
    int getCurrTick();
    void incCurrTick();
    
private:
    int m_score;
    int ticks;
    int currtick;
};

class SnarlBot: public Robot
{
public:
    SnarlBot(StudentWorld* world, int startX, int startY, Direction startDir);
    virtual void doSomething();
    //virtual void damage(unsigned int damageAmt);
};

class KleptoBot : public Robot
{
public:
    KleptoBot(StudentWorld* world, int startX, int startY, int imageID,
              unsigned int hitPoints, unsigned int score);
    virtual void doSomething();
    virtual bool countsInFactoryCensus() const;
    virtual void damage(unsigned int damageAmt);
    int getTurnDist();
    int getTurnCount();
    void incTurnCount();
private:
    int turndist;
    int turncount;
    int goodieid;
};

class RegularKleptoBot : public KleptoBot
{
public:
    RegularKleptoBot(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool isShootingRobot() const;
};

class AngryKleptoBot : public KleptoBot
{
public:
    AngryKleptoBot(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};

class KleptoBotFactory : public Actor
{
public:
    enum ProductType { REGULAR, ANGRY };
    KleptoBotFactory(StudentWorld* world, int startX, int startY, ProductType type);
    virtual void doSomething();
    virtual bool stopsBullet() const;
private:
    enum ProductType ptype;
};

class Exit : public Actor
{
public:
    Exit(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const;
private:
    bool playedSound;
};

class Wall: public Actor
{
public:
    Wall(int startX, int startY, StudentWorld* stw);
    virtual void doSomething();
    virtual bool stopsBullet() const;
private:
    
};

class Boulder : public Actor
{
public:
    Boulder(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool isDamageable() const;
    virtual bool stopsBullet() const;
    bool bePushedTo(Direction dir);
};

class Hole: public Actor
{
public:
    Hole(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool allowsBoulder() const;
};

class Bullet : public Actor
{
public:
    Bullet(StudentWorld* world, int startX, int startY, Direction startDir);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const;
};

class PickupableItem: public Actor
{
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID,
                   unsigned int score);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const;
    int getScore() const;
private:
    int m_score;
};

class Jewel : public PickupableItem
{
public:
    Jewel(StudentWorld* world, int startX, int startY);
};

class Goodie : public PickupableItem
{
public:
    Goodie(StudentWorld* world, int startX, int startY, int imageID,
           unsigned int score);
    virtual void doSomething();
    virtual bool isStealable() const;
    
    // Set whether this goodie is currently stolen.
    void setStolen(bool status);
private:
    bool stolen;
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};

class RestoreHealthGoodie : public Goodie
{
public:
    RestoreHealthGoodie(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};

class AmmoGoodie : public Goodie
{
public:
    AmmoGoodie(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp



#endif // ACTOR_H_
