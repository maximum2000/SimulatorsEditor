#include <d3d11.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include "TextureLoader.h"
#include "imgui_impl_dx11.h"
#include "Render.h"

    using namespace EditorMMTextureLoader;

    bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

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
                if (ret = LoadTextureFromFile((std::string(u8"C:/Users/VR/Desktop/projects c++/SimulatorsEditor/src/editor/img/") + std::string(ElementNames[i]) + u8".png").c_str(), &Temp.myTexture, &Temp.imageWidth, &Temp.imageHeight))
                //if (ret = LoadTextureFromFile((std::string(u8"C:/Users/VR/Desktop/projects/SE_branch_dvolkov/SimulatorsEditor/src/editor/img/") + std::string(ElementNames[i]) + u8".png").c_str(), &Temp.myTexture, &Temp.imageWidth, &Temp.imageHeight))
                {
                    IM_ASSERT(ret);
                }
            }
            Temp.imageHeight *= 1;
            Temp.imageWidth *= 1;
            Textures.push_back(Temp);
        }
    }

    bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
    {
        // Load from disk into a raw RGBA buffer
        int image_width = 0;
        int image_height = 0;
        unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
        if (image_data == NULL)
            return false;

        // Create texture
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = image_width;
        desc.Height = image_height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = image_data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        EditorMMRender::CreateTexture(&desc, &subResource, &pTexture);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        EditorMMRender::CreateTextureView(pTexture, &srvDesc, out_srv);
        pTexture->Release();

        *out_width = image_width;
        *out_height = image_height;
        stbi_image_free(image_data);

        return true;
    }