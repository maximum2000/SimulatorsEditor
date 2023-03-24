#include "ElementsData.h"
#include "render.h"
#include <imgui_impl_win32.h>
#include <map>
#include <vector>

namespace ScenariosEditorElementsData
{
	// Elements data
	static const std::vector<const char*> ElementNames = { "uzel", "Start", "clear", "message", "sound", "script", "pilon", "arrow",
										"pause", "push", "select", "outcome", "answer", "variable_value", "random", "danger" };
	static const std::vector<int> ElementTypes = { 1, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 11, 11, 11, 11 }; // pin amount and location
	static std::map <std::string, std::string> AttributesHints = {
		{u8"Name", u8"Название сценария"},
		{u8"Discription", u8"Описание сценария"},
		{u8"start_user_x", u8"Стартовое положение x"},
		{u8"start_user_y", u8"Стартовое положение y"},
		{u8"start_user_z", u8"Стартовое положение z"},
		{u8"start_user_angle", u8"Стартовое направление взгляда"},
		{u8"start_scene", u8"Стартовая сцена"},
		{u8"start_state_file", u8"Файл с состоянием"},
		{u8"Text", u8"Текст"},
		{u8"WithButtonOk", u8"True - с клавишей ОК\nFalse - Без неё"},
		{u8"name", u8"Название звука"},
		{u8"function", u8"Имя функции"},
		{u8"argumentFloat", u8"Аргумент число"},
		{u8"argumentString", u8"Аргумент строка"},
		{u8"objectName", u8"Имя объекта, куда поставить. Если пусто, значит по координатам"},
		{u8"x", u8"Координата x"},
		{u8"y", u8"Координата y"},
		{u8"z", u8"Координата z"},
		{u8"CenterObjectName", u8"Имя объекта"},
		{u8"LookAtObjectName", u8"Имя объекта"},
		{u8"NeedPush", u8"False - нет, True - да"},
		{u8"Duration", u8"Число секунд"},
		{u8"Names", u8"Имена объектов через \";\""},
		{u8"messageToReport", u8"Описание последствия"},
		{u8"isError", u8"True - Ошибка\nFalse - Ошибки нет"},
		{u8"causeToReport", u8"Описание причины"},
		{u8"LossesHealthAndDead", u8"Потери здоровья и смерти"},
		{u8"LossesEcology", u8"Потери экологические"},
		{u8"LossesEconomic", u8"Потери экономические"},
		{u8"LossesOther", u8"Потери другие"},
		{u8"Detection", u8"True - С учётом обнаружения\nFalse - без"},
		{u8"Diagnostics", u8"True - С учётом диагностики\nFalse - без"},
		{u8"MakingDecisions", u8"True - С учётом принятия решений\nFalse - без"},
		{u8"ActionsPerformed", u8"True - С учётом выполняемых действий\nFalse - без"},
		{u8"Question", u8"Вопрос"},
		{u8"Answer1", u8"Правильный ответ"},
		{u8"Answer2", u8"Неправильный ответ 1"},
		{u8"Answer3", u8"Неправильный ответ 2"},
		{u8"Answer4", u8"Неправильный ответ 3"},
		{u8"MyActionsButton", u8"True - С кнопкой \"Мои действия\"\nFalse - без"},
		{u8"variable_X", u8"Переменная x"},
		{u8"variable_A", u8"Переменная A, если число - значит число, если строка - значит значение переменной"},
		{u8"variable_B", u8"Переменная B, если число - значит число, если строка - значит значение переменной"},
		{u8"need", u8"Переменная (0..1)"},
		{u8"gas", u8"Переменная (0..1)"},
		{u8"object", u8"Имя объекта"},
		{u8"distance_to_object", u8"Минимальная дистанция"}
	};
	static std::vector<Texture> Textures;
	static float TextureZoom = 2; // used for texture scaling

	// Load texture from file, should change later
	void TempLoad()
	{
		for (int i = 0; i < ElementNames.size(); i++)
		{
			bool ret = false;
			Texture Temp{};
			while (!ret)
			{
				if (ret = ScenariosEditorRender::LoadTextureFromFile((std::string(u8"img/") + std::string(ElementNames[i]) + u8".png").c_str(), &Temp.Payload, &Temp.Width, &Temp.Height))
				{
					IM_ASSERT(ret);
				}
			}
			Temp.Height *= TextureZoom;
			Temp.Width *= TextureZoom;
			Textures.push_back(Temp);
		}
	}

	void ElementsData::Initialization()
	{
		TempLoad();
	}

	int ElementsData::NumberOfElements()
	{
		return (int)ElementNames.size();
	}

	const char* ElementsData::GetElementName(int index)
	{
		return ElementNames[index];
	}
	Texture ElementsData::GetElementTexture(int index, float Zoom)
	{
		Texture ret = Textures[index];
		ret.Height *= Zoom;
		ret.Width *= Zoom;
		return ret;
	}
	int ElementsData::GetElementType(int index)
	{
		return ElementTypes[index];
	}
	std::string ElementsData::GetAttributeHint(std::string attribute)
	{
		return AttributesHints[attribute];
	}
}