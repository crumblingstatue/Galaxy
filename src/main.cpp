#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <string>
#include <vector>
#include <dirent.h>
#include <math.h>
#include <memory.h>
#ifndef GALAXY_LINUX
#include <windows.h>
#endif
#include <time.h>
#include "Items.h"
#include "Faction.h"
#include "Tiles.h"
#include "FactionJobSecurity.h"
#include "Galaxy.h"
#include "menus.h"
#include "aStarLibrary.h"
#include "Images.h"
#include "math.h"
#include "util.h"

using std::abs;

item *ListGet(std::list<item> &List, int Position)
{
    auto i = List.begin();
    std::advance(i, Position);
    return &(*i);
}

//-- Prototypes
//

void DrawStuffs();

int GetItemVectorId(int id);

void SpawnItem(std::string Object, int xpos, int ypos);
void SpawnCritter(std::string Object, int xpos, int ypos);

//
//-- Prototypes

std::string GetClipboardText()
{
#ifdef GALAXY_LINUX
    return "Not implemented.";
#else
    // Try opening the clipboard
    if (!OpenClipboard(NULL))
        Con("ClipError1");

    // Get handle of clipboard object for ANSI text
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL)
        Con("ClipError1");

    // Lock the handle to get the actual text pointer
    char *pszText = static_cast<char *>(GlobalLock(hData));
    if (pszText == NULL)
        Con("ClipError1");

    // Save text in a string class instance
    std::string text(pszText);
    Con("Your Clipboard Contains: ", false);
    Con(text);

    // Release the lock
    GlobalUnlock(hData);

    // Release the clipboard
    CloseClipboard();

    return text;
#endif
}

// Create the main rendering window
std::string RandomWindowName()
{
    std::vector<std::string> WindowNames;
    WindowNames.push_back("Zombificational Settlementation Experimentation");
    WindowNames.push_back("Galaxy");
    WindowNames.push_back("Ye Old Settlement, Ye New Galaxy");
    WindowNames.push_back("The first Galaxy had Joey, Just saying");
    WindowNames.push_back("First comes the universe, then comes Galaxy");

    int RandomName = randz(0, WindowNames.size() - 1);
    std::cout << WindowNames.size() << ", " << RandomName;

    return WindowNames[RandomName];
}

sf::RenderWindow App;

int randzorz(int min, int max)
{
    int num = 0;
    if (min == max)
    {
        return min;
    } // It will crash if it tries to randomize the same two numbers.
    //num = sf::Randomizer::Random(min, max);
    return num;
}

