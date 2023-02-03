#pragma once
#include <d3d11.h>
#include <vector>
#include <string>

namespace ScenariosEditorElementsData
{

	// Texture data
	struct Texture
	{
		ID3D11ShaderResourceView* Payload;
		int Height;
		int Width;
	};

	class ElementsData
	{
	public:
		static void Initialization();
		static int NumberOfElements();
		static const char* GetElementName(int index);
		static Texture GetElementTexture(int index);
		static int GetElementType(int index);
		static std::string GetAttributeHint(std::string attribute);
	private:
		ElementsData() // forbid to create object of this class
		{
		}
	};

}