#include "StudentWorld.h"
#include "Level.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{
    level = 0;
}

StudentWorld::~StudentWorld()
{
    list<Actor*>::iterator iter = actorlist.begin();
    while (iter != actorlist.end())
    {
        delete (*iter);
        iter = actorlist.erase(iter);
    }
}

int StudentWorld::move()
{
    if (bonuspoints > 0)
        bonuspoints--;
    list<Actor*>::iterator iter = actorlist.begin();
    while (iter != actorlist.end())
    {
        if (levelFinished)
        {
            levelFinished = false;
            level++;
            applyBonusPoints();
            cleanUp();
            bonuspoints = 1000;
            loadLevel();
            return GWSTATUS_FINISHED_LEVEL;
        }
        if (!(*iter)->isAlive())
        {
            if ((*iter)->getID() == IID_PLAYER)
                return GWSTATUS_PLAYER_DIED;
            delete (*iter);
            iter = actorlist.erase(iter);
        }
        else
        {
            (*iter)->doSomething();
            iter++;
        }
    }
    setDisplayText();
    return GWSTATUS_CONTINUE_GAME;
}
void StudentWorld::setDisplayText()
{
    int score = getPoints();
    int level = getLevel();
    unsigned int bonus = getBonus();
    int livesLeft = getLives();
    // Next, create a string from your statistics, of the form:
    // Score: 0000100 Level: 03 Lives: 3 Health: 70% Ammo: 216 Bonus: 34
    ostringstream os;
    os.fill('0');
    os << "Score: " << setw(7) << score << " Level: " << setw(2) << level << " Lives: ";
    os.fill(' ');
    os << setw(2) << livesLeft << " Health: " << setw(3) << getPlayerHP() << "% Ammo: " << setw(3) << getPlayerAmmo() << " Bonus:" << setw(4) << bonus << endl;
    string s = os.str();
    // Finally, update the display text at the top of the screen with your
    // newly created stats
    setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

int StudentWorld::init()
{
    loadLevel();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator iter = actorlist.begin();
    while (iter != actorlist.end())
    {
        delete (*iter);
        iter = actorlist.erase(iter);
    }
}


int StudentWorld::getActorID(int x, int y)
{
    list<Actor*>::iterator iter = actorlist.begin();
    while (iter != actorlist.end())
    {
        if ((*iter)->getX() == x && (*iter)->getY() == y)
        {
            return (*iter)->getID();
        }
        iter++;
    }
    return -1;
}

int StudentWorld::loadLevel()
{
    std::ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << level << ".dat";
    cout << oss.str();
    string curLevel = oss.str();
    Level lev(assetDirectory());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found	||
        result == Level:: load_fail_bad_format)
        return -1; //	something	bad	happened!
    //	otherwise	the	load	was	successful	and	you	can	access	the
    //	contents	of	the	level – here’s	an	example
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            Level::MazeEntry item = lev.getContentsOf(i,j);
            switch(item)
            {
                case Level::player:
                    actorlist.push_front(new Player(i, j, this));
                    break;
                case Level::wall:
                    actorlist.push_front(new Wall(i, j, this));
                    break;
                case Level::boulder:
                    actorlist.push_front(new Boulder(this, i, j));
                    break;
                case Level::hole:
                    actorlist.push_front(new Hole(this, i, j));
                    break;
                case Level::horiz_snarlbot:
                    actorlist.push_front(new SnarlBot(this, i, j, GraphObject::Direction::right));
                    break;
                case Level::vert_snarlbot:
                    actorlist.push_front(new SnarlBot(this, i, j, GraphObject::Direction::down));
                    break;
                case Level::jewel:
                    jewels++;
                    actorlist.push_front(new Jewel(this, i, j));
                    break;
                case Level::extra_life:
                    actorlist.push_front(new ExtraLifeGoodie(this, i, j));
                    break;
                case Level::restore_health:
                    actorlist.push_front(new RestoreHealthGoodie(this, i, j));
                    break;
                case Level::ammo:
                    actorlist.push_front(new AmmoGoodie(this, i, j));
                    break;
                case Level::exit:
                    actorlist.push_front(new Exit(this, i, j));
                    break;
                case Level::angry_kleptobot_factory:
                    actorlist.push_front(new KleptoBotFactory(this, i, j, KleptoBotFactory::ANGRY));
                    break;
                case Level::kleptobot_factory:
                    actorlist.push_front(new KleptoBotFactory(this, i, j, KleptoBotFactory::REGULAR));
                    break;
                    
                default:
                    break;
            }
        }
    }
    return 1; // success!
}

