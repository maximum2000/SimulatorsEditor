#include "ElementsData.h"
#include "render.h"
#include <string>
#include <imgui_impl_win32.h>

namespace ScenariosEditorElementsData
{
	// Elements data
	static const std::vector<const char*> ElementNames = { "uzel", "start", "clear", "message", "sound", "script", "pilon", "arrow",
										"pause", "push", "select", "outcome", "answer", "variable_value", "random", "danger" };
	static const std::vector<int> ElementTypes = { 1, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 11, 11, 11, 11 }; // pin amount and location
	static std::vector<Texture> Textures;
	static float TextureZoom = 2; // used for texture scaling

	// Load texture from file, should change later
	void TempLoad()
	{
		for (int i = 0; i < ElementNames.size(); i++)
		{
			bool ret = false;
			Texture Temp;
			while (!ret)
			{
				if (ret = ScenariosEditorRender::LoadTextureFromFile((std::string(u8"C:/Users/VR/Desktop/projects/SimulatorsEditor/src/editor/img/") + std::string(ElementNames[i]) + u8".png").c_str(), &Temp.Payload, &Temp.Width, &Temp.Height))
				{
					IM_ASSERT(ret);
				}
			}
			Temp.Height *= TextureZoom;
			Temp.Width *= TextureZoom;
			Textures.push_back(Temp);
		}
	}
	
	void ElementsData::Initialization()
	{
		TempLoad();
	}

	int ElementsData::NumberOfElements()
	{
		return ElementNames.size();
	}
	
	const char* ElementsData::GetElementName(int index)
	{
		return ElementNames[index];
	}
	Texture ElementsData::GetElementTexture(int index)
	{
		return Textures[index];
	}
	int ElementsData::GetElementType(int index)
	{
		return ElementTypes[index];
	}
}