#ifndef CONFIGURATIONSINGLETON_HPP
# define CONFIGURATIONSINGLETON_HPP

# include "LocationInfo.hpp"
# include "main.hpp"

# include <iostream>

class LocationInfo;

class ConfigurationSingleton {
	private :
		/* variables */
		LocationInfo*	_tree;

		/* Constructors */
		ConfigurationSingleton(void);
		ConfigurationSingleton(const ConfigurationSingleton& copy);
		ConfigurationSingleton& operator = (const ConfigurationSingleton& op);
		~ConfigurationSingleton(void) {}

		/* functions */
//		std::list<std::string>  split(const std::string& str, std::string myDelim);
		std::list<std::string>	fileInit();

		void	fileParse(std::list<std::string> inputFile);
		void 	downgradeConfigList(LocationInfo& localHead, std::list<std::string>::iterator& stringParserIter);

	public :
		static ConfigurationSingleton* getInstance(void);
		LocationInfo* getTreeHead(void) const { return _tree; } 
} ;

#endif