#include "Server.hpp"

Server::Server() {
    memset(_fds, 0, sizeof(_fds));
    char hostname[HOSTNAME_LEN];
    _hostname = (gethostname(hostname, HOSTNAME_LEN) != -1) ? hostname : "\0";
}

/* debag funtions */
void    printFdsArray(pollfd *fds, int nfds) {
    std::cout << std::endl << "=========== open client sockets ===========" << std::endl;
    for (int i = 0; i < nfds; i++) {
        std::cout << i << " | fd : " << fds[i].fd << " | event : " 
            << fds[i].events << " | revent : " << fds[i].revents << std::endl;   
    }
    std::cout << "=================== end ===================" << std::endl << std::endl;
}

//void recoursePrinter(LocationInfo* locationInf) {
//    std::cout << "-==GENERAL==-" << std::endl;
//    std::cout << "type : " << locationInf->getType() << std::endl;
//    std::cout << "lctn : " << locationInf->getLocation() << std::endl;
//    std::multimap<std::string, std::string> config = locationInf->getConfigList();
//    if (!config.empty()) {
//        std::cout << "-==DATA==-" << std::endl;
//        for (std::multimap<std::string, std::string>::iterator configIter = config.begin();
//             configIter != config.end(); ++configIter) {
//            std::cout << "parameter : " << configIter->first <<
//                      " | value : " << configIter->second << std::endl;
//        }
//    }
//    std::list<LocationInfo*> location = locationInf->getDownGradeList();
//    // std::cout << "size : " << location.size() << std::endl;
//    if (!location.empty()) {
//        for (std::list<LocationInfo*>::iterator locationIter = location.begin();
//             locationIter != location.end(); ++locationIter) {
//            std::cout << "-==INCLUDE==-" << std::endl;
//            recoursePrinter(*locationIter);
//        }
//    }
//    std::cout << "-==END of " << locationInf->getType() << " ==-" << std::endl;
//}

/* class constructors/destructors */

/* private class functiones */
sockaddr_in Server::setIdInfo(const ListenSocket& serverInfo) {
	struct sockaddr_in addr;    // информация об IP адресе	
	memset(&addr, 0, sizeof(addr));
	/* https://www.opennet.ru/cgi-bin/opennet/man.cgi?topic=socket&category=2 */ 
	addr.sin_family = AF_INET;  // IPv4 протоколы Интернет
	addr.sin_addr.s_addr = inet_addr(serverInfo.getIP().c_str());    
	addr.sin_port = htons(serverInfo.getPort());
	return addr;
}

bool Server::findInListenSockets(int fd) {
    for (int i = 0; i < _numOfListenSocket; ++i) {
        if (_fds[i].fd == fd) {
            return true;
        }
    }
    return false;
}

int Server::addNewClientSocket(int &nfds, int i) {
	int clientSocket = accept(_fds[i].fd, NULL, NULL);
	if (clientSocket == SOCKET_ERROR) {
		perror("Error : TCP connection failure");
		return SOCKET_ERROR;
	}
//	std::cout << "Succsessful connection : " << _fds[i].fd << std::endl;
    printMsg(i, clientSocket, "on descriptor ", " client successfully connected");
	_fds[nfds].fd = clientSocket;
	_fds[nfds].events = POLLIN;
	_fds[nfds].revents = 0;
    // привязка серверного сокета к клиентскому
    std::list<ListenSocket*>::const_iterator it = _activeServers.begin();
    for (; it != _activeServers.end(); it++) {
        if ((*it)->getFd() == _fds[i].fd) {
            _activeClients.push_back(new ClientSocket(clientSocket, (*it)));
        }
    }
	nfds++;
	return 0;
}

void Server::closeClientSocket(int &nfds, int &i) {
	close(_fds[i].fd);
    for (std::list<ClientSocket *>::const_iterator it = _activeClients.begin(); it != _activeClients.end(); it++) {
        if ((*it)->getFD() == _fds[i].fd) {
            printMsg((*it)->getServer()->getNb(), (*it)->getFD(), "on descriptor ", " client disconnected");
            _activeClients.erase(it);
        }
    }
	for (int j = i; j < nfds - 1; j++) {
		_fds[j] = _fds[j + 1];
	}
	--nfds;
	--i;
}

int Server::waitForPoll(int nfds) {
	int pollStatus = poll(_fds, nfds, -1);
	if (pollStatus == -1) {
        printMsg(-1, -1, RED, "server error: poll failure", "");
//		perror("Error : poll failure");
	} else if (!pollStatus) {
        printMsg(-1, -1, RED, "server error: poll timeout", "");
//		perror("Error : poll timeout");
	}
	return pollStatus;
}

