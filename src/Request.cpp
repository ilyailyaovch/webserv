//
// Created by Attack Cristina on 07.09.2022.
//

#include "Request.hpp"

Request::Request() : _message("") {}

Request::Request(std::string method) : _method(method), _message("") {}

Request::Request(const Request &copy) { *this = copy; }

Request &Request::operator=(const Request &src) {
    if (this != &src) {
        _method = src._method;
        _message = src._message;
        _body = src._body;
        _maxBodySize = src._maxBodySize;
    }
    return  *this;
}

Request::~Request() {}

void Request::bodyMapPushBack(std::string key, std::string value) {
    _body.insert(std::pair<std::string, std::string>(key, value));
}

bool Request::readToBuffer(int clientSocket, bool isHeader) {
    char sym;
    int byteIn = recv(clientSocket, &sym, 1, 0);
    if (byteIn > EMPTY_BUFFER) {
        _message += sym;
        if (isHeader && _message.length() > 4 && _message.substr(_message.length() - 4) == "\r\n\r\n") {
            return false;
        }
    } else if (byteIn == EMPTY_BUFFER) {
        return false;
    } else if (byteIn == SOCKET_ERROR) {
        perror("Error : failure reading from TCP");
    }
    return true;
}

void Request::setMaxBodySize(const std::string sizeFromConfig) {
    int exp = 1;
    int found;
    std::string tmp;

    if (sizeFromConfig.find("K") != std::string::npos || sizeFromConfig.find("k") != std::string::npos) {
        found = sizeFromConfig.find("K") != std::string::npos ? sizeFromConfig.find("K") : sizeFromConfig.find("k");
        exp = 1000;
    } else if (sizeFromConfig.find("M") != std::string::npos || sizeFromConfig.find("m") != std::string::npos) {
        found = sizeFromConfig.find("M") != std::string::npos ? sizeFromConfig.find("M") : sizeFromConfig.find("m");
        exp = 1000000;
    } else {
        found = sizeFromConfig.length();
    }
    tmp = sizeFromConfig.substr(0, found);

    _maxBodySize = (std::stod(tmp) != 0 && std::stod(tmp) < std::numeric_limits<size_t>::max() / exp) ? std::stod(tmp) * exp : MAX_BODY_SIZE;
}

void Request::parseRequest(int clientSocket, ListenSocket* server) {
    // посимвольное считывание шапки запроса
    server->getConfigList().count("client_max_body_size") ? setMaxBodySize(server->getConfigList().find("client_max_body_size")->second) : setMaxBodySize(MAX_BODY_SIZE);
    printValue("maxBodySize", std::to_string(_maxBodySize));

    while (readToBuffer(clientSocket, true)) {}
    _message[_message.length()] = '\0';

    // сплит шапки по строкам
    std::list<std::string> rawData = split(_message, "\n");

    // сплит первой строки - метод, ссылка, версия протокола
    std::list<std::string> firstLine = split(*(rawData.begin()), " ");
    std::list<std::string>::const_iterator it = firstLine.begin();
    _method = *(it++);
    bodyMapPushBack("Request-URI", *(it++));
    bodyMapPushBack("HTTP-Version", *it);


    // создаем мапу из шапки
    it = ++rawData.begin();
    for(; it != rawData.end() && (*it).length() > 1; ++it) {
        bodyMapPushBack((*it).substr(0, (*it).find(":")), (*it).substr((*it).find(" ") + 1));
    }

    // если не ГЕТ, то посимвольно считываем сообщение после шапки
    _message = "";
    if (_method.compare("GET")) {
        for (int i = 0; i < stoi(_body.find("Content-Length")->second, nullptr, 10); i++) {
            readToBuffer(clientSocket, false);
        }
        _message[_message.length()] = '\0';
    }
    std::cout << VIOLET << "======= HTTP REQUEST =======" << END << std::endl;
    for (std::map<std::string, std::string>::iterator it = _body.begin();
         it != _body.end(); ++it) {
        std::cout << BLUE << it->first << ": " << END << it->second << std::endl;
    }
    printValue("Message", _message);
    std::cout << VIOLET << "======= ============ =======" << END << std::endl;
}
