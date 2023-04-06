#include "SchemeStorage.h"
#include "ImageLoader.h"
#include <locale>
#include <codecvt>
#include <string>
#include <iostream>
#include <imgui_impl_dx11.h>
#include <atlstr.h>
#include "stb_image.h"
#include <algorithm>


using namespace EditorMMImageLoader;

namespace EditorMMSchemeStorage
{
	CStringA GetUtf8String(LPCWSTR lpwz)
	{
		CStringA strRet;

		if (lpwz == nullptr || (*lpwz == 0))
			return strRet;

		int len = WideCharToMultiByte(CP_UTF8, 0, lpwz, -1, NULL, 0, NULL, NULL);

		WideCharToMultiByte(CP_UTF8, 0, lpwz, -1, strRet.GetBufferSetLength(len), len, NULL, NULL);

		strRet.ReleaseBuffer();

		return strRet;
	}

	void SchemeStorage::LoadFileToStorage(const wchar_t* FilePath, ImVec2 Pos)
	{
		if (size_t(FilePath) != 0)
		{
			bool ret = false;
			Scheme Temp;
			Temp.Pos = Pos;
			char test[250];
			stbi_convert_wchar_to_utf8(test, 250, FilePath);
			if (ret = LoadTextureFromFile(test, &Temp.myTexture, &Temp.imageWidth, &Temp.imageHeight))
			{
				IM_ASSERT(ret);
			}
			else
			{
				std::string ErrorMessage = "При попытке загрузить схему произошла ошибка\n";
				ErrorMessage += stbi_failure_reason();
				MessageBoxA(NULL, ErrorMessage.c_str(), "Ошибка", MB_OK);
				return;
			}
			Temp.AspectRatio = (float)Temp.imageWidth / (float)Temp.imageHeight;
			SchemeStorage::Storage.push_back(Temp);
		}
	}
	int SchemeStorage::size()
	{
		return Storage.size();
	}
	void SchemeStorage::Swap(int First, int Second)
	{
		std::iter_swap(Storage.begin() + First, Storage.begin() + Second);
	}
	void SchemeStorage::Delete(int Element)
	{
		Storage.erase(Storage.begin() + Element);
	}
	void SchemeStorage::Clear()
	{
		Storage.clear();
	}
	void SchemeStorage::PushFront(int Element)
	{
		Scheme Mem = Storage[Element];
		Storage.erase(Storage.begin() + Element);
		Storage.push_back(Mem);
	}
	void SchemeStorage::PushBack(int Element)
	{
		Scheme Mem = Storage[Element];
		Storage.erase(Storage.begin() + Element);
		Storage.insert(Storage.begin(), Mem);
	}
}