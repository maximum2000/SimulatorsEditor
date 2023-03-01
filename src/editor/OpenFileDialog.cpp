#include "OpenFileDialog.h"
#include <shobjidl.h> 

namespace ScenarioEditorFileDialog
{
	const wchar_t* OpenFileDialog()
	{
        PWSTR pszFilePath{};
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
            COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr))
        {
            IFileOpenDialog* pFileOpen;

            // Create the FileOpenDialog object.
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

            if (SUCCEEDED(hr))
            {
                // Show the Open dialog box.
                hr = pFileOpen->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    IShellItem* pItem;
                    hr = pFileOpen->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    }
                }
                pFileOpen->Release();
            }
            CoUninitialize();
        }
        return pszFilePath;
	}
}