void StudentWorld::addPoints(int pointAmt)
{
    points += pointAmt;
}

int StudentWorld::getPoints()
{
    return points;
}

int StudentWorld::getBonus()
{
    return bonuspoints;
}

int StudentWorld::getLevel()
{
    return level;
}

void StudentWorld::incLevel()
{
    level++;
}

Actor* StudentWorld::getActor(int x, int y)
{
    list<Actor*>::iterator iter = actorlist.begin();
    while (iter != actorlist.end())
    {
        if ((*iter)->getX() == x && (*iter)->getY() == y)
        {
            return *iter;
        }
        iter++;
    }
    return nullptr;
}

list<Actor*> StudentWorld::getAllActors(int x, int y)
{
    list<Actor*> temp;
    list<Actor*>::iterator iter = actorlist.begin();
    while (iter != actorlist.end())
    {
        if ((*iter)->getX() == x && (*iter)->getY() == y)
        {
            temp.push_front(*iter);
        }
        iter++;
    }
    return temp;
}

/** Can agent move to x,y?  (dx and dy indicate the direction of motion)
bool StudentWorld::canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy) const
{
}
**/

// Can a boulder move to x,y?
bool StudentWorld::canBoulderMoveTo(int x, int y)
{
    if (getActor(x, y) == nullptr || getActor(x, y)->getID() == IID_HOLE)
        return true;
    else
        return false;
}

// Try to cause damage to something at a's location.  (a is only ever
// going to be a bullet.)  Return true if something stops a --
// something at this location prevents a bullet from continuing.
bool StudentWorld::damageSomething(Actor* a, unsigned int damageAmt)
{
    if (getActor(a->getX(), a->getY())->isDamageable())
    {
        getActor(a->getX(), a->getY())->damage(damageAmt);
        return true;
    }
    return false;
}

// If a bullet were at x,y moving in direction dx,dy, could it hit the
// player without encountering any obstructions?
bool StudentWorld:: existsClearShotToPlayer(int x, int y, int dx, int dy)
{
    if (dx == 1)
    {
        for (int i = x+1; i < 16; i++)
        {
            if (getActor(i, y) != nullptr)
            {
                if (getActor(i, y)->getID() == IID_PLAYER)
                    return true;
                else if (getActor(i, y)->stopsBullet())
                    return false;
            }
        }
    }
    if (dx == -1)
    {
        for (int i = x-1; i > 0; i--)
        {
            if (getActor(i, y) != nullptr)
            {
                if (getActor(i, y)->getID() == IID_PLAYER)
                    return true;
                if (getActor(i, y)->stopsBullet())
                    return false;
            }
        }
    }
    if (dy == 1)
        
    {
        for (int i = y+1; i < 16; i++)
        {
            if (getActor(x, i) != nullptr)
            {
                if (getActor(x, i)->getID() == IID_PLAYER)
                    return true;
                if (getActor(x, i)->stopsBullet())
                    return false;
            }
        }
    }
    if (dy == -1)
    {
        for (int i = y-1; i > 0; i--)
        {
            if (getActor(x, i) != nullptr)
            {
                if (getActor(x, i)->getID() == IID_PLAYER)
                    return true;
                if (getActor(x, i)->stopsBullet())
                    return false;
            }
        }
    }
    return false;
}

// If a factory is at x,y, how many items of the type that should be
// counted are in the rectangle bounded by x-distance,y-distance and
// x+distance,y+distance?  Set count to that number and return true,
// unless an item is on the factory itself, in which case return false
// and don't care about count.  (The items counted are only ever going
// KleptoBots.)

void StudentWorld::addActor(Actor* actor)
{
    actorlist.push_front(actor);
}

// Are there any jewels left on this level?
bool StudentWorld::anyJewels()
{
    if (jewels == 0)
        return false;
    else
        return true;
}

// Reduce the count of jewels on this level by 1.
void StudentWorld::decJewels()
{
    jewels--;
}

// Indicate that the player has finished the level.
void StudentWorld::setLevelFinished()
{
    levelFinished = true;
}

void StudentWorld::applyBonusPoints()
{
    points += bonuspoints;
}

int StudentWorld::getPlayerHP()
{
    return playerhp;
}

void StudentWorld::setPlayerHP(int hp)
{
    playerhp = hp;
}

int StudentWorld::getPlayerAmmo()
{
    return playerammo;
}

void StudentWorld::setPlayerAmmo(int ammo)
{
    playerammo = ammo;
}
    
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
