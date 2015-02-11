#ifndef __FACTION_H_INCLUDED__
#define __FACTION_H_INCLUDED__

#include "Items.h"
#include "Effects.h"
#include "InputState.h"
#include "JobManager.h"
#include "Text.h"
#include "defs.h"

#include <list>
#include <set>

// TODO: Make it so faction units cannot harm eachother,
// Do the same with Allied Factions.
class Faction;

extern Faction *conFact;

class Npc;
std::set<int> npcTrace(int xa, int ya, int xb, int yb, int id,
                       std::set<int> exceptions);
extern std::vector<Npc> npclist;

class Npc
{
public:
    // TODO: Add racial defaults for attributes and skill growth,
    // Will allow interesting mechanics and fix spawning variety.
    sf::Sprite img;
    std::list<Item> inventory;
    std::list<Item> equipped;
    std::string bloodcontent;
    std::string tags;
    std::string Faction;
    bool attacking;
    bool firstStrike;
    bool imgRotates;
    float angle;
    float turnSpeed;
    sf::Vector2f speed;
    sf::Vector2f shootPos;
    bool prone;
    bool needsPath;
    bool toDelete;

    bool hasJob;

    sf::Vector2f momentum;

    class BodyDefinition
    {
    public:
        // Supposed to house the creature when it's fully healthy.
        std::string originalBodyParts;

        std::string bodyParts;

        void bodyPartFind(std::string part, int amount);

        int head;
        int eyes;  // Mask = Eyes+Mouth
        int mouth; // Mask = Eyes+Mouth
        int neck;
        int chest;
        int back;
        int waist;
        int legs;
        int rightupperarm; // int rua
        int rightlowerarm; // int rla
        int righthand;     // rh
        int rightgrasp;    // rg
        int leftupperarm;  // int lua
        int leftlowerarm;  // int lla
        int lefthand;      // lh
        int leftgrasp;     // lg
        int rightupperleg; // rul
        int rightlowerleg; // rll
        int rightfoot;     // rf
        int leftupperleg;  // lul
        int leftlowerleg;  // lll
        int leftfoot;      // lf
    };
    BodyDefinition body;

    float rot;
    float xxx;
    float yyy;
    float degrees;
    std::vector<int> exceptions;

    std::set<int> melee(int min, int max, int range,
                        std::set<int> exception = std::set<int>());

    class PathFinding
    {
    public:
        int *mypathBank[1];
        // stores length of the found path for critter
        int mypathLength;
        // stores current position along the chosen path for critter
        int mypathLocation;
        int mypathStatus;
        int myxPath;
        int myyPath;
        std::vector<int> xPa;
        std::vector<int> yPa;
        void endMem();
        void myFindPath(int sx, int sy, int ex, int ey);
        int myReadPathX(int pathfinderID, int xpathLocation);
        int myReadPathY(int pathfinderID, int ypathLocation);
        void myReadPath(int pathfinderID, int currentX, int currentY,
                        int pixelsPerFrame);
    };
    PathFinding pathFinding;

    short imgstrx;
    short imgstry;
    short imgendx;
    short imgendy;
    bool isPlayer;
    bool isNPC;
    bool hasSpawned;
    bool needsFood;
    bool allowedFood;
    bool needsWater;
    bool allowedDrink;
    bool allowedMove;
    bool grappling;
    int cbaseid;
    int id;
    int id2;
    std::string name;
    char surname;
    bool alive;
    bool stillalive;
    unsigned int ticksalive;
    bool useditem;
    bool canmove;
    std::string target;
    sf::Vector2f targetPos;
    int targetId;
    int targetVectorId;
    bool atTarget;
    bool hasTarget;
    std::string action;
    short regenrate;
    short regentimer;
    short regentimerint;
    float moverateint;
    float moverate;
    float movetimer;
    float movetimerint;
    float movetimerrate;
    float attacktimer;
    float attacktimerint;
    short hungerrate;
    short hungertimer;
    short hungertimerint;
    short thirstrate;
    short thirsttimer;
    short thirsttimerint;
    short breathrate; // not needed?
    short breathtimer;
    short breathtimerint;
    std::string race;
    int age;
    // 1 = Babeh, 2 = Child, 3 = Teenager, 4 = Young Adult,
    // 5 = Aged Adult, 6 = Elder
    int ageType;
    std::string gender;
    char direction;
    int worshippers;
    int gypos;
    int gxpos;
    int planet;
    float ypos;
    float xpos;
    float zpos;
    int rypos;
    int rxpos;
    float maxhealth;
    float health;
    short mana;
    short reginterntemp;
    short interntemp;
    short regtemp;
    short temp;
    short breathmax;
    long maxhunger;
    float hunger;
    short nutrition;
    long maxthirst;
    float thirst;
    short maxstamina;
    short stamina;
    int credits;

