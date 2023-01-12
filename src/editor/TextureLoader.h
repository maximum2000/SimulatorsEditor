#pragma once

namespace EditorMMTextureLoader 
{
	struct LoadedTexture
	{
		ID3D11ShaderResourceView* my_texture;
		int my_image_height;
		int my_image_width;
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
	};
}