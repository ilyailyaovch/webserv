#ifndef LOCATION_INFO_HPP
# define LOCATION_INFO_HPP

# include <fstream>
# include <map>
# include <string>
# include <cstring>
# include <list>

class LocationInfo {
	private :
		/* variables */
		std::string 								_type;
		std::string 								_location;
		std::multimap<std::string, std::string> 	_config;
		std::list<LocationInfo*>					_downGrade;
		
	public :
		LocationInfo();
		LocationInfo(std::string name, std::string location);
		LocationInfo(const LocationInfo& copy);
		LocationInfo& operator = (const LocationInfo& op);
		~LocationInfo() {}

		void	configMapPushBack(std::string key, std::string value);
		void	configListPushBack(LocationInfo* config);

		void	setType(std::string type) 			{ _type = type; }
		void	setLocation(std::string location) 	{ _location = location; }

		const std::string& getType() const 										{ return _type; };
		const std::string& getLocation() const 									{ return _location; };
		const std::list<LocationInfo*>& getDownGradeList() const 				{ return _downGrade; };
		const std::multimap<std::string, std::string>& getConfigList() const 	{ return _config; };
};

static std::ostream& operator << (std::ostream& os, const std::multimap<std::string, std::string>& src) {
    if (!src.empty()) {
        for (std::multimap<std::string, std::string>::const_iterator it = src.begin(); it != src.end(); ++it) {
            os << "\t" << it->first << " : " << it->second << std::endl;
        }
    } else {
        os << "\tEmpty map" << std::endl;
    }
    return os;
}

static std::ostream& operator << (std::ostream& os, const std::list<LocationInfo*>& src) {
    if (!src.empty()) {
        for (std::list<LocationInfo*>::const_iterator it = src.begin(); it != src.end(); ++it) {
            os << "\t" << (*it) << std::endl;
        }
    } else {
        os << "\tEmpty list" << std::endl;
    }
    return os;
}

static std::ostream& operator << (std::ostream& os, const LocationInfo& src) {
    os	<< "================== start ==================" << std::endl
          << "Type : " << src.getType() << std::endl
          << "Location : " << src.getLocation() << std::endl
          << "Config : " << src.getConfigList() << std::endl
          << "List of locations inside : " << src.getLocation() << std::endl
          << "=================== end ===================" << std::endl;
    return os;
}

#endif