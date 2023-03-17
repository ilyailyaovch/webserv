#ifndef CLIENT_SOCKET_HPP
# define CLIENT_SOCKET_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "ListenSocket.hpp"

class Response;
class Request;
class ListenSocket;

class ClientSocket {
	private:
		/* variables */
		int 			_fd;
		Request			_inputRequest;
		Response*		_outputResponse;
        ListenSocket*   _server;
		int				_readCounter;

	public:
		/* constructors */
		ClientSocket(int fd, ListenSocket* socket);
		ClientSocket(const ClientSocket& copy)  { *this = copy; };
		ClientSocket& operator = (const ClientSocket& op);
		~ClientSocket();

		/* public functions */
        void setRequest(int clientSocket);
        bool setResponse(int clientSocket);

		/* getters */
		int	getFD(void) const 						{ return _fd; }
		const Request&	getRequest(void) const		{ return _inputRequest; }
		const Response*	getResponse(void) const		{ return _outputResponse; }
        const ListenSocket* getServer(void) const 	{ return _server; }
		int	getCounter(void) const 					{ return _readCounter; }				
};

#endif