#include <d3d11.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include "TextureLoader.h"
#include "imgui_impl_dx11.h"
#include "Render.h"
#include "ImageLoader.h"

using namespace EditorMMTextureLoader;
using namespace EditorMMImageLoader;



    std::vector<const char*> TextureLoader::ElementNames = {"square_red", "triangle_red", "circle_red",
                                                    "square_green", "triangle_green", "circle_green",
                                                   "square_blue", "triangle_blue", "circle_blue" };
    std::vector<LoadedTexture> TextureLoader::Textures;

    int TextureLoader::GetTextureCount()
    {
        return Textures.size();
    }

    LoadedTexture TextureLoader::GetTextureByIndex(int index)
    {
        return Textures[index];
    }

    std::string TextureLoader::GetTextureNameByIndex(int index)
    {
        return std::string(ElementNames[index]);
    }
    int TextureLoader::GetTextureWidthByIndex(int index)
    {
        return Textures[index].imageWidth;
    }
    int TextureLoader::GetTextureHeightByIndex(int index)
    {
        return Textures[index].imageHeight;
    }

    void TextureLoader::LoadToList()
    {
        for (int i = 0; i < ElementNames.size(); i++)
        {
            bool ret = false;
            LoadedTexture Temp;
            while (!ret)
            {
                //if (ret = LoadTextureFromFile((std::string(u8"C:/Users/VR/Desktop/projects c++/SimulatorsEditor/src/editor/img/") + std::string(ElementNames[i]) + u8".png").c_str(), &Temp.myTexture, &Temp.imageWidth, &Temp.imageHeight))
                if (ret = LoadTextureFromFile((std::string(u8"C:/Users/VR/Desktop/projects/SE_branch_dvolkov/SimulatorsEditor/src/editor/img/") + std::string(ElementNames[i]) + u8".png").c_str(), &Temp.myTexture, &Temp.imageWidth, &Temp.imageHeight))
                {
                    IM_ASSERT(ret);
                }
            }
            Temp.imageHeight *= 1;
            Temp.imageWidth *= 1;
            Textures.push_back(Temp);
        }
    }

    