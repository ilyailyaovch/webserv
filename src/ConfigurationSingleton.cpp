#include "ConfigurationSingleton.hpp"

ConfigurationSingleton::ConfigurationSingleton() : _tree(NULL) {
	fileParse(fileInit());
}

ConfigurationSingleton*	ConfigurationSingleton::getInstance(void) {
	static ConfigurationSingleton* 	_instance;
	if (!_instance) {
		_instance = new ConfigurationSingleton();
	}
	return _instance;
}

//std::list<std::string> ConfigurationSingleton::split(const std::string& str, std::string myDelim) {
//	std::list<std::string> dest;
//	char* delim = (char *)myDelim.c_str();
//    char* pTempStr = strdup(str.c_str());
//    char* pWord = strtok(pTempStr, delim);
//    while(pWord != NULL) {
//        dest.push_back(pWord);
//        pWord = strtok(NULL, delim);
//    }
//    free(pTempStr);
//	return dest;
//}

std::list<std::string>	ConfigurationSingleton::fileInit() {
	std::ifstream file;
	file.open("configuration.conf",
		std::ios::in | std::ios::ate);
	if (file.fail()) {
		perror("Error : can't open input file");
		exit(1);
	}
	int size = file.tellg();
	char buffer[size];
	file.seekg(0);
	file.read(buffer, size);
	buffer[size] = '\0';

	std::list<std::string> configInfo = split(buffer, "\n");

//	debug / print file
//		for (std::list<std::string>::iterator a = configInfo.begin(); a != configInfo.end(); a++) {
//			std::cout << *a << std::endl;
//		}
//		std::cout << std::endl;

	return configInfo;
}

void ConfigurationSingleton::downgradeConfigList(LocationInfo& localHead, 
	std::list<std::string>::iterator& stringParserIter) {
	LocationInfo *downGrade = new LocationInfo();
	std::list<std::string> infoString = split(*stringParserIter, " \t\r\v\n\f");
	// std::cout << "here open : "<< *stringParserIter << " size : " << infoString.size() << std::endl;
	if (infoString.size() == 2) {
		downGrade->setType(*infoString.begin());
	} else if (infoString.size() == 3) {
		std::list<std::string>::iterator tmpIter = ++infoString.begin();
		// std::cout << "here push : "<< *tmpIter << std::endl;
		downGrade->setType(*infoString.begin());
		downGrade->setLocation(*tmpIter);
	} else {
		exit(1);
	}
	while (true) {
		++(stringParserIter);
		infoString = split(*stringParserIter, " \t\r\v\n\f;");
		if (*infoString.begin() == "#" || (*infoString.begin()).empty()) {
			continue;
		}
		std::list<std::string>::iterator tmpIter = --infoString.end();
		if (*tmpIter == "{" || *tmpIter == "}") {
			if (*tmpIter == "{") {
				// std::cout << "here open : "<< **stringParserIter << " size : " << infoString.size() << std::endl;
				downgradeConfigList(*downGrade, stringParserIter);
				continue;
			} 
			if (*tmpIter == "}") {
				// std::cout << "here close : "<< **stringParserIter << " size : " << infoString.size() << std::endl;
				break;
			} 
		} else if (infoString.size() == 2) {
			std::list<std::string>::iterator tmpIter = ++infoString.begin();
			downGrade->configMapPushBack(*infoString.begin(), *tmpIter);
		} else if (infoString.size() > 2) {
//            printValue("infostring", *infoString.begin());
            std::list<std::string>::iterator tmpIter = infoString.begin();
			std::string stringUnion = *tmpIter++;
            stringUnion += *tmpIter++;
//			for (std::list<std::string>::iterator tmpIter = infoString.begin();
//				tmpIter != infoString.end(); ++tmpIter) {
//					stringUnion += *tmpIter;
//			}
//            printValue("union", stringUnion);
            downGrade->configMapPushBack(stringUnion, *tmpIter);
		} else {
			std::cout << "here open : "<< *stringParserIter << " size : " << infoString.size() << std::endl;
			exit(1);
		}
	}
	localHead.configListPushBack(downGrade);
}
/* debug function / recource printer */
void recoursePrinterConf(LocationInfo* locationInf, int level) {
    std::string tabs = "";
    for (int i = 0; i < level; i++) {
        tabs += "  ";
    }
	std::cout << tabs << BLUE << locationInf->getType() << END << std::endl;
	std::cout << tabs << "type : " << locationInf->getType() << std::endl;
	std::cout << tabs << "lctn : " << locationInf->getLocation() << std::endl;
	std::multimap<std::string, std::string> config = locationInf->getConfigList();
	if (!config.empty()) {
		std::cout << tabs << VIOLET << "Parameters" << END << std::endl;
		for (std::multimap<std::string, std::string>::iterator configIter = config.begin(); 
			configIter != config.end(); ++configIter) {
			std::cout << tabs << tabs << configIter->first <<
				" : " << configIter->second << std::endl;
		}
	}
	std::list<LocationInfo*> location = locationInf->getDownGradeList();
	// std::cout << "size : " << location.size() << std::endl;
	if (!location.empty()) {
		for (std::list<LocationInfo*>::iterator locationIter = location.begin(); 
			locationIter != location.end(); ++locationIter) {
//				std::cout << "-==INCLUDE==-" << std::endl;
				recoursePrinterConf(*locationIter, level + 1);
		}
	}
    std::cout << tabs << BLUE << locationInf->getType() << " end" << END << std::endl;
}
// */

void	ConfigurationSingleton::fileParse(std::list<std::string> inputFile) {
	std::list<std::string>::iterator stringParserIterator = inputFile.begin(); // создал итератор на начало
	LocationInfo* treeHead = new LocationInfo("root", "base"); // создал объект в котором будет храниться голова дерева
	while (stringParserIterator != inputFile.end()) { // парсинг файла до последней строчки
		std::list<std::string> infoString = split(*stringParserIterator, " \t\r\v\n\f");
		if (*stringParserIterator != "" && *(--infoString.end()) == "{") {
			downgradeConfigList(*treeHead, stringParserIterator); // рекурсивная функция
		}
		++stringParserIterator;
	}
	_tree = treeHead; // голову списка сохраняем, как голову дерева

	/* debug / recourse printer for tree */
	std::cout << VIOLET << "Recursive printer for configuration file" << END << std::endl;
	recoursePrinterConf(_tree, 0);
    std::cout << "--------------------------------" << std::endl;
    std::cout << "--------------------------------" << std::endl;
	// */
}