/* public clss functiones */
int Server::initListningSocket(const ListenSocket&  serverInfo) {
	struct sockaddr_in addr = setIdInfo(serverInfo);
	int listningSocket = socket(addr.sin_family, SOCK_STREAM, 0);
	if (listningSocket == SOCKET_ERROR) {
	    perror("Error : cannot create a socket");
	    return SOCKET_ERROR;
	}	
	/* фикс проблемы с "повисшим" bind */
	int enable = 1;
	signal(SIGPIPE, SIG_IGN);
	// setsockopt(listningSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	// signal(SIGPIPE, SIG_IGN);
	if (setsockopt(listningSocket, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(enable)) < 0) {
	    return SOCKET_ERROR;
	}	
	if (setsockopt(listningSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
	    return SOCKET_ERROR;
	}
	// int opt_len = sizeof(int);
	// int err = getsockopt(listningSocket, SOL_SOCKET, SO_LINGER, &enable, (socklen_t *)&opt_len);
	// std::cout << "get : " << err << std::endl;
	// if (err == -1) {
	// 	std::cout << "errno : " << errno << std::endl;
	// }
	/* привязка сокета IP-адресу */
	if (bind(listningSocket, (const sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
	    perror("Error : cannot bind a socket");
	    close(listningSocket);
	    return SOCKET_ERROR;
	}
	/* Подготовим сокета к принятию входящих соединений от клиентов. */
	if (listen(listningSocket, SOMAXCONN) == SOCKET_ERROR) {  
	    perror("Error : listen failure");
	    close(listningSocket);
	    return SOCKET_ERROR;
	}
	return listningSocket;
}

void Server::setRequestByFd(int fd) {
    for (std::list<ClientSocket*>::const_iterator it = _activeClients.begin(); it != _activeClients.end(); it++) {
        if ((*it)->getFD() == fd) {
            (*it)->setRequest(fd);
        }
    }
}

bool Server::setResponseByFd(int fd) {
    for (std::list<ClientSocket*>::const_iterator it = _activeClients.begin(); it != _activeClients.end(); it++) {
        if ((*it)->getFD() == fd) {
            return (*it)->setResponse(fd);
        }
    }
    return false;
}

void Server::run() {
    /* заполняем струтуру в которой будем хранить информацию о состоянии установленных соединений */
    std::cout << timestamp() << YELLOW << "Welcome to the webserver, (c) acristin, aarchiba, utygett. Host machine: "
            << END << _hostname << YELLOW << ", configuration settings: " << END << "configuration.conf"
            << std::endl << GRN << "To quit press CTRL+C" << END << std::endl;
    createListSockets();
    int nfds = _numOfListenSocket;
    while (true) {
        /* ожидаем изменения состояния открытого сокета */
        if (waitForPoll(nfds) <= 0) {
			break;
		}
        /* ожидает запрос на установку TCP-соединения от удаленного хоста. */
        for (int i = 0; i < nfds; ++i) {
            if (_fds[i].revents == 0) {
                continue;
            } else if (findInListenSockets(_fds[i].fd)) {
				if (addNewClientSocket(nfds, i)) {
					continue;
				}
            } else if (_fds[i].revents == POLLIN) {
                setRequestByFd(_fds[i].fd);
                _fds[i].events = POLLOUT;
                _fds[i].revents = 0;
            } else if (_fds[i].revents == POLLOUT) {
                if (setResponseByFd(_fds[i].fd)) {
                    _fds[i].events = POLLIN;
                }
                _fds[i].revents = 0;
            } else if (_fds[i].revents != POLLOUT && _fds[i].revents != POLLIN ) {
				//debug ! ! ! !
                printMsg(-1, _fds[i].revents, VIOLET, "server error: close client socket with revent: ", "");
//				std::cout << "revent : " << _fds[i].revents << std::endl;
				//debug ! ! ! !
                closeClientSocket(nfds, i);
            } else {
                printMsg(-1, -1, RED, "server error: wrong revent", "");
//                perror("Error : wrong revent");
                break;
            }
        }
    }
    closeListSockets();
}

void    Server::createListSockets() {
	ConfigurationSingleton* infoFromConfig = ConfigurationSingleton::getInstance();
	LocationInfo* root = (infoFromConfig->getTreeHead());
	/* поиск пространства с информацией о listen socket */
	while (!root->getDownGradeList().empty() && (*(root->getDownGradeList().begin()))->getType() != "server") {
		root = *(root->getDownGradeList().begin());
	}
	/* лист с информацией о listen socket ах, который надо обработать при инициализации сокетов */
	std::list<LocationInfo*>::const_iterator listOfServerIter = root->getDownGradeList().begin();
	_numOfListenSocket = root->getDownGradeList().size();
	/* инифиализация сокетов */
    for (int i = 0; i < _numOfListenSocket; ++i) {
		ListenSocket *newSocketFromConfig = new ListenSocket(*(listOfServerIter++)); // не забыть добавить делете
        int tmpFd = initListningSocket(*newSocketFromConfig);
        std::stringstream info;
        info << *newSocketFromConfig;
        printMsg(i, tmpFd, "on descriptor ", info.str());
        if (tmpFd != SOCKET_ERROR) {
            _fds[i].fd = tmpFd;
            _fds[i].events = POLLIN;
            newSocketFromConfig->setNb(i);
            newSocketFromConfig->setFd(tmpFd);
        }
        _activeServers.push_back(newSocketFromConfig);
    }
}

void    Server::closeListSockets() {
    for (int i = 0; i < _numOfListenSocket; ++i) {
        close (_fds[i].fd);
    }
}
