#include "ListenSocket.hpp"

//* debug function / recource printer изначально подавать level=0
//void recoursePrinterConf(LocationInfo* locationInf, int level) {
//    std::string tabs = "";
//    for (int i = 0; i < level; i++) {
//        tabs += "  ";
//    }
//    std::cout << tabs << BLUE << locationInf->getType() << END << std::endl;
//    std::cout << tabs << "type : " << locationInf->getType() << std::endl;
//    std::cout << tabs << "lctn : " << locationInf->getLocation() << std::endl;
//    std::multimap<std::string, std::string> config = locationInf->getConfigList();
//    if (!config.empty()) {
//        std::cout << tabs << VIOLET << "Parameters" << END << std::endl;
//        for (std::multimap<std::string, std::string>::iterator configIter = config.begin();
//             configIter != config.end(); ++configIter) {
//            std::cout << tabs << tabs << configIter->first <<
//                      " :\t" << configIter->second << std::endl;
//        }
//    }
//    std::list<LocationInfo*> location = locationInf->getDownGradeList();
//    // std::cout << "size : " << location.size() << std::endl;
//    if (!location.empty()) {
//        for (std::list<LocationInfo*>::iterator locationIter = location.begin();
//             locationIter != location.end(); ++locationIter) {
////				std::cout << "-==INCLUDE==-" << std::endl;
//            recoursePrinterConf(*locationIter, level + 1);
//        }
//    }
//    std::cout << tabs << BLUE << locationInf->getType() << " end" << END << std::endl;
//}

ListenSocket::ListenSocket(LocationInfo* server) 
	// :
	// _name(server->getConfigList().find("server_name")->second),
	// _port(stoi(*(++(split(server->getConfigList().find("listen")->second, ":").begin())))),
	// _ip(*(split(server->getConfigList().find("listen")->second, ":").begin())),
	// _locations(server->getDownGradeList())
{
    _configList = server->getConfigList();
//	std::multimap<std::string, std::string> mapWithServerInfo = server->getConfigList();
    if (_configList.count("autoindex")) {
        if (!_configList.find("autoindex")->second.compare("on")) {
            _autoindex = 1;
        } else {
            _autoindex = 0;
        }
    } else {
        _autoindex = 0;
    }
	_name = _configList.find("server_name")->second;
	std::list<std::string> splitForIpPort = split(server->getConfigList().find("listen")->second, ":");
	std::list<std::string>::iterator IpPortIter = splitForIpPort.begin();
	_ip = *(IpPortIter++);
	_port = stoi(*(IpPortIter));
	_locations = server->getDownGradeList();

	/* debug / constructor printer */
//    std::stringstream info;
//    info << *this;
//    printMsg(0, "", info.str());
//	 std::cout << *this << "HERE" << std::endl;

//    recoursePrinter(server);
}
ListenSocket&	ListenSocket::operator = (const ListenSocket& op) {
	if (this != &op) {
		_port = op._port;
		_ip = op._ip;
		_name = op._name;
		_locations = op._locations;
        _configList = op._configList;
	}
	return *this;
}

//std::list<std::string> ListenSocket::split(const std::string& str, std::string myDelim) {
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

