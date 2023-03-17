//
// Created by Attack Cristina on 10/15/22.
//

#ifndef MAIN_HPP
# define MAIN_HPP

# define BLUE "\033[1;34m"
//# define BLUE1 "\033[0;34m"
//# define BLUE "\033[2;34m"
//# define GRN "\033[0;32m"
# define GRN "\033[1;32m"
//# define GRN2 "\033[2;32m"
# define RED "\033[0;31m"
//# define RED2 "\033[2;31m"
//# define RED "\033[2;31m"
# define VIOLET "\033[0;35m"
//# define VIOLET1 "\033[1;35m"
//# define VIOLET "\033[2;35m"
//# define YELLOW1 "\033[0;33m"
//# define YELLOW2 "\033[1;33m"
# define YELLOW "\033[1;33m"
# define TICK "\xE2\x9C\x94"
# define END "\033[0m"

# define HOSTNAME_LEN 30
# define MAX_BODY_SIZE 10000


# include <list>
# include <iostream>

void                    ctrl_c_handler(int sig);
std::string             timestamp();

void                    printMsg(int fd, std::string msg1, std::string msg2);
void                    printMsg(int srvNb, int clntSd, std::string msg1, std::string msg2);
void                    printMsg(int srvNb, int clntSd, std::string COLOR, std::string msg1, std::string msg2);

void                    printValue(std::string key, std::string value);

std::string             urlDecode(std::string const & address);
std::string             urlEncode(std::string str);

std::list<std::string>  split(const std::string& str, std::string myDelims);
std::list<std::string>  splitStr(const std::string& str, std::string delim, std::string postfix);
std::string             replace(std::string src, std::string s1, std::string s2);
#endif //MAIN_HPP
