#pragma once
#include <d3d11.h>
#include <vector>
#include <imgui.h>

namespace EditorMMSchemeStorage
{
	struct Scheme
	{
		ID3D11ShaderResourceView* myTexture;
		int imageHeight;
		int imageWidth;
		ImVec2 Pos;
		float AspectRatio;
	};
	class SchemeStorage
	{
	public:
		Scheme& operator[](unsigned index)
		{
			return Storage[index];
		}
		void LoadFileToStorage(const wchar_t* FilePath, ImVec2 Pos);
		void Swap(int First, int Second);
		int size();
		void Delete(int Element);
		void Clear();
		void PushFront(int Element);
		void PushBack(int Element);
	private:
		std::vector<Scheme> Storage;
	};
}