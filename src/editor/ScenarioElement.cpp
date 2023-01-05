#include <string>
#include <vector>
class ScenarioElement
{
public:
	
protected:
	//static constexpr const char* name = "demo";
	std::string ScenarioGUID, name, caption;
	float x, y, alfa;
	std::vector<int> pins;
};