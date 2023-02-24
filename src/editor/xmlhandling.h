//
// XML-Canvas connection
//

#pragma once

namespace ScenariosEditorXML
{
	class ScenariosDOM
	{
	public:
		void LoadFrom(const wchar_t* path);
		bool SaveTo(const wchar_t* path);
		bool CheckFile(const wchar_t* path);
	private:
	};
}