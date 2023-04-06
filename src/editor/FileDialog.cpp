#include "FileDialog.h"
#include <shobjidl.h> 
#include <iostream>

namespace EditorFileDialog
{
	const COMDLG_FILTERSPEC ImageFilter[] {
					{ L"Изображение", L"*.jpeg;*.jpg;*.png;*.bmp"},
					{ L"Все файлы", L"*.*"},
					};
	const wchar_t* ImageDefaultExtension {L"jpeg;jpg;png;bmp"};

	// DialogType: 0 - Open, 1 - Save; FormatType: 0 - Image
	const wchar_t* FileDialog(int DialogType, int FormatType, int SelectedFormat)
	{
		PWSTR pszFilePath{};
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileDialog* pFile;
			switch (DialogType)
			{
			case 0:
				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
					IID_IFileOpenDialog, reinterpret_cast<void**>(&pFile));
				break;
			case 1:
				hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
					IID_IFileSaveDialog, reinterpret_cast<void**>(&pFile));
				break;
			default:
				return L"";
			}
			if (SUCCEEDED(hr))
			{
				switch (FormatType)
				{
				case 0: 
					pFile->SetFileTypes(2, ImageFilter);
					pFile->SetDefaultExtension(ImageDefaultExtension);
					break;
				}
				pFile->SetFileTypeIndex(SelectedFormat);
				hr = pFile->Show(NULL);

				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFile->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					}
				}
				pFile->Release();
			}
			CoUninitialize();
		}
		return pszFilePath;
	}
	const wchar_t* OpenFileDialog(int FormatType, int SelectedFormat)
	{
		return FileDialog(0, FormatType, SelectedFormat);
	}
	const wchar_t* SaveFileDialog(int FormatType, int SelectedFormat)
	{
		return FileDialog(1, FormatType, SelectedFormat);
	}
}