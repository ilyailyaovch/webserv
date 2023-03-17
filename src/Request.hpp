//
// Created by Attack Cristina on 07.09.2022.
//

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <map>
# include <string>
# include <list>
# include <iostream>
# include <cstring>

# include <sys/types.h>
# include <sys/socket.h>
# include "main.hpp"
# include "ListenSocket.hpp"

# define END_ERROR          1
# define EMPTY_BUFFER		0
# define SOCKET_ERROR       -1
# define MAX_SHORT          32767
# define READ_BUFFER_SIZE   3000

class ListenSocket;

class Request {
private:
    std::map<std::string, std::string>  _body;
    std::string                         _method;
    std::string                         _message;
    size_t                              _maxBodySize;

    bool                    readToBuffer( int clientSocket, bool isHeader );

public:
    Request();
    Request(std::string method);
    Request(const Request& copy);
    Request& operator=(const Request& src);
    ~Request();

    const std::map<std::string, std::string>&   getBody() const  { return _body; };
    const std::string&                          getMethod() const { return _method; };
    const std::string&                          getMessage() const { return _message; };
    const size_t&                               getMaxBodySize() const { return _maxBodySize; };

    void setMethod( const std::string method ) { _method = method; };
    void setMaxBodySize( const std::string sizeFromConfig );
    void setMaxBodySize ( const size_t size ) { _maxBodySize = size; };

    void bodyMapPushBack(std::string key, std::string value);
    void parseRequest(int clientSocket, ListenSocket* server);
};

#endif //REQUEST_HPP
