#include "ConfigurationSingleton.hpp"
#include "Server.hpp"
#include "main.hpp"

std::string urlDecode(std::string const & address)
{
    std::string outp;
    unsigned int decodedI;
    size_t maxLen = address.find('?');

    for (size_t i = 0; i < address.length() && (i < maxLen || maxLen == std::string::npos); i++)
    {
        if (address[i] != '%')
        {
            if (address[i] == '+')
                outp += ' ';
            else
                outp += address[i];
        }
        else
        {
            sscanf(address.substr(i + 1, 2).c_str(), "%x", & decodedI);
            outp += static_cast<char>(decodedI);
            i += 2;
        }
    }
    return outp;
}

std::string urlEncode(std::string str)
{
    std::string new_str = "";
    char c;
    int ic;
    const char* chars = str.c_str();
    char bufHex[10];
    int len = strlen(chars);

    for(int i=0;i<len;i++){
        c = chars[i];
        ic = c;
        // uncomment this if you want to encode spaces with +
        /*if (c==' ') new_str += '+';
        else */if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') new_str += c;
        else {
            sprintf(bufHex,"%X",c);
            if(ic < 16)
                new_str += "%0";
            else
                new_str += "%";
            new_str += bufHex;
        }
    }
    return new_str;
}

std::string replace(std::string src, std::string s1, std::string s2) {
    std::string dest = "";
    size_t index;
    if (s1 == s2) { return src; }
    index = src.find(s1);
    while (index != std::string::npos) {
        dest = src.substr(0, index) + s2 + src.substr(index + s1.length());
        src = dest;
        index = src.find(s1);
    }
    return dest;
}

void ctrl_c_handler(int sig) {
    (void) sig;
    std::cout << "\r" << GRN << "Server was stopped by CTRL+C" << std::endl;
    exit(0);
}

std::list<std::string> split(const std::string& str, std::string myDelims) {
    std::list<std::string> dest;
    char* delim = (char *)myDelims.c_str();
    char* pTempStr = strdup(str.c_str());
    char* pWord = strtok(pTempStr, delim);
    while(pWord != NULL) {
        dest.push_back(pWord);
        pWord = strtok(NULL, delim);
    }
    free(pTempStr);
    return dest;
}

std::list<std::string> splitStr(const std::string& str, std::string delim, std::string postfix) {
    std::list<std::string> dest;
    std::string subStr;
    size_t pos = 0;
    size_t startIndex = 0;

    if (delim != "") {
        for (pos = str.find(delim, pos++); pos != std::string::npos; pos = str.find(delim, pos + 1)) { //???? {
            subStr.clear();
            subStr.append(str, startIndex, pos - startIndex);
            if (!subStr.empty()) {
                dest.push_back(subStr);
            }
            startIndex = pos + delim.size();
            if (pos + delim.size() != std::string::npos)
                pos += (delim.size() - 1);
        }
    }
    subStr.clear();
    subStr.append(str, startIndex, std::string::npos);
    dest.push_back(subStr);

    return dest;
}

std::string timestamp() {
    std::stringstream out;
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    out << END << "[" << std::setfill('0')
              << std::setw(2) << now->tm_hour << ":"
              << std::setw(2) << now->tm_min << ":"
              << std::setw(2) << now->tm_sec << "] ";
    return out.str();
}

void printMsg(int srvNb, std::string msg1, std::string msg2) {
    std::string msg = "";
    if (srvNb >= 0) {
        msg += "server[" + std::to_string(srvNb) + "]: ";
    }
    msg += msg1 + msg2;
    std::cout << timestamp() << YELLOW << msg << END << std::endl;
}

void printMsg(int srvNb, int clntSd, std::string msg1, std::string msg2)
{
    std::string msg = END + timestamp() + YELLOW;
    if (srvNb >= 0)
        msg += "server[" + std::to_string(srvNb) + "]: ";
    msg += msg1;
    clntSd >= 0 ? msg += END + std::to_string(clntSd) + YELLOW : msg += " ";
//    msg2 = replace(msg2, "\n", "\n\t");
    msg += msg2 + END;
    std::cout << msg << std::endl;
}

void printMsg(int srvNb, int clntSd, std::string COLOR, std::string msg1, std::string msg2)
{
    std::string msg = END + timestamp() + YELLOW;
    if (srvNb >= 0)
        msg += "server[" + std::to_string(srvNb) + "]: ";
    msg += END + COLOR + msg1;
    clntSd >= 0 ? msg += END + std::to_string(clntSd) + COLOR : msg += " ";
//    msg2 = replace(msg2, "\n", "\n\t");
    msg += msg2 + END;
    std::cout << msg << std::endl;
}

void printValue(std::string key, std::string value) {
    std::cout << BLUE << key << " : " << END << value << std::endl;
}

int main() {
    signal(SIGINT, ctrl_c_handler);
    printMsg(-1, "Starting server ...", "");
    Server testServer;
    testServer.run();
    return 0;
}
