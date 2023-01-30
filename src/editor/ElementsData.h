#pragma once
#include <d3d11.h>
#include <vector>

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
	private:
		ElementsData() // forbid to create object of this class
		{
		}
	};

}