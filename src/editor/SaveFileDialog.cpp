#include "SaveFileDialog.h"
#include <shobjidl.h> 

namespace ScenarioEditorFileDialog
{
    const wchar_t* SaveFileDialog()
    {
        PWSTR pszFilePath{};
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
            COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr))
        {
            IFileSaveDialog* pFileSave;

            // Create the FileSaveDialog object.
            hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
                IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

            if (SUCCEEDED(hr))
            {
                // Show the Save dialog box.
                COMDLG_FILTERSPEC rgSpec[] =
                {
                    { L"Матмодель", L"*.model"},
                    { L"Все файлы", L"*.*"},
                };
                pFileSave->SetFileTypes(2,rgSpec);
                pFileSave->SetFileTypeIndex(1);
                pFileSave->SetDefaultExtension(L"model");
                hr = pFileSave->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    IShellItem* pItem;
                    hr = pFileSave->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    }
                }
                pFileSave->Release();
            }
            CoUninitialize();
        }
        return pszFilePath;
    }
}