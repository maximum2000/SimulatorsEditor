#include "ScenarioStorage.h"


namespace ScenarioEditorScenarioStorage
{
	struct Scenario
	{
		std::string Name;
		GUID Guid;
	};
	static std::vector <Scenario> Scenarios;
	static int ActualScenario;
	void ClearScenarioStorage()
	{
		ActualScenario = 0;
		Scenarios.clear();
		Scenarios.push_back({ u8"<Вне сценария>", NULL });
	}
	void AddScenarioStorageElement(std::string name, GUID guid)
	{
		Scenarios.push_back({ name, guid });
	}
	std::vector<std::string> GetScenarioNames()
	{
		std::vector<std::string> ret;
		for (auto const& ent1 : Scenarios)
		{
			ret.push_back(ent1.Name);
		}
		return ret;
	}
	void SetActualScenario(int Scenario)
	{
		ActualScenario = Scenario;
	}
	std::string GetActualGUID()
	{
		GUID guid = Scenarios[ActualScenario].Guid;
		char guid_cstr[39];
		snprintf(guid_cstr, sizeof(guid_cstr),
			"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		if (std::string(guid_cstr) == u8"00000000-0000-0000-0000-000000000000")
			return "";
		return std::string(guid_cstr);
	}
}