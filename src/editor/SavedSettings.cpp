#include "SavedSettings.h"
#include <Shlwapi.h>
#include <Shlobj_core.h>
#include "pugixml.hpp"
#pragma execution_character_set("utf-8")

namespace ScenarioEditorSavedSettings
{
	std::wstring SettingsPath;
	pugi::xml_document Model;
	std::string toUtf8(const std::wstring& str)
	{
		std::string ret;
		int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
		if (len > 0)
		{
			ret.resize(len);
			WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), &ret[0], len, NULL, NULL);
		}
		return ret;
	}
	void Initialization()
	{
		TCHAR szPath[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath)))
		{
			PathAppendW(szPath, TEXT("MMEditor"));
			if (CreateDirectory(szPath, NULL) ||
				ERROR_ALREADY_EXISTS == GetLastError())
			{
				PathAppendW(szPath, TEXT("SavedSettings"));
				SettingsPath = szPath;
			}
			Model.load_file(SettingsPath.c_str());
		}
	}
	void AddToRecentFiles(std::wstring Path)
	{
		std::string PathUTF = toUtf8(Path);
		if (Model.child("RecentFiles") == nullptr)
			Model.append_child("RecentFiles");
		for (pugi::xml_node File : Model.child("RecentFiles").children())
		{
			if (std::string(File.text().as_string()) == PathUTF) File.parent().remove_child(File);
		}
		int n = Model.child("RecentFiles").select_nodes("File").size();
		if (n == 0)
		{
			Model.child("RecentFiles").append_child("File").text() = PathUTF.c_str();
		}
		else
		{
			Model.child("RecentFiles").insert_child_before("File", Model.child("RecentFiles").first_child()).text() = PathUTF.c_str();
			if (n >= 5)
				Model.child("RecentFiles").remove_child(Model.child("RecentFiles").last_child());
		}
		Model.save_file(SettingsPath.c_str());
	}
	std::vector<std::string> GetRecentFiles()
	{
		std::vector<std::string> Ret;
		for (pugi::xml_node File : Model.child("RecentFiles").children())
		{
			Ret.push_back(File.text().as_string());
		}
		return Ret;
	}
}