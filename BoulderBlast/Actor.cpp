#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <stdlib.h>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

int directionToX(GraphObject::Direction dir)
{
    if (dir == GraphObject::left)
    {
        return -1;
    }
    else if (dir == GraphObject::right)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int directionToY(GraphObject::Direction dir)
{
    if (dir == GraphObject::up)
    {
        return 1;
    }
    else if (dir == GraphObject::down)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

// ACTOR
Actor::Actor(int imageID, int startX, int startY, StudentWorld* stw,
             unsigned int hitPoints, Direction startDir):
GraphObject(imageID, startX, startY, startDir)
{
    stworld = stw;
    setVisible(true);
    isalive = true;
    hp = hitPoints;
}

StudentWorld* Actor::getWorld() const
{
    return stworld;
}

bool Actor::isAlive() const
{
    return isalive;
}

void Actor::setDead()
{
    isalive = false;
}

void Actor::decHitPoints(unsigned int amt)
{
    hp -= amt;
}

bool Actor::allowsAgentColocation() const
{
    return false;
}

bool Actor::allowsBoulder() const
{
    return false;
}

bool Actor::countsInFactoryCensus() const
{
    return false;
}

bool Actor::stopsBullet() const
{
    return false;
}

bool Actor::isDamageable() const
{
    return false;
}

void Actor::damage(unsigned int damageAmt)
{
    hp -= damageAmt;
}

//virtual bool bePushedBy(Agent* a, int x, int y);

bool Actor::isStealable() const
{
    return false;
}

int Actor::getHitPoints() const
{
    return hp;
}

void Actor::setHitPoints(int amt)
{
    hp = amt;
}

bool Actor::tryToBeKilled(unsigned int damageAmt)
{
    hp -= damageAmt;
    if (hp <= 0)
    {
        setDead();
        return true;
    }
    return false;
}

// AGENT

Agent::Agent(int imageID, int startX, int startY, StudentWorld* stw,
             unsigned int hitPoints, Direction startDir):
Actor(imageID, startX, startY, stw, hitPoints, startDir)
{
}

bool Agent::moveIfPossible()
{
    int x, y;
    if (getDirection() == down)
    {
        x = getX();
        y = getY()-1;
    }
    else if (getDirection() == up)
    {
        x = getX();
        y = getY()+1;
    }
    else if (getDirection() == left)
    {
        x = getX()-1;
        y = getY();
    }
    else if (getDirection() == right)
    {
        x = getX()+1;
        y = getY();
    }
    
    bool moveable = true;
    list<Actor*> temp = getWorld()->getAllActors(x, y);
    list<Actor*>::iterator iter = temp.begin();
    while (iter != temp.end())
    {
        PickupableItem* wp = dynamic_cast<PickupableItem*>(*iter);
        Bullet* wq = dynamic_cast<Bullet*>(*iter);
        Exit* wr = dynamic_cast<Exit*>(*iter);
        if (!(*iter == nullptr || wp != nullptr || wq != nullptr))
        {
            moveable = false;
        }
        if (wr != nullptr)
            moveable = true;
        if (getID() == IID_PLAYER && (*iter)->getID() == IID_BOULDER)
        {
            Boulder* bould = dynamic_cast<Boulder*>(*iter);
            if (bould->bePushedTo(getDirection()))
            {
                moveTo(x, y);
                return true;
            }
        }
        iter++;
    }
    if (moveable)
    {
        moveTo(x, y);
        return true;
    }
    return false;
}

bool Agent::canPushBoulders() const
{
    return false;
}

bool Agent::isDamageable() const
{
    return true;
}

void Agent::fire()
{
    if (getDirection() == left)
        getWorld()->addActor(new Bullet(getWorld(), getX()-1, getY(), left));
    if (getDirection() == right)
        getWorld()->addActor(new Bullet(getWorld(), getX()+1, getY(), right));
    if (getDirection() == up)
        getWorld()->addActor(new Bullet(getWorld(), getX(), getY()+1, up));
    if (getDirection() == down)
        getWorld()->addActor(new Bullet(getWorld(), getX(), getY()-1, down));
}



// PLAYER

void Player::doSomething()
{
    if (getHitPoints() <= 0)
    {
        getWorld()->decLives();
        setDead();
    }
    int keyp = 0;
    if (getWorld()->getKey(keyp))
    {
        switch(keyp)
        {
            case KEY_PRESS_DOWN:
                setDirection(down);
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                break;
            case KEY_PRESS_SPACE:
                if (getAmmo() > 0)
                {
                    fire();
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    decreaseAmmo();
                }
                break;
            default:
                break;
        }
        if (keyp != KEY_PRESS_SPACE)
            moveIfPossible();
    }
    getWorld()->setPlayerHP(getHealthPct());
    getWorld()->setPlayerAmmo(getAmmo());
}

Player::Player(int startX, int startY, StudentWorld* stw, Direction dir):
Agent(IID_PLAYER, startX, startY, stw, 20, right)
{
    stworld = stw;
    ammo = 20;
}

bool Player::canPushBoulders() const
{
    return true;
}

int Player::shootingSound() const
{
    return SOUND_PLAYER_FIRE;
}

// Get player's health percentage
unsigned int Player::getHealthPct() const
{
    return getHitPoints()*5;
}

// Get player's amount of ammunition
unsigned int Player::getAmmo() const
{
    return ammo;
}

// Restore player's health to the full amount.
void Player::restoreHealth()
{
    setHitPoints(20);
}

// Increase player's amount of ammunition.
void Player::increaseAmmo()
{
    ammo+=20;
}

void Player::decreaseAmmo()
{
    ammo--;
}

// ROBOT

Robot::Robot(int imageID, int startX, int startY, StudentWorld* stw,
             unsigned int hitPoints, Direction startDir, unsigned int score):
Agent(imageID, startX, startY, stw, hitPoints, startDir)
{
    currtick = 0;
    m_score = score;
    ticks = (28 - getWorld()->getLevel()) / 4;
    if (ticks < 3)
        ticks = 3;
}

bool Robot::canPushBoulders() const
{
    return false;
}
int Robot::shootingSound() const
{
    return SOUND_ENEMY_FIRE;
}
bool Robot::isShootingRobot() const
{
    return false;
}

int Robot::getScore() const
{
    return m_score;
}

void Robot::damage(unsigned int damageAmt)
{
    Actor::damage(damageAmt);
    if (getHitPoints() <= 0)
    {
        setDead();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->addPoints(getScore());
    }
}

bool Robot::stopsBullet() const
{
    return true;
}

bool Robot::isDamageable() const
{
    return true;
}

int Robot::getTicks()
{
    return ticks;
}

int Robot::getCurrTick()
{
    return currtick;
}

void Robot::incCurrTick()
{
    currtick++;
}

// SNARLBOT

SnarlBot::SnarlBot(StudentWorld* world, int startX, int startY, Direction startDir):
Robot(IID_SNARLBOT, startX, startY, world, 10, startDir, 100)
{
}

void SnarlBot::doSomething()
{
    if (!isAlive())
        return;
    if (getCurrTick() % getTicks() == 0)
    {
        if (getWorld()->existsClearShotToPlayer(getX(), getY(), directionToX(getDirection()), directionToY(getDirection())))
        {
            fire();
            getWorld()->playSound(SOUND_ENEMY_FIRE);
        }
        else if (!moveIfPossible())
        {
            if (getDirection() == left)
                setDirection(right);
            else if (getDirection() == right)
                setDirection(left);
            else if (getDirection() == up)
                setDirection(down);
            else if (getDirection() == down)
                setDirection(up);
        }
        incCurrTick();
    }
    else
        incCurrTick();
}

// KLEPTOBOT

KleptoBot::KleptoBot(StudentWorld* world, int startX, int startY, int imageID,
                     unsigned int hitPoints, unsigned int score):
Robot(imageID, startX, startY, world, hitPoints, right, score)
{
    turndist = rand() % 6 + 1;
    turncount = 0;
}

void KleptoBot::doSomething()
{
    if (!isAlive())
        return;
    if (getCurrTick() % getTicks() == 0)
    {
        list<Actor*> temp = getWorld()->getAllActors(getX(), getY());
        list<Actor*>::iterator iter = temp.begin();
        while (iter != temp.end())
        {
            if ((*iter)->isStealable())
            {
                cout << "hello" <<endl;
                if ((rand() % 10 + 1) == 1)
                {
                    cout << "blakka" << endl;
                    goodieid = (*iter)->getID();
                    (*iter)->setDead();
                    getWorld()->playSound(SOUND_ROBOT_MUNCH);
                    incCurrTick();
                    return;
                }
            }
            iter++;
        }
        if (turncount != turndist)
        {
            if (moveIfPossible())
            {
                turncount++;
                incCurrTick();
                return;
            }
        }
        turndist = rand() % 6 + 1;
        int intd = rand() % 4 + 1;
        if (intd == 1)
        {
            setDirection(left);
            if (moveIfPossible())
            {
                incCurrTick();
                return;
            }
            else
            {
                setDirection(up);
                if (moveIfPossible())
                {
                    incCurrTick();
                    return;
                }
                else
                {
                    setDirection(right);
                    if (moveIfPossible())
                    {
                        incCurrTick();
                        return;
                    }
                    else
                    {
                        setDirection(down);
                        if (moveIfPossible())
                        {
                            incCurrTick();
                            return;
                        }
                        else
                        {
                            setDirection(left);
                            incCurrTick();
                            return;
                        }
                    }
                }
            }
        }
        if (intd == 2)
        {
            setDirection(up);
            if (moveIfPossible())
            {
                incCurrTick();
                return;
            }
            else
            {
                setDirection(right);
                if (moveIfPossible())
                {
                    incCurrTick();
                    return;
                }
                else
                {
                    setDirection(down);
                    if (moveIfPossible())
                    {
                        incCurrTick();
                        return;
                    }
                    else
                    {
                        setDirection(left);
                        if (moveIfPossible())
                        {
                            incCurrTick();
                            return;
                        }
                        else
                        {
                            setDirection(up);
                            incCurrTick();
                            return;
                        }
                    }
                }
            }
        }
        if (intd == 3)
        {
            setDirection(right);
            if (moveIfPossible())
            {
                incCurrTick();
                return;
            }
            else
            {
                setDirection(down);
                if (moveIfPossible())
                {
                    incCurrTick();
                    return;
                }
                else
                {
                    setDirection(left);
                    if (moveIfPossible())
                    {
                        incCurrTick();
                        return;
                    }
                    else
                    {
                        setDirection(up);
                        if (moveIfPossible())
                        {
                            incCurrTick();
                            return;
                        }
                        else
                        {
                            setDirection(right);
                            incCurrTick();
                            return;
                        }
                    }
                }
            }
        }
        if (intd == 4)
        {
            setDirection(down);
            if (moveIfPossible())
            {
                incCurrTick();
                return;
            }
            else
            {
                setDirection(left);
                if (moveIfPossible())
                {
                    incCurrTick();
                    return;
                }
                
                else
                {
                    setDirection(up);
                    if (moveIfPossible())
                    {
                        incCurrTick();
                        return;
                    }
                    else
                    {
                        setDirection(right);
                        if (moveIfPossible())
                        {
                            incCurrTick();
                            return;
                        }
                        else
                        {
                            setDirection(down);
                            incCurrTick();
                            return;
                        }
                    }
                }
            }
        }
        incCurrTick();
    }
    else
        incCurrTick();
}

bool KleptoBot::countsInFactoryCensus() const
{
    return true;
}

void KleptoBot::damage(unsigned int damageAmt)
{
    if (getHitPoints() > 0)
    {
        Actor::damage(2);
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    }
    else
    {
        switch (goodieid)
        {
            case 11:
                getWorld()->addActor(new RestoreHealthGoodie(getWorld(), getX(), getY()));
                break;
            case 12:
                getWorld()->addActor(new ExtraLifeGoodie(getWorld(), getX(), getY()));
                break;
            case 13:
                getWorld()->addActor(new AmmoGoodie(getWorld(), getX(), getY()));
                break;
        }
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->addPoints(getScore());
        setDead();
    }
}

int KleptoBot::getTurnDist()
{
    return turndist;
}

int KleptoBot::getTurnCount()
{
    return turncount;
}

void KleptoBot::incTurnCount()
{
    turncount++;
}

// REGULARKLEPTOBOT

RegularKleptoBot::RegularKleptoBot(StudentWorld* world, int startX, int startY):
KleptoBot(world, startX, startY, IID_KLEPTOBOT, 5, 10)
{
}

void RegularKleptoBot::doSomething()
{
    KleptoBot::doSomething();
}

bool RegularKleptoBot::isShootingRobot() const
{
    return false;
};


// ANGRYKLEPTOBOT

AngryKleptoBot::AngryKleptoBot(StudentWorld* world, int startX, int startY):
KleptoBot(world, startX, startY, IID_KLEPTOBOT, 8, 20)
{
}

void AngryKleptoBot::doSomething()
{
    if (getWorld()->existsClearShotToPlayer(getX(), getY(), directionToX(getDirection()), directionToY(getDirection())))
    {
        if (getCurrTick() % getTicks() == 0)
        {
            fire();
            getWorld()->playSound(SOUND_PLAYER_FIRE);
            incCurrTick();
        }
    }
    KleptoBot::doSomething();
}

// EXIT

Exit::Exit(StudentWorld* world, int startX, int startY):
Actor(IID_EXIT, startX, startY, world, 0, none)
{
    setVisible(false);
    playedSound = false;
}

void Exit::doSomething()
{
    if (!getWorld()->anyJewels())
    {
        setVisible(true);
        if (playedSound == false)
        {
            getWorld()->playSound(SOUND_REVEAL_EXIT);
            playedSound = true;
        }
    }
    if (isVisible() && getWorld()->getActor(getX(), getY())->getID() == IID_PLAYER)
        {
            getWorld()->playSound(SOUND_FINISHED_LEVEL);
            getWorld()->addPoints(2000);
            getWorld()->setLevelFinished();
            getWorld()->applyBonusPoints();
        }
}

bool Exit::allowsAgentColocation() const
{
    return true;
}


// WALL
Wall::Wall(int startX, int startY, StudentWorld* stw):
Actor(IID_WALL, startX, startY, stw, 0, none)
{
}

void Wall::doSomething()
{
}

bool Wall::stopsBullet() const
{
    return true;
}

// BOULDER

Boulder::Boulder(StudentWorld* stw, int startX, int startY):
Actor(IID_BOULDER, startX, startY, stw, 10, none)
{
}

void Boulder::doSomething()
{
    if (getHitPoints() <= 0)
        setDead();
}

bool Boulder::isDamageable() const
{
    return true;
}

bool Boulder::stopsBullet() const
{
    return true;
}

bool Boulder::bePushedTo(Direction dir)
{
    if (dir == left)
    {
        if (getWorld()->getActor(getX()-1, getY()) == nullptr || getWorld()->getActor(getX()-1, getY())->getID() == IID_HOLE)
        {
            moveTo(getX()-1, getY());
            return true;
        }
    }
    if (dir == right)
    {
        if (getWorld()->getActor(getX()+1, getY()) == nullptr || getWorld()->getActor(getX()+1, getY())->getID() == IID_HOLE)
        {
            moveTo(getX()+1, getY());
            return true;
        }
    }
    if (dir == up)
    {
        if (getWorld()->getActor(getX(), getY()+1) == nullptr || getWorld()->getActor(getX(), getY()+1)->getID() == IID_HOLE)
        {
            moveTo(getX(), getY()+1);
            return true;
        }
    }
    if (dir == down)
    {
        if (getWorld()->getActor(getX(), getY()-1) == nullptr || getWorld()->getActor(getX(), getY()-1)->getID() == IID_HOLE)
        {
            moveTo(getX(), getY()-1);
            return true;
        }
    }
    return false;
}

// HOLE

Hole::Hole(StudentWorld* stw, int startX, int startY):
Actor(IID_HOLE, startX, startY, stw, 0, none)
{
}

void Hole::doSomething()
{
    if(!isAlive())
        return;
    list<Actor*> temp = getWorld()->getAllActors(getX(), getY());
    list<Actor*>::iterator iter = temp.begin();
    while (iter != temp.end())
    {
            if ((*iter)->getID() == IID_BOULDER)
            {
                setDead();
                (*iter)->setDead();
            }
        iter++;
    }
}

bool Hole::allowsBoulder() const
{
    return true;
}

// BULLET

Bullet::Bullet(StudentWorld* stw, int startX, int startY, Direction startDir):
Actor(IID_BULLET, startX, startY, stw, 0, startDir)
{
}

void Bullet::doSomething()
{
    if (!isAlive())
        return;
    list<Actor*> temp = getWorld()->getAllActors(getX(), getY());
    list<Actor*>::iterator iter = temp.begin();
    while (iter != temp.end())
    {
        if ((*iter)->isDamageable())
        {
            (*iter)->damage(2);
            setDead();
            return;
        }
        if ((*iter)->getID() == IID_WALL || (*iter)->getID() == IID_ROBOT_FACTORY)
        {
            setDead();
            return;
        }
        iter++;
    }
    if (getDirection() == down)
        moveTo(getX(), getY()-1);
    if (getDirection() == up)
        moveTo(getX(), getY()+1);
    if (getDirection() == left)
        moveTo(getX()-1, getY());
    if (getDirection() == right)
        moveTo(getX()+1, getY());
}
bool Bullet::allowsAgentColocation() const
{
    return true;
}


// KLEPTOBOTFACTORY

KleptoBotFactory::KleptoBotFactory(StudentWorld* world, int startX, int startY, ProductType type):
Actor(IID_ROBOT_FACTORY, startX, startY, world, 0, none)
{
    ptype = type;
}

void KleptoBotFactory::doSomething()
{
    int count = 0;
    for (int i = -3; i < 4; i++)
    {
        for (int j = -3; j < 4; j++)
        {
            if (getWorld()->getActor(getX()+i, getY()+j) != nullptr)
            {
                if (getWorld()->getActor(getX()+i, getY()+j)->countsInFactoryCensus())
                {
                    count++;
                }
            }
        }
    }
    list<Actor*> temp = getWorld()->getAllActors(getX(), getY());
    list<Actor*>::iterator iter = temp.begin();
    while (iter != temp.end())
    {
        if ((*iter)->getID() != IID_ANGRY_KLEPTOBOT && (*iter)->getID() != IID_KLEPTOBOT)
        {
            if (count < 3)
            {
                if ((rand() % 50 + 1) == 1)
                {
                    if (ptype == ANGRY)
                        getWorld()->addActor(new AngryKleptoBot(getWorld(), getX(), getY()));
                    if (ptype == REGULAR)
                        getWorld()->addActor(new RegularKleptoBot(getWorld(), getX(), getY()));
                    getWorld()->playSound(SOUND_ROBOT_BORN);
                }
            }
        }
        iter++;
    }
}

bool KleptoBotFactory::stopsBullet() const
{
    return true;
}


// PICKUPABLEITEM

PickupableItem::PickupableItem(StudentWorld* world, int startX, int startY, int imageID, unsigned int score):
Actor(imageID, startX, startY, world, 0, none)
{
    m_score = score;
}

void PickupableItem::doSomething()
{
    if (!isAlive())
        return;
    list<Actor*> temp = getWorld()->getAllActors(getX(), getY());
    list<Actor*>::iterator iter = temp.begin();
    while (iter != temp.end())
    {
        if ((*iter)->getID() == IID_PLAYER)
        {
            if (getID() == IID_JEWEL)
                getWorld()->decJewels();
            getWorld()->addPoints(getScore());
            getWorld()->playSound(SOUND_GOT_GOODIE);
            setDead();
        }
        iter++;
    }
}

bool PickupableItem:: allowsAgentColocation() const
{
    return true;
}

int PickupableItem:: getScore() const
{
    return m_score;
}

// JEWEL

Jewel::Jewel(StudentWorld* world, int startX, int startY):
PickupableItem(world, startX, startY, IID_JEWEL, 50)
{
}

// GOODIE

Goodie::Goodie(StudentWorld* world, int startX, int startY, int imageID,
               unsigned int score):
PickupableItem(world, startX, startY, imageID, score)
{
    stolen = false;
}

void Goodie::doSomething()
{
}

bool Goodie::isStealable() const
{
    return true;
}

// Set whether this goodie is currently stolen.
void Goodie::setStolen(bool status)
{
    stolen = status;
}

// EXTRALIFEGOODIE

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* world, int startX, int startY):
Goodie(world, startX, startY, IID_EXTRA_LIFE, 1000)
{
}

void ExtraLifeGoodie::doSomething()
{
    PickupableItem::doSomething();
    if (getWorld()->getActor(getX(),getY())->getID() == IID_PLAYER)
        getWorld()->incLives();
}


// RESTOREHEALTHGOODIE

RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld* world, int startX, int startY):
Goodie(world, startX, startY, IID_RESTORE_HEALTH, 500)
{
}

void RestoreHealthGoodie::doSomething()
{
    PickupableItem::doSomething();
    if (getWorld()->getActor(getX(),getY())->getID() == IID_PLAYER)
    {
        Player* temp = dynamic_cast<Player*>(getWorld()->getActor(getX(),getY()));
        temp->restoreHealth();
    }
}

// AMMOGOODIE

AmmoGoodie::AmmoGoodie(StudentWorld* world, int startX, int startY):
Goodie(world, startX, startY, IID_AMMO, 100)
{
}

void AmmoGoodie::doSomething()
{
    PickupableItem::doSomething();
    if (getWorld()->getActor(getX(),getY())->getID() == IID_PLAYER)
    {
        Player* temp = dynamic_cast<Player*>(getWorld()->getActor(getX(),getY()));
        temp->increaseAmmo();
    }
}


