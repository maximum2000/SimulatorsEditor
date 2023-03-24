//
// XML-Storage connection
//

#pragma once

namespace ScenariosEditorXML
{
	class ScenariosDOM
	{
	public:
		void LoadFrom(const wchar_t* path);
		bool SaveTo(const wchar_t* path);
		// When trying to open file we should check if it is xml file
		bool CheckFile(const wchar_t* path);
	private:
	};
}