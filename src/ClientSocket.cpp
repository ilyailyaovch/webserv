#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fd, ListenSocket* socket) : 
	_fd(fd), 
	_server(socket), 
	_outputResponse(new Response()), 
	_readCounter(0) {}

ClientSocket::~ClientSocket() {

//    delete _outputResponse;
//    delete &_inputRequest;
}

ClientSocket& ClientSocket::operator = (const ClientSocket& op) {
	if (this != &op) {
		_fd = op._fd;
        _server = op._server;
		_inputRequest = op._inputRequest;
		_outputResponse = op._outputResponse;
	}
	return *this;
}

void ClientSocket::setRequest(int clientSocket) {
    Request* request = new Request();
    request->parseRequest(clientSocket, _server);
    _inputRequest = *request;
}

bool ClientSocket::setResponse(int clientSocket) {
//	std::cout << "fd : " << this->getFD() << " | counter : " << this->getCounter() << std::endl;
    return _outputResponse->generateResponse(*this, clientSocket, _inputRequest, _readCounter);
}
