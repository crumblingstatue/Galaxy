#include "Weaponry.h"
#include "math.h"

#include <iostream>

void explosion(int xpos, int ypos, int /*size*/, int /*power*/, bool /*frag*/)
{
    std::cout << math::closeishS(xpos, ypos) << " Is the stuff\n";
    //  std::cout << plat << std::endl;

    //    std::vector<NPC>::iterator Targets;
    //    for( Targets = npclist.begin(); Targets != npclist.end(); ++Targets ){

    //    std::cout << Targets.name << std::endl;
}