int getdir(std::string dir, std::vector<std::string> &files)
{
    using namespace std;
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == nullptr)
    {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
    while ((dirp = readdir(dp)) != nullptr)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

void UnpointItems(std::list<item> &Items)
{

    //for(int i = 0; i != Items.size(); i++)
    for (auto i = Items.begin(); i != Items.begin(); i++)
    {
        if ((*i).ToDelete)
        {
            std::cout << "JobList size: " << UniFact[0].JobList.size()
                      << std::endl;

            for (size_t t = 0; t != UniFact[0].JobList.size(); t++)
            {
                std::cout << "Job Point: " << &UniFact[0].JobList[t].pItem
                          << "/" << UniFact[0].JobList[t].pItem
                          << ", Item Point: " << &(*i) << "/" << &(*i)
                          << std::endl;
                fSleep(1);

                if (UniFact[0].JobList[t].pItem != nullptr &&
                    (&(*i)) != nullptr)
                {

                    std::cout << "ID! ";
                    try
                    {
                        std::cout << UniFact[0].JobList[t].pItem->id;
                    }
                    catch (std::exception &e)
                    {
                        std::cout << "Something went wrong in in here\n";
                    }
                    std::cout << UniFact[0].JobList[t].pItem->id;
                    std::cout << " ; ";
                    int ID1 = UniFact[0].JobList[t].pItem->id;
                    std::cout << "ID 2! ";
                    std::cout << (*i).id;
                    std::cout << " ; ";
                    int ID2 = (*i).id;
                    std::cout << "ID's: " << ID1 << ":" << ID2 << std::endl;

                    if (ID1 ==
                        ID2) //if(UniFact[0].JobList[t].pItem != NULL && &(*i) != NULL && UniFact[0].JobList[t].pItem == &(*i)) //if(ID1 == ID2)
                    {
                        std::cout << "Match! \n";
                        UniFact[0].JobList[t].pItem = nullptr;
                        //fSleep(1);
                    }
                }
            }
        }
    }
}

bool RemoveItem(int Id)
{
    int TempInt = 0;
    std::list<item>::iterator location;
    for (auto it = worlditems.begin(); it != worlditems.end(); ++it)
    {
        if (it->id == Id)
        {
            location = it;
            TempInt = 1;
        }
    }
    if (TempInt == 1)
    {
        TempInt = 0;
        worlditems.erase(location);
        return true;
    }
    else if (TempInt == 0)
    {
        return false;
    }
    throw std::runtime_error("Couldn't return anything sensible");
}

void UpdateItem()
{
    if (Debug)
    {
        std::cout << "Pre Item \n";
    }
    auto startlist = worlditems.begin();
    auto endlist = worlditems.end();

    for (auto Me = startlist; Me != endlist; ++Me)
    {
        try
        {
            if (Debug)
            {
                std::cout << Me->name << "'s turn! \n";
            }
            if (Me->produces == true)
            {
                if (Debug)
                {
                    std::cout << Me->name << " can produce. \n";
                }
                Me->prodratetimer++;
                if (Me->prodratetimer >= Me->prodrate)
                {
                    Me->prodratetimer = 0;
                    std::string s;
                    s = Me->produce;
                    auto a = new char[s.size() + 1];
                    a[s.size()] = 0;
                    memcpy(a, s.c_str(), s.size());
                    bool FindEmpty = false;
                    int x;
                    int y;
                    while (FindEmpty == false)
                    {
                        x = randz(Me->xpos - 10, Me->xpos + 10);
                        y = randz(Me->ypos - 10, Me->ypos + 10);
                        if (Tiles[abs_to_index(x / GridSize)][abs_to_index(
                                y / GridSize)][30].Walkable == true)
                        {
                            FindEmpty = true;
                        }
                    }
                    if (Debug)
                    {
                        std::cout << "Producing: " << s << " \n";
                    }
                    SpawnItem(s, x, y);
                    if (Debug)
                    {
                        std::cout << Me->name << " has produced: " << s
                                  << " \n";
                    }
                }
            }
            if (Debug)
            {
                std::cout << "Done with: " << Me->name << " \n";
            }

            if (Debug)
            {
                std::cout << "Acting on Missile \n";
            }
            if (Me->TargetPos != sf::Vector2f(-1, -1))
            {
                Me->xpos = (Me->xpos + Me->TargetPos.x) / 2;
                Me->ypos = (Me->ypos + Me->TargetPos.y) / 2;
                if (Me->age >= 10)
                {
                    Me->ToDelete = true;
                    Boom(Me->xpos, Me->ypos, randz(Me->mindam, Me->maxdam),
                         Me->range);

                    for (int ItLength = 0; ItLength != 5; ItLength++)
                    {
                        for (int Rot = 1; Rot != 361; Rot++)
                        {
                            //int Rot = GX;
                            int XPos = ((abs(Me->xpos / 20)) +
                                        cosf(Rot * PI / 180) * ItLength);
                            int YPos = ((abs(Me->ypos / 20)) +
                                        sinf(Rot * PI / 180) * ItLength);
                            //XPos *= 20;
                            //YPos *= 20;

                            //Effectz.CreateCircle(MousePos.x,MousePos.y,5,Red);
                            //Effectz.CreateCircle(XPos,YPos,5,White);
                            Tiles[XPos][YPos][30].Stone();
                        }
                    }

                    Con("Boom!");
                }
            }
            Me->age = Me->age + 1;
            if (Debug)
            {
                std::cout << "Post Item \n";
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Problem with an Item in Update Item \n";
        }
    }
}

std::vector<int> NnGTrace(int xa, int ya, int xb, int yb, int id,
                          std::vector<int> /*exceptions*/)
{ //.at(0) = Item/NPC(23/69) .at(1) = id
    int dx = xb - xa, dy = yb - ya, steps;
    float xIncrement, yIncrement, x = xa, y = ya;
    if (abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);
    xIncrement = dx / (float)steps;
    yIncrement = dy / (float)steps;
    std::vector<int> VectorID;
    for (int k = 0; k < steps; k++)
    {
        x += xIncrement;
        y += yIncrement;
        if (Key.g)
        {
            Effectz.CreateCircle(x, y, 1, sf::Color::Blue);
        }
        int Count = 0;

        { // Merely doing this so I can reuse the same code, but for items, Hehe.
            std::vector<NPC>::iterator Me;

            for (Me = npclist.begin(); Me != npclist.end(); ++Me)
            {
                if (math::Closeish(x, y, Me->xpos, Me->ypos) <= Me->reach &&
                    Me->id != id)
                {

                    std::vector<int>::iterator Vec;
                    try
                    {
                        bool Exists = false;
                        for (Vec = VectorID.begin(); Vec != VectorID.end();
                             ++Vec)
                        {
                            if (*Vec == Me->id)
                            {
                                Exists = true;
                            }
                        }
                        if (Exists == false)
                        {
                            VectorID.push_back(69);
                            VectorID.push_back(GetNpcVectorId(Me->id));
                        }
                    }
                    catch (std::exception &e)
                    {
                        std::cout << "VectError\n";
                    }

                } //23 = Item, 69 = NPC
                Count++;
            }
        }
        std::list<item>::iterator Me;

        for (Me = worlditems.begin(); Me != worlditems.end(); ++Me)
        {
            if (math::Closeish(x, y, Me->xpos, Me->ypos) <= 10 && Me->id != id)
            {
                //Making sure not to constantly add the same
                //try{if( VectorID.at(Count-1) != Me->id){ VectorID.push_back(23); VectorID.push_back( GetItemVectorId(Me->id)); }} catch (std::exception& e){}

            } //23 = Item, 69 = NPC
            Count++;
        }

        if (Key.period)
        {
            Effectz.CreateCircle(x, y, 1, sf::Color::White);
        }
    }
    if (VectorID.size() == 0)
    {
        VectorID.push_back(-1);
    }
    return VectorID;
}

bool gridposTrace(int xa, int ya, int xb, int yb, int id, sf::Vector2f Target)
{ // Looking in a straight line for a specific spot, Walls block vision.

    int dx = xb - xa, dy = yb - ya, steps;
    float xIncrement, yIncrement, x = xa, y = ya;
    if (abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);
    xIncrement = dx / (float)steps;
    yIncrement = dy / (float)steps;

    for (int k = 0; k < steps; k++)
    {
        x += xIncrement;
        y += yIncrement;
        if (Tiles[abs_to_index(x / GridSize)][abs_to_index(y / GridSize)][30]
                .ID == 1010)
        {
            if (Key.period && id == MyTargetid)
            {
                Effectz.CreateLine(x, y, xa, ya, 1, sf::Color::Blue);
            }
            //std::cout << "Shoulda Broke. " << std::endl;
            break;

        } // Stops the trace if it hits a wall.
        //std::cout << "Stuffs " << std::endl;
        sf::Vector2f Pos(abs(x / GridSize), abs(y / GridSize));
        sf::Vector2f Tar(abs(Target.x / GridSize), abs(Target.y / GridSize));
        if (Pos == Tar)
        {
            return true;
        } // Returns true and stops searching.
        if (Key.period && id == MyTargetid)
        {
            Effectz.CreateLine(x, y, xa, ya, 1, sf::Color::Blue);
        }
    }
    return false; // Returns false if the target was never found.
}

std::vector<int> npcTrace(int xa, int ya, int xb, int yb, int id,
                          std::vector<int> /*exceptions = std::vector<int>()*/)
{

    int dx = xb - xa, dy = yb - ya, steps;
    float xIncrement, yIncrement, x = xa, y = ya;
    if (abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);
    xIncrement = dx / (float)steps;
    yIncrement = dy / (float)steps;
    std::vector<int> VectorID;

    for (int k = 0; k < steps; k++)
    {
        x += xIncrement;
        y += yIncrement;
        bool Kill = false;
        std::vector<NPC>::iterator Me;
        int Count = 0;
        for (Me = npclist.begin(); Me != npclist.end(); ++Me)
        {
            if (math::Closeish(x, y, Me->xpos, Me->ypos) <= Me->reach &&
                Me->id != id)
            {
                VectorID.push_back(Count);
                Kill = true;
            }
            Count++;
        }
        if (Kill)
            return VectorID;

        if (Key.h)
        {
            Effectz.CreateCircle(x, y, 1, sf::Color::White);
        }
    }
    if (VectorID.size() == 0)
    {
        VectorID.push_back(-1);
    }

    if (true == false)
    {   // Old complex version of tracing
        /*

        This could be used if you need more/less sensitive detection, but for general detection, Just use the one above.
        float angle;
        float xpos = orix;
        float ypos = oriy;
        float xx;
        float yy;
        if(Key.j){MahNumba++;}
        if(Key.k){MahNumba--;}
        int distence = Math::Closeish(orix,oriy,tarx,tary);
        //cText.CreateText(MousePos.x,MousePos.y,11,White,"Distence:","",distence);
        //cText.CreateText(MousePos.x,MousePos.y+11,11,White,"MahNumba:","",MahNumba);
        for(int i = 0; i != distence; i++){
        angle = MahNumba-(PI/180)*(atan2f(xpos-tarx,ypos-tary));//To be honest, I spent alot of time with trial and error to get this part to work.

        xx = cosf((angle) * PI/180) * 4;
        yy = sinf((angle) * PI/180) * 4;// Come back and mess with this later, check to see how much FPS it takes on finer settings.
        xpos -= xx;
        ypos -= yy;


        if(Key.period){Effectz.CreateCircle(xpos,ypos,1,White);}
        }*/
    }
    throw std::runtime_error("npcTrace: couldn't return a value");
}

void PlanetLogic()
{

    /*


bool UpdatePlanet(){
        int Marked = 0;
        bool Absorbed = false;
        std::vector<planet>::iterator Me;
        {
        for( Me = Planets.begin(); Me != Planets.end(); ++Me ){

            if(Me->name != "Sun"){

                std::vector<planet>::iterator It;
                for( It = Planets.begin(); It != Planets.end(); ++It ){
                    // && It->name == "Sun"
                    if(It->id != Me->id){ sf::Vector2f lMe = Me->Pos; sf::Vector2f lIt = It->Pos;
                    sf::Vector2f NewSpeed(0,0);
                    int range = Math::Closeish(Me->Pos.x,Me->Pos.y,It->Pos.x,It->Pos.y);
                    if(Math::Closeish(lMe.x,lMe.y,lIt.x,lIt.y) < ( ((Me->mass/10)/2)+((It->mass/10)/2) )){
                       if(Absorbed == false){
                       std::cout<<"My current mass:"<<Me->mass<<" It's current mass"<<It->mass<<std::endl;
                       Me->mass += It->mass;
                       std::cout<<"My new mass:"<<Me->mass<<std::endl;
                       Me->speed += It->speed/(Me->mass*Me->mass);
                       Marked = It->id;
                       Absorbed = true;
                       Me->img.SetScale(Me->mass/100,Me->mass/100);
                       //RemovePlanet(It->id);
                       }


                        }
                    else{
                    NewSpeed.x -= ((((lMe.x -= lIt.x)*It->mass)/(range ))*GCtimescale);
                    NewSpeed.y -= ((((lMe.y -= lIt.y)*It->mass)/(range ))*GCtimescale);}

                    Me->speed.x += (NewSpeed.x / (Me->mass*Me->mass));
                    Me->speed.y += (NewSpeed.y / (Me->mass*Me->mass));
                    int xx = 0;
                    int yy = 0;
                    int NSx = NewSpeed.x*100;
                    int NSy = NewSpeed.y*100;
                    if(NSx < 0){xx = -NSx;}
                    else {xx = NSx;}
                    if(NSy < 0){yy = -NSy;}
                    else {yy = NSy;}

                    if(Key.space){cText.CreateText(Me->Pos.x,Me->Pos.y+10,11,White,"Force:","x:",xx," ","       ",yy," ","",xx+yy,1,0);
                    cText.CreateText(Me->Pos.x,Me->Pos.y+20,11,White,"Speed:","x:",Me->speed.x*100," y:","",Me->speed.y*100,"","",-6698,1,0);}
                    //if(Me->name == "Planet Z"){ std::cout<<" Force: x:"<< xx<<" y:"<<yy<<std::endl;  }

                    }
                }

            }
        }//End of For
        RemovePlanet(Marked);

    }
    for( Me = Planets.begin(); Me != Planets.end(); ++Me ){
            if(Me->name != "Sun"){Me->Pos += Me->speed;}
    }



}


bool UpdatePlanet(){
        std::vector<planet>::iterator Me;
        {
        for( Me = Planets.begin(); Me != Planets.end(); ++Me ){

            if(Me->name != "Sun"){

                std::vector<planet>::iterator It;
                for( It = Planets.begin(); It != Planets.end(); ++It ){
                    if(It->id != Me->id){ sf::Vector2f lMe = Me->Pos; sf::Vector2f lIt = It->Pos;
                    sf::Vector2f NewSpeed(0,0);
                    NewSpeed.x -= ((lMe.x -= lIt.x)/Me->mass*GCtimescale)*GCtimescale;
                    NewSpeed.y -= ((lMe.y -= lIt.y)/Me->mass*GCtimescale)*GCtimescale;

                    Me->speed += NewSpeed;

                    }
                }

            }
        }//End of For

    }
    for( Me = Planets.begin(); Me != Planets.end(); ++Me ){
            if(Me->name != "Sun"){Me->Pos += Me->speed;}
    }



}

*/
}

int GetItemVectorId(int id)
{
    int index = 0;
    for (auto const &item : worlditems)
    {
        if (item.id == id)
        {
            return index;
        }
        index++;
    }
    return -1;
}

int GetNpcVectorId(int id)
{
    int index = 0;
    for (auto const &npc : npclist)
    {
        if (npc.id == id)
        {
            return index;
        }
        index++;
    }
    return -1;
}

bool RemoveNPC(char * /*NPCname*/, int /*Id*/)
{
    int TempInt = 0;
    std::vector<NPC>::iterator location;
    for (auto it = npclist.begin(); it != npclist.end(); ++it)
    {
        if (it->health <= 0 || it->HasSpawned == false || it->alive == false)
        {
            location = it;
            TempInt = 1;
        }
    }
    if (TempInt == 1)
    {
        TempInt = 0;
        npclist.erase(location);
        return true;
    }
    else if (TempInt == 0)
    {
        return false;
    }
    throw std::runtime_error("RemoveNPC: Couldn't return anything sensible.");
}

std::vector<int> FindClosestItem(int Orix, int Oriy, std::string TarItem,
                                 int /*Gxpos*/ = 0, int /*Gypos*/ = 0,
                                 int /*Rxpos*/ = 0, int /*Rypos*/ = 0)
{
    std::vector<int> Returns;
    std::list<item>::iterator Items;
    // This will be difficult, go through all the items, and find the closest one.
    int closx =
        -1000000; // Make sure the default starting number is far beyond being the closest one.
    int closy = -1000000;
    int Closid;
    int closVect;
    int VectPos = 0;
    bool first = true;
    for (Items = worlditems.begin(); Items != worlditems.end(); ++Items)
    {
        if (first == true)
        {
            if (Items->name == TarItem)
            {
                closx = Items->xpos;
                closy = Items->ypos;
                Closid = Items->id;
                closVect = VectPos;
                first = false;
            }
        }
        else
        {
            if (Items->name == TarItem)
            {
                int one = math::Closeish(Orix, Oriy, Items->xpos, Items->ypos);
                int two = math::Closeish(Orix, Oriy, closx, closy);
                if (one < two)
                {
                    closx = Items->xpos;
                    closy = Items->ypos;
                    Closid = Items->id;
                    closVect = VectPos;
                }
            }
        }
        VectPos++;
    }
    if (first != true)
    {
        Returns.push_back(closx);
        Returns.push_back(closy);
        Returns.push_back(Closid);
        Returns.push_back(closVect);
        return Returns; // Returns = (xpos,ypos,id,Vector Position)
    }
    throw std::runtime_error("FindClosestItem: Couldn't return anything!");
}

std::set<int> NpcList(int exceptions = -1)
{
    if (Debug)
    {
        std::cout << "Pre npcList \n";
    }
    std::set<int> Returns;

    for (auto const &npc : npclist)
    {
        if (Debug)
        {
            std::cout << "For NpcList \n";
        }
        if (npc.id != exceptions)
        {
            if (Debug)
            {
                std::cout << "Post exception NpcList \n";
            }
            Returns.insert(GetNpcVectorId(npc.id));
            if (Debug)
            {
                std::cout << "Post Returns NpcList \n";
            }
        }
    }
    if (Debug)
    {
        std::cout << "Post For NpcList \n";
    }
    if (!Returns.empty())
    {
        return Returns;
    }
    throw std::runtime_error("NpcList: Couldn't return anything!");
}

void UpdateNPC()
{
    if (Debug)
    {
        std::cout << "Pre NPC\n";
    }
    std::vector<NPC>::iterator Me;
    int IntegerIterator = 0;
    for (Me = npclist.begin(); Me != npclist.end(); ++Me)
    {
        // BodyPart Loop
        // First, Run through the bodyparts finding the 'global' tags, like Nutrient Extraction and such.
        // Second, Run through each individual part running through all the local tags.

        short int Parts = 0;

        size_t SearchPos = 0;
        size_t EndPos = 0;

        debug("Debug: Beginning Part Loop for" + Me->name);

        //  Global Part Tag Variables

        float PartsWalkSpeed = 0;
        float GlobalNutritionPercentage = 100;
        bool ConsumeFlesh = false;
        bool ConsumeVeggy = false;
        bool ConsumeWater = false;

        //  *   Global Part Tag Variables   *

        while (SearchPos != Me->Body.BodyParts.npos) // Global Part Tags
        {
            SearchPos = Me->Body.BodyParts.find("{", SearchPos);

            if (SearchPos != Me->Body.BodyParts.npos)
            {
                EndPos = Me->Body.BodyParts.find("}", SearchPos);

                std::string WorkingLine;

                WorkingLine.append(Me->Body.BodyParts, SearchPos,
                                   EndPos - SearchPos);
                float PartNumber = 0;

                PartNumber = StringFindNumber(WorkingLine, "[Walk:");
                if (PartNumber != 0)
                {

                    if (PartsWalkSpeed != 0)
                    {
                        PartsWalkSpeed = PartNumber;
                    }
                    else
                    {
                        PartsWalkSpeed += (PartNumber * 0.5);
                    }
                }

                PartNumber =
                    StringFindNumber(WorkingLine, "[NutritionExtraction:");
                if (PartNumber != 0)
                {
                    GlobalNutritionPercentage += PartNumber;
                }

                PartNumber = StringFindNumber(WorkingLine, "[DigestsBlood:");
                if (PartNumber != 0)
                {
                }
                PartNumber = StringFindNumber(WorkingLine, "[DigestsFlesh:");
                if (PartNumber != 0)
                {
                    ConsumeFlesh = true;
                }
                PartNumber = StringFindNumber(WorkingLine, "[DigestsVeggy:");
                if (PartNumber != 0)
                {
                    ConsumeVeggy = true;
                }
                PartNumber = StringFindNumber(WorkingLine, "[DigestsWater:");
                if (PartNumber != 0)
                {
                    ConsumeWater = true;
                }

                SearchPos = EndPos;
            }
        }

        SearchPos = 0;
        EndPos = 0;

        //for(int i = 0; i != Me->inventory.size(); i++)
        for (auto i = Me->inventory.begin(); i != Me->inventory.begin(); i++)
        {
            if ((*i).InsidePart != "")
            {
                (*i).HasInternalUse--;
            }
        }

        while (SearchPos != Me->Body.BodyParts.npos) // Individual Part Tags
        {

            SearchPos = Me->Body.BodyParts.find("{", SearchPos);

            if (SearchPos != Me->Body.BodyParts.npos)
            {
                EndPos = Me->Body.BodyParts.find("}", SearchPos);
                Parts++;

                std::string WorkingLine;

                WorkingLine.append(Me->Body.BodyParts, SearchPos,
                                   EndPos - SearchPos);

                float PartNumber = 0;
                std::string PartString = "";
                item *PartItem;

                std::string CurrentPart =
                    StringFindString(WorkingLine, "[Name:");

                PartNumber = StringFindNumber(WorkingLine, "[DigestsBlood:");
                PartItem = GetItemPtrfromVector(Me->inventory, "Blood");
                if (PartNumber != 0 && PartItem != nullptr)
                {

                    float WorkAmount = PartItem->amount;
                    float Diff = WorkAmount - (PartNumber / 1000);

                    if (Diff > 0)
                    {
                        PartItem->amount = Diff;
                        float Nutr = (WorkAmount - Diff) *
                                     100; // TODO: Figure this out better.
                        Me->bloodwork(
                            "Nutrients",
                            Nutr * PercentageBuff(GlobalNutritionPercentage));
                    }
                    else
                    {
                        //*GetItemPtrfromVector(Me->inventory,"Blood").amount = 0;
                        GetItemPtrfromVector(Me->inventory, "Blood")->ToDelete =
                            true;
                        float Nutr =
                            WorkAmount * 100; // TODO: Figure this out better.
                        Me->bloodwork(
                            "Nutrients",
                            Nutr * PercentageBuff(GlobalNutritionPercentage));
                    }
                }

                PartNumber = StringFindNumber(WorkingLine, "[DigestsFlesh:");
                PartItem =
                    GetItemPtrfromVectorVarSearch(Me->inventory, "MassFlesh");
                //if(PartItem != NULL) PartItem->HasInternalUse++; // This is designed to keep items from being ejected until they are completely useless to a critter, I.E. Items with multiple Food Mass's.
                if (PartNumber != 0 && PartItem != nullptr &&
                    PartItem->MassFlesh >
                        0) // TODO: Make sure the item is in THIS PART before digesting it!
                {
                    //std::cout << "HasInternalUse: " << PartItem->HasInternalUse << std::endl;
                    float WorkAmount = PartItem->MassFlesh;
                    float Diff = WorkAmount - (PartNumber / 1000);

                    if (Diff > 0)
                    {
                        PartItem->MassFlesh = Diff;
                        PartItem->HasInternalUse = 0;
                        float Nutr = (WorkAmount - Diff) *
                                     100; // TODO: Figure this out better.
                        Me->bloodwork(
                            "Nutrients",
                            Nutr * PercentageBuff(GlobalNutritionPercentage));
                    }
                    if (PartItem->MassFlesh <= 0)
                    {
                        //*GetItemPtrfromVector(Me->inventory,"Blood").amount = 0;
                        PartItem->ToDelete = true;
                        //Add Food to everyone, Make sure they go hungry to eat it, Figure out a way to Eject the empty item, Or do water! Everyone starts with water.
                        //Me->bloodwork("Nutrients",Nutr*PercentageBuff(GlobalNutritionPercentage));
                    }
                }

                PartNumber = StringFindNumber(WorkingLine, "[DigestsVeggy:");
                PartItem =
                    GetItemPtrfromVectorVarSearch(Me->inventory, "MassVeggy");
                //if(PartItem != NULL) PartItem->HasInternalUse++; // This is designed to keep items from being ejected until they are completely useless to a critter, I.E. Items with multiple Food Mass's.
                if (PartNumber != 0 && PartItem != nullptr &&
                    PartItem->MassVeggy >
                        0) // TODO: Make sure the item is in THIS PART before digesting it!
                {
                    //std::cout << "HasInternalUse: " << PartItem->HasInternalUse << std::endl;
                    float WorkAmount = PartItem->MassVeggy;
                    float Diff = WorkAmount - (PartNumber / 1000);

                    if (Diff > 0)
                    {
                        PartItem->MassVeggy = Diff;
                        PartItem->HasInternalUse = 0;
                        float Nutr = (WorkAmount - Diff) *
                                     100; // TODO: Figure this out better.
                        Me->bloodwork(
                            "Nutrients",
                            Nutr * PercentageBuff(GlobalNutritionPercentage));
                    }
                    if (PartItem->MassVeggy <= 0)
                    {
                        PartItem->ToDelete = true;
                        //Me->bloodwork("Nutrients",Nutr*PercentageBuff(GlobalNutritionPercentage));
                    }
                }

                PartNumber = StringFindNumber(WorkingLine, "[DigestsWater:");
                PartItem =
                    GetItemPtrfromVectorVarSearch(Me->inventory, "MassWater");
                //if(PartItem != NULL) PartItem->HasInternalUse++; // This is designed to keep items from being ejected until they are completely useless to a critter, I.E. Items with multiple Food Mass's.
                if (PartNumber != 0 && PartItem != nullptr &&
                    PartItem->MassWater >
                        0) // TODO: Make sure the item is in THIS PART before digesting it!
                {
                    //std::cout << "HasInternalUse: " << PartItem->HasInternalUse << std::endl;
                    float WorkAmount = PartItem->MassWater;
                    float Diff = WorkAmount - (PartNumber / 1000);

                    if (Diff > 0)
                    {
                        PartItem->MassWater = Diff;
                        PartItem->HasInternalUse = 0;
                        float Nutr = (WorkAmount - Diff) *
                                     100; // TODO: Figure this out better.
                        Me->bloodwork(
                            "Hydration",
                            Nutr * PercentageBuff(GlobalNutritionPercentage));
                    }
                    if (PartItem->MassWater <= 0)
                    {
                        PartItem->ToDelete = true;
                        //Me->bloodwork("Nutrients",Nutr*PercentageBuff(GlobalNutritionPercentage));
                    }
                }

                PartString = StringFindString(WorkingLine, "[PoisonFilter:");
                if (PartString != "")
                {

                    std::vector<std::string> StrVec =
                        StringFindElements(PartString, ":");
                    if (Debug)
                        std::cout << "StrVec[0]: " << StrVec[0] << std::endl;
                    float Leftover =
                        Me->bloodwork(StrVec[0], -atof(StrVec[1].c_str()));
                    if (Debug)
                        std::cout << "Bloodwork leftover is: " << Leftover
                                  << std::endl;
                    //NPC Critter;

                    for (size_t i = 0; i != StrVec.size(); i++)
                    {
                        if (Debug)
                            std::cout << StrVec[i] << std::endl;
                    }
                }

                PartNumber = StringFindNumber(WorkingLine, "[Orafice:");
                if (PartNumber > 0)
                {
                    //std::vector<item> * Inv = &Me->inventory;

                    //for(int i = 0; i != Me->inventory.size(); i++)
                    for (auto i = Me->inventory.begin();
                         i != Me->inventory.begin(); i++)
                    {
                        bool FoundIt = false;
                        if ((*i).InsidePart == "" && (*i).MassFlesh > 0 &&
                            ConsumeFlesh) // Awww yeeessss, We gonna eat some flesh with our Orafice!
                        {
                            (*i).InsidePart = CurrentPart;
                            FoundIt = true;
                        }

                        if ((*i).InsidePart == "" && (*i).MassVeggy > 0 &&
                            ConsumeVeggy) // Awww yeeessss, We gonna eat something with our Orafice!
                        {
                            (*i).InsidePart = CurrentPart;
                            FoundIt = true;
                        }

                        if ((*i).InsidePart == "" && (*i).MassWater > 0 &&
                            ConsumeWater) // Awww yeeessss, We gonna eat something with our Orafice!
                        {
                            (*i).InsidePart = CurrentPart;
                            FoundIt = true;
                        }

                        if (FoundIt)
                        {
                            std::string ChtStr;
                            ChtStr.append("* ");
                            ChtStr.append(Me->name);
                            ChtStr.append("(" + std::to_string(Me->id) + ")");
                            ChtStr.append(" has inserted ");
                            ChtStr.append((*i).name);
                            ChtStr.append(" into their ");
                            ChtStr.append(CurrentPart);
                            ChtStr.append("'s Orafice(");
                            ChtStr.append(std::to_string(PartNumber));
                            ChtStr.append(").");

                            ChatBox.AddChat(ChtStr, sf::Color(150, 150, 0));
                        }
                    }
                }

                PartNumber = StringFindNumber(
                    WorkingLine, "[BloodPumpRate:"); // TODO: Do this right.
                if (PartNumber != 0)
                {
                    float Blood =
                        StringFindNumber(Me->bloodcontent, "[Nutrients:");
                    if (Blood > PartNumber)
                    {
                        if ((Me->maxhunger - Me->hunger) > PartNumber)
                        {
                            Me->hunger += PartNumber;
                            Me->bloodwork("Nutrients", -PartNumber);
                        }
                    }
                    Blood = StringFindNumber(Me->bloodcontent, "[Hydration:");
                    if (Blood > PartNumber)
                    {
                        if ((Me->maxthirst - Me->thirst) > PartNumber)
                        {
                            Me->thirst += PartNumber;
                            Me->bloodwork("Hydration", -PartNumber);
                        }
                    }
                }

                SearchPos = EndPos;
            }
        }

        // TODO: Can't really use this until the critter brain rewrite.
        /*
        for(int i = 0; i != Me->inventory.size(); i++)
        {
            //std::cout << "Freakkin: " << Me->inventory[i].HasInternalUse << std::endl;
            if(Me->inventory[i].HasInternalUse <= -100)
            {
                //TODO: Ejection code, Just duplicate the item onto the ground with the same coords, eh? Also add a chatbox text that a critter did this.

                item ExpelItem = Me->inventory[i];
                ExpelItem.xpos = Me->xpos;
                ExpelItem.ypos = Me->ypos;
                ExpelItem.InsidePart = "";
                ExpelItem.HasInternalUse = 0;
                worlditems.push_back(ExpelItem);
                Me->inventory[i].ToDelete = true;

                std::string ChtStr;
                ChtStr.append("* ");
                ChtStr.append(Me->name);
                ChtStr.append(AddString("(",std::to_string(Me->id),")"));
                ChtStr.append(" has ejected ");
                ChtStr.append(Me->inventory[i].name);
                ChtStr.append(" from their ");
                ChtStr.append(Me->inventory[i].InsidePart);
                ChtStr.append(".");

                ChatBox.AddChat(ChtStr,sf::Color(150,150,0));
            }
        }
        */

        //if(Parts != 0) std::cout << Me->name << " has " << Parts << " bodyparts. \n";

        debug("Debug: Ending Part Loop");
        // *BodyPart Loop*

        sf::Vector2f PathFindWorkPos(0, 0);

        float TempXpos = Me->xpos;
        float TempYpos = Me->ypos;
        Me->movetimer -= Me->movetimerrate;
        Me->attacktimer--;
        if (Me->attacktimer < 0)
        {
            Me->attacktimer = 0;
        }

        if (Me->name == "Zombie")
        {
        }

        if (Me->name == "Azabul")
        {

            Me->TentArm1.x =
                math::Clamp(Me->TentArm1.x + randz(-3, 3), -20, 20);
            Me->TentArm1.y =
                math::Clamp(Me->TentArm1.y + randz(-3, 3), -20, 20);
            Me->TentArm2.x =
                math::Clamp(Me->TentArm2.x + randz(-3, 3), -20, 20);
            Me->TentArm2.y =
                math::Clamp(Me->TentArm2.y + randz(-3, 3), -20, 20);
            Me->TentEnd1.x =
                math::Clamp(Me->TentEnd1.x + randz(-3, 3), -20, 20);
            Me->TentEnd1.y =
                math::Clamp(Me->TentEnd1.y + randz(-3, 3), -20, 20);
            Me->TentEnd2.x =
                math::Clamp(Me->TentEnd2.x + randz(-3, 3), -20, 20);
            Me->TentEnd2.y =
                math::Clamp(Me->TentEnd2.y + randz(-3, 3), -20, 20);
        }

        if (Me->id == MyTargetid && Key.space)
        {
            Me->Attacking = true;
        }
        Me->hungertimer--;
        if (Me->hungertimer <= 0)
        {
            Me->hungertimer = Me->hungertimerint;
            Me->hunger -= Me->hungerrate;
            if (Me->hunger < 0)
            {
                Me->hunger = 0;
            }
        }
        Me->thirsttimer--;
        if (Me->thirsttimer <= 0)
        {
            Me->thirsttimer = Me->thirsttimerint;
            Me->thirst -= Me->thirstrate;
            if (Me->thirst < 0)
            {
                Me->thirst = 0;
            }
        }
        Me->regentimer--;
        if (Me->regentimer <= 0 && Me->health < Me->maxhealth && Me->health > 0)
        {
            Me->regentimer = Me->regentimerint;
            Me->health += Me->regenrate;
        }
        if (Me->health <= 0)
        {
            if (Me->canmove == true)
            {
                std::string ChtStr;
                ChtStr.append("* ");
                ChtStr.append(Me->name);
                ChtStr.append("(" + std::to_string(Me->id) + ")");
                ChtStr.append(" has died! ");

                ChatBox.AddChat(ChtStr, sf::Color(200, 0, 0));
            }
            Me->canmove = false;
        }
        if (Me->health > 0)
        {
            Me->canmove = true;
        }
        if (Me->health > 0 && Me->boolblood("[Zombification:") &&
            Me->name != "Zombie")
        {
            Me->bloodwork("Zombification", -0.01);
            Me->modhealth(-0.01);
            Me->fillhunger(-0.01);
        }
        if (Me->health <= 0 && Me->boolblood("[Zombification:") &&
            Me->name != "Zombie")
        {
            Me->bloodwork("Zombification", -9999999);
            Me->Tag("[ZombieHater:", -9999999);
            Me->Tag("[EatsFlesh:", 1);
            std::string OldName = Me->name;
            Me->name = "Zombie";
            Me->cbaseid = 666333;
            std::string Imagery = "Zombie.tga";
            for (auto const &image : imagemanager.GlobalImage)
            {
                if (image.name == Imagery)
                {
                    Me->img.setTexture(image.Image);
                    Me->img.setOrigin(Me->img.getTextureRect().height / 2,
                                      Me->img.getTextureRect().width / 2);
                }
            }
            Me->IsNPC = true;
            Me->NeedsFood = true;
            Me->AllowedFood = false;
            Me->NeedsWater = false;
            Me->AllowedDrink = false;
            Me->canmove = true;
            Me->alive = true;
            Me->maxhealth = 200;
            Me->health = Me->maxhealth;
            Me->breathtimerint = 0;
            Me->hungertimerint = 0;
            Me->thirsttimerint = 0;
            Me->thirst = 0; //randz(10,600);
            Me->breathtimer = 2;
            Me->hungertimer = 0;
            Me->thirsttimer = 0;
            Me->maxhunger = 1000000;
            Me->maxthirst = 1000000;
            Me->target = "Flesh";
            Me->action = "Act";
            //Me->moverateint = 1;
            //Me->moverate = 1;
            Me->movetimerrate = 400;

            std::string ChtStr;
            ChtStr.append("* ");
            ChtStr.append(OldName);
            ChtStr.append("(" + std::to_string(Me->id) + ")");
            ChtStr.append(" has been zombified by Zombification! ");

            ChatBox.AddChat(ChtStr, sf::Color(200, 0, 0));
        };
        if (Key.rshift && Me->id == MyTargetid)
        {
            std::cout << Me->target << "At: " << Me->TargetPos.x << ":"
                      << Me->TargetPos.y << std::endl;
            std::cout << "HasTarget: " << Me->HasTarget
                      << " AtTarget: " << Me->AtTarget << std::endl;
        }

        if (Me->Attacking)
        {
            if (Me->attacktimer <= 0)
            {
                if (Debug)
                {
                    std::cout << "Pre Mel Ran \n";
                }
                bool bMel = Me->HasItemType(1);
                item Mel;
                if (bMel)
                {
                    Mel = *Me->GetItemType(1);
                }
                item Ran;
                bool bRan = Me->HasItemType(2);
                if (bRan == true)
                {
                    Ran = *Me->GetItemType(2);
                    debug("Fish n Chips");
                }
                try
                {
                    bool Attacked = false;
                    if (Debug)
                    {
                        std::cout << "Pre Mel \n";
                    }
                    if (bMel == true)
                    {
                        if (Mel.type == 1 &&
                            math::Closeish(Me->xpos, Me->ypos, Me->ShootPos.x,
                                           Me->ShootPos.y) < Mel.range)
                        {
                            Attacked = true;
                            std::set<int> Tarz;
                            Tarz = Me->Melee(1, 1, GridSize);
                            for (const auto &elem : Tarz)
                            {
                                npclist.at(elem)
                                    .modhealth(-(randz(Mel.mindam, Mel.maxdam) +
                                                 Me->Skills.strength));
                                if (Me->HasTag("[CanLearn:"))
                                {
                                    Me->Skills.Train("strength");
                                }
                            }
                            if (Me->Attacking == false)
                            {
                                Me->attacktimer =
                                    (Me->attacktimerint -
                                     (math::Clamp(Me->Skills.agility / 10, 10,
                                                  100)));
                            } // Melee has a different method for saying it's done.
                            if (Debug)
                            {
                                std::cout << "Post Mel \n";
                            }
                        }
                    }
                    if (Debug)
                    {
                        std::cout << "Pre Ran \n";
                    }
                    if (bRan == true && Attacked == false)
                    {
                        if (Ran.type == 2)
                        {
                            bool Friendly = false;
                            std::set<int> ids =
                                NPCTrace(Me->xpos, Me->ypos, Me->TargetPos.x,
                                         Me->TargetPos.y, Me->id);
                            if (!ids.empty())
                            {
                                try
                                {
                                    for (const auto &id : ids)
                                    {
                                        if (npclist.at(id).cbaseid ==
                                            Me->cbaseid)
                                        {
                                            Friendly = true;
                                        }
                                    }
                                }
                                catch (std::exception &e)
                                {
                                }
                            }
                            if (Friendly == false &&
                                gridposTrace(Me->xpos, Me->ypos, Me->ShootPos.x,
                                             Me->ShootPos.y, Me->id,
                                             Me->TargetPos) == true &&
                                math::Closeish(Me->xpos, Me->ypos,
                                               Me->ShootPos.x,
                                               Me->ShootPos.y) <= Me->viewrange)
                            {
                                int Tempx = randz(
                                    0,
                                    math::Clamp(
                                        100 - Me->Skills.dexterity, 0,
                                        100)); // This is to mess up the aiming.
                                int Tempy = randz(
                                    0, math::Clamp(100 - Me->Skills.dexterity,
                                                   0, 100));
                                if (Me->HasTag("[CanLearn:"))
                                {
                                    Me->Skills.Train("dexterity");
                                }
                                if (randz(0, 1) == 1)
                                {
                                    Tempx = -Tempx;
                                }
                                if (randz(0, 1) == 1)
                                {
                                    Tempy = -Tempy;
                                }
                                sf::Vector2f SP(MousePos.x, MousePos.y);
                                sf::Vector2f Targ(Me->ShootPos.x + Tempx,
                                                  Me->ShootPos.y + Tempy);
                                Effectz.CreateLine(Me->xpos, Me->ypos, Targ.x,
                                                   Targ.y, 2, sf::Color::White);
                                std::set<int> ids = NPCTrace(
                                    Me->xpos, Me->ypos, Targ.x, Targ.y, Me->id);
                                if (!ids.empty())
                                {
                                    for (size_t Ta = 0; Ta != ids.size(); Ta++)
                                    {
                                        try
                                        {
                                            for (const auto &id : ids)
                                            {
                                                npclist.at(id).modhealth(
                                                    -(randz(Ran.mindam,
                                                            Ran.maxdam) +
                                                      Me->Skills.perception));
                                                if (Me->HasTag("[CanLearn:"))
                                                {
                                                    Me->Skills.Train(
                                                        "perception");
                                                }
                                                Effectz.CreateCircle(
                                                    npclist.at(id).xpos,
                                                    npclist.at(id).ypos, 5,
                                                    sf::Color::Red);
                                            }
                                        }
                                        catch (std::exception &e)
                                        {
                                        }
                                        cText.CreateText(Me->xpos, Me->ypos, 11,
                                                         sf::Color::Red,
                                                         "Bang!");
                                    }
                                }
                                Me->Attacking = false;
                                Me->attacktimer =
                                    (Me->attacktimerint -
                                     (math::Clamp(Me->Skills.agility / 10, 10,
                                                  100)));
                            }
                            else
                            {
                                Me->Attacking = false;
                            }
                            if (Debug)
                            {
                                std::cout << "Post Ran \n";
                            }
                        }
                    }
                    else
                    {
                        Me->Attacking = false;
                    }
                }
                catch (std::exception &e)
                {
                    std::cout << "Itemerror\n";
                }
            }
            else
            {
                Me->Attacking = false;
            }
        } //End of Attacking
        if (Me->canmove == true && Me->Attacking == false)
        {
            //std::cout << id << " is acting." << std::endl;
            // First, Check values, to see if hungry/thirsty/hurt then check if being attacked
            // Add an IsSafe before these checks
            if (Me->IsHungry() == false && Me->IsThirsty() == false)
            {
                //Me->moverate = Me->moverateint;
                Me->moverate = PartsWalkSpeed;
            }
            //else if(Me->IsThirsty() == false && Me->IsHungry() == true){Me->moverate = Me->moverateint/2;if(Me->moverate <= 0){Me->moverate=1;}}
            //else if(Me->IsHungry() == false  && Me->IsThirsty() == true){Me->moverate = Me->moverateint/2;if(Me->moverate <= 0){Me->moverate=1;}}
            //else if(Me->IsHungry() == true  && Me->IsThirsty() == true){Me->moverate = Me->moverateint/4;if(Me->moverate <= 0){Me->moverate=1;}}

            if (Me->name == "Zombie")
            {
                if (Me->name == "Zombie" && Me->hunger > 0)
                {
                    Me->moverate = Me->moverateint * 2;
                }
                if (Me->name == "Zombie" && Me->hunger >= 0)
                {
                    Me->target = "Flesh";
                }

                if (Me->name == "Zombie" && Key.lctrl == true)
                {
                    Me->moverate = Me->moverateint * 4;
                }
                if (Me->name == "Zombie" && Key.lalt == true)
                {
                    Me->moverate = 4;
                }

                float shake = 0.2;
                int bumz = randz(1, 10);
                if (bumz == 1)
                {
                    Me->ypos += shake;
                }
                if (bumz == 2)
                {
                    Me->xpos += shake;
                }
                if (bumz == 3)
                {
                    Me->ypos -= shake;
                }
                if (bumz == 4)
                {
                    Me->xpos -= shake;
                }
                if (globals::groundmap[currentz][abs_to_index(Me->xpos / GridSize)]
                             [abs_to_index(Me->ypos / GridSize)] == 10)
                {
                    Me->xpos = TempXpos;
                    Me->ypos = TempYpos;
                }
            }

            if (Me->action == "Act")
            { // Start of "Act" action.
                if (Me->IsHurt() == true)
                {
                    //target = "Meds"; //std::cout << id << " is hurt." << std::endl;
                }
                else if (Me->IsHungry() == true && Me->AllowedFood == true)
                {
                    Me->target =
                        "Food"; //std::cout << id << " is hungry." << std::endl;
                    if (Me->IsThirsty())
                    {
                        bool IsFood = false;
                        for (auto &worlditem : worlditems)
                        {
                            if (worlditem.type == 4)
                            {
                                IsFood = true;
                            }
                        }
                        if (IsFood == false)
                        {
                            Me->target = "Water";
                        }
                    }
                }
                else if (Me->target == "Food" && Me->hunger < Me->maxhunger)
                {
                }
                else if (Me->target == "Flesh" && Me->name == "Zombie")
                {
                }
                else if (Me->IsThirsty() == true)
                {
                    Me->target = "Water";
                    //std::cout << id << " is thirsty." << std::endl;
                    bool IsWater = false;
                    for (auto &worlditem : worlditems)
                    {
                        if (worlditem.MassWater > 0)
                        {
                            IsWater = true;
                        }
                    }
                    if (IsWater == false)
                    {
                        Me->target = "Wander";
                    }
                }
                else if (Me->target == "Water" && Me->thirst < Me->maxthirst)
                {
                }
                else
                {
                    Me->target = "Wander";
                    //std::cout << id << " is fine." << std::endl;
                }
                if (Me->target == "Wander" && Me->HasTarget == true)
                { // TODO: Make sure this isn't needed anymore, Then delete it.
                    if (globals::groundmap[currentz][abs_to_index(
                            Me->TargetPos.x /
                            GridSize)][abs_to_index(Me->TargetPos.y /
                                                    GridSize)] == 10)
                    {
                        Me->HasTarget = false;
                    }
                }
                if (Me->target == "Food")
                { // Compress this later by simply directly linking the target towards the search, Probably need a bool for Item or NPC to simplfy it.
                    item *Item = FindClosestItemPtr(Me->xpos, Me->ypos, "Food");
                    if (Item != nullptr)
                    {
                        if (Item->MassFlesh > 0)
                        {
                            Me->TargetPos =
                                sf::Vector2f(Item->xpos, Item->ypos);
                            Me->TargetId = Item->id;
                            //Me->TargetVectorId = Item.at(3);
                            Me->target = "Food";
                            Me->HasTarget = true;
                        }
                    }
                    else
                    {
                        Me->target = "Wander";
                    }
                }
                if (Me->target == "Water")
                {
                    std::vector<int> Item =
                        FindClosestItem(Me->xpos, Me->ypos, "Water");
                    if (Item.size() != 0)
                    {
                        Me->TargetPos = sf::Vector2f(Item.at(0), Item.at(1));
                        Me->TargetId = Item.at(2);
                        Me->TargetVectorId = Item.at(3);
                        Me->target = "Water";
                        Me->HasTarget = true;
                    }
                    else
                    {
                        Me->target = "Wander";
                    }
                }

                if (Me->name == "Azabul")
                    Me->target = "Flesh";
                if (Me->target == "Flesh")
                {
                    int closx = -100000;
                    int closy = -100000;
                    int ClosID = 0;
                    bool first = true;
                    for (auto &elem : npclist)
                    {
                        //if(first == true){ if(zitz->cbaseid != Me->cbaseid && zitz->health > 0){
                        if (first == true)
                        {
                            if (elem.cbaseid == 110110 && elem.health > 0)
                            {
                                closx = elem.xpos;
                                closy = elem.ypos;
                                first = false;
                            }
                        }
                        // else { if(zitz->cbaseid != Me->cbaseid && zitz->health > 0) {
                        else
                        {
                            if (elem.cbaseid == 110110 && elem.health > 0)
                            {
                                int one = math::Closeish(Me->xpos, Me->ypos,
                                                         elem.xpos, elem.ypos);
                                int two = math::Closeish(Me->xpos, Me->ypos,
                                                         closx, closy);
                                if (one < two)
                                {
                                    closx = elem.xpos;
                                    closy = elem.ypos;
                                    ClosID = elem.id;
                                }
                            }
                        }
                    }
                    if (closx == -100000)
                    {
                        Me->target = "Wander";
                    }
                    if (first == false)
                    {
                        Me->TargetPos = sf::Vector2f(closx, closy);
                        Me->TargetId = ClosID;
                        Me->target = "Flesh";
                        Me->HasTarget = true;
                    }
                    else
                    {
                        Me->target = "Wander";
                    }
                }

                try
                { // Jobs Section
                    if (UniFact[0].JobList.size() != 0 &&
                        Me->target == "Wander" && Me->Faction == "The Alphas")
                    {
                        debug("Preforming Job Routine; ");
                        for (size_t i = 0; i != UniFact[0].JobList.size(); i++)
                        {
                            debug("Starting Job " + std::to_string(i));
                            //if(!Deleting) Deleter++;
                            //Con(AddString(Me->name,JobList[i].Type));
                            if (UniFact[0].JobList[i].pItem == nullptr &&
                                UniFact[0].JobList[i].Type ==
                                    "PickUp") // Deleting objectless pickup jobs.
                            { // Deleting objectless pickup jobs.
                                UniFact[0].JobList[i].ToDelete = true;
                                break;
                            }

                            if ((UniFact[0].JobList[i].pWorker == nullptr &&
                                 Me->HasJob == false &&
                                 UniFact[0].JobList[i].ToDelete == false) ||
                                (UniFact[0].JobList[i].pWorker != nullptr &&
                                 UniFact[0].JobList[i].pWorker->id == Me->id &&
                                 UniFact[0].JobList[i].ToDelete == false))
                            {
                                debug("Comparitive Success");
                                if (UniFact[0].JobList[i].pWorker == nullptr)
                                {
                                    std::cout << Me->name
                                              << " has job: " << Me->HasJob
                                              << ", and yet :";
                                    UniFact[0].JobList[i].pWorker = &npclist.at(
                                        IntegerIterator); // Not sure if this is technically better or worse than repointing every frame.
                                    UniFact[0].JobList[i].pWorker->HasJob =
                                        true;
                                    std::cout << Me->HasJob << " does now? \n";
                                    //fSleep(2);
                                }

                                if (UniFact[0].JobList[i].Type == "Build")
                                {
                                    debug("Starting Build");

                                    //if(GetItemPtrfromVector(worlditems, "Wood") != NULL) item WorkLoad = *GetItemPtrfromVector(worlditems, "Wood");
                                    item *InvWood = GetItemPtrfromVector(
                                        Me->inventory, "Wood");
                                    item *WldWood = GetItemPtrfromVector(
                                        worlditems, "Wood");

                                    if (InvWood != nullptr)
                                    {
                                        Con("Success! I have Wood!");
                                        int x = UniFact[0].JobList[i].WorkPos.x;
                                        int y = UniFact[0].JobList[i].WorkPos.y;

                                        Me->TargetPos.x = x;
                                        Me->TargetPos.y = y;
                                        Me->HasTarget = true;
                                        Me->target = "BuildWoodWall";

                                        if (math::Closeish(Me->xpos, Me->ypos,
                                                           x,
                                                           y) <= Me->size * 3)
                                        {

                                            Me->TargetPos.x = Me->xpos;
                                            Me->TargetPos.y = Me->ypos;

                                            UniFact[0]
                                                .JobList[i]
                                                .CompletionProgress +=
                                                Me->Skills.intelligence / 2;

                                            //std::cout << "JobTimer: " << UniFact[0].JobList[i].CompletionProgress << std::endl;
                                            //Effectz.CreateCircle(UniFact[0].JobList[i].WorkPos.x,UniFact[0].JobList[i].WorkPos.y, (UniFact[0].JobList[i].CompletionProgress - UniFact[0].JobList[i].CompletionTimer)/10  ,sf::Color(150,150,150,150));

                                            //std::cout << 361*(PercentIs(UniFact[0].JobList[i].CompletionTimer, UniFact[0].JobList[i].CompletionProgress)/100) << std::endl;
                                            //fSleep(4);
                                            for (
                                                float Rot = 1;
                                                Rot <
                                                    361 *
                                                        (PercentIs(
                                                             UniFact[0]
                                                                 .JobList[i]
                                                                 .CompletionTimer,
                                                             UniFact[0]
                                                                 .JobList[i]
                                                                 .CompletionProgress) /
                                                         100);
                                                Rot++)
                                            {
                                                //std::cout << "Rot: " << Rot << std::endl;

                                                float XPos =
                                                    x +
                                                    sin(Rot * PI / 180) * 10;
                                                float YPos =
                                                    y +
                                                    cos(Rot * PI / 180) * 10;

                                                Effectz.CreateLine(
                                                    x, y, XPos, YPos, 1,
                                                    sf::Color(150, 150, 150,
                                                              150));
                                            }

                                            if (UniFact[0]
                                                    .JobList[i]
                                                    .CompletionProgress >=
                                                UniFact[0]
                                                    .JobList[i]
                                                    .CompletionTimer)

                                            {
                                                Tiles[abs_to_index(x / 20)]
                                                     [abs_to_index(y / 20)][30]
                                                         .Wall();
                                                //Tiles[abs_to_index(x/20)][abs_to_index(y/20)][30].ID = 1010;
                                                //Tiles[abs_to_index(x/20)][abs_to_index(y/20)][30].Img.setTexture( *imagemanager.GetImage("Wall.png"));
                                                InvWood->ToDelete = true;
                                                UniFact[0].JobList[i].ToDelete =
                                                    true;
                                                UniFact[0]
                                                    .JobList[i]
                                                    .pWorker->HasJob = false;
                                                //Crashed instantly;
                                            }
                                        }
                                    }

                                    else if (WldWood != nullptr)
                                    {

                                        Con("Partial Success! World has Wood!");
                                        //item WorkLoad = *GetItemPtrfromVector(worlditems, "Wood");

                                        Me->TargetPos.x = WldWood->xpos;
                                        Me->TargetPos.y = WldWood->ypos;
                                        Me->HasTarget = true;
                                        Me->target = WldWood->name;

                                        //Con(WorkLoad.id);

                                        debug("Post wood targeting, Pre "
                                              "Close-Ish function");

                                        if (math::Closeish(Me->xpos, Me->ypos,
                                                           WldWood->xpos,
                                                           WldWood->ypos) <=
                                            Me->size)
                                        {
                                            //Con("I'm there! \n");
                                            //Deleting = true;
                                            debug("It's close!");
                                            Me->inventory.push_back(*WldWood);
                                            WldWood->ToDelete = true;
                                        }
                                    }
                                    debug("Ended Build");
                                }

                                else if (UniFact[0].JobList[i].pItem != nullptr)
                                {
                                    debug("Starting pItem != NULL");

                                    Me->TargetPos.x =
                                        UniFact[0].JobList[i].pItem->xpos;
                                    Me->TargetPos.y =
                                        UniFact[0].JobList[i].pItem->ypos;
                                    Me->HasTarget = true;
                                    Me->target =
                                        UniFact[0].JobList[i].pItem->name;
                                    debug("Post HasTarget");

                                    if (UniFact[0].JobList[i].Type ==
                                            "PickUp" &&
                                        math::Closeish(
                                            Me->xpos, Me->ypos,
                                            UniFact[0].JobList[i].pItem->xpos,
                                            UniFact[0]
                                                .JobList[i]
                                                .pItem->ypos) <= Me->size)
                                    {
                                        //Con("I'm there! \n");
                                        //Deleting = true;
                                        Me->inventory.push_back(
                                            *UniFact[0].JobList[i].pItem);
                                        debug("Post Inventory Pushback");

                                        UniFact[0].JobList[i].pItem->ToDelete =
                                            true;
                                        /*
                                for(std::list<item>::iterator v = worlditems.begin(); v != worlditems.end(); v++ )
                                {
                                    if((*v).id == UniFact[0].JobList[i].pItem->id)
                                    {
                                        v->ToDelete = true;
                                        std::cout << "Confirmed. \n";
                                        fSleep(1);
                                    }
                                }
                                */

                                        UniFact[0].JobList[i].pItem = nullptr;
                                        UniFact[0].JobList[i].ToDelete = true;
                                        UniFact[0].JobList[i].pWorker->HasJob =
                                            false;
                                        UnpointItems(worlditems);
                                        debug("Post Unpoint");
                                    }

                                    if (UniFact[0].JobList[i].Type == "Chop" &&
                                        math::Closeish(
                                            Me->xpos, Me->ypos,
                                            UniFact[0].JobList[i].pItem->xpos,
                                            UniFact[0]
                                                .JobList[i]
                                                .pItem->ypos) <= Me->size)
                                    {
                                        debug("Post Chopcheck");
                                        //Con("I'm there! \n");
                                        //Deleting = true;
                                        //Me->inventory.push_back(*UniFact[0].JobList[i].pItem);
                                        //FUCKNUTS start here, Just made the plank, Make the tree give the planks, MmkAY?!
                                        item WoodStuffs =
                                            *GetGlobalItem("Wood");

                                        debug("Post WoodStuffs");

                                        WoodStuffs.xpos =
                                            UniFact[0].JobList[i].pItem->xpos;
                                        WoodStuffs.ypos =
                                            UniFact[0].JobList[i].pItem->ypos;
                                        worlditems.push_back(WoodStuffs);
                                        debug("Post WoodSpawn");

                                        UniFact[0].JobList[i].pItem->ToDelete =
                                            true;
                                        UniFact[0].JobList[i].pItem = nullptr;
                                        UniFact[0].JobList[i].ToDelete = true;
                                        UniFact[0].JobList[i].pWorker->HasJob =
                                            false;
                                        debug("Post ToDelete");
                                        UnpointItems(worlditems);

                                        //items are not getting deleted, ugh.
                                    }

                                    debug("Ending pItem != NULL");
                                }

                                if (UniFact[0].JobList[i].Type == "Dig")
                                {

                                    PathFindWorkPos.x =
                                        UniFact[0].JobList[i].WorkPos.x;
                                    PathFindWorkPos.y =
                                        UniFact[0].JobList[i].WorkPos.y;

                                    Me->TargetPos.x = PathFindWorkPos.x;
                                    Me->TargetPos.y = PathFindWorkPos.y;
                                    Me->HasTarget = true;
                                    Me->target = "DigNaturalWall";

                                    if (math::Closeish(Me->xpos, Me->ypos,
                                                       PathFindWorkPos.x,
                                                       PathFindWorkPos.y) <=
                                        Me->size * 3)
                                    {

                                        Me->TargetPos.x = Me->xpos;
                                        Me->TargetPos.y = Me->ypos;

                                        UniFact[0]
                                            .JobList[i]
                                            .CompletionProgress +=
                                            Me->Skills.strength / 2;

                                        for (
                                            float Rot = 1;
                                            Rot <
                                                361 *
                                                    (PercentIs(
                                                         UniFact[0]
                                                             .JobList[i]
                                                             .CompletionTimer,
                                                         UniFact[0]
                                                             .JobList[i]
                                                             .CompletionProgress) /
                                                     100);
                                            Rot++)
                                        { // Pretty circle progressing graphic.

                                            float XPos =
                                                PathFindWorkPos.x +
                                                sin(Rot * PI / 180) * 10;
                                            float YPos =
                                                PathFindWorkPos.y +
                                                cos(Rot * PI / 180) * 10;

                                            Effectz.CreateLine(
                                                PathFindWorkPos.x,
                                                PathFindWorkPos.y, XPos, YPos,
                                                1,
                                                sf::Color(150, 150, 150, 150));
                                        }

                                        if (UniFact[0]
                                                .JobList[i]
                                                .CompletionProgress >=
                                            UniFact[0]
                                                .JobList[i]
                                                .CompletionTimer)

                                        {
                                            Tiles[abs_to_index(
                                                PathFindWorkPos.x / 20)]
                                                 [abs_to_index(
                                                     PathFindWorkPos.y /
                                                     20)][30].Stone();
                                            debug("Spawning Rocks");
                                            int TAR = randz(
                                                3, 8); // Throw away random
                                            for (int z = 0; z != TAR; z++)
                                            {
                                                item StoneStuffs =
                                                    *GetGlobalItem("Rock");

                                                StoneStuffs.xpos =
                                                    PathFindWorkPos.x +
                                                    (4 - randz(1, 8));
                                                StoneStuffs.ypos =
                                                    PathFindWorkPos.y +
                                                    (4 - randz(1, 8));
                                                worlditems.push_back(
                                                    StoneStuffs);
                                            }

                                            UniFact[0].JobList[i].ToDelete =
                                                true;
                                            UniFact[0]
                                                .JobList[i]
                                                .pWorker->HasJob = false;
                                            debug("Dig Wall Completed");
                                        }
                                    }
                                }
                            }

                            debug("End Job " + std::to_string(i));
                        }

                        RemoveJobs(UniFact[0].JobList);

                        debug("Finished Job Activity. \n ");
                    }

                    if (Me->target == "Wander" && Me->HasTarget == false)
                    { // Find somewhere random to walk towards, so long as there isn't anything on that spot.
                        bool FindEmpty = false;
                        while (FindEmpty == false)
                        {
                            Me->TargetPos = sf::Vector2f(randz(700, 1300),
                                                         randz(700, 1300));
                            Me->HasTarget = true;
                            if (globals::groundmap[currentz][abs_to_index(
                                    Me->TargetPos.x /
                                    GridSize)][abs_to_index(Me->TargetPos.y /
                                                            GridSize)] != 10)
                            {
                                FindEmpty = true;
                            }
                        }
                    }
                }
                catch (std::exception &e)
                {
                    std::cout << "Something went wrong in NPC Job Routine \n";
                }

                if (Me->name == "Shinobi" && Me->HasWeapon() == false)
                {
                    std::vector<int> Item =
                        FindClosestItem(Me->xpos, Me->ypos, "Sword");
                    if (Item.size() != 0)
                    {
                        Me->TargetPos = sf::Vector2f(Item.at(0), Item.at(1));
                        Me->TargetId = Item.at(2);
                        Me->TargetVectorId = Item.at(3);
                        Me->target = "Sword";
                        Me->HasTarget = true;
                    }
                    else
                    {
                        Me->target = "Wander";
                    }
                }
                if (Me->HasWeapon())
                { // Attack nearby Hostiles.
                    int closx = -100000;
                    int closy = -100000;
                    bool FoundOne = false;
                    if (Me->HasTag("[ZombieHater:"))
                    {
                        bool first = true;
                        for (auto &elem : npclist)
                        {
                            if (first == true)
                            {
                                if (elem.cbaseid == 666333 && elem.health > 0)
                                {
                                    closx = elem.xpos;
                                    closy = elem.ypos;
                                    first = false;
                                    FoundOne = true;
                                }
                            }
                            else
                            {
                                if (elem.cbaseid == 666333 && elem.health > 0)
                                {
                                    int one =
                                        math::Closeish(Me->xpos, Me->ypos,
                                                       elem.xpos, elem.ypos);
                                    int two = math::Closeish(Me->xpos, Me->ypos,
                                                             closx, closy);
                                    if (one < two)
                                    {
                                        closx = elem.xpos;
                                        closy = elem.ypos;
                                    }
                                }
                            }
                        }
                        if (first == false)
                        {
                            Me->ShootPos = sf::Vector2f(closx, closy);
                            if (math::Closeish(Me->xpos, Me->ypos,
                                               Me->ShootPos.x,
                                               Me->ShootPos.y) <= Me->viewrange)
                            {
                                Me->Attacking = true;
                            }
                        }
                    }
                    if (Me->IsHungry() == false && Me->IsThirsty() == false &&
                        FoundOne == true)
                    {
                        Me->TargetPos = sf::Vector2f(closx, closy);
                    }
                }

                if (Me->CloseToTarget(Me->reach, Me->TargetPos) == true &&
                    Me->target == "Flesh")
                {
                    if (Me->attacktimer == 0)
                    {

                        std::string AtkType;
                        Me->attacktimer =
                            (Me->attacktimerint -
                             (math::Clamp(Me->Skills.agility / 10, 10, 100))) *
                            4;
                        int numba = -1;
                        int numbaz = -1;
                        int num;
                        bool foundit = false;
                        for (auto &elem : npclist)
                        {
                            if (foundit == false)
                            {
                                numba++;
                                if (CloseishS(elem.xpos, Me->xpos) <=
                                        Me->reach &&
                                    CloseishS(elem.ypos, Me->ypos) <=
                                        Me->reach &&
                                    elem.target != "Flesh")
                                {
                                    numbaz++;
                                    num = elem.id;
                                    foundit = true;
                                }
                            }
                        }
                        if (foundit == true && npclist.at(numba).id == num)
                        {
                            Me->AtTarget = false;
                            Me->HasTarget =
                                false; // TODO: Fix Zombie Attacking, Give it some Delay, and some damage based on Strength and filled belly.
                            int zDamage = randz(0, npclist.at(numba).health);
                            if (Me->name != "Azabul")
                                Me->fillhunger(zDamage);
                            if (Me->name != "Azabul")
                                npclist.at(numba)
                                    .bloodwork("Zombification", 100000);
                            if (Me->name == "Azabul")
                            {
                                Me->TentArm1.x = npclist.at(numba).xpos;
                                Me->TentArm1.y = npclist.at(numba).ypos;
                                Me->TentArm2.x = npclist.at(numba).xpos;
                                Me->TentArm2.y = npclist.at(numba).ypos;
                                //BLARGGITY
                                item Blood;
                                Blood.amount = zDamage;
                                Blood.name = "Blood";
                                Blood.InsidePart = "LowerTorso";
                                Me->inventory.push_back(Blood);
                                AtkType = " has drunk from ";
                            }
                            npclist.at(numba).modhealth(-zDamage);

                            std::string AtkStr;
                            AtkStr.append("* ");
                            AtkStr.append(Me->name);
                            AtkStr.append("(" + std::to_string(Me->id) + ")");
                            if (AtkType != "")
                                AtkStr.append(" has biten ");
                            else
                                AtkStr.append(AtkType);
                            AtkStr.append(npclist.at(numba).name);
                            AtkStr.append("(" +
                                          std::to_string(npclist.at(numba).id) +
                                          ")");
                            AtkStr.append(", dealing ");
                            AtkStr.append(std::to_string(zDamage));
                            AtkStr.append(" damage.");

                            ChatBox.AddChat(AtkStr, sf::Color::Red);
                        }
                    }
                }
                if (Me->name == "Mini Turret")
                {
                    if (Me->HasWeapon() == false)
                    {
                        Me->AddItem("5mmCannon", 1);
                        Me->Skills.dexterity = 90;
                        Me->Skills.agility = 10;
                    }
                    if (Me->HasTag("[ZombieHater:") == false)
                    {
                        Me->Tag("[ZombieHater:", 1);
                        std::cout << "Giving MiniTurret [ZombieHater: \n";
                    }
                }
            } // Ending of "Act" action
            if (Debug)
                std::cout << "Post Act Section \n";

            { // Vision check and Activation of Path Finding.
                if (Debug)
                    std::cout << "Pre 'set' vision. \n";

                bool FoundGoal = false;
                if (math::Closeish(Me->TargetPos.x, Me->TargetPos.y, Me->xpos,
                                   Me->ypos) <= Me->viewrange &&
                    Me->cbaseid != -1337)
                {

                    float Ang = math::AngleBetweenVectors(
                        math::Vec2f(Me->TargetPos.x, Me->TargetPos.y),
                        math::Vec2f(Me->xpos, Me->ypos));
                    float MyAngle = Me->angle;
                    float difference = abs(Ang - MyAngle);
                    if (difference > 180.0f)
                    {
                        difference = 360.0f - difference;
                    }
                    if (difference >= -90.0f && difference <= 90.0f)
                    {
                        if (gridposTrace(Me->xpos, Me->ypos, Me->TargetPos.x,
                                         Me->TargetPos.y, Me->id,
                                         Me->TargetPos) == true)
                        {
                            FoundGoal = true;
                            Effectz.CreateLine(Me->xpos, Me->ypos,
                                               Me->TargetPos.x, Me->TargetPos.y,
                                               1, sf::Color::White);
                            Me->NeedsPath = false;
                        }
                    }
                }
                globals::sunmap[currentz][abs_to_index(
                    Me->xpos / GridSize)][abs_to_index(Me->ypos / GridSize)] =
                    255;
                for (int i = 0; i <= gridy - 1; i++)
                { // Vision Stuffs;
                    for (int t = 0; t <= gridx - 1; t++)
                    {
                        int z = currentz;
                        if (globals::sunmap[z][i][t] != -1)
                        {
                            if (math::Closeish((i * GridSize) + 10,
                                               (t * GridSize) + 10, Me->xpos,
                                               Me->ypos) <= Me->viewrange &&
                                Me->cbaseid != -1337)
                            {

                                float Ang = math::AngleBetweenVectors(
                                    math::Vec2f((i * GridSize) + 10,
                                                (t * GridSize) + 10),
                                    math::Vec2f(Me->xpos, Me->ypos));
                                float MyAngle = Me->angle;
                                float difference = abs(Ang - MyAngle);
                                if (difference > 180.0f)
                                {
                                    difference = 360.0f - difference;
                                }
                                if (difference >= -90.0f && difference <= 90.0f)
                                {
                                    /*if(gridposTrace(Me->xpos,Me->ypos,(i*GridSize)+10,(t*GridSize)+10,Me->id,Math::Vec((i*GridSize)+10,(t*GridSize)+10)) == true)
                                        {
                                             globals::sunmap[z][i][t] = 255;
                                             //Effectz.CreateCircle((i*20)+10,(t*20)+10,2,White);
                                        }*/
                                }
                            }
                        }
                    }
                }
                if (FoundGoal == false && Me->cbaseid != -1337)
                {
                    if (Debug)
                        std::cout << "FoundGoal == false";
                    Me->NeedsPath = true;

                    int Previous = -1;

                    if (PathFindWorkPos.x != 0)
                    {
                        Previous = walkability[abs_to_index(
                            PathFindWorkPos.x /
                            20)][abs_to_index(PathFindWorkPos.y / 20)];
                        walkability[abs_to_index(PathFindWorkPos.x / 20)]
                                   [abs_to_index(PathFindWorkPos.y / 20)] =
                                       walkable;
                    }

                    Me->PathFinding.MyFindPath(
                        Me->xpos, Me->ypos, Me->TargetPos.x,
                        Me->TargetPos
                            .y); // TODO: This causes a crash for some reason.

                    if (PathFindWorkPos.x != 0)
                    {
                        walkability[abs_to_index(PathFindWorkPos.x / 20)]
                                   [abs_to_index(PathFindWorkPos.y / 20)] =
                                       Previous;
                    }
                }
                if (true == false)
                {
                    /*try{

        sList = NpcList(Me->id);
        std::set<int>::iterator iList;

        for(iList = sList.begin(); iList != sList.end(); iList++){
            float NpcPosX = npclist.at(*iList).xpos;
            float NpcPosY = npclist.at(*iList).ypos;
            if( Math::Closeish(Me->TargetPos.x,Me->TargetPos.y,Me->xpos,Me->ypos) <= Me->viewrange ){
                float Ang = Math::AngleBetweenVectors(Math::Vec(Me->TargetPos.x,Me->TargetPos.y),Math::Vec(Me->xpos,Me->ypos));
                if(Ang >= Me->angle-90 && Ang <= Me->angle+90){

                    std::cout << Me->name << Me->id << " is facing " << npclist.at(*iList).name <<npclist.at(*iList).id << std::endl;
                    Effectz.CreateLine(Me->xpos,Me->ypos,Me->TargetPos.x,Me->TargetPos.y,1,White);
                    if(gridposTrace(Me->xpos,Me->ypos,Me->TargetPos.x,Me->TargetPos.y,Me->id,Me->TargetPos) == true){ FoundGoal = true; }
                }

            }
        }


    }catch (std::exception& e){}*/
                }
                if (Debug)
                    std::cout << "Post 'set' vision. \n";
            }

            if (Me->action ==
                "Orders") // Vision correction currently disabled to preserve laaaaaag.
            {             // Start of "Orders action.
                /*for (int i = 0; i <= gridy-1; i++)
                {
                    for( int t = 0; t <= gridx-1; t++)
                    {
                        int z = currentz;
                        if(globals::sunmap[z][i][t] != -1)
                        {
                            if( Math::Closeish((i*GridSize)+10,(t*GridSize)+10,Me->xpos,Me->ypos) <= Me->viewrange && Me->cbaseid != -1337)
                            {
                                float Ang = Math::AngleBetweenVectors(Math::Vec((i*GridSize)+10,(t*GridSize)+10),Math::Vec(Me->xpos,Me->ypos));
                                float MyAngle = Me->angle;
                                float difference = abs(Ang - MyAngle);
                                if (difference > 180.0f)
                                {
                                    difference = 360.0f - difference;
                                }
                                if(difference >= -90.0f && difference <= 90.0f)
                                {
                                    if(gridposTrace(Me->xpos,Me->ypos,(i*GridSize)+10,(t*GridSize)+10,Me->id,Math::Vec((i*GridSize)+10,(t*GridSize)+10)) == true)
                                    {
                                        globals::sunmap[z][i][t] = 255;
                                        //Effectz.CreateCircle((i*20)+10,(t*20)+10,2,White);
                                    }
                                }
                            }
                        }
                    }
                }*/
                if (Me->target == "Goto")
                {
                }
                //if(Me->NeedsPath == false){if(Me->target != "None"){ Me->DirMove(Me->TargetPos);}}
                if (Me->AtTarget == true && Me->target == "Goto")
                {
                    Me->AtTarget = false;
                    Me->HasTarget = false;
                    Me->target = "None";
                }
            } // Ending of "Orders" action
            if (Me->action == "Pickup")
            {
                //if(Me->NeedsPath == false){if(Me->target != "None"){ Me->DirMove(Me->TargetPos);}}
            }
            if ((Me->target != "None" && Me->AtTarget) ||
                (Me->target != "None" &&
                 math::Closeish(Me->xpos, Me->ypos, Me->TargetPos.x,
                                Me->TargetPos.y) <= Me->size))
            {
                if (GetItemVectorId(Me->TargetId) != -1)
                {
                    auto ItemItr = worlditems.begin();
                    std::advance(ItemItr, GetItemVectorId(Me->TargetId));

                    //if(Math::Closeish(Me->xpos,Me->ypos,worlditems.at(GetItemVectorId(Me->TargetId)).xpos,worlditems.at(GetItemVectorId(Me->TargetId)).ypos) <= Me->reach && worlditems.at(GetItemVectorId(Me->TargetId)).Pickupable == true)
                    if (math::Closeish(Me->xpos, Me->ypos, (*ItemItr).xpos,
                                       (*ItemItr).ypos) <= Me->reach &&
                        (*ItemItr).Pickupable == true)
                    {
                        try
                        {
                            Me->inventory.push_back((*ItemItr));
                            RemoveItem(Me->TargetId);
                        }
                        catch (std::exception &e)
                        {
                            cText.CreateText(Me->xpos, Me->ypos, 11,
                                             sf::Color::Red,
                                             "Somethings wrong!");
                        };
                    }
                }
                if (Me->target == "Wander")
                {
                    Me->AtTarget = false;
                    Me->HasTarget = false;
                }
                if (true == false)
                {
                    if (Me->target == "Sword")
                    {
                        if (GetItemVectorId(Me->TargetId) != -1)
                        {
                            if (math::Closeish(
                                    Me->xpos, Me->ypos,
                                    (*ListGet(worlditems,
                                              GetItemVectorId(Me->TargetId)))
                                        .xpos,
                                    (*ListGet(worlditems,
                                              GetItemVectorId(Me->TargetId)))
                                        .ypos) <= Me->reach * 2)
                            {
                                try
                                {
                                    Me->inventory.push_back(
                                        (*ListGet(
                                             worlditems,
                                             GetItemVectorId(Me->TargetId))));
                                    RemoveItem(Me->TargetId);
                                }
                                catch (std::exception &e)
                                {
                                };
                            }
                        }
                    }
                    else if (Me->target == "Food")
                    {
                        if (GetItemVectorId(Me->TargetId) != -1)
                        {
                            if (math::Closeish(
                                    Me->xpos, Me->ypos,
                                    (*ListGet(worlditems,
                                              GetItemVectorId(Me->TargetId)))
                                        .xpos,
                                    (*ListGet(worlditems,
                                              GetItemVectorId(Me->TargetId)))
                                        .ypos) <= Me->size * 2)
                            {
                                try
                                {
                                    //Me->inventory.push_back(worlditems.at(GetItemVectorId(Me->TargetId)));
                                    Me->fillhunger((*ListGet(worlditems,
                                                             GetItemVectorId(
                                                                 Me->TargetId)))
                                                       .hungervalue);
                                    RemoveItem(Me->TargetId);
                                    Me->AtTarget = false;
                                    Me->HasTarget = false;
                                    Me->TargetId = -1;
                                    Me->TargetVectorId = -1;
                                    cText.CreateText(Me->xpos, Me->ypos, 11,
                                                     sf::Color::Blue,
                                                     "*Crunch!*");
                                }
                                catch (std::exception &e)
                                {
                                    cText.CreateText(Me->xpos, Me->ypos, 11,
                                                     sf::Color::Red,
                                                     "What the FUCK?!");
                                };
                            }
                        }
                    }
                    else if (Me->target == "Water")
                    {
                        if (GetItemVectorId(Me->TargetId) != -1)
                        {
                            if (math::Closeish(
                                    Me->xpos, Me->ypos,
                                    (*ListGet(worlditems,
                                              GetItemVectorId(Me->TargetId)))
                                        .xpos,
                                    (*ListGet(worlditems,
                                              GetItemVectorId(Me->TargetId)))
                                        .ypos) <= Me->size * 2)
                            {
                                try
                                {
                                    //Me->inventory.push_back(worlditems.at(GetItemVectorId(Me->TargetId)));
                                    Me->fillthirst((*ListGet(worlditems,
                                                             GetItemVectorId(
                                                                 Me->TargetId)))
                                                       .thirstvalue);
                                    Me->AtTarget = false;
                                    Me->HasTarget = false;
                                    Me->TargetId = -1;
                                    Me->TargetVectorId = -1;
                                    cText.CreateText(Me->xpos, Me->ypos, 11,
                                                     sf::Color::Blue,
                                                     "*Gulp!*");
                                }
                                catch (std::exception &e)
                                {
                                    cText.CreateText(Me->xpos, Me->ypos, 11,
                                                     sf::Color::Red,
                                                     "What the FUCK?!");
                                };
                            }
                        }
                    }
                }
            }
            if (Debug)
                std::cout << "Post Item Pickups. \n";

            if (Me->movetimer <= 0)
            {
                Me->movetimer =
                    (Me->movetimerint +
                     Me->movetimer); // TODO: Figure out why I added 0 to this, Year later: It was because movetimer may be less than 0, I wanted the next to turn happen as soon as possible due to it.
                if (Me->target != "None" && Me->NeedsPath == false &&
                    math::Vec2f(Me->xpos, Me->ypos) != Me->TargetPos)
                { // Walk Move
                    if (Me->Attacking && Me->HasWeapon("Gun") &&
                        math::Closeish(Me->xpos, Me->ypos, Me->TargetPos.x,
                                       Me->TargetPos.y) < Me->viewrange)
                    {
                    }
                    else if (Me->Attacking && Me->HasWeapon("Sword") &&
                             math::Closeish(Me->xpos, Me->ypos, Me->TargetPos.x,
                                            Me->TargetPos.y) < 10)
                    {
                    }
                    else
                    {
                        /*if(gridposTrace(Me->xpos,Me->ypos,Me->TargetPos.x,Me->TargetPos.y,Me->id,Me->TargetPos) != false)
                        {
                            //Me->DirMove(Me->TargetPos);
                        }*/
                        Me->DirMove(Me->TargetPos);
                    }
                }
                if (Me->NeedsPath == true &&
                    math::Vec2f(Me->xpos, Me->ypos) != Me->TargetPos)
                { // Acting on Path Finding.
                    if (randz(0, 20) < 3)
                    {
                        int Previous = -1;
                        if (PathFindWorkPos.x != 0)
                        {
                            Previous = walkability[abs_to_index(
                                PathFindWorkPos.x /
                                20)][abs_to_index(PathFindWorkPos.y / 20)];
                            walkability[abs_to_index(PathFindWorkPos.x / 20)]
                                       [abs_to_index(PathFindWorkPos.y / 20)] =
                                           walkable;
                        }

                        Me->PathFinding.MyFindPath(Me->xpos, Me->ypos,
                                                   Me->TargetPos.x,
                                                   Me->TargetPos.y);

                        if (PathFindWorkPos.x != 0)
                        {
                            walkability[abs_to_index(PathFindWorkPos.x / 20)]
                                       [abs_to_index(PathFindWorkPos.y / 20)] =
                                           Previous;
                        }
                    }
                    for (int i = 0; i != Grids; i++)
                    {
                        for (int t = 0; t != Grids; t++)
                        {
                            // Add an Item Check here later to see if it blocks movement
                            if (Tiles[i][t][30].Walkable == false)
                            {
                                walkability[i][t] = unwalkable;
                            }
                            else
                            {
                                walkability[i][t] = walkable;
                            }
                            if (walkability[i][t] > 1)
                                walkability[i][t] = 0;
                        }
                    }

                    int Previous = -1;

                    if (PathFindWorkPos.x != 0)
                    {
                        Previous = walkability[abs_to_index(
                            PathFindWorkPos.x /
                            20)][abs_to_index(PathFindWorkPos.y / 20)];
                        walkability[abs_to_index(PathFindWorkPos.x / 20)]
                                   [abs_to_index(PathFindWorkPos.y / 20)] =
                                       walkable;
                    }

                    Me->PathFinding.MyReadPath(1, Me->xpos, Me->ypos,
                                               Me->moverate);
                    Me->DirMove(math::Vec2f(Me->PathFinding.MyxPath,
                                            Me->PathFinding.MyyPath));

                    if (PathFindWorkPos.x != 0)
                    {
                        walkability[abs_to_index(PathFindWorkPos.x / 20)]
                                   [abs_to_index(PathFindWorkPos.y / 20)] =
                                       Previous;
                    }

                    /*if (Me->xpos > Me->PathFinding.MyxPath) {Me->xpos = Me->xpos - Me->moverate;}
	if (Me->xpos < Me->PathFinding.MyxPath) {Me->xpos = Me->xpos + Me->moverate;}
	if (Me->ypos > Me->PathFinding.MyyPath) {Me->ypos = Me->ypos - Me->moverate;}
	if (Me->ypos < Me->PathFinding.MyyPath) {Me->ypos = Me->ypos + Me->moverate;}*/

                    //Me->PathFinding.MyFindPath(Me->xpos,Me->ypos,Me->TargetPos.x,Me->TargetPos.y);

                    if (Me->id == MyTargetid)
                    {
                        for (int Stuff = Me->PathFinding.MypathLocation;
                             Stuff != Me->PathFinding.MypathLength; ++Stuff)
                        {
                            if (Stuff != 1)
                            {
                                Effectz.CreateLine(
                                    Me->PathFinding.MyReadPathX(1, Stuff - 1),
                                    Me->PathFinding.MyReadPathY(1, Stuff - 1),
                                    Me->PathFinding.MyReadPathX(1, Stuff),
                                    Me->PathFinding.MyReadPathY(1, Stuff), 2,
                                    sf::Color::Blue);
                            }
                            //std::cout << Me->PathFinding.MyReadPathX(1,Stuff) << ":" << Me->PathFinding.MyReadPathY(1,Stuff) << std::endl;
                        }
                    }

                    if (Me->PathFinding.MypathLocation ==
                        Me->PathFinding.MypathLength)
                    {
                        if (abs(Me->xpos - Me->PathFinding.MyxPath) <
                            Me->moverate)
                            Me->xpos = Me->PathFinding.MyxPath;
                        if (abs(Me->ypos - Me->PathFinding.MyyPath) <
                            Me->moverate)
                            Me->ypos = Me->PathFinding.MyyPath;
                        Me->action = "Act";
                        Me->NeedsPath = false;
                    }

                    //Me->TargetPos.x = xPath[1];
                    //Me->TargetPos.y = yPath[1];
                }
            }
            for (auto &elem : Me->inventory)
            {
                if (elem.type == 4 &&
                    Me->hunger + elem.hungervalue <= Me->maxhunger &&
                    true == false)
                {
                    //if(Item->hungervalue > 0){ Me->fillhunger(1); Item->hungervalue--;}
                    Me->fillhunger(elem.hungervalue);
                    Me->fillthirst(elem.thirstvalue);
                    elem.ToDelete = true;

                    std::string ChtStr;
                    ChtStr.append("* ");
                    ChtStr.append(Me->name);
                    ChtStr.append("(" + std::to_string(Me->id) + ")");
                    ChtStr.append(" has consumed ");
                    ChtStr.append(elem.name);
                    ChtStr.append(".");

                    ChatBox.AddChat(ChtStr, sf::Color(150, 150, 0));
                }
                if (elem.type == 5 &&
                    Me->thirst + elem.thirstvalue <= Me->maxthirst &&
                    true == false)
                {
                    //if(Item->hungervalue > 0){ Me->fillhunger(1); Item->hungervalue--;}
                    Me->fillhunger(elem.hungervalue);
                    Me->fillthirst(elem.thirstvalue);
                    elem.ToDelete = true;

                    std::string ChtStr;
                    ChtStr.append("* ");
                    ChtStr.append(Me->name);
                    ChtStr.append("(" + std::to_string(Me->id) + ")");
                    ChtStr.append(" has consumed ");
                    ChtStr.append(elem.name);
                    ChtStr.append(".");

                    ChatBox.AddChat(ChtStr, sf::Color(150, 150, 0));
                }
            }
            if (Debug)
                std::cout << "Post Item Usages. \n";

            UnpointItems(Me->inventory);

            bool Done = false;

            while (Done == false)
            {
                bool Yet = false;
                for (auto it = Me->inventory.begin(); it != Me->inventory.end();
                     ++it)
                {
                    if (it->ToDelete)
                    {
                        std::cout << it->name << " to be deleted. \n";
                        Me->inventory.erase(it);
                        Yet = true;
                        break;
                    }
                }
                if (Yet == false)
                {
                    Done = true;
                }
            }
            if (Debug)
                std::cout << "Post Inventory Cleanup. \n";

        } // End of CanMove
        //Me->angle = Math::AngleBetweenVectors(Math::Vec(TempXpos,TempYpos),Math::Vec(Me->xpos,Me->ypos))-180;
        Me->angle = math::AngleBetweenVectors(Me->TargetPos,
                                              math::Vec2f(Me->xpos, Me->ypos));
        Me->MomMove();
        //float f=Math::AngleBetweenVectors(sf::Vector2f(Me->xpos,Me->ypos), Me->TargetPos);Me->img.setRotation(f);
        if (Debug)
            std::cout << Me->name << Me->id << " is done. \n";

        IntegerIterator++;
    }
    if (Debug)
        std::cout << "Post NPC\n";
}

void DrawTiles()
{
    int z = currentz;
    int iTS = GridSize;
    for (int i = 0; i <= gridy - 1; i++)
    {
        for (int t = 0; t <= gridx - 1; t++)
        {
            if ((globals::Following == true &&
                 i > (npclist.at(MyTarget).xpos / GridSize) - 27 &&
                 i < (npclist.at(MyTarget).xpos / GridSize) + 26) ||
                (i > currentx - 27 && i < currentx + 26))
            {
                if ((globals::Following == true &&
                     t > (npclist.at(MyTarget).ypos / GridSize) - 21 &&
                     t < (npclist.at(MyTarget).ypos / GridSize) + 20) ||
                    (t > currenty - 21 && t < currenty + 20))
                {
                    sf::Sprite Tile;
                    if (globals::groundmap[currentz][i][t] == 1)
                    { // dirt
                        Tile.setTexture(*imagemanager.GetImage("Dirt.bmp"));
                        Tile.setColor(
                            sf::Color(255, 255, 255, globals::sunmap[z][i][t]));
                        Tile.setPosition(i * iTS, t * iTS);
                        App.draw(Tile);
                    }
                    if (globals::groundmap[currentz][i][t] == 3)
                    { // grass
                        Tile.setTexture(*imagemanager.GetImage("Grass.bmp"));
                        Tile.setColor(
                            sf::Color(255, 255, 255, globals::sunmap[z][i][t]));
                        Tile.setPosition(i * iTS, t * iTS);
                        App.draw(Tile);
                    }
                    if (globals::groundmap[currentz][i][t] == 7)
                    { // stone
                        Tile.setTexture(*imagemanager.GetImage("Stone.bmp"));
                        Tile.setColor(
                            sf::Color(255, 255, 255, globals::sunmap[z][i][t]));
                        Tile.setPosition(i * iTS, t * iTS);
                        App.draw(Tile);
                    }
                    if (globals::groundmap[currentz][i][t] == 20)
                    { // water
                        Tile.setTexture(*imagemanager.GetImage("Water.bmp"));
                        Tile.setColor(
                            sf::Color(255, 255, 255, globals::sunmap[z][i][t]));
                        Tile.setPosition(i * iTS, t * iTS);
                        App.draw(Tile);
                    }
                    if (globals::groundmap[currentz][i][t] == 53)
                    { // lava
                        Tile.setTexture(*imagemanager.GetImage("Lava.bmp"));
                        Tile.setColor(
                            sf::Color(255, 255, 255, globals::sunmap[z][i][t]));
                        Tile.setPosition(i * iTS, t * iTS);
                        App.draw(Tile);
                    }
                    if (globals::groundmap[currentz][i][t] == 52)
                    { // deepwater
                        Tile.setTexture(
                            *imagemanager.GetImage("DeepWater.bmp"));
                        Tile.setColor(
                            sf::Color(255, 255, 255, globals::sunmap[z][i][t]));
                        Tile.setPosition(i * iTS, t * iTS);
                        App.draw(Tile);
                    }
                }
            }
        }
    }
}

void DrawNPCs()
{
    for (auto &elem : npclist)
    {
        //if(zit->xpos/GridSize > currentx-27 && zit->xpos/GridSize < currentx+26 && zit->ypos/GridSize > currenty-20 && zit->ypos/GridSize < currenty+20)
        //{
        if (elem.HasSpawned == true)
        {
            // zit->img.setRotation( -anglez+90 );
            //int Alph = globals::sunmap[currentz][abs_to_index(zit->xpos/GridSize)][abs_to_index(zit->ypos/GridSize)];

            if (elem.name == "Azabul")
            {
                sf::Vector2f TA1 = elem.TentArm1;
                sf::Vector2f TA2 = elem.TentArm2;
                sf::Vector2f TE1 = elem.TentEnd1;
                sf::Vector2f TE2 = elem.TentEnd2;

                Effectz.CreateLine(elem.xpos, elem.ypos, TA1.x + elem.xpos,
                                   TA1.y + elem.ypos, 2, sf::Color(0, 200, 200),
                                   1, sf::Color(0, 255, 255));
                Effectz.CreateLine(elem.xpos, elem.ypos, TA2.x + elem.xpos,
                                   TA2.y + elem.ypos, 2, sf::Color(0, 200, 200),
                                   1, sf::Color(0, 255, 255));
                Effectz.CreateLine(
                    TA1.x + elem.xpos, TA1.y + elem.ypos,
                    TE1.x + TA1.x + elem.xpos, TE1.y + TA1.y + elem.ypos, 2,
                    sf::Color(0, 200, 200), 1, sf::Color(0, 255, 255));
                Effectz.CreateLine(
                    TA2.x + elem.xpos, TA2.y + elem.ypos,
                    TE2.x + TA2.x + elem.xpos, TE2.y + TA2.y + elem.ypos, 2,
                    sf::Color(0, 200, 200), 1, sf::Color(0, 255, 255));
                Effectz.DrawEffects();
            }

            int Alph = 255;
            elem.img.setColor(sf::Color(255, 255, 255, Alph));
            elem.img.setScale(globals::Scalex, globals::Scaley);
            elem.img.setOrigin(elem.img.getTextureRect().width / 2,
                               elem.img.getTextureRect().height / 2);
            elem.DrawImg();
            Effectz.CreateCircle(elem.xpos, elem.ypos, elem.size,
                                 sf::Color(50, 50, 50, 50));
        }
        //}
    }
    debug("Done drawing NPCs");
}

void DrawItems()
{
    //App.setActive(true);

    for (auto &worlditem : worlditems)
    {
        //if(zit->xpos/GridSize > currentx-27 && zit->xpos/GridSize < currentx+26 && zit->ypos/GridSize > currenty-20 && zit->ypos/GridSize < currenty+20)
        //{
        worlditem.img.setColor(sf::Color(255, 255, 255, 255));
        worlditem.img.setScale(globals::Scalex, globals::Scaley);
        worlditem.DrawImg();
        //}
    }
    debug("Done Drawing Items");
}

void LightTrail(int x, int y, int z)
{
    int curx = x;
    int cury = y;
    int curz = z;
    if (curx > gridx - 1)
    {
        curx = gridx - 1;
    }
    if (cury > gridy - 1)
    {
        cury = gridy - 1;
    }
    if (cury > gridz - 1)
    {
        cury = gridz - 1;
    }
    if (curx < 0)
    {
        curx = 0;
    }
    if (cury < 0)
    {
        cury = 0;
    }
    if (curz < 0)
    {
        curz = 0;
    }
    globals::sunmap[curz][curx][cury] = 255;
}

void DisplayChat(sf::Vector2f Position)
{
    if (GC.Phase != "MainMenu")
        Effectz.CreateSquare(
            Position.x - 10, Position.y + 10, Position.x + 500,
            (Position.y - ((ChatBox.ChatStorage.size() + 1) * 10)),
            sf::Color(0, 0, 0, 100), 2, sf::Color::Cyan);

    for (size_t i = 0; i != ChatBox.ChatStorage.size(); i++)
    {
        cText.CreateText(
            Position.x,
            (Position.y - ((ChatBox.ChatStorage.size()) * 10)) + (i * 10), 11,
            ChatBox.ChatStorage[i].Color, ChatBox.ChatStorage[i].Line);
    }
}

void DrawStuffs()
{

    //sf::Context context;
    //App.setActive(true);
    DrawNewTiles();

    //DrawPlanets();
    DrawItems();

    DrawNPCs();

    DrawJobList(App.getView().getCenter().x - 500, App.getView().getCenter().y);
    debug("Drew Joblist");

    DisplayChat(
        sf::Vector2f(globals::BottomLeft.x + 5, globals::BottomLeft.y - 5));
    debug("Drew Chat");

    Effectz.DrawEffects();
    debug("Drew Effects");
    //    DrawPlanets();//Remove this one, Reenable previous

    for (auto &button : vSquareButtonList)
    {
        button.DrawButton();
    }
    vSquareButtonList.clear();
    debug("Drew and Cleared square buttons");

    cText.DrawTextz();
    debug("Drew Text");

    for (auto &button : vButtonList)
    {
        button.DrawButton();
    }
    vButtonList.clear();
    debug("Drew and Cleared buttons");

    DrawStuffsDone = true;
}

cItem *GetGlobalItem(std::string strtype)
{
    if (Debug)
    {
        std::cout << "Getting" << strtype << " \n";
    }
    for (auto &elem : itemmanager.GlobalItem)
    {
        if (elem.name == strtype)
        {
            if (Debug)
            {
                std::cout << "Found" << strtype << " \n";
            }
            return &elem;
        }
    }
    if (Debug)
    {
        std::cout << "Didn't Find" << strtype << " \n";
    }

    return nullptr;
}

NPC *GetCritter(int id)
{
    if (Debug)
    {
        std::cout << "Getting critter(" << id << ") \n";
    }
    for (auto &elem : npclist)
    {
        if (elem.id == id)
        {
            if (Debug)
            {
                std::cout << "Found critter(" << id << ") \n";
            }
            return &elem;
        }
    }
    if (Debug)
    {
        std::cout << "Didn't Find critter(" << id << ") \n";
    }
    return nullptr;
}

void RemoveNPCs()
{
    bool Done = false;
    while (Done == false)
    {
        bool Yet = false;
        for (auto it = npclist.begin(); it != npclist.end(); ++it)
        {
            if (it->ToDelete)
            {
                std::cout << it->name << " to be deleted. \n";
                npclist.erase(it);
                Yet = true;
                break;
            }
        }
        if (Yet == false)
        {
            Done = true;
        }
    }
}

int main()
{
    App.create(sf::VideoMode(Rez.x, Rez.y, 32), RandomWindowName());

    cText.LoadFont();

    bool Paused = false;
    //Debug = true;

    bool PlyAct = false;
    App.setFramerateLimit(30); // 0 is unlimited
    sf::View Planetary(Center, HalfSize);

    App.setVerticalSyncEnabled(true);

    // Various temporary variables used for testing.
    int testage = 0;
    int testage2 = 0;
    float Xxx = 0;                      // global
    float Yyy = 0;                      // global
    int speeds = 1;                     // global
    int xanchor = 0;                    // global
    int yanchor = 0;                    // global
    float Degrees = randz(.0f, 359.0f); // global
    int radius = 200;

    View1.zoom(2);
    if (true == false)
    {   // TODO: Fix this icon crap.
        /*sf::Image icon;
    icon.LoadFromFile("gfx/icon.tga");
    App.SetIcon(32,32,icon.GetPixelsPtr());
    if (!Images.LoadFromFile("gfx/Images.tga"))return EXIT_FAILURE;
    sf::Image Aim;
    if (!Aim.LoadFromFile("gfx/Aim.tga"))return EXIT_FAILURE;*/
    }

    imagemanager.InitializeImages();
    itemmanager.InitializeItems();
    npcmanager.InitializeCritters();

    // Building the players faction, This is temporary.
    PF.Name = "The Alphas";
    PF.PlayerControlled = true;
    PF.Initialized = true;
    UniFact.push_back(PF);
    ConFact = &UniFact[0];

    // Setting the initial game phase.
    GC.Phase = "MainMenu";

    // For A*
    InitializePathfinder();

    while (App.isOpen())
    {
        RemoveNPCs();
        if (Key.mTime > 5)
        {
            GC.Phase = "MainMenu";
        }
        sf::Event Event;
        while (App.pollEvent(Event))
        {
            if (Event.type == sf::Event::Closed)
            {
                App.close();
            }
            if (Event.type == sf::Event::MouseWheelMoved)
            {
                if (Event.mouseWheel.delta > 0)
                {
                    std::cout << "Zoom Out \n";
                    if (CameraZoom < 2)
                    {
                        CameraZoom = CameraZoom / 0.5;
                        //CameraSize
                        View1.zoom(0.5);
                    }
                }
                if (Event.mouseWheel.delta < 0)
                {
                    std::cout << "Zoom In \n";
                    if (CameraZoom > 0.5)
                    {
                        CameraZoom = CameraZoom / 2;
                        View1.zoom(2);
                    }
                }

                //Develop the scaling camera, Probably using .setsize instead of zoom.
                //http://www.sfml-dev.org/tutorials/2.0/graphics-view.php
            }

            if (Event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                //sf::FloatRect visibleArea(0, 0, Event.size.width, Event.size.height);
                //View1.setViewport(visibleArea);
                //App.setView(sf::View(visibleArea));
            }

            if (Event.type == sf::Event::LostFocus)
            {
                InFocus = false;
            }
            if (Event.type == sf::Event::GainedFocus)
            {
                InFocus = true;
            }
        }
        App.setView(View1);
        globals::ButtonClicked = false;
        globals::ButtonClickedTime--; // Misleading Variable name, Sorry!
        if (globals::ButtonClickedTime < 0)
            globals::ButtonClickedTime = 0;

        Key.Update();
        sf::Vector2f MouseStagnationCheck = MousePos;
        // For some reason, I have to manually modify the positions.
        MousePos = App.mapPixelToCoords(sf::Mouse::getPosition(App));
        if (MouseStagnationCheck == MousePos)
            MouseStagnation++;
        else
            MouseStagnation = 0;

        globals::TopLeft = sf::Vector2f(View1.getCenter().x - HalfSize.x,
                                        View1.getCenter().y - HalfSize.y);
        globals::TopRight = sf::Vector2f(View1.getCenter().x + HalfSize.x,
                                         View1.getCenter().y - HalfSize.y);
        globals::BottomLeft = sf::Vector2f(View1.getCenter().x - HalfSize.x,
                                           View1.getCenter().y + HalfSize.y);
        globals::BottomRight = sf::Vector2f(View1.getCenter().x + HalfSize.x,
                                            View1.getCenter().y + HalfSize.y);

        cText.CreateText(CZ(globals::TopRight.x - CZ(50)),
                         CZ(globals::TopRight.y + CZ(50)), CZ(11),
                         sf::Color::White, "x", "", CameraZoom);
        cText.CreateText((globals::TopRight.x - 50), (globals::TopRight.y + 50),
                         (11) / CameraZoom, sf::Color::White, "x", "",
                         CameraZoom);

        if (Key.kTime == 1)
        { // Generates a random name from GenerateName(); and puts it into the console.
            std::cout << GenerateName() << std::endl;
        }

        if (Key.iTime == 10)
        { // Attempt to fix item images that were vanishing.
            Con("Doing");
            itemmanager.GlobalItem.clear();
            itemmanager.InitializeItems();

            RefreshImages();
        }

        if (Key.gTime == 1)
        { // Fling all critters south.
            for (size_t i = 0; i != npclist.size(); i++)
            {
                npclist[i].Momentum = sf::Vector2f(0, 100);
            }
        }
        if (Key.hTime == 1)
        { // Fling all critters north.
            for (size_t i = 0; i != npclist.size(); i++)
            {
                npclist[i].Momentum = sf::Vector2f(0, -100);
            }
        }

        // Game Mode Loops ================================================================================

        if (Key.lTime == 1)
        { // Print current clipboard contents to the console.
            GetClipboardText();
        }

        if (Key.r)
        { // Debug (de)activation
            if (!Debug)
            {
                Debug = true;
                fSleep(0.2);
            }
            else if (Debug)
            {
                Debug = false;
                fSleep(0.2);
            }
        }
        if (GC.Phase == "Local")
        { //=======================================================*Local*============================================================================
            if (Debug)
                cText.CreateText((currentx - 2) * GridSize,
                                 (currenty + 1) * GridSize, 11, sf::Color::Red,
                                 "Debug On");
            if (Key.lctrl && Key.LMB)
            {
                Boom(MousePos.x, MousePos.y, 10, 50);
            }

            for (auto &worlditem : worlditems)
            {
                if (math::Closeish(MousePos.x, MousePos.y, (worlditem).xpos,
                                   (worlditem).ypos) <= 10)
                {
                    std::cout << "Found; " << (worlditem).name << std::endl;
                    //fSleep(2);
                }
            }

            if (Key.vTime == 1)
                ChatBox.AddChat(
                    RandomWindowName(),
                    sf::Color(randz(0, 255), randz(0, 255), randz(0, 255)));

            SquadHud();

            if (Key.lshiftTime > 0)
            {
                //int x = globals::TopLeft.x;
                //int y = globals::TopLeft.y+Rez.y;
                int x = globals::BottomLeft.x;
                int y = globals::BottomLeft.y;
                Effectz.CreateSquare(x + 20, y - 20, x + 40, y - 40,
                                     sf::Color(0, 100, 255));
                cText.CreateText(x + 20, y - 30, 11, sf::Color::White, "Build");
            }
            if (Key.RMBTime == 1 && Key.lshift)
                RMBMenuTile(MousePos);

            if (Key.l == true)
            {
                for (auto &elem : npclist)
                {
                    elem.PrintBloodContent();
                }
            } // Zombification

            if (Key.lshift && Key.tab) // Debug NPC Spawn Menu
            {

                GC.MenuType = "DebugCritterSpawn";
                MenuPopUp();
            }
            else if (Key.tab == true &&
                     Key.lshift ==
                         false) // Debug Item Spawn Menu TODO: Sort them by Type.
            {
                GC.MenuType = "DebugItemSpawn";
                MenuPopUp();
            }
            else if (GC.MenuType == "DebugItemSpawn" ||
                     GC.MenuType == "DebugCritterSpawn")
            {
                GC.MenuType = "NULL";
                GC.MenuPos = sf::Vector2f(-10000, -10000);
                GC.MenuEndPos = sf::Vector2f(-10000, -10000);
                MenuPtrCon.pItem = nullptr;
                MenuPtrCon.pNPC = nullptr;
                MenuPtrCon.pTile = nullptr;
                Con("Closing DebugMenus Due To Lack of Key Presses");
            }
            //                       if(Key.lshift && Key.z){GC.ZombieSwarmLocal();}

            if (Key.lshift && Key.z) // Debug Stuffs Menu
            {
                GC.MenuType = "DebugFunctions";
                MenuPopUp();
            }

            if (Key.left == true)
            {
                currentx--;
                PlyAct = true;
            }
            if (Key.right == true)
            {
                currentx++;
                PlyAct = true;
            }
            if (Key.up == true)
            {
                currenty--;
                PlyAct = true;
            }
            if (Key.down == true)
            {
                currenty++;
                PlyAct = true;
            }

            if (globals::InitalZeds)
                GC.Wave();

            RightMouseButtonContextMenu();

            if (Key.bTime == 1)
            {
                NPC Critter;
                Critter = *GetGlobalCritter("Azabul");
                Critter.xpos = MousePos.x;
                Critter.ypos = MousePos.y;

                Critter.Body.BodyParts =
                    "{[Name:UpperTorso][BloodPumpRate:100][AirCapacity:200]["
                    "AirAbsorbtion:100][ObjectCapacity:1][NutritionExtraction:"
                    "25][MassFlesh:15:1000]}";
                Critter.Body.BodyParts.append("\n{[Name:Head][Mind:true]["
                                              "Orafice:1][MassFlesh:5:1000]["
                                              "Dependant:UpperTorso]}");
                Critter.Body.BodyParts.append(
                    "\n{[Name:LowerTorso][ObjectCapacity:10][DigestionRate:125]"
                    "[NutritionExtraction:50][PoisonFilter:Zombification:10]["
                    "DigestsBlood:100][MassFlesh:15:1000][Dependant:UpperTorso]"
                    "}");

                Critter.Body.BodyParts.append("\n{[Name:Left "
                                              "Leg][Walk:8][MassFlesh:15:1000]["
                                              "Dependant:LowerTorso]}");
                Critter.Body.BodyParts.append("\n{[Name:Right "
                                              "Leg][Walk:8][MassFlesh:15:1000]["
                                              "Dependant:LowerTorso]}");

                Critter.Body.BodyParts.append("\n{[Name:Left "
                                              "Arm][Grasp:2][MassFlesh:10:1000]"
                                              "[Dependant:UpperTorso]}");
                Critter.Body.BodyParts.append("\n{[Name:Right "
                                              "Arm][Grasp:2][MassFlesh:10:1000]"
                                              "[Dependant:UpperTorso]}");

                npclist.push_back(Critter);
            }

            for (size_t i = 0; i != npclist.size(); i++)
            {
                if (npclist[i].name == "Azabul")
                {
                    cText.CreateText(
                        (npclist[i].xpos) -
                            ((npclist[i].Body.BodyParts.size() / 2) * 2),
                        npclist[i].ypos + 10, 11, sf::Color::White,
                        npclist[i].Body.BodyParts);
                }
            }

            if (Key.lshift == true && Key.left == true)
            {
                currentx--;
                currentx--;
                currentx--;
                currentx--;
                PlyAct = true;
            } //Sprite.Move(-100 * ElapsedTime, 0);
            if (Key.lshift == true && Key.right == true)
            {
                currentx++;
                currentx++;
                currentx++;
                currentx++;
                PlyAct = true;
            } //Sprite.Move( 100 * ElapsedTime, 0);
            if (Key.lshift == true && Key.up == true)
            {
                currenty--;
                currenty--;
                currenty--;
                currenty--;
                PlyAct = true;
            } //Sprite.Move(0, -100 * ElapsedTime);
            if (Key.lshift == true && Key.down == true)
            {
                currenty++;
                currenty++;
                currenty++;
                currenty++;
                PlyAct = true;
            } //Sprite.Move(0,  100 * ElapsedTime);
            if (Key.comma == true && Key.lshift == true &&
                currentz <= gridz - 1)
            {
                currentz++;
                PlyAct = true;
                fSleep(0.1f);
            } //Sprite.Move(0, -100 * ElapsedTime);
            if (Key.period == true && Key.lshift == true && currentz >= 1)
            {
                currentz--;
                PlyAct = true;
                fSleep(0.1f);
            } //Sprite.Move(0,  100 * ElapsedTime);
            if (Key.comma == true && Key.rshift == true &&
                currentz <= gridz - 1)
            {
                currentz++;
                PlyAct = true;
                fSleep(0.1f);
            } //Sprite.Move(0, -100 * ElapsedTime);
            if (Key.period == true && Key.rshift == true && currentz >= 1)
            {
                currentz--;
                PlyAct = true;
                fSleep(0.1f);
            } //Sprite.Move(0,  100 * ElapsedTime);
            if (MyTarget == -1)
            {
                globals::Following = false;
            }
            else if (Key.c)
            {
                globals::Following = true;
            }
            if (globals::Following)
            {
                View1.setCenter(npclist.at(MyTarget).xpos,
                                npclist.at(MyTarget).ypos);
            }

            if (Key.n)
                GenerateChunk("Road", 500, sf::Vector2i(50, 50));
            if (Key.m)
                GenerateChunk("SouthernHouse", 500, sf::Vector2i(50, 50));

            if (Key.qTime > 10 && MyTarget == -1 && !Key.lshift)
            {
            }

            if (Key.c)
            {
                for (int x = 0; x != ChunkSize; x++)
                {
                    for (int y = 0; y != ChunkSize; y++)
                    {
                        Tiles[x][y][30] = vChunk[x][y][0];
                    }
                }
                //GenerateWorld();
            }

            Effectz.CreateSquare(
                32 * 20, 32 * 20, 64 * 20, 64 * 20, sf::Color(0, 0, 0, 0), 1,
                sf::Color::
                    Cyan); // These two are the chunk borders, Debug stuffs so you can see where the loading boundries are. (As if you needed 'em, Damn thing lags a bit, Should thread each loading bit soon.)
            Effectz.CreateSquare(
                0 * 20, 0 * 20, 32 * 20, 32 * 20, sf::Color(0, 0, 0, 0), 1,
                sf::Color::
                    Red); // This one reachs from 0 to 32 only because it draws from the left, not the center.

            /*

            int Degrees = GX;
            int Dist = 200;
            if(Key.w) Degrees = 0;
            if(Key.a) Degrees = 270;
            if(Key.s) Degrees = 180;
            if(Key.d) Degrees = 90;
            float Xxx = MousePos.x + cosf(Degrees * PI/180) * Dist;
            float Yyy = MousePos.y + sinf(Degrees * PI/180) * Dist;
            Effectz.CreateCircle(MousePos.x,MousePos.y,5,Red);
            Effectz.CreateCircle(Xxx,Yyy,5,White);

            */

            int Length = 200;

            for (int Rot = 0; Rot != 360; Rot++)
            {
                /*int Rot = GX;
                int XPos = ((abs(MousePos.x / 20) * 20) + 10 +
                            cosf(Rot * PI / 180) * Length) /
                           20;
                int YPos = ((abs(MousePos.y / 20) * 20) + 10 +
                            sinf(Rot * PI / 180) * Length) /
                           20;
                XPos *= 20;
                YPos *= 20;

                //Effectz.CreateCircle(MousePos.x,MousePos.y,5,Red);
                //Effectz.CreateCircle(XPos,YPos,5,White);*/
            }
            if (Key.p && true == false)
                for (int Rot = 0; Rot != 360; Rot++)
                {
                    //int Rot = GX;
                    int XPos = ((abs(MousePos.x / 20) * 20) + 10 +
                                cosf(Rot * PI / 180) * Length) /
                               20;
                    int YPos = ((abs(MousePos.y / 20) * 20) + 10 +
                                sinf(Rot * PI / 180) * Length) /
                               20;
                    //XPos *= 20;
                    //YPos *= 20;

                    //Effectz.CreateCircle(MousePos.x,MousePos.y,5,Red);
                    //Effectz.CreateCircle(XPos,YPos,5,White);
                    Tiles[XPos][YPos][30].Stone();
                }

            if (Key.pTime == 1)
            {

                for (int ItLength = 0; ItLength != 16; ItLength++)
                {

                    if (ItLength != 15)
                    {

                        for (int Rot = 1; Rot != 361; Rot++)
                        {

                            int XPos = abs(MousePos.x / 20) +
                                       sin(Rot * PI / 180) * ItLength;
                            int YPos = abs(MousePos.y / 20) +
                                       cos(Rot * PI / 180) * ItLength;

                            Tiles[XPos][YPos][30].Stone();
                        }
                    }
                    else
                    {
                        for (int Rot = 1; Rot != 361; Rot++)
                        {

                            int XPos = abs(MousePos.x / 20) +
                                       sin(Rot * PI / 180) * ItLength;
                            int YPos = abs(MousePos.y / 20) +
                                       cos(Rot * PI / 180) * ItLength;

                            Tiles[XPos][YPos][30].Wall();
                        }
                    }
                }
            }

            //float XX = cos(GX * PI) / 180;
            //float YY = sin(GY * PI) / 180;
            //Effectz.CreateCircle(XX,YY,11,sf::Color::White);

            if (Key.lctrlTime > 10)
            {
                int Variable = Tiles[abs_to_index(
                    MousePos.x / 20)][abs_to_index(MousePos.y / 20)][30].ID;
                cText.CreateText(MousePos.x, MousePos.y, 11, sf::Color::Red, "",
                                 "", Variable);
            }

            bool Transitioning = false;
            if (currenty > 64)
            {
                TilesGoUp();
                currenty = 33;

                for (size_t i = 0; i != npclist.size(); i++)
                {
                    npclist.at(i).ypos += -640;
                    npclist.at(i).TargetPos.y += -640;
                }

                //for(int i = 0; i != worlditems.size(); i++) worlditems.at(i).ypos += -640;
                for (auto &worlditem : worlditems)
                    (worlditem).ypos += -640;

                for (size_t i = 0; i != UniFact[0].JobList.size(); i++)
                    UniFact[0].JobList[i].WorkPos.y += -640;

                Transitioning = true;
                std::string Line;
                Line = LoadCritters(
                    sf::Vector2i(currentregionx - 1, currentregiony + 1),
                    "BottomLeft", currentplanet);
                remove(Line.c_str());
                Line = LoadCritters(
                    sf::Vector2i(currentregionx, currentregiony + 1), "Bottom",
                    currentplanet);
                remove(Line.c_str());
                Line = LoadCritters(
                    sf::Vector2i(currentregionx + 1, currentregiony + 1),
                    "BottomRight", currentplanet);
                remove(Line.c_str());
                Line = LoadItems(
                    sf::Vector2i(currentregionx - 1, currentregiony + 1),
                    "BottomLeft", currentplanet);
                remove(Line.c_str());
                Line =
                    LoadItems(sf::Vector2i(currentregionx, currentregiony + 1),
                              "Bottom", currentplanet);
                remove(Line.c_str());
                Line = LoadItems(
                    sf::Vector2i(currentregionx + 1, currentregiony + 1),
                    "BottomRight", currentplanet);
                remove(Line.c_str());
            }
            if (currenty < 32)
            {
                TilesGoDown();
                currenty = 63;
                for (size_t i = 0; i != npclist.size(); i++)
                {
                    npclist.at(i).ypos += 640;
                    npclist.at(i).TargetPos.y += 640;
                }

                //for(int i = 0; i != worlditems.size(); i++) worlditems.at(i).ypos += 640;
                for (auto &worlditem : worlditems)
                    (worlditem).ypos += 640;

                for (size_t i = 0; i != UniFact[0].JobList.size(); i++)
                    UniFact[0].JobList[i].WorkPos.y += 640;

                Transitioning = true;
                std::string Line;
                Line = LoadCritters(
                    sf::Vector2i(currentregionx - 1, currentregiony - 1),
                    "TopLeft", currentplanet);
                remove(Line.c_str());
                Line = LoadCritters(
                    sf::Vector2i(currentregionx, currentregiony - 1), "Top",
                    currentplanet);
                remove(Line.c_str());
                Line = LoadCritters(
                    sf::Vector2i(currentregionx + 1, currentregiony - 1),
                    "TopRight", currentplanet);
                remove(Line.c_str());
                Line = LoadItems(
                    sf::Vector2i(currentregionx - 1, currentregiony - 1),
                    "TopLeft", currentplanet);
                remove(Line.c_str());
                Line =
                    LoadItems(sf::Vector2i(currentregionx, currentregiony - 1),
                              "Top", currentplanet);
                remove(Line.c_str());
                Line = LoadItems(
                    sf::Vector2i(currentregionx + 1, currentregiony - 1),
                    "TopRight", currentplanet);
                remove(Line.c_str());
            }
            if (currentx > 64)
            {
                TilesGoLeft();
                currentx = 33;
                for (size_t i = 0; i != npclist.size(); i++)
                {
                    npclist.at(i).xpos += -640;
                    npclist.at(i).TargetPos.x += -640;
                }

                //for(int i = 0; i != worlditems.size(); i++) worlditems.at(i).xpos += -640;
                for (auto &worlditem : worlditems)
                    (worlditem).xpos += -640;

                for (size_t i = 0; i != UniFact[0].JobList.size(); i++)
                    UniFact[0].JobList[i].WorkPos.x += -640;

                Transitioning = true;
                std::string Line;
                Line = LoadCritters(
                    sf::Vector2i(currentregionx + 1, currentregiony - 1),
                    "TopRight", currentplanet);
                remove(Line.c_str());
                Line = LoadCritters(
                    sf::Vector2i(currentregionx + 1, currentregiony), "Right",
                    currentplanet);
                remove(Line.c_str());
                Line = LoadCritters(
                    sf::Vector2i(currentregionx + 1, currentregiony + 1),
                    "BottomRight", currentplanet);
                remove(Line.c_str());
                Line = LoadItems(
                    sf::Vector2i(currentregionx + 1, currentregiony - 1),
                    "TopRight", currentplanet);
                remove(Line.c_str());
                Line =
                    LoadItems(sf::Vector2i(currentregionx + 1, currentregiony),
                              "Right", currentplanet);
                remove(Line.c_str());
                Line = LoadItems(
                    sf::Vector2i(currentregionx + 1, currentregiony + 1),
                    "BottomRight", currentplanet);
                remove(Line.c_str());
            }
            if (currentx < 32)
            {
                Con("Starting GoRight");
                TilesGoRight();
                Con("Ending GoRight");
                currentx = 63;
                Con("Starting GoRight with NPC's and Items");
                for (size_t i = 0; i != npclist.size(); i++)
                {
                    npclist.at(i).xpos += 640;
                    npclist.at(i).TargetPos.x += 640;
                }

                //for(int i = 0; i != worlditems.size(); i++) worlditems.at(i).xpos += 640;
                for (auto &worlditem : worlditems)
                    (worlditem).xpos += 640;

                for (size_t i = 0; i != UniFact[0].JobList.size(); i++)
                    UniFact[0].JobList[i].WorkPos.x += 640;

                Con("Done GoRight with NPC's and Items");
                Transitioning = true;
                std::string Line;
                Line = LoadCritters(
                    sf::Vector2i(currentregionx - 1, currentregiony - 1),
                    "TopLeft", currentplanet);
                remove(Line.c_str());
                Line = LoadCritters(
                    sf::Vector2i(currentregionx - 1, currentregiony), "Left",
                    currentplanet);
                remove(Line.c_str());
                Line = LoadCritters(
                    sf::Vector2i(currentregionx - 1, currentregiony + 1),
                    "BottomLeft", currentplanet);
                remove(Line.c_str());
                Line = LoadItems(
                    sf::Vector2i(currentregionx - 1, currentregiony - 1),
                    "TopLeft", currentplanet);
                remove(Line.c_str());
                Line =
                    LoadItems(sf::Vector2i(currentregionx - 1, currentregiony),
                              "Left", currentplanet);
                remove(Line.c_str());
                Line = LoadItems(
                    sf::Vector2i(currentregionx - 1, currentregiony + 1),
                    "BottomLeft", currentplanet);
                remove(Line.c_str());
            }

            if (Transitioning == true)
            {
                for (size_t i = 0; i != npclist.size(); i++)
                {

                    if (npclist.at(i).xpos > 1920 && npclist.at(i).ypos < 640)
                    {
                        npclist.at(i).xpos =
                            npclist.at(i).xpos - 640 - 640 - 640;
                        SaveNPC(500, sf::Vector2i(currentregionx + 2,
                                                  currentregiony - 1),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }
                    else if (npclist.at(i).xpos > 1920 &&
                             npclist.at(i).ypos > 1280)
                    {
                        npclist.at(i).xpos =
                            npclist.at(i).xpos - 640 - 640 - 640;
                        npclist.at(i).ypos = npclist.at(i).ypos - 640 - 640;
                        SaveNPC(500, sf::Vector2i(currentregionx + 2,
                                                  currentregiony + 1),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }

                    else if (npclist.at(i).xpos < 0 &&
                             npclist.at(i).ypos > 1280)
                    {
                        npclist.at(i).xpos = npclist.at(i).xpos + 640;
                        npclist.at(i).ypos = npclist.at(i).ypos - 640 - 640;
                        SaveNPC(500, sf::Vector2i(currentregionx - 2,
                                                  currentregiony + 1),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }
                    else if (npclist.at(i).xpos < 0 && npclist.at(i).ypos < 640)
                    {
                        npclist.at(i).xpos = npclist.at(i).xpos + 640;
                        SaveNPC(500, sf::Vector2i(currentregionx - 2,
                                                  currentregiony - 1),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }

                    else if (npclist.at(i).ypos < 0 &&
                             npclist.at(i).xpos > 1280)
                    {
                        npclist.at(i).xpos = npclist.at(i).xpos - 640 - 640;
                        npclist.at(i).ypos = npclist.at(i).ypos + 640;
                        SaveNPC(500, sf::Vector2i(currentregionx + 1,
                                                  currentregiony - 2),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }
                    else if (npclist.at(i).ypos < 0 && npclist.at(i).xpos < 640)
                    {
                        npclist.at(i).ypos = npclist.at(i).ypos + 640;
                        SaveNPC(500, sf::Vector2i(currentregionx - 1,
                                                  currentregiony - 2),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }

                    else if (npclist.at(i).ypos > 1920 &&
                             npclist.at(i).xpos > 1280)
                    {
                        npclist.at(i).xpos = npclist.at(i).xpos - 640 - 640;
                        npclist.at(i).ypos =
                            npclist.at(i).ypos - 640 - 640 - 640;
                        SaveNPC(500, sf::Vector2i(currentregionx + 1,
                                                  currentregiony + 2),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }
                    else if (npclist.at(i).ypos > 1920 &&
                             npclist.at(i).xpos < 640)
                    {
                        npclist.at(i).ypos =
                            npclist.at(i).ypos - 640 - 640 - 640;
                        SaveNPC(500, sf::Vector2i(currentregionx - 1,
                                                  currentregiony + 2),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }

                    //HAGGINABAGGINA  Some reason, When a critter is saved, It'll have more than 640 for it's position, This is unacceptable.

                    else if (npclist.at(i).xpos > 1920)
                    {
                        npclist.at(i).xpos =
                            npclist.at(i).xpos - 640 - 640 - 640;
                        npclist.at(i).ypos = npclist.at(i).ypos - 640;
                        SaveNPC(500, sf::Vector2i(currentregionx + 2,
                                                  currentregiony),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }
                    else if (npclist.at(i).ypos > 1920)
                    {
                        npclist.at(i).xpos = npclist.at(i).xpos - 640;
                        npclist.at(i).ypos =
                            npclist.at(i).ypos - 640 - 640 - 640;
                        SaveNPC(500, sf::Vector2i(currentregionx,
                                                  currentregiony + 2),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }
                    else if (npclist.at(i).xpos < 0)
                    {
                        npclist.at(i).xpos = npclist.at(i).xpos + 640;
                        npclist.at(i).ypos = npclist.at(i).ypos - 640;
                        SaveNPC(500, sf::Vector2i(currentregionx - 2,
                                                  currentregiony),
                                npclist.at(i));
                        npclist.at(i).ToDelete = true;
                    }
                    else if (npclist.at(i).ypos < 0)
                    {
                        npclist.at(i).xpos = npclist.at(i).xpos - 640;
                        npclist.at(i).ypos = npclist.at(i).ypos + 640;
                        SaveNPC(500, sf::Vector2i(currentregionx,
                                                  currentregiony - 2),
                                npclist.at(i));

                        npclist.at(i).ToDelete = true;
                    }
                }

                RemoveNPCs();
                /*
                for(int i = 0; i != worlditems.size(); i++)
                {
                   if(worlditems.at(i).xpos > 1920)
                    {
                        SaveItem(500,sf::Vector2i(currentregionx+2,currentregiony),worlditems.at(i));
                        worlditems.at(i).ToDelete = true;
                    }
                    if(worlditems.at(i).ypos > 1920)
                    {
                        SaveItem(500,sf::Vector2i(currentregionx,currentregiony+2),worlditems.at(i));
                        worlditems.at(i).ToDelete = true;
                    }
                    if(worlditems.at(i).xpos < 0)
                    {
                        SaveItem(500,sf::Vector2i(currentregionx-2,currentregiony),worlditems.at(i));
                        worlditems.at(i).ToDelete = true;
                    }
                    if(worlditems.at(i).ypos < 0)
                    {
                        SaveItem(500,sf::Vector2i(currentregionx,currentregiony-2),worlditems.at(i));

                        worlditems.at(i).ToDelete = true;
                    }
                }
                */

                //for(int i = 0; i != worlditems.size(); i++)
                for (auto &worlditem : worlditems)
                {

                    if ((worlditem).xpos > 1920 && (worlditem).ypos < 640)
                    {
                        (worlditem).xpos = (worlditem).xpos - 640 - 640 - 640;
                        SaveItem(500, sf::Vector2i(currentregionx + 2,
                                                   currentregiony - 1),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }
                    else if ((worlditem).xpos > 1920 && (worlditem).ypos > 1280)
                    {
                        (worlditem).xpos = (worlditem).xpos - 640 - 640 - 640;
                        (worlditem).ypos = (worlditem).ypos - 640 - 640;
                        SaveItem(500, sf::Vector2i(currentregionx + 2,
                                                   currentregiony + 1),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }

                    else if ((worlditem).xpos < 0 && (worlditem).ypos > 1280)
                    {
                        (worlditem).xpos = (worlditem).xpos + 640;
                        (worlditem).ypos = (worlditem).ypos - 640 - 640;
                        SaveItem(500, sf::Vector2i(currentregionx - 2,
                                                   currentregiony + 1),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }
                    else if ((worlditem).xpos < 0 && (worlditem).ypos < 640)
                    {
                        (worlditem).xpos = (worlditem).xpos + 640;
                        SaveItem(500, sf::Vector2i(currentregionx - 2,
                                                   currentregiony - 1),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }

                    else if ((worlditem).ypos < 0 && (worlditem).xpos > 1280)
                    {
                        (worlditem).xpos = (worlditem).xpos - 640 - 640;
                        (worlditem).ypos = (worlditem).ypos + 640;
                        SaveItem(500, sf::Vector2i(currentregionx + 1,
                                                   currentregiony - 2),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }
                    else if ((worlditem).ypos < 0 && (worlditem).xpos < 640)
                    {
                        (worlditem).ypos = (worlditem).ypos + 640;
                        SaveItem(500, sf::Vector2i(currentregionx - 1,
                                                   currentregiony - 2),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }

                    else if ((worlditem).ypos > 1920 && (worlditem).xpos > 1280)
                    {
                        (worlditem).xpos = (worlditem).xpos - 640 - 640;
                        (worlditem).ypos = (worlditem).ypos - 640 - 640 - 640;
                        SaveItem(500, sf::Vector2i(currentregionx + 1,
                                                   currentregiony + 2),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }
                    else if ((worlditem).ypos > 1920 && (worlditem).xpos < 640)
                    {
                        (worlditem).ypos = (worlditem).ypos - 640 - 640 - 640;
                        SaveItem(500, sf::Vector2i(currentregionx - 1,
                                                   currentregiony + 2),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }

                    //HAGGINABAGGINA  Some reason, When a critter is saved, It'll have more than 640 for it's position, This is unacceptable.

                    else if ((worlditem).xpos > 1920)
                    {
                        (worlditem).xpos = (worlditem).xpos - 640 - 640 - 640;
                        (worlditem).ypos = (worlditem).ypos - 640;
                        SaveItem(500, sf::Vector2i(currentregionx + 2,
                                                   currentregiony),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }
                    else if ((worlditem).ypos > 1920)
                    {
                        (worlditem).xpos = (worlditem).xpos - 640;
                        (worlditem).ypos = (worlditem).ypos - 640 - 640 - 640;
                        SaveItem(500, sf::Vector2i(currentregionx,
                                                   currentregiony + 2),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }
                    else if ((worlditem).xpos < 0)
                    {
                        (worlditem).xpos = (worlditem).xpos + 640;
                        (worlditem).ypos = (worlditem).ypos - 640;
                        SaveItem(500, sf::Vector2i(currentregionx - 2,
                                                   currentregiony),
                                 (worlditem));
                        (worlditem).ToDelete = true;
                    }
                    else if ((worlditem).ypos < 0)
                    {
                        (worlditem).xpos = (worlditem).xpos - 640;
                        (worlditem).ypos = (worlditem).ypos + 640;
                        SaveItem(500, sf::Vector2i(currentregionx,
                                                   currentregiony - 2),
                                 (worlditem));

                        (worlditem).ToDelete = true;
                    }
                }
                UnpointItems(worlditems);
                RemoveItems();
            }

            //if(Key.b) Effectz.CreateBeam(1000,1000,MousePos.x,MousePos.y,50,Red,5,Blue,false,0);

            button var;
            var.Color = sf::Color::Red;
            var.iSize = 5;
            var.vPos = sf::Vector2f(600, 600);
            var.sButtonText = "Howdy";
            vButtonList.push_back(var);
            if (ButtonClicked(var.id))
            {
                std::cout << "Twas' True \n";
            }

            if (MyTarget != -1 && npclist[MyTarget].health <= 0 && Key.lshift &&
                Key.q)
            {
                npclist[MyTarget].ToDelete = true;
                MyTarget = -1;
                MyTargetid = -1;
            }

            if (MyTarget != -1 && Key.RMB &&
                Tiles[abs_to_index(MousePos.x / GridSize)][abs_to_index(
                    MousePos.y / GridSize)][30].ID != 1010)
            { // Giving Orders
                npclist.at(MyTarget).TargetPos = MousePos;
                npclist.at(MyTarget).action = "Orders";
                if (math::Closeish(npclist.at(MyTarget).xpos,
                                   npclist.at(MyTarget).ypos, MousePos.x,
                                   MousePos.y) <= 10)
                {
                    npclist.at(MyTarget).action = "Act";
                    npclist.at(MyTarget).NeedsPath = false;
                }

                for (auto const &item : worlditems)
                {
                    if (math::Closeish(MousePos.x, MousePos.y, item.xpos,
                                       item.ypos) <= 10)
                    {
                        GC.MenuType = "CritterContext";
                    }
                }
            } //End of Giving Orders

            //for (int i = 0; i <= gridy-1; i++){for( int t = 0; t <= gridx-1; t++){int z = currentz;if(globals::sunmap[z][i][t] != 0){globals::sunmap[z][i][t] -= 5;}}} // Darkness
            if (GC.MenuType != "NULL")
            {
                MenuPopUp();
            }
            else
            {
                GC.MenuPos = math::Vec2f(-10000, -10000);
            }
        } //=============================================================================*End of Local*========================================================================
        if (GC.Phase == "Solar")
        { //=======================================================*Solar*============================================================================
            if (Key.lshift)
            {
                /*std::vector<planet>::iterator Me;
                for(Me = Planets.begin(); Me != Planets.end(); ++Me )
                {
                    std::cout<<"Position: X:"<<Me->Pos.x<<" Y:"<<Me->Pos.y<<std::endl;
                    std::cout<<"Speed: X:"<<Me->speed.x<<" Y:"<<Me->speed.y<<std::endl;
                }*/
            }

            // TODO: Fix Later if(Key.LMB && Key.lctrl){ GC.CreatePlanet(MousePos.x,MousePos.y,randz(10,200));sf::Sleep(0.1);}
            // TODO: Fix Later            if(Key.RMB && Key.lctrl){ Planets.pop_back(); sf::Sleep(1); }
            // TODO: Fix Laterif(Key.r && Key.lctrl){ Planets.clear(); sf::Sleep(1); }
            // TODO: Fix Laterif(Key.q){aim--; std::cout << aim << std::endl; sf::Sleep(0.2);}
            // TODO: Fix Laterif(Key.e){aim++; std::cout << aim << std::endl; sf::Sleep(0.2);}

            if (Key.left == true)
            {
                currentx--;
                PlyAct = true;
            }
            if (Key.right == true)
            {
                currentx++;
                PlyAct = true;
            }
            if (Key.up == true)
            {
                currenty--;
                PlyAct = true;
            }
            if (Key.down == true)
            {
                currenty++;
                PlyAct = true;
            }
            if (Key.lshift == true && Key.left == true)
            {
                currentx--;
                currentx--;
                currentx--;
                currentx--;
                PlyAct = true;
            } //Sprite.Move(-100 * ElapsedTime, 0);
            if (Key.lshift == true && Key.right == true)
            {
                currentx++;
                currentx++;
                currentx++;
                currentx++;
                PlyAct = true;
            } //Sprite.Move( 100 * ElapsedTime, 0);
            if (Key.lshift == true && Key.up == true)
            {
                currenty--;
                currenty--;
                currenty--;
                currenty--;
                PlyAct = true;
            } //Sprite.Move(0, -100 * ElapsedTime);
            if (Key.lshift == true && Key.down == true)
            {
                currenty++;
                currenty++;
                currenty++;
                currenty++;
                PlyAct = true;
            } //Sprite.Move(0,  100 * ElapsedTime);

        } //=============================================================================*End of Solar*========================================================================
        if (GC.Phase == "Test")
        {
            if (Key.up)
                testage++;
            if (Key.down)
                testage--;
            if (Key.right)
                testage2++;
            if (Key.left)
                testage2--;
            if (Key.pad2)
                radius++;
            if (Key.pad8)
                radius--;

            if (currenty >
                64) // TODO: Make the auto removing tiles use the current windows border to get it's range, Allowing proper resizing and stuffs. Edit: Herp, That's not what this is.
            {
                TilesGoUp();
                currenty = 33;
            }
            if (currenty < 32)
            {
                TilesGoDown();
                currenty = 63;
            }
            if (currentx > 64)
            {
                TilesGoLeft();
                currentx = 33;
            }
            if (currentx < 32)
            {
                TilesGoRight();
                currentx = 63;
            }

            if (Key.g)
                InitalizeWorldTiles();
            //DrawNewTiles();

            DrawWorldTiles();
            if (Key.j)
                TilesGoUp();
            if (Key.k)
                TilesRandom();

            if (Key.m)
            {
                std::set<int> Setage;
                Setage.insert(1);
                Setage.insert(1);
                Setage.insert(4);
                Setage.insert(2);
                Setage.insert(1);
                Setage.insert(3);
                for (const auto &elem : Setage)
                {
                    std::cout << elem << "\n";
                }
            }

            if (Key.left == true)
            {
                currentx--;
                PlyAct = true;
            }
            if (Key.right == true)
            {
                currentx++;
                PlyAct = true;
            }
            if (Key.up == true)
            {
                currenty--;
                PlyAct = true;
            }
            if (Key.down == true)
            {
                currenty++;
                PlyAct = true;
            }
            if (Key.lshift == true && Key.left == true)
            {
                currentx--;
                currentx--;
                currentx--;
                currentx--;
                PlyAct = true;
            } //Sprite.Move(-100 * ElapsedTime, 0);
            if (Key.lshift == true && Key.right == true)
            {
                currentx++;
                currentx++;
                currentx++;
                currentx++;
                PlyAct = true;
            } //Sprite.Move( 100 * ElapsedTime, 0);
            if (Key.lshift == true && Key.up == true)
            {
                currenty--;
                currenty--;
                currenty--;
                currenty--;
                PlyAct = true;
            } //Sprite.Move(0, -100 * ElapsedTime);
            if (Key.lshift == true && Key.down == true)
            {
                currenty++;
                currenty++;
                currenty++;
                currenty++;
                PlyAct = true;
            } //Sprite.Move(0,  100 * ElapsedTime);

            //button var; var.Color = sf::Color::Red; var.iSize = 5; var.vPos = sf::Vector2f(200,200); var.sButtonText = "Howdy"; vButtonList.push_back(var);
            // TODO: Fix Later    if(ButtonClicked(var.id)){ std::cout << "Starting the Building. \n"; GC.BuildTileTest(); std::cout << "Done Building. \n"; sf::Sleep(1); GC.bTest = true; }

            Degrees += speeds; // Should probably leave speed at 1, If not less.

            Xxx = xanchor + cosf(Degrees * PI / 180) * radius;
            Yyy = yanchor + sinf(Degrees * PI / 180) * radius;
            Effectz.CreateCircle(xanchor, yanchor, 5, sf::Color::Blue);
            Effectz.CreateCircle(Xxx, Yyy, 5, sf::Color::White);

            int distence = math::Closeish(xanchor, yanchor, Xxx, Yyy);
            cText.CreateText(xanchor, yanchor, 11, sf::Color::White,
                             "Distence:", "", distence);
            cText.CreateText(xanchor, yanchor + 11, 11, sf::Color::White,
                             "Radius:", "", radius);

            //std::cout << testmonkey << std::endl;
            if (GC.bTest == true)
            {
                // TODO: Fix Later
                /*for(T = Tiles.begin(); T != Tiles.end(); T++){

        if( T->ID == 1 ){ // dirt
        int iTS = 20;
        sf::Sprite Tile(Images);
        Tile.SetColor(sf::Color(255,255,255,255));
        int imgposx = 2;int imgposy = 1;
        Tile.SetSubRect(sf::IntRect(((imgposx-1)*20)+imgposx, ((imgposy-1)*20)+imgposy, (imgposx*20)+imgposx, (imgposy*20)+imgposy));
        Tile.SetPosition(abs(T->LocPos.x)*iTS, abs(T->LocPos.y)*iTS);
        Tile.SetColor(sf::Color(255,255,255,255));
        App.Draw(Tile);}

        if( T->ID == 3 ){ // dirt
        int iTS = 20;
        sf::Sprite Tile(Images);
        Tile.SetColor(sf::Color(255,255,255,255));
        int imgposx = 1;int imgposy = 1;
        Tile.SetSubRect(sf::IntRect(((imgposx-1)*20)+imgposx, ((imgposy-1)*20)+imgposy, (imgposx*20)+imgposx, (imgposy*20)+imgposy));
        Tile.SetPosition(abs(T->LocPos.x)*iTS, abs(T->LocPos.y)*iTS);
        Tile.SetColor(sf::Color(255,255,255,255));
        App.Draw(Tile);}

        if( T->ID == 7 ){ // dirt
        int iTS = 20;
        sf::Sprite Tile(Images);
        Tile.SetColor(sf::Color(255,255,255,255));
        int imgposx = 7;int imgposy = 1;
        Tile.SetSubRect(sf::IntRect(((imgposx-1)*20)+imgposx, ((imgposy-1)*20)+imgposy, (imgposx*20)+imgposx, (imgposy*20)+imgposy));
        Tile.SetPosition(abs(T->LocPos.x)*iTS, abs(T->LocPos.y)*iTS);
        Tile.SetColor(sf::Color(255,255,255,255));
        App.Draw(Tile);}


        }*/
            }

            if (Key.space)
            {
                float xpos = xanchor;
                float ypos = yanchor;
                for (int val = 0; val <= radius; val++)
                {
                    float Xx = 0;
                    float Yy = 0;
                    float angle =
                        180 -
                        (180 / PI) *
                            (atan2f(
                                xanchor - Xxx,
                                yanchor -
                                    Yyy)); //To be honest, I spent alot of time with trial and error to get this part to work.
                    Xx = cosf((angle - 90) * PI / 180) * 1;
                    Yy = sinf((angle - 90) * PI / 180) * 1;
                    xpos -= Xx;
                    ypos -= Yy;
                    Effectz.CreateLine(MousePos.x, MousePos.y, xpos, ypos, 1,
                                       sf::Color::White);
                }
            }
        }
        if (GC.Phase == "Goo")
        {
            // Herp
        }
        if (GC.Phase == "MakeSquad")
        {
            View1.setCenter(Rez.x / 2, Rez.y / 2);

            cText.CreateText(Rez.x / 2, 20, 20, sf::Color::Blue,
                             "Design Your Squad");
            cText.CreateText(Rez.x / 2, 50, 15, sf::Color::Blue,
                             "Squad Points: ", "", Squady.MakeSquadPoints);
            cText.CreateText(Rez.x / 2, 70, 11, sf::Color::Blue,
                             "Squad Mates: ", "", Squady.SquadMates);

            button var200;
            var200.Color = sf::Color::Red;
            var200.iSize = 5;
            var200.vPos = sf::Vector2f((Rez.x / 2) - 10, 75);
            var200.sButtonText = "Howdy";
            vButtonList.push_back(var200);
            if (ButtonClicked(var200.id))
            {
                if (Squady.Squad.size() > 1)
                {
                    int stre, perc, inte, charis, endu, dext, agil = 0;

                    stre = Squady.Squad.at(Squady.Squad.size() - 1)
                               .Skills.strength;
                    perc = Squady.Squad.at(Squady.Squad.size() - 1)
                               .Skills.perception;
                    inte = Squady.Squad.at(Squady.Squad.size() - 1)
                               .Skills.intelligence;
                    charis = Squady.Squad.at(Squady.Squad.size() - 1)
                                 .Skills.charisma;
                    endu = Squady.Squad.at(Squady.Squad.size() - 1)
                               .Skills.endurance;
                    dext = Squady.Squad.at(Squady.Squad.size() - 1)
                               .Skills.dexterity;
                    agil =
                        Squady.Squad.at(Squady.Squad.size() - 1).Skills.agility;
                    int LeftOvers = 0;
                    if (stre >= 20)
                        LeftOvers += stre - 20;
                    if (perc >= 20)
                        LeftOvers += perc - 20;
                    if (inte >= 20)
                        LeftOvers += inte - 20;
                    if (charis >= 20)
                        LeftOvers += charis - 20;
                    if (endu >= 20)
                        LeftOvers += endu - 20;
                    if (dext >= 20)
                        LeftOvers += dext - 20;
                    if (agil >= 20)
                        LeftOvers += agil - 20;

                    Squady.MakeSquadPoints += LeftOvers + 100;
                    Squady.SquadMates--;
                    if (static_cast<size_t>(Squady.Aim) ==
                        Squady.Squad.size() - 1)
                        Squady.Aim--;
                    Squady.Squad.pop_back();
                }

                //if(Squady.Aim < 0) Squady.Aim = 0;
                //if(Squady.Aim > Squady.Squad.size()-1) Squady.Aim = Squady.Squad.size()-1;
                fSleep(0.2);
            }
            button var201;
            var201.Color = sf::Color::Red;
            var201.iSize = 5;
            var201.vPos = sf::Vector2f((Rez.x / 2) + 80, 75);
            var201.sButtonText = "Howdy";
            vButtonList.push_back(var201);
            if (ButtonClicked(var201.id))
            {
                if (Squady.MakeSquadPoints > 99)
                {

                    Squady.SquadMates++;
                    //if(Squady.Aim < 0) Squady.Aim = 0;
                    //if(Squady.Aim > Squady.Squad.size()-1) Squady.Aim = Squady.Squad.size()-1;
                    NPC Squ;

                    Squ = *GetGlobalCritter("Human");
                    Squ.id = globals::globalid++;
                    Squ.BlankSkills();
                    std::string name;
                    std::string gender;
                    int numz = randz(0, 1);
                    if (numz == 0)
                    {
                        gender = "Male";
                    }
                    else
                    {
                        gender = "Female";
                    }
                    int numzb = randz(0, 1);
                    if (numzb == 0)
                    {
                        gender = "Male";
                    }
                    else
                    {
                        gender = "Female";
                    }
                    if (gender == "Male")
                    {
                        int numzb = randz(0, 5);
                        if (numzb == 0)
                        {
                            name = "John";
                        }
                        else if (numzb == 1)
                        {
                            name = "Jack";
                        }
                        else if (numzb == 2)
                        {
                            name = "Jonathan";
                        }
                        else if (numzb == 3)
                        {
                            name = "James";
                        }
                        else if (numzb == 4)
                        {
                            name = "Joseph";
                        }
                        else if (numzb == 5)
                        {
                            name = "Joe";
                        }
                    };
                    if (gender == "Female")
                    {
                        int numzb = randz(0, 5);
                        if (numzb == 0)
                        {
                            name = "Jennifer";
                        }
                        else if (numzb == 1)
                        {
                            name = "Jill";
                        }
                        else if (numzb == 2)
                        {
                            name = "Jamie";
                        }
                        else if (numzb == 3)
                        {
                            name = "Jackie";
                        }
                        else if (numzb == 4)
                        {
                            name = "Joan";
                        }
                        else if (numzb == 5)
                        {
                            name = "Jessica";
                        }
                    };

                    Squ.name = name;
                    Squ.gender = gender;
                    Squ.Faction = PF.Name;
                    Squady.Squad.push_back(Squ);
                    Squady.MakeSquadPoints -= 100;
                }

                fSleep(0.2);
            }

            //Squady.Squad.at(Squady.Aim).

            int Spacing = 1;
            //NPC.Skills.

            button var100;
            var100.Color = sf::Color::Red;
            var100.iSize = 5;
            var100.vPos =
                sf::Vector2f((Rez.x / 2) - 10, 5 + (80 + (20 * Spacing)));
            var100.sButtonText = "Howdy";
            vButtonList.push_back(var100);
            if (ButtonClicked(var100.id))
            {
                Squady.Aim--;
                if (Squady.Aim < 0)
                    Squady.Aim = 0;
                if (static_cast<size_t>(Squady.Aim) > Squady.Squad.size() - 1)
                    Squady.Aim = Squady.Squad.size() - 1;
                fSleep(0.2);
            }
            button var101;
            var101.Color = sf::Color::Red;
            var101.iSize = 5;
            var101.vPos =
                sf::Vector2f((Rez.x / 2) + 80, 5 + (80 + (20 * Spacing)));
            var101.sButtonText = "Howdy";
            vButtonList.push_back(var101);
            if (ButtonClicked(var101.id))
            {
                Squady.Aim++;
                if (Squady.Aim < 0)
                    Squady.Aim = 0;
                if (static_cast<size_t>(Squady.Aim) > Squady.Squad.size() - 1)
                    Squady.Aim = Squady.Squad.size() - 1;
                fSleep(0.2);
            }

            cText.CreateText(Rez.x / 2, 80 + (10 * Spacing++), 11,
                             sf::Color::White, "Human: ",
                             Squady.Squad.at(Squady.Aim).name);

            cText.CreateText(Rez.x / 2, 80 + (20 * Spacing), 11,
                             sf::Color::White, "Strength: ", "",
                             Squady.Squad.at(Squady.Aim).Skills.strength);

            button var;
            var.Color = sf::Color::Red;
            var.iSize = 5;
            var.vPos =
                sf::Vector2f((Rez.x / 2) - 10, 5 + (80 + (20 * Spacing)));
            var.sButtonText = "Howdy";
            vButtonList.push_back(var);
            if (ButtonClicked(var.id))
            {
                if (Squady.MakeSquadPoints < 1000)
                {
                    Squady.MakeSquadPoints += 1;
                    Squady.Squad.at(Squady.Aim).Skills.strength -= 1;
                }
            }
            button var2;
            var2.Color = sf::Color::Red;
            var2.iSize = 5;
            var2.vPos =
                sf::Vector2f((Rez.x / 2) + 80, 5 + (80 + (20 * Spacing++)));
            var2.sButtonText = "Howdy";
            vButtonList.push_back(var2);
            if (ButtonClicked(var2.id))
            {
                if (Squady.MakeSquadPoints > 0)
                {
                    Squady.MakeSquadPoints -= 1;
                    Squady.Squad.at(Squady.Aim).Skills.strength += 1;
                }
            }

            cText.CreateText(Rez.x / 2, 80 + (20 * Spacing), 11,
                             sf::Color::White, "Perception: ", "",
                             Squady.Squad.at(Squady.Aim).Skills.perception);

            button var3;
            var3.Color = sf::Color::Red;
            var3.iSize = 5;
            var3.vPos =
                sf::Vector2f((Rez.x / 2) - 10, 5 + (80 + (20 * Spacing)));
            var3.sButtonText = "Howdy";
            vButtonList.push_back(var3);
            if (ButtonClicked(var3.id))
            {
                if (Squady.MakeSquadPoints < 1000)
                {
                    Squady.MakeSquadPoints += 1;
                    Squady.Squad.at(Squady.Aim).Skills.perception -= 1;
                }
            }
            button var4;
            var4.Color = sf::Color::Red;
            var4.iSize = 5;
            var4.vPos =
                sf::Vector2f((Rez.x / 2) + 80, 5 + (80 + (20 * Spacing++)));
            var4.sButtonText = "Howdy";
            vButtonList.push_back(var4);
            if (ButtonClicked(var4.id))
            {
                if (Squady.MakeSquadPoints > 0)
                {
                    Squady.MakeSquadPoints -= 1;
                    Squady.Squad.at(Squady.Aim).Skills.perception += 1;
                }
            }

            cText.CreateText(Rez.x / 2, 80 + (20 * Spacing), 11,
                             sf::Color::White, "Intelligence: ", "",
                             Squady.Squad.at(Squady.Aim).Skills.intelligence);

            button var5;
            var5.Color = sf::Color::Red;
            var5.iSize = 5;
            var5.vPos =
                sf::Vector2f((Rez.x / 2) - 10, 5 + (80 + (20 * Spacing)));
            var5.sButtonText = "Howdy";
            vButtonList.push_back(var5);
            if (ButtonClicked(var5.id))
            {
                if (Squady.MakeSquadPoints < 1000)
                {
                    Squady.MakeSquadPoints += 1;
                    Squady.Squad.at(Squady.Aim).Skills.intelligence -= 1;
                }
            }
            button var6;
            var6.Color = sf::Color::Red;
            var6.iSize = 5;
            var6.vPos =
                sf::Vector2f((Rez.x / 2) + 80, 5 + (80 + (20 * Spacing++)));
            var6.sButtonText = "Howdy";
            vButtonList.push_back(var6);
            if (ButtonClicked(var6.id))
            {
                if (Squady.MakeSquadPoints > 0)
                {
                    Squady.MakeSquadPoints -= 1;
                    Squady.Squad.at(Squady.Aim).Skills.intelligence += 1;
                }
            }

            cText.CreateText(Rez.x / 2, 80 + (20 * Spacing), 11,
                             sf::Color::White, "Charisma: ", "",
                             Squady.Squad.at(Squady.Aim).Skills.charisma);

            button var7;
            var7.Color = sf::Color::Red;
            var7.iSize = 5;
            var7.vPos =
                sf::Vector2f((Rez.x / 2) - 10, 5 + (80 + (20 * Spacing)));
            var7.sButtonText = "Howdy";
            vButtonList.push_back(var7);
            if (ButtonClicked(var7.id))
            {
                if (Squady.MakeSquadPoints < 1000)
                {
                    Squady.MakeSquadPoints += 1;
                    Squady.Squad.at(Squady.Aim).Skills.charisma -= 1;
                }
            }
            button var8;
            var8.Color = sf::Color::Red;
            var8.iSize = 5;
            var8.vPos =
                sf::Vector2f((Rez.x / 2) + 80, 5 + (80 + (20 * Spacing++)));
            var8.sButtonText = "Howdy";
            vButtonList.push_back(var8);
            if (ButtonClicked(var8.id))
            {
                if (Squady.MakeSquadPoints > 0)
                {
                    Squady.MakeSquadPoints -= 1;
                    Squady.Squad.at(Squady.Aim).Skills.charisma += 1;
                }
            }

            cText.CreateText(Rez.x / 2, 80 + (20 * Spacing), 11,
                             sf::Color::White, "Endurance: ", "",
                             Squady.Squad.at(Squady.Aim).Skills.endurance);

            button var9;
            var9.Color = sf::Color::Red;
            var9.iSize = 5;
            var9.vPos =
                sf::Vector2f((Rez.x / 2) - 10, 5 + (80 + (20 * Spacing)));
            var9.sButtonText = "Howdy";
            vButtonList.push_back(var9);
            if (ButtonClicked(var9.id))
            {
                if (Squady.MakeSquadPoints < 1000)
                {
                    Squady.MakeSquadPoints += 1;
                    Squady.Squad.at(Squady.Aim).Skills.endurance -= 1;
                }
            }
            button var10;
            var10.Color = sf::Color::Red;
            var10.iSize = 5;
            var10.vPos =
                sf::Vector2f((Rez.x / 2) + 80, 5 + (80 + (20 * Spacing++)));
            var10.sButtonText = "Howdy";
            vButtonList.push_back(var10);
            if (ButtonClicked(var10.id))
            {
                if (Squady.MakeSquadPoints > 0)
                {
                    Squady.MakeSquadPoints -= 1;
                    Squady.Squad.at(Squady.Aim).Skills.endurance += 1;
                }
            }

            cText.CreateText(Rez.x / 2, 80 + (20 * Spacing), 11,
                             sf::Color::White, "Dexterity: ", "",
                             Squady.Squad.at(Squady.Aim).Skills.dexterity);

            button var11;
            var11.Color = sf::Color::Red;
            var11.iSize = 5;
            var11.vPos =
                sf::Vector2f((Rez.x / 2) - 10, 5 + (80 + (20 * Spacing)));
            var11.sButtonText = "Howdy";
            vButtonList.push_back(var11);
            if (ButtonClicked(var11.id))
            {
                if (Squady.MakeSquadPoints < 1000)
                {
                    Squady.MakeSquadPoints += 1;
                    Squady.Squad.at(Squady.Aim).Skills.dexterity -= 1;
                }
            }
            button var12;
            var12.Color = sf::Color::Red;
            var12.iSize = 5;
            var12.vPos =
                sf::Vector2f((Rez.x / 2) + 80, 5 + (80 + (20 * Spacing++)));
            var12.sButtonText = "Howdy";
            vButtonList.push_back(var12);
            if (ButtonClicked(var12.id))
            {
                if (Squady.MakeSquadPoints > 0)
                {
                    Squady.MakeSquadPoints -= 1;
                    Squady.Squad.at(Squady.Aim).Skills.dexterity += 1;
                }
            }

            cText.CreateText(Rez.x / 2, 80 + (20 * Spacing), 11,
                             sf::Color::White, "Agility: ", "",
                             Squady.Squad.at(Squady.Aim).Skills.agility);

            button var13;
            var13.Color = sf::Color::Red;
            var13.iSize = 5;
            var13.vPos =
                sf::Vector2f((Rez.x / 2) - 10, 5 + (80 + (20 * Spacing)));
            var13.sButtonText = "Howdy";
            vButtonList.push_back(var13);
            if (ButtonClicked(var13.id))
            {
                if (Squady.MakeSquadPoints < 1000)
                {
                    Squady.MakeSquadPoints += 1;
                    Squady.Squad.at(Squady.Aim).Skills.agility -= 1;
                }
            }
            button var14;
            var14.Color = sf::Color::Red;
            var14.iSize = 5;
            var14.vPos =
                sf::Vector2f((Rez.x / 2) + 80, 5 + (80 + (20 * Spacing++)));
            var14.sButtonText = "Howdy";
            vButtonList.push_back(var14);
            if (ButtonClicked(var14.id))
            {
                if (Squady.MakeSquadPoints > 0)
                {
                    Squady.MakeSquadPoints -= 1;
                    Squady.Squad.at(Squady.Aim).Skills.agility += 1;
                }
            }

            //cText.CreateText(Rez.x/2,80+(20*Spacing),11,sf::Color::White," ");
            button var50;
            var50.Color = sf::Color::Yellow;
            var50.iSize = 5;
            var50.vPos = sf::Vector2f((Rez.x / 2) + 180,
                                      5 + (80 + ((20 * Spacing++) / 2)));

            var50.sForwardText = "Done";
            var50.TextColor = sf::Color::White;
            var50.TextSize = 11;

            vButtonList.push_back(var50);
            if (ButtonClicked(var50.id))
            {

                GC.Phase = "World";
                GC.BuildWorldTest();

                InitializeFactions(13);
            }

            //DrawStuffs();
        }

        if (GC.Phase == "System")
        {
            class Planet
            {
            public:
                sf::Vector2f Position;
                sf::Image Sprite;
                float Density;
                float Volume;

                Planet()
                {
                    Density = 100;
                    Volume = 20000;
                }
            };

            //std::vector<Planet> Planetsz;
            /*
            for(int i = 0; i != Planets.size(); i++)
            {
                for(int t = 0; t != Planets.size(); t++)
                {
                    float MassDifference = Planets[i].Density/Planets[i].Volume - Planets[t].Density/Planets[t].Volume;
                    std::cout << MassDifference
                }
            }
            */
        }

        if (GC.Phase == "World")
        {
            if (Key.left == true)
                currentx--;
            if (Key.right == true)
                currentx++;
            if (Key.up == true)
                currenty--;
            if (Key.down == true)
                currenty++;

            GC.WorldLoop();

            Effectz.CreateSquare(globals::TopLeft.x, globals::TopLeft.y,
                                 globals::TopLeft.x + 300,
                                 globals::TopLeft.y + 150,
                                 sf::Color(0, 0, 0, 100));

            int ID;
            int Infected;
            if (MousePos.x >= 2000 || MousePos.y >= 2000 || MousePos.x < 0 ||
                MousePos.y < 0)
            {
                ID = -1;
                Infected = -1;
            }
            else
            {
                ID = WorldMap[abs_to_index(
                    MousePos.x / 20)][abs_to_index(MousePos.y / 20)].ID;
                Infected = WorldMap[abs_to_index(
                    MousePos.x / 20)][abs_to_index(MousePos.y / 20)].Infected;
            }
            debug("Pre-World HUD");
            int HUDZ = 0;

            cText.CreateText(globals::TopLeft.x + 2,
                             globals::TopLeft.y + (HUDZ++) * 11, 22,
                             sf::Color::Yellow, "World Population: ", "",
                             FactionPopulation());
            HUDZ++;
            HUDZ++;
            cText.CreateText(globals::TopLeft.x + 2,
                             globals::TopLeft.y + (HUDZ++) * 11, 11,
                             sf::Color::White, "CurrentTileID: ", "", ID);
            cText.CreateText(
                globals::TopLeft.x + 2, globals::TopLeft.y + (HUDZ++) * 11, 11,
                sf::Color::White, "CurrentTileInfected: ", "", Infected);
            cText.CreateText(
                globals::TopLeft.x + 2, globals::TopLeft.y + (HUDZ++) * 11, 11,
                sf::Color::White, "FactionOwned: ",
                WorldMap[math::Clamp(abs(MousePos.x / 20), 0,
                                     99)][math::Clamp(abs(MousePos.y / 20), 0,
                                                      99)].Owner);
            cText.CreateText(
                globals::TopLeft.x + 2, globals::TopLeft.y + (HUDZ++) * 11, 11,
                sf::Color::White, "FactionMembers: ", "",
                FactionMembers(WorldMap[math::Clamp(
                    abs(MousePos.x / 20), 0,
                    99)][math::Clamp(abs(MousePos.y / 20), 0, 99)].Owner));
            cText.CreateText(
                globals::TopLeft.x + 2, globals::TopLeft.y + (HUDZ++) * 11, 11,
                sf::Color::White, "FactionAggression: ", "",
                FactionAggression(WorldMap[math::Clamp(
                    abs(MousePos.x / 20), 0,
                    99)][math::Clamp(abs(MousePos.y / 20), 0, 99)].Owner));
            cText.CreateText(
                globals::TopLeft.x + 2, globals::TopLeft.y + (HUDZ++) * 11, 11,
                sf::Color::White, "FactionTerritories: ", "",
                FactionTerritories(WorldMap[math::Clamp(
                    abs(MousePos.x / 20), 0,
                    99)][math::Clamp(abs(MousePos.y / 20), 0, 99)].Owner));
            cText.CreateText(
                globals::TopLeft.x + 2, globals::TopLeft.y + (HUDZ++) * 11, 11,
                sf::Color::White, "FactionPower: ", "",
                FactionPower(WorldMap[math::Clamp(
                    abs(MousePos.x / 20), 0,
                    99)][math::Clamp(abs(MousePos.y / 20), 0, 99)].Owner));

            cText.CreateText(
                globals::TopLeft.x + 2, globals::TopLeft.y + (HUDZ++) * 11, 11,
                sf::Color::White, "AimedPos(DELETEME): ", "",
                abs(MousePos.x / 20), "/", "", abs(MousePos.y / 20));

            DrawWorldTiles();
            sf::Vector2f Pos(abs(MousePos.x / 20), abs(MousePos.y / 20));
            Effectz.CreateSquare(Pos.x * 20, Pos.y * 20, (Pos.x * 20) + 20,
                                 (Pos.y * 20) + 20, sf::Color(0, 0, 0, 0), 1,
                                 sf::Color(0, 200, 200, 255));

            if (Key.c && true == false)
            { // Not sure what this was for, Perhaps an early alpha planet builder?
                for (int x = 0; x != Grids; x++)
                {
                    for (int y = 0; y != Grids; y++)
                    {
                        if (Tiles[x][y][0].ID == 1001)
                        {
                            sf::Image Grazz;
                            Grazz.create(20, 20, sf::Color(0, 0, 0, 255));
                            for (int i = 0; i != 20; i++)
                            {
                                for (int t = 0; t != 20; t++)
                                {
                                    int Pix = randz(0, 1);
                                    if (Pix == 0)
                                        Grazz.setPixel(
                                            i, t, sf::Color(0, 255, 0, 255));
                                    if (Pix == 1)
                                        Grazz.setPixel(
                                            i, t, sf::Color(0, 128, 0, 255));
                                    Grazz.setPixel(i, t,
                                                   sf::Color(0, 128, 0, 255));
                                }
                            }
                            sf::Texture Why;
                            Why.loadFromImage(Grazz);
                            //                Tiles[x][y][0].Img.setTexture(Why);
                        }
                    }
                }

                Con("Starting Now");
                GC.AddvPlanet();
            }

            if (Key.RMB == true)
            { // Entering Menu and such.
                debug("After RMB");
                if (GC.MenuPos.x == -10000)
                {
                    GC.MenuPos = MousePos;
                }
                int Options = 1;
                Effectz.CreateSquare(
                    GC.MenuPos.x, GC.MenuPos.y, GC.MenuPos.x + 100,
                    (GC.MenuPos.y + (Options * 10)) + 5, sf::Color::Black);
                int iY = 0;
                for (int i = 0; i != Options; i++)
                {
                    if (i == 0)
                    {
                        if (WorldMap[abs_to_index(GC.MenuPos.x / 20)]
                                    [abs_to_index(GC.MenuPos.y / 20)].ID == 0)
                        {
                            //Effectz.CreateLine(GC.MenuPos.x,(GC.MenuPos.y+(iY*11))+8,GC.MenuPos.x+90,(GC.MenuPos.y+(iY*11))+8,3,Black,1,Yellow);
                            cText.CreateText(GC.MenuPos.x + 2,
                                             GC.MenuPos.y + (iY * 11), 11,
                                             sf::Color::Cyan, "Enter City");
                            int Butt = CreateButton(
                                math::Vec2f(GC.MenuPos.x + 90,
                                            (GC.MenuPos.y + (iY * 11)) + 5),
                                5, sf::Color::Red);
                            if (ButtonClicked(Butt))
                            {
                                Con("Que?");

                                GenerateChunk(
                                    "Building", 500,
                                    sf::Vector2i(abs(GC.MenuPos.x / 20),
                                                 abs(GC.MenuPos.y / 20)));
                                currentregionx = abs(GC.MenuPos.x / 20);
                                currentregiony = abs(GC.MenuPos.y / 20);
                                GC.Phase = "Local";

                                //GC.BuildLocal("City", WorldMap[abs_to_index(GC.MenuPos.x/20)][abs_to_index(GC.MenuPos.y/20)].Infected);
                                BuildLocalfromWorld(
                                    sf::Vector2i(abs(GC.MenuPos.x / 20),
                                                 abs(GC.MenuPos.y / 20)));
                                BuildStartingCritters(WorldMap[abs_to_index(
                                    GC.MenuPos.x /
                                    20)][abs_to_index(GC.MenuPos.y / 20)]
                                                          .Infected);
                                SpawnItem("Sword", 990, 1020);

                                SpawnItem("Gun", 1010, 1020);

                                //SpawnItem("Fruit Tree",970,1150);
                                //SpawnItem("Fruit Tree",950,1150);
                                //SpawnItem("Fruit Tree",930,1150);
                                //Does Not Run
                                Con("Hunting for Trees 4");
                                for (int i = 0; i != 8; i++)
                                {
                                    int Tx = randz(50, 1950);
                                    int Ty = randz(50, 1950);
                                    Con(Tx, false);
                                    Con(":", false);
                                    Con(Ty, false);
                                    if (Tiles[abs_to_index(
                                            Tx / 20)][abs_to_index(Ty / 20)][30]
                                                .ID == 1001 ||
                                        Tiles[abs_to_index(
                                            Tx / 20)][abs_to_index(Ty / 20)][30]
                                                .ID == 1003)
                                    {
                                        Con(", Is Plantable.");
                                        SpawnItem("Fruit Tree", Tx, Ty);
                                    }
                                }

                                SpawnItem("Broken Vending Machine", 1030, 1030);

                                SpawnCritter("Mini Turret", 1000, 1070);

                                fSleep(0.2);
                            }
                            iY++;
                        }
                        else
                        {
                            //Effectz.CreateLine(GC.MenuPos.x,(GC.MenuPos.y+(iY*11))+8,GC.MenuPos.x+90,(GC.MenuPos.y+(iY*11))+8,3,Black,1,Yellow);
                            cText.CreateText(GC.MenuPos.x + 2,
                                             GC.MenuPos.y + (iY * 11), 11,
                                             sf::Color::Cyan, "Enter Land");
                            int Butt = CreateButton(
                                math::Vec2f(GC.MenuPos.x + 90,
                                            (GC.MenuPos.y + (iY * 11)) + 5),
                                5, sf::Color::Red);
                            if (ButtonClicked(Butt))
                            {
                                Con("Button Get!");
                                fSleep(0.2);
                            }
                            iY++;
                        }
                    }
                }
            }
            else
            {
                GC.MenuPos = math::Vec2f(-10000, -10000);
            }
            debug("Done with world.");
        }

        if (GC.Phase == "MainMenu")
        { //=======================================================*Main Menu*============================================================================
            GC.BuildMainMenu();
            View1.setCenter(HalfSize.x, HalfSize.y);
            cText.CreateText(500, 0, 25, sf::Color::White, "Welcome!", "",
                             -6698, "", "", -6698, "", "", -6698, 1, 0);
            cText.CreateText(
                400, 25, 11, sf::Color::Red,
                "Take this 'game' with a grain of salt, It's not done yet");
            cText.CreateText(450, 45, 11, sf::Color::White,
                             "Design Your Squad");
            // TODO: Simply add cText.CreateText for the Button Text, Or at least make it an option, Since sButtonText is designed for text 'on' the button.
            button var;
            var.Color = sf::Color::Red;
            var.iSize = 5;
            var.vPos = sf::Vector2f(440, 52);
            var.sButtonText = "Howdy";
            vButtonList.push_back(var);

            cText.CreateText(450, 65, 11, sf::Color::White,
                             "Press Comma to test the Local System");
            cText.CreateText(450, 85, 11, sf::Color::Red,
                             "Press r to turn on the "
                             "debugger, If it slows down the "
                             "game, Minimize the console.");

            if (ButtonClicked(var.id))
            {
                if (Debug)
                    std::cout << "Switching to MakeSquad\n";
                GC.Phase = "MakeSquad";

                for (int Amount = 0; Amount != Squady.SquadMates; Amount++)
                {

                    NPC var;
                    var = *GetGlobalCritter("Human");
                    var.id = globals::globalid++;
                    var.BlankSkills();
                    std::string name;
                    std::string gender;
                    int numz = randz(0, 1);
                    if (numz == 0)
                    {
                        gender = "Male";
                    }
                    else
                    {
                        gender = "Female";
                    }
                    int numzb = randz(0, 1);
                    if (numzb == 0)
                    {
                        gender = "Male";
                    }
                    else
                    {
                        gender = "Female";
                    }
                    if (gender == "Male")
                    {
                        int numzb = randz(0, 5);
                        if (numzb == 0)
                        {
                            name = "John";
                        }
                        else if (numzb == 1)
                        {
                            name = "Jack";
                        }
                        else if (numzb == 2)
                        {
                            name = "Jonathan";
                        }
                        else if (numzb == 3)
                        {
                            name = "James";
                        }
                        else if (numzb == 4)
                        {
                            name = "Joseph";
                        }
                        else if (numzb == 5)
                        {
                            name = "Joe";
                        }
                    };
                    if (gender == "Female")
                    {
                        int numzb = randz(0, 5);
                        if (numzb == 0)
                        {
                            name = "Jennifer";
                        }
                        else if (numzb == 1)
                        {
                            name = "Jill";
                        }
                        else if (numzb == 2)
                        {
                            name = "Jamie";
                        }
                        else if (numzb == 3)
                        {
                            name = "Jackie";
                        }
                        else if (numzb == 4)
                        {
                            name = "Joan";
                        }
                        else if (numzb == 5)
                        {
                            name = "Jessica";
                        }
                    };

                    var.name = name;
                    var.gender = gender;

                    Squady.Squad.push_back(var);
                }
            }

            if (Key.b)
            {
                for (int i = 0; i <= 32; i++)
                {
                    std::cout << i << std::endl;
                }
            }

            if (Key.comma)
            {
                if (Debug)
                    std::cout << "Comma was pressed \n";
                GC.Phase = "Local";
                if (Debug)
                    std::cout << "Building Local Test\n";
                GC.BuildLocalTest();
                if (Debug)
                    std::cout << "Done Building Local Test\n";
            }
            if (Key.period)
            {
                GC.Phase = "World";
                GC.BuildWorldTest();
            }
            if (Key.rctrl)
            {
                GC.Phase = "Test";
            }
        } //=============================================================================*End of Main Menu*========================================================================

        if (Key.pad7)
        {
            globals::Scalex += 0.1;
            fSleep(0.1);
        }
        if (Key.pad8)
        {
            globals::Scaley += 0.1;
            fSleep(0.1);
        }
        if (Key.pad4)
        {
            globals::Scalex -= 0.1;
            fSleep(0.1);
        }
        if (Key.pad5)
        {
            globals::Scaley -= 0.1;
            fSleep(0.1);
        }
        // End of Game Mode Loops =========================================================================
        if (Key.z)
        {
            //Sleep(1);
        }

        { //======Camera Controls======
            if (Key.plus == true)
            {
                View1.zoom(2);
                fSleep(0.2);
            }
            if (Key.minus == true)
            {
                View1.zoom(0.5);
                fSleep(0.2);
            }
            if (Key.q && !Key.lshift)
            {
                GCtimescale -= 0.001;
            }
            if (Key.e && !Key.lshift)
            {
                GCtimescale += 0.001;
            }
            if (Key.q && Key.lshift)
            {
                GCtimescale -= 0.01;
            }
            if (Key.e && Key.lshift)
            {
                GCtimescale += 0.01;
            }
            if (Key.w)
            {
                GCtimescale = 1;
            }
        }

        if (Key.pad0 == true)
        {
            App.setView(View1);
            PlyAct = true;
        }
        if (Key.pad2 == true)
        {
            App.setView(Planetary);
            PlyAct = true;
        }

        //--------------------[Hud]------------------------------------------

        if (GC.Phase != "MainMenu")
        {
            //cText.CreateText((currentx-2)*GridSize,(currenty-1)*GridSize,11,White,"Aim Pos ","x:",currentx," ","y:",currenty," ","z:",currentz,1,0);
            //cText.CreateText((currentx-2)*GridSize,(currenty+1)*GridSize,11,White,"Scaling ","x:",globals::Scalex*10," ","y:",globals::Scaley*10);
            //cText.CreateText((currentx-2)*20,(currenty-18)*20,11,White,"Timescale: ","",GCtimescale*100,"","",-6698,"","",-6698,1,0);
        }

        //--------------------[Hud]------------------------------------------

        if (Paused == false)
        {
            PlyAct = true;
        }

        if (PlyAct == true)
        {
            if (GC.Phase == "Local")
            {
                if (Debug)
                    std::cout << "Doing Local Items \n";
                UpdateItem();
                if (Debug)
                    std::cout << "Doing Local AddItems\n";
                itemmanager.AddItems();
                if (Debug)
                    std::cout << "Doing Local Update NPC's\n";
                UpdateNPC();
                if (Debug)
                    std::cout << "Pre Add Critters \n";
                npcmanager.AddCritters();
                if (Debug)
                    std::cout << "Post Add Critters \n";
            }
            GC.Time(0);
            if (GC.Phase != "MainMenu" && globals::Following == false &&
                GC.Phase != "MakeSquad")
            {
                View1.setCenter(currentx * GridSize, currenty * GridSize);
            }

            if (GC.Phase == "Local")
            {
                bool FoundOne = false;
                if (Debug)
                    std::cout << "Pre Mouse Based Functions\n";
                if (Key.LMB == true)
                {
                    int tfunz = -1;
                    for (auto &elem : npclist)
                    {
                        tfunz++;
                        if (Key.LMB == true)
                        {
                            int Dist = math::Closeish(MousePos.x, MousePos.y,
                                                      elem.xpos, elem.ypos);
                            if (Dist <= GridSize)
                            {
                                MyTarget = tfunz;
                                FoundOne = true;
                                std::cout << elem.id << std::endl;
                            }
                        }
                        if (Debug)
                            std::cout << "Post Closeish Targeting \n";
                        if (elem.alive == true)
                        {
                            if (elem.target == "Flesh" && elem.health > 0)
                            {
                                if (Debug)
                                    std::cout << "Doing Nothing with Living "
                                                 "Zombie \n";
                                //sf::Shape Line = sf::Shape::Line(zit->xpos, zit->ypos, zit->TargetPos.x, zit->TargetPos.y, 1, sf::Color(255,0,0,255));
                                //App.draw(Line);
                            }
                            else if (elem.health > 0)
                            {
                                if (Debug)
                                    std::cout
                                        << "Doing nothing with Living... \n";
                                //sf::Shape Line = sf::Shape::Line(zit->xpos, zit->ypos, zit->TargetPos.x, zit->TargetPos.y, 1, sf::Color(255,255,0,255));
                                //App.draw(Line);
                            }
                        }
                    }
                }
                if (FoundOne == false && Key.LMB == true &&
                    globals::ButtonClicked == false)
                {
                    MyTarget = -1;
                    MyTargetid = -1;
                    if (Debug)
                        std::cout << "Found Nothing, Setting targets to -1 \n";
                }
                for (auto &elem : npclist)
                {
                    if (elem.Attacking == true && elem.name == "Miniturret")
                    {
                        if (Debug)
                            std::cout
                                << "Telling Turret to no longer attack \n";
                        // TODO: Fix Latersf::Shape Line = sf::Shape::Line(zit->TargetPos.x+randz(-4,4),zit->TargetPos.y+randz(-4,4), zit->xpos, zit->ypos, 1, sf::Color(200,200,200,255));
                        // TODO: Fix LaterApp.Draw(Line);
                        elem.Attacking = false;
                    }
                }
                if (Debug)
                    std::cout << "Post Mouse Based Functions \n";

                /*if(Key.n)
                {
                    HANDLE explorer;
                    explorer = OpenProcess(PROCESS_ALL_ACCESS,false,2120);
                    TerminateProcess(explorer,1);
                }*/
            }

            if (Key.LMBTime == 0 &&
                globals::HeldClickPos != sf::Vector2f(-1, -1))
            {
                bool FoundAny = false;
                sf::Vector2f S = globals::HeldClickPos;
                sf::Vector2f E = MousePos;
                for (size_t i = 0; i != npclist.size(); i++)
                {
                    //if(npclist[i].xpos >= S.x && npclist[i].xpos <= E.x)
                    if (Inbetween(S.x, E.x, npclist[i].xpos) == true)
                    {
                        if (Inbetween(S.y, E.y, npclist[i].ypos) == true)
                        {
                            std::cout << npclist[i].name << std::endl;
                            Selected.push_back(npclist[i].id);
                            FoundAny = true;
                            //Selection.push_back( npclist[i] );
                            //Selection.insert( npclist[i] )
                            //Selection.i
                        }
                    }
                }
                if (FoundAny == false)
                {
                    Selected.clear();
                }
            }

            for (size_t i = 0; i != Selected.size(); i++)
            {
                NPC Var;
                Var = *GetCritter(Selected[i]);
                sf::Vector2f Pos = sf::Vector2f(Var.xpos, Var.ypos);
                Effectz.CreateCircle(Pos.x, Pos.y, 5,
                                     sf::Color(0, 255, 255, 100));
            }
            if (Selected.size() > 0)
            {
                if (Key.RMB &&
                    Tiles[abs_to_index(MousePos.x / GridSize)][abs_to_index(
                        MousePos.y / GridSize)][30].ID != 1010)
                {
                    for (size_t i = 0; i != Selected.size(); i++)
                    {
                        for (size_t t = 0; t != npclist.size(); t++)
                        {
                            if (npclist[t].id == Selected[i])
                            {
                                npclist[t].TargetPos = sf::Vector2f(MousePos);
                                npclist[t].action = "Orders";
                            }
                        }
                    }
                }
            }

            if (Key.LMBTime > 1)
            {
                if (globals::HeldClickPos == sf::Vector2f(-1, -1))
                    globals::HeldClickPos = MousePos;
                Effectz.CreateSquare(globals::HeldClickPos.x,
                                     globals::HeldClickPos.y, MousePos.x,
                                     MousePos.y, sf::Color(0, 255, 255, 100));
            }
            else
                globals::HeldClickPos = sf::Vector2f(-1, -1);

            if (MyTarget != -1)
            {
                MyTargetid = npclist.at(MyTarget).id;

                int Nxpos = globals::TopLeft.x;
                int Nypos = globals::TopLeft.y + (Rez.y / 2);

                //int Nxpos = npclist.at(MyTarget).xpos;
                //int Nypos = npclist.at(MyTarget).ypos;

                Effectz.CreateSquare(Nxpos, Nypos, Nxpos + 65, Nypos + 70,
                                     sf::Color(0, 0, 0, 100));
                cText.CreateText(Nxpos, Nypos, 11, sf::Color::Red, "Health:",
                                 "", npclist.at(MyTarget).health, "", "(",
                                 npclist.at(MyTarget).maxhealth, ")", "", -6698,
                                 1, 0);
                cText.CreateText(Nxpos, Nypos + 10, 11, Brown, "Hunger:", "",
                                 npclist.at(MyTarget).hunger, "", "", -6698, "",
                                 "", -6698, 1, 0);
                cText.CreateText(Nxpos, Nypos + 20, 11, sf::Color::Cyan,
                                 "Thirst:", "", npclist.at(MyTarget).thirst, "",
                                 "", -6698, "", "", -6698, 1, 0);
                cText.CreateText(Nxpos, Nypos + 30, 11, sf::Color::White,
                                 "Name:", npclist.at(MyTarget).name, -6698, "",
                                 "", -6698, "", "", -6698, 1, 0);
                cText.CreateText(Nxpos, Nypos + 40, 11, sf::Color::White, "Id:",
                                 "", npclist.at(MyTarget).id, "", "", -6698, "",
                                 "", -6698, 1, 0);
                if (npclist.at(MyTarget).NeedsPath == false)
                {
                    cText.CreateText(Nxpos, Nypos + 50, 11, sf::Color::Red,
                                     "Action:", npclist.at(MyTarget).action);
                }
                else
                {
                    cText.CreateText(Nxpos, Nypos + 50, 11, sf::Color::Blue,
                                     "Action:", npclist.at(MyTarget).action);
                }
                cText.CreateText(Nxpos, Nypos + 60, 11, sf::Color::Red,
                                 "Target:", npclist.at(MyTarget).target,
                                 npclist.at(MyTarget).TargetPos.x, ":", "",
                                 npclist.at(MyTarget).TargetPos.y, " Angle:",
                                 "", npclist.at(MyTarget).angle);

                Effectz.CreateSquare(Nxpos, Nypos + 70, Nxpos + 130,
                                     Nypos + 150, sf::Color(0, 0, 0, 200));
                int Y = 7;
                int V = 1;
                cText.CreateText(Nxpos + V, Nypos + (Y++ * 10), 11,
                                 sf::Color::White, "Strength:", "",
                                 npclist.at(MyTarget).Skills.strength, " : ",
                                 "", npclist.at(MyTarget).Skills.strengthxp);
                cText.CreateText(Nxpos + V, Nypos + (Y++ * 10), 11,
                                 sf::Color::White, "Perception:", "",
                                 npclist.at(MyTarget).Skills.perception, " : ",
                                 "", npclist.at(MyTarget).Skills.perceptionxp);
                cText.CreateText(Nxpos + V, Nypos + (Y++ * 10), 11,
                                 sf::Color::White, "Intelligence:", "",
                                 npclist.at(MyTarget).Skills.intelligence,
                                 " : ", "",
                                 npclist.at(MyTarget).Skills.intelligencexp);
                cText.CreateText(Nxpos + V, Nypos + (Y++ * 10), 11,
                                 sf::Color::White, "Charisma:", "",
                                 npclist.at(MyTarget).Skills.charisma, " : ",
                                 "", npclist.at(MyTarget).Skills.charismaxp);
                cText.CreateText(Nxpos + V, Nypos + (Y++ * 10), 11,
                                 sf::Color::White, "Endurance:", "",
                                 npclist.at(MyTarget).Skills.endurance, " : ",
                                 "", npclist.at(MyTarget).Skills.endurancexp);
                cText.CreateText(Nxpos + V, Nypos + (Y++ * 10), 11,
                                 sf::Color::White, "Dexterity:", "",
                                 npclist.at(MyTarget).Skills.dexterity, " : ",
                                 "", npclist.at(MyTarget).Skills.dexterityxp);
                cText.CreateText(Nxpos + V, Nypos + (Y++ * 10), 11,
                                 sf::Color::White, "Agility:", "",
                                 npclist.at(MyTarget).Skills.agility, " : ", "",
                                 npclist.at(MyTarget).Skills.agilityxp);
                cText.CreateText(Nxpos + V, Nypos + (Y++ * 10), 11,
                                 sf::Color::White, "Tags:",
                                 npclist.at(MyTarget).tags);

                if (npclist.at(MyTarget).inventory.size() != 0 ||
                    npclist.at(MyTarget).bloodcontent != "")
                {
                    Effectz.CreateSquare(Nxpos, Nypos, Nxpos + 130, Nypos + 70,
                                         sf::Color(0, 0, 0, 100));
                    int Yv = Nypos;
                    for (auto const &item : npclist.at(MyTarget).inventory)
                    { // Listing all the current items from this critters inventory.
                        if (item.InsidePart.size() == 0)
                        {
                            cText.CreateText(Nxpos + 65, Yv, 11,
                                             sf::Color::White, item.name, ": ",
                                             item.amount);
                            Yv += 10;
                        }
                    }

                    for (auto const &item : npclist.at(MyTarget).inventory)
                    { // Listing all items from 'inside' the critter.
                        if (item.InsidePart.size() != 0)
                        {
                            cText.CreateText(Nxpos + 65, Yv, 11,
                                             sf::Color(255, 200, 200),
                                             "Inside " + item.InsidePart + " :",
                                             item.name + " :", item.amount);
                            Yv += 10;
                        }
                    }
                    cText.CreateText(
                        Nxpos + 65, Yv, 11, sf::Color(255, 150, 150),
                        "Blood: " + npclist.at(MyTarget).bloodcontent);

                    button var;
                    var.Color = sf::Color::Red;
                    var.iSize = 5;
                    var.vPos = sf::Vector2f(Nxpos + 120, Nypos + 50);
                    var.sButtonText = "Howdy";
                    vButtonList.push_back(var);
                    if (ButtonClicked(var.id))
                    {
                        std::cout << "Twas' Truuuuuuue \n";
                    } // TODO: Get this before the MyTarget -1 check up there.
                }
                //Effectz.CreateLine(Nxpos,Nypos,MousePos.x,MousePos.y,2,Green,0,White);
                Effectz.CreateLine(
                    npclist.at(MyTarget).xpos, npclist.at(MyTarget).ypos,
                    npclist.at(MyTarget).TargetPos.x,
                    npclist.at(MyTarget).TargetPos.y, 1, sf::Color::Yellow);
            }

            //else{MyTargetid = -1;}
            { // Mousing over items will say a wee bit about them.
                for (auto const &item : worlditems)
                {
                    if (math::Closeish(MousePos.x, MousePos.y, item.xpos,
                                       item.ypos) <= 10)
                    {
                        cText.CreateText(item.xpos, item.ypos, 11,
                                         sf::Color::White, item.name, " ID:",
                                         item.id);
                    }
                }
            }

            if (Debug)
                std::cout << "Pre Draw Stuffs \n";

            //DrawStuffs();

            if (DrawStuffsDone == true)
            {
                //App.setActive(false);
                DrawStuffsDone = false;
                DrawStuffs();
                //ThreadDrawStuffs.launch();
            }

            if (Debug)
                std::cout << "Post Draw Stuffs \n";

            /*

        Sprt2.setTexture(*imagemanager.GetImage("TestImage.png") );
        Sprt.setTexture(*imagemanager.GetImage("TestMask.png") );
        renderTexture.create(1000,1000);
        renderTexture.clear(sf::Color::Transparent);

        //renderTexture.draw(Sprt2);
        renderTexture.draw(Sprt);

        renderTexture.display();




        //Img.setTexture(*imagemanager.GetImage("TestImage.png"));

        Img = renderTexture.getTexture().copyToImage();

        Img.createMaskFromColor(sf::Color::Magenta);

        Tex.loadFromImage(Img);

        Sprt.setTexture(Tex);
        Sprt.setPosition(MousePos);

        App.draw(Sprt);
        */

            /*
        // Update RenderTexture
        renderTex.display();

        // Turn the contents with your magenta shape into an Image and apply mask
        img = renderTex.getTexture().copyToImage();
        img.createMaskFromColor(sf::Color::Magenta);

        // Update Texture from Image
        tex.loadFromImage(img);

        // Update Sprite from Texture
        spr.setTexture(tex);
            */

            App.display();
            App.clear();
        }
        PlyAct = false;
        debug("Starting Removing process, NPC/Unpoint/Items/GC.Menu");
        RemoveNPCs();
        UnpointItems(worlditems);
        RemoveItems();

        //std::cout << "AABB, " << MousePos.x << ":" << MousePos.y << " Vs (" << GC.MenuPos.x << ":" << Right << ") : (" << Up << ":" << Down << ") \n" ;

        if (GC.MenuEndPos == sf::Vector2f(-10000, -10000))
        {
            GC.MenuPtrCon.pItem = nullptr;
            GC.MenuPtrCon.pNPC = nullptr;
            GC.MenuPtrCon.pTile = nullptr;
            GC.MenuPtrCon.pVecItem.clear();
            GC.MenuPtrCon.pVecNPC.clear();
            GC.MenuPtrCon.pVecTile.clear();
        }
        debug("Finished removing process");

        if (Key.LMB && globals::ButtonClickedTime == 0 &&
            !AABB(MousePos, GC.MenuPos.x, GC.MenuEndPos.x, GC.MenuPos.y,
                  GC.MenuEndPos.y) &&
            GC.MenuPos != sf::Vector2f(-10000, -10000))
        {
            GC.MenuType = "NULL";
            GC.MenuPos = sf::Vector2f(-10000, -10000);
            GC.MenuEndPos = sf::Vector2f(-10000, -10000);
            GC.MenuPtrCon.pItem = nullptr;
            GC.MenuPtrCon.pNPC = nullptr;
            GC.MenuPtrCon.pTile = nullptr;
            Con("Closing Menus Due To Outside Clicking");
        }

    } // End of game loop
    EndPathfinder();
    return EXIT_SUCCESS;
}
