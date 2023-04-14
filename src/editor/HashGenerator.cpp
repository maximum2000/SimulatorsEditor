#include "HashGenerator.h"
#include <functional>
#include <string>
#include <ctime>    
#include <chrono>
#include "sha256.h"
#include <iostream>
namespace EditorMMHashGenerator
{
	std::string GenerateHash(int operation)
	{
		time_t rawtime;
		tm timeinfo;
		char buffer[256];
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", &timeinfo);
		const auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count() % 1000;
		std::string str(buffer);
		str += std::to_string(ms);
		str += std::to_string(operation);
		return EditorMMsha256::SHA256(str);
	}
}