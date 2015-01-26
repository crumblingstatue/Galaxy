#ifndef __IMAGES_H_INCLUDED__
#define __IMAGES_H_INCLUDED__

#include "Game.h"





extern int getdir(std::string dir, std::vector<std::string> &files);

class cImageHolder{
    public:
    sf::Texture Image;
    std::string name;
    cImageHolder()
    {

    }
};

class cImageManager {
    public:
    std::vector<cImageHolder> GlobalImage;

    sf::Texture * GetImage(std::string Input){
        for(int i = 0; i != GlobalImage.size(); i++)
        {
            if(GlobalImage.at(i).name == Input)
            {
                return &GlobalImage.at(i).Image;
            }
        }
        for(int i = 0; i != GlobalImage.size(); i++)
        {
            if(GlobalImage.at(i).name == "Error.bmp")
            {
                return &GlobalImage.at(i).Image;
            }
        }
    }

    sf::Texture GetvImage(std::string Input){
        for(int i = 0; i != GlobalImage.size(); i++)
        {
            if(GlobalImage.at(i).name == Input)
            {
                return GlobalImage.at(i).Image;
            }
        }
        for(int i = 0; i != GlobalImage.size(); i++)
        {
            if(GlobalImage.at(i).name == "Error.bmp")
            {
                return GlobalImage.at(i).Image;
            }
        }
    }


    void InitializeImages(){
        using namespace std;
        string dir = string(".//data//gfx");
        vector<string> files = vector<string>();
        getdir(dir,files);
        for(unsigned int i = 0; i < files.size(); i++)
        {
            string line("data\\gfx\\");
            string ending(files[i]);
            if(files[i] != "." && files[i] != "..")
            {
                line.append(ending);
                cImageHolder image;
                image.Image.loadFromFile(line);
                sf::Vector2u Size = image.Image.getSize();
                //std::cout << Size.x << " : " << Size.y << std::endl;

                std::string namebit = files[i];
                image.name = namebit;
                image.Image.setSmooth(false);
                GlobalImage.push_back(image);
            }
        }
    }
    cImageManager()
    {
    }
}; cImageManager imagemanager;


#endif // IMAGES_H_INCLUDED
