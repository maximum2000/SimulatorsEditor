#include "ScenarioElement.cpp"

class Attribute
{
public:
protected:
	std::string Name, ValueS;
	float ValueF;
};


class Uzel : ScenarioElement
{
public:
protected:

};
class Start : ScenarioElement
{
public:
protected:
	Attribute Name, Description, start_user_x, start_user_y, start_user_z, start_user_angle, start_scene, start_state_file;
};
class Clear : ScenarioElement
{
public:
protected:
};
class Message : ScenarioElement
{
public:
protected:
	Attribute Text, WithButtonOk;
};
class Sound : ScenarioElement
{
public:
protected:
	Attribute Name;
};
class Script : ScenarioElement
{
public:
protected:
	Attribute function, argumentFloat, argumentString;
};
class Pilon : ScenarioElement
{
public:
protected:
	Attribute objectName, x, y, z;
};
class Arrow : ScenarioElement
{
public:
protected:
	Attribute CenterObjectName, LookAtObjectName, NeedPush;
};
class Pause : ScenarioElement
{
public:
protected:
	Attribute Duration;
};
class Push : ScenarioElement
{
public:
protected:
	Attribute Names;
};
class Select : ScenarioElement
{
public:
protected:
	Attribute Names, NeedPush;
};
class Outcome : ScenarioElement
{
public:
protected:
	Attribute messageToReport, isError, causeToReport, LossesHealthAndDead, LossesEcology,
		LossesEconomic, LossesOther, Detection, Diagnostics, MakingDecisions, ActionsPerformed;
};
class Answer : ScenarioElement
{
public:
protected:
	Attribute Question, Answer1, Answer2, Answer3, Answer4, MyActionsButton;
};
class VariableValue : ScenarioElement
{
public:
protected:
	Attribute variable_X, variable_A, variable_fix_A, variable_B, variable_fix_B;
};
class Random : ScenarioElement
{
public:
protected:
	Attribute need;
};
class Danger : ScenarioElement
{
public:
protected:
	Attribute gas, object, distance_to_object;
};