    sf::Vector2f tentArm1;
    sf::Vector2f tentArm2;
    sf::Vector2f tentEnd1;
    sf::Vector2f tentEnd2;

    bool consumeFlesh;
    bool consumeVeggy;
    bool consumeWater;

    void effectStats();

    class Skill
    {
    public:
        int strength;
        int dexterity;
        int agility;
        int charisma;
        int endurance;
        int intelligence;
        int wisdom;
        int perception;
        int strengthxp;
        int dexterityxp;
        int charismaxp;
        int endurancexp;
        int intelligencexp;
        int wisdomxp;
        int perceptionxp;
        int agilityxp;

        void Train(std::string skill, int amount = 1, int skillgain = 1);
    };
    Skill skills;

    class TargetInfo
    {
    public:
        Item *item;
        Npc *npc;
        Tile *tile;
    };
    TargetInfo targetInfo;

    int killcount;
    short reach;
    short size;
    short stench;
    float viewangle;
    float viewrange;

    Npc();
    void reCreateSkills();
    void blankSkills();
    bool hasWeapon(std::string weapon = "");
    Item *getItemType(int type);
    bool hasItemType(int type);
    void drawImg();
    void move(sf::Vector2f tar);
    void dirMove(sf::Vector2f tar);
    void momMove();
    void moveNorth();
    void moveNorthEast();
    void moveEast();
    void moveSouthEast();
    void moveSouth();
    void moveSouthWest();
    void moveWest();
    void moveNorthWest();
    bool isHungry();
    bool isThirsty();
    bool isHurt();
    void breath();
    void sethealth(float amount);
    bool modhealth(float amount);
    void fillhunger(float amount);
    void fillthirst(int amount);
    std::string getname();
    void spawn(int gposx, int gposy, int rposx, int rposy, int posx, int posy, int posz);
    bool boolblood(std::string ailment);
    bool hasTag(std::string tag);
    void tag(std::string tag, int amount);
    float bloodwork(std::string aliment, float amount);
    float bloodworkXX(std::string aliment, int amount);
    bool closeToTarget(int distance, sf::Vector2f Tar);
    void addItem(std::string const &itemname, int amount);

    short minmeleedamage;
    short maxmeleedamage;
    short minrangeddamage;
    short maxrangeddamage;

    unsigned char dirgrid[GRID_Y][GRID_X];
    unsigned char valuegrid[GRID_Y][GRID_X];
    unsigned char followgrid[GRID_Y][GRID_X];

    void printConsoleInfo();
    void printBloodContent();
    bool operator<(Npc const &other) const;
};

void setTestage();

void timeTest();

class NpcManager
{
public:
    std::vector<Npc> globalCritter;
    std::vector<Npc> addedCritters;

    void addCritters();
    void initializeCritters();
};
extern NpcManager npcmanager;

class MakeSquad
{
public:
    float makeSquadPoints;
    int squadMates;
    std::vector<Npc> squad;
    int aim;

    MakeSquad();
};

extern MakeSquad squady;
extern std::vector<Npc> worldCritters;

int getNpcVectorId(int id);
std::set<int> npcTrace(int xa, int ya, int xb, int yb, int id,
                       std::set<int> exceptions = std::set<int>());
void saveNPC(int planet, sf::Vector2i region, Npc &critter);

class Territory
{
public:
    sf::Vector2i worldTile;
    sf::Vector2i galaxyTile;
    bool toDelete;

    Territory();
};

void cleanTerritories(std::vector<Territory> &territories);

class Faction
{
public:
    std::string name;
    bool playerControlled;
    bool initialized;

    int members;

    sf::Color primaryColor;
    sf::Color secondaryColor;

    int creativity;
    int aggressiveness;

    float techAgriculture;
    float techArchitecture;
    float techEnergyProduction;
    float techWeaponryMass;
    float techWeaponryEnergy;
    float techWeaponryExplosive;
    float techWeaponrySharp;
    float techWeaponryBlunt;
    float techDiplomacy;
    float techMedical;
    float techRobotics;

    std::vector<Territory> territories;

    std::vector<std::string> knownMagic;
    std::vector<Item> items;

    std::vector<Job> jobList;
    Faction();
};

extern std::vector<Faction> uniFact;

int factionMembers(std::string factionName);
float factionAggression(std::string factionName);
int factionTerritories(std::string factionName);
float factionPower(std::string factionName);
int factionPopulation();
void initializeFactions(int generateMax = 10);
Npc *getGlobalCritter(std::string strtype);
void spawnCritter(std::string object, int xpos, int ypos);

extern Faction g_pf;

void buildStartingCritters(int zedAmount = 0);
std::string loadCritters(sf::Vector2i worldPos, std::string direction,
                         int planet = 500);
void boom(int xpos, int ypos, int damage, int size);
void squadHud();

#endif // FACTION_H_INCLUDED
