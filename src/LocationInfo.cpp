#include "LocationInfo.hpp"

LocationInfo::LocationInfo() : _type(""), _location(""), _downGrade(0) {}

LocationInfo::LocationInfo(std::string name, std::string location) 
	: _type(name), _location(location), _downGrade(0) {}

LocationInfo::LocationInfo(const LocationInfo& copy) { *this = copy; }

LocationInfo&	LocationInfo::operator = (const LocationInfo& op) {
	if (this != &op) {
		_type = op._type;
		_config = op._config;
	}
	return *this;
}

void	LocationInfo::configMapPushBack(std::string key, std::string value) {
	_config.insert(std::pair<std::string, std::string>(key, value));
}

void	LocationInfo::configListPushBack(LocationInfo* config) {
	_downGrade.push_back(config);
}