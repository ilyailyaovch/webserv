#ifndef LISTEN_SOCKET_HPP
# define LISTEN_SOCKET_HPP

# include <fstream>
# include <iostream>
# include <sstream>
# include "LocationInfo.hpp"
# include "main.hpp"

class ListenSocket {
	private:
		/* variables */
		int 						            _port;
		std::string					            _ip;
		std::string					            _name;
		std::list<LocationInfo*>	            _locations;
        std::multimap<std::string, std::string> _configList;
        int                                     _fd;
        int                                     _nb;
        bool                                    _autoindex;

		/* functions */
//		std::list<std::string>  split(const std::string& str, std::string myDelim);

	public:
		/* constructors */
        ListenSocket() {}
		ListenSocket(LocationInfo* server);
		ListenSocket(const ListenSocket& copy)  { *this = copy; }
		ListenSocket& operator = (const ListenSocket& op);
		~ListenSocket() {}

		/* getters */
		int	getPort(void) const 				                            { return _port; }
		int getLocationSize(void) const 		                            { return _locations.size(); }
		const std::string&	getIP(void) const	                            { return _ip; }
		const std::string&	getName(void) const	                            { return _name; }
        const int getFd(void) const                                         { return _fd; }
        const int getNb(void) const                                         { return _nb; }
        const std::list<LocationInfo*>& getLocations(void) const            { return _locations; }
        const std::multimap<std::string, std::string>& getConfigList(void) const { return _configList; };
        const bool getAutoindex(void) const                                 { return _autoindex; }

        void setFd(const int fd) { _fd = fd; }
        void setNb(const int nb) { _nb = nb; }
};

static std::ostream& operator << (std::ostream& os, const ListenSocket& soc) {
	os	<< " server " << END << soc.getName() << YELLOW
		<< " started successfully: listening on " << END << soc.getIP()<< ":" << soc.getPort() << YELLOW
		<< ", number of locations : " << END << soc.getLocationSize();
    return os;
}

#endif