#pragma once

namespace EditorMMTextureLoader 
{
	struct LoadedTexture
	{
		ID3D11ShaderResourceView* myTexture;
		int imageHeight;
		int imageWidth;
	};

	class TextureLoader
	{
	private:
		static std::vector<const char*> ElementNames;
		static std::vector<LoadedTexture> Textures;
	public:
		void LoadToList();
		int GetTextureCount();
		LoadedTexture GetTextureByIndex(int index);
		std::string GetTextureNameByIndex(int index);
	};
}