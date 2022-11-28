#include "ScenarioElement.cpp"
class DemoObj : ScenarioElement
{
public:
	DemoObj()
	{

	}
protected:
	static constexpr const char* name = "demo";
	int param;
	const char* TexturePath()
	{
		return "test";
	}
};