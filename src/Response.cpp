#include "Response.hpp"
#include <unistd.h>

Response::Response() : _httpVersion("HTTP/1.1"), _autoindex(false) {
    initStatusCodes();
    initContentTypes();
}

Response::Response(const Response &copy) { *this = copy; }

Response &Response::operator=(const Response &src) {
    if (this != &src) {
        _httpVersion = src._httpVersion;
        _status = src._status;
        _body = src._body;
    }
    return *this;
}

Response::~Response() {}

std::string Response::UriDecode(const std::string & sSrc) {
    std::map<std::string, std::string> uriSymbs;
    uriSymbs["%20"] = " ";
    uriSymbs["%22"] = "\"";
    uriSymbs["%25"] = "%";
    uriSymbs["%2D"] = "-";
    uriSymbs["%2E"] = ".";
    uriSymbs["%3C"] = "<";
    uriSymbs["%3E"] = ">";
    uriSymbs["%5C"] = "\\";
    uriSymbs["%5E"] = "^";
    uriSymbs["%5F"] = "_";
    uriSymbs["%60"] = "`";
    uriSymbs["%7B"] = "{";
    uriSymbs["%7C"] = "|";
    uriSymbs["%7D"] = "}";
    uriSymbs["%7E"] = "~";

    std::size_t found = sSrc.find("%");
    if (found != std::string::npos) {
        std::string sResult = replace(sSrc, sSrc.substr(found, 3),
                                      uriSymbs.find(sSrc.substr(found, 3))->second);
        return sResult;
    } else {
        return sSrc;
    }
}

int Response::isValidPath(std::string path) {
    struct stat buf;
    const char * pathChar = path.c_str();

    FILE * fileOpen;

    lstat(pathChar, & buf);
    fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
    if (fileOpen == NULL){
        return -1;
    } else if (S_ISDIR(buf.st_mode)) {
        fclose(fileOpen);
        return 1;
    } else if (S_ISREG(buf.st_mode)) {
        fclose(fileOpen);
        return 0;
    }
    return -1;
}

void Response::removeSlashes(std::string &path) {
    path.erase(std::unique(path.begin(), path.end(), both_slashes()), path.end());
    if (path.find("/") == 0) {
        path = path.substr(1, path.length() - 1);
    }
    if (path.rfind("/") == path.length() - 1) {
        path = path.substr(0, path.length() - 1);
    }
}

std::string Response::getFileName(ClientSocket client, Request request) {
    std::list<LocationInfo*> serverLocation = client.getServer()->getLocations();
    const ListenSocket* server = client.getServer();

    std::string host = request.getBody().find("Host")->second;
    std::string root = client.getServer()->getConfigList().count("root") ? client.getServer()->getConfigList().find("root")->second : "";
    std::string requestURI = urlDecode(request.getBody().find("Request-URI")->second);

    removeSlashes(root);
    removeSlashes(requestURI);

    std::string referer;
    if (request.getBody().count("Referer")) {
        referer = request.getBody().find("Referer")->second;
        referer = referer.substr(referer.find(host.substr(0, host.length() - 1)) + host.length() - 1);
    } else {
        referer = "";
    }
    removeSlashes(referer);

    if ((requestURI.rfind("/") == requestURI.length() - 1)
        || requestURI.substr(requestURI.rfind("/") + 1).find(".") == std::string::npos) {
        // на вход подается директория
        if (_autoindex) { // обработка директорий в случае включенного автоиндекса
            for (std::list<LocationInfo*>::const_iterator it = serverLocation.begin(); it != serverLocation.end(); it++) {
                if (!(*it)->getLocation().compare("/" + requestURI.substr(0, requestURI.find("/")))) { // ищем (первую) директорию из requestUri в локациях сервера
                    std::string localRoot = (*it)->getConfigList().count("root") ? (*it)->getConfigList().find("root")->second : ""; // корень локации сервера
                    int found = requestURI.find("/") == std::string::npos ? requestURI.length() : requestURI.find("/") + 1; // заменяем (первую) директорию в requestUri на директорию локации сервера
                    if (isValidPath(root + "/" + localRoot + "/" + requestURI.substr(found)) == 1) { // если такая папка существует, ищем внутри индекс
                        if ((*it)->getConfigList().count("index")) { // нашли индекс
                            return UriDecode(root + "/" + localRoot + "/" + requestURI.substr(found) + "/" +
                                             (*it)->getConfigList().find("index")->second);
                        } else { // не нашли индекс
                            return UriDecode(root + "/" + server->getConfigList().find("error404")->second);
                        }
                    } else if (isValidPath(root + "/" + localRoot + "/" + requestURI.substr(found)) == 0) { // если это файл (без расширения), то возвращаем файл
                        return UriDecode(root + "/" + localRoot + "/" + requestURI.substr(found));
                    }
                }
            }
            return UriDecode(root + "/" + server->getConfigList().find("error404")->second); // директории нет в локациях сервера
        }
        if (!_autoindex) { // обработка директории в случае выключенного автоиндекса
            for (std::list<LocationInfo*>::const_iterator it = serverLocation.begin(); it != serverLocation.end(); it++) {
                if (!(*it)->getLocation().compare("/" + requestURI.substr(0, requestURI.find("/")))) { // ищем (первую) директорию из requestUri в локациях сервера
                    if (isValidPath(root + "/" + requestURI) > 0) { // если весь путь валидный (с подпапками)
                        lsHtml("/" + root + "/" + requestURI); // если папка
                        return "resources/.listing.html";
                    } else if (isValidPath(root + "/" + requestURI) == 0) {
                        return UriDecode(root + "/" + requestURI); // если файл
                    }
                }
            }
            if (isValidPath(root + "/" + requestURI) > 0) { // локации в сервере нет, но в корневой директории есть папка или файл
                lsHtml("/" + root + "/" + requestURI); // если папка
                return "resources/.listing.html";
            } else if (isValidPath(root + "/" + requestURI) == 0) {
                return UriDecode(root + "/" + requestURI); // если файл
            }
        }
        return UriDecode(root + "/" + server->getConfigList().find("error404")->second); // заглушка для ифов
    } else {
        // на вход подается файл
        std::string refPath = referer.substr(0, referer.rfind("/") + 1);
        if (isValidPath(root + "/" + requestURI.substr(requestURI.find(refPath) + refPath.length())) == 0) { // для 404, невалидные папки удаляются
            return UriDecode(root + "/" + requestURI.substr(requestURI.find(refPath) + refPath.length()));
        }
        if (isValidPath(root + "/" + requestURI) == 0) { // исходный путь вылидный
            return UriDecode(root + "/" + requestURI);
        }
        return UriDecode(root + "/" + server->getConfigList().find("error404")->second); // заглушка для ифов
    }
}

bool Response::lsHtml(std::string uri) {
    std::string result = "";
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    char cwd[PATH_MAX];
    static std::string currentDir;

    result += "<!DOCTYPE html>\n"
              "<html>\n"
              "\n"
              "<head>\n"
              "\t<meta charset=\"utf-8\">\n"
              "\t<title>\n"
              "\t\tWebServ\n"
              "\t</title>\n"
              "\t<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-iYQeCzEYFbKjA/T2uDLTpkwGzCiq6soy8tYaI1GyVh/UjpbCx/TYkiZhlZB6+fzT\" crossorigin=\"anonymous\">\n"
              "\t<link href=\"https://fonts.googleapis.com/css2?family=Montserrat:wght@300&display=swap\" rel=\"stylesheet\">\n"
              "</head>\n"
              "\n"
              "<body style=\"font-family: 'Montserrat';\">\n"
              "<div class=\"container m-4 p-4\">\n"
              "<div class=\"row justify-content-center\">\n";

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        currentDir = std::string(cwd) + uri + "/";
        printValue("Current working dir", currentDir);
        if ((dir = opendir(currentDir.c_str())) != NULL) {
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] != '.' || (entry->d_name[0] == '.' && entry->d_name[1] == '.')) {
                    std::string nextDir = "." + uri + "/";
                    std::string tmp = nextDir + entry->d_name;
                    if (stat(tmp.c_str(), &info) != 0) {
                        std::cout << "STAT error : " << tmp.c_str() << std::endl;
                    } else if (S_ISDIR(info.st_mode)) {
                        result += "<div style=\"font-size: 18px;margin-bottom: 5px;\"><a href=\"./";
                        result += std::string(entry->d_name) + "/";
                        result += "\" style=\"display: inline-block;width: 70%;\">" + std::string(entry->d_name) + "/";
                        result += "</a><span>-</span></div>\n";
                    } else {
                        result += "<div style=\"font-size: 18px;margin-bottom: 5px;\"><a href=\"./";
                        result += std::string(entry->d_name);
                        result += "\" style=\"display: inline-block;width: 70%;\">" + std::string(entry->d_name);
                        result += "</a><span>-</span></div>\n";
                    }
                }
            }
            closedir(dir);
        } else {
            return false;
        }
    }
    result += "</div>\n</div>\n</body>\n</html>";
    std::ofstream file("resources/.listing.html");
    file << result;
    file.close();
    return true;
}

void Response::fillHeaders(ClientSocket client, std::string fileName, int contentLength) {
    bodyMapPushBack("Server", client.getServer()->getName());
    std::time_t tt;
    time (&tt);
    char resDate[100];

    if (strftime(resDate, sizeof(resDate), "%a, %d %b %Y %H:%M:%S GMT", std::localtime(&tt))) {
        bodyMapPushBack("Date", resDate);
    }

    if (!fileName.empty()) {
        struct stat attrib;
        stat(fileName.c_str(), &attrib);
        if (strftime(resDate, sizeof(resDate), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&(attrib.st_mtime)))) {
            bodyMapPushBack("Last-Modified", resDate);
        }
        if (_contentTypes.count(fileName.substr(fileName.rfind(".") + 1))) {
            bodyMapPushBack("Content-Type", _contentTypes.find(fileName.substr(fileName.rfind(".") + 1))->second);
        } else {
            bodyMapPushBack("Content-Type", "undefined");
        }
    } else {
        bodyMapPushBack("Content-Type", " text/html; charset=utf-8");
    }

    if (contentLength > 0) {
        bodyMapPushBack("Content-Length", std::to_string(contentLength));
    }
    bodyMapPushBack("Version", _httpVersion);
    bodyMapPushBack("Connection", "Closed");
}

bool Response::generateResponse(ClientSocket client, int clientSocket, Request request, int& readCounter) {
    _autoindex = client.getServer()->getAutoindex();
    std::list<LocationInfo*> serverLocation = client.getServer()->getLocations();

    std::string host = request.getBody().find("Host")->second.substr(0, request.getBody().find("Host")->second.length() - 1);
    std::string referer = request.getBody().count("Referer") ? request.getBody().find("Referer")->second : "";
    std::string requestURI = urlDecode(request.getBody().find("Request-URI")->second);
    std::string link = referer.substr(0, referer.length() - 1);
    removeSlashes(requestURI);

    std::string location = "";
    if (link.find(host) != std::string::npos) {
        location = link.substr(link.find(host) + host.length() + 1);
    }
    if (location.find("/") == std::string::npos) {
        location = "";
    } else {
        location = location.substr(0, location.find("/"));
    }

    std::list<std::string> allowMethods;

    for (std::list<LocationInfo*>::const_iterator it = serverLocation.begin(); it != serverLocation.end(); it++) {
        if (!(*it)->getLocation().compare("/" + location)) {
            if ((*it)->getConfigList().count("allow_methods")) {
                allowMethods = split((*it)->getConfigList().find("allow_methods")->second, "/");
            }
        }
    }

    if (!request.getMethod().compare("GET") && std::find(allowMethods.begin(), allowMethods.end(), "GET") != allowMethods.end()) {
        return GETResponse(client, clientSocket, request, readCounter);
    } else if (!request.getMethod().compare("POST") && std::find(allowMethods.begin(), allowMethods.end(), "POST") != allowMethods.end()) {
        return POSTResponse(client, clientSocket, request);
    } else if (!request.getMethod().compare("DELETE") && std::find(allowMethods.begin(), allowMethods.end(), "DELETE") != allowMethods.end()) {
        return DELETEResponse(client, clientSocket, request);
    } else {
        return BadMethodResponse(client, clientSocket, request, readCounter);
    }
}

std::string Response::deleteFileName(ClientSocket client, Request request, int clientSocket) {
    std::string root = client.getServer()->getConfigList().count("root") ? client.getServer()->getConfigList().find("root")->second : "";
    std::string requestURI = request.getBody().find("Request-URI")->second;

    removeSlashes(root);
    removeSlashes(requestURI);

    std::string fileName = root + "/" + requestURI;

    if (isValidPath(fileName) == 0) {
        printMsg(client.getServer()->getNb(), clientSocket, "on descriptor ", " file removed: " + fileName);
        remove(fileName.c_str());
        _status = std::make_pair(200, _statusCodes.find(200)->second);
        return "<!DOCTYPE html>\n"
               "<html>\n"
               "<head>\n"
               "    <meta charset=\"utf-8\">\n"
               "    <title>200 File deleted</title>\n"
               "    <link href=\"https://fonts.googleapis.com/css2?family=Montserrat:wght@300&display=swap\" rel=\"stylesheet\">\n"
               "    <link href=\"https://fonts.googleapis.com/css2?family=Silkscreen&display=swap\" rel=\"stylesheet\">\n"
               "    <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-iYQeCzEYFbKjA/T2uDLTpkwGzCiq6soy8tYaI1GyVh/UjpbCx/TYkiZhlZB6+fzT\" crossorigin=\"anonymous\">\n"
               "</head>\n"
               "<body style=\"font-family: 'Montserrat';\">\n"
               "    <div class=\"container mt-5 pt-5\">\n"
               "        <div class=\"row justify-content-center\">\n"
               "            <div class=\"col-4 text-center\">\n"
               "                <h1 style=\"font-family: 'Silkscreen', cursive;\">200</h1>\n"
               "                <h1>File deleted</h1>\n"
               "            </div>\n"
               "        </div>\n"
               "    </div>\n"
               "</body>\n"
               "</html>";
    } else {
        printMsg(client.getServer()->getNb(), clientSocket, RED, "on descriptor ", " no content found: " + fileName);
        _status = std::make_pair(404, _statusCodes.find(404)->second);
        return "<!DOCTYPE html>\n"
               "<html>\n"
               "<head>\n"
               "    <meta charset=\"utf-8\">\n"
               "    <title>404 Not found</title>\n"
               "    <link href=\"https://fonts.googleapis.com/css2?family=Montserrat:wght@300&display=swap\" rel=\"stylesheet\">\n"
               "    <link href=\"https://fonts.googleapis.com/css2?family=Silkscreen&display=swap\" rel=\"stylesheet\">\n"
               "    <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-iYQeCzEYFbKjA/T2uDLTpkwGzCiq6soy8tYaI1GyVh/UjpbCx/TYkiZhlZB6+fzT\" crossorigin=\"anonymous\">\n"
               "</head>\n"
               "<body style=\"font-family: 'Montserrat';\">\n"
               "    <div class=\"container mt-5 pt-5\">\n"
               "        <div class=\"row justify-content-center\">\n"
               "            <div class=\"col-4 text-center\">\n"
               "                <h1 style=\"font-family: 'Silkscreen', cursive;\">404</h1>\n"
               "                <h1>Not found</h1>\n"
               "            </div>\n"
               "        </div>\n"
               "    </div>\n"
               "</body>\n"
               "</html>";
    }
}

bool Response::DELETEResponse(ClientSocket client, int clientSocket, Request request) {
    printValue("Method", "DELETE");
    std::stringstream response;

    fillHeaders(client, "", -1);
    std::string htmlText = deleteFileName(client, request, clientSocket);
    bodyMapPushBack("Content-Length", std::to_string(htmlText.length()));
    response << _httpVersion << " " << _status.first << " " << _status.second << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _body.begin(); it != _body.end(); it++) {
        response << it->first << ": " << it->second << "\r\n";
    }

    response << "\r\n";
    response << htmlText << "\r\n";
    std::cout << RED << "======= HTTP RESPONSE =======\n" << END << response.str() << RED << "======= ============= =======\n" << END << std::endl;
    if (send(clientSocket, response.str().c_str(), response.str().length(), MSG_NOSIGNAL) == SOCKET_ERROR) {
        printMsg(client.getServer()->getNb(), clientSocket, RED, "on descriptor ", " send message failure");
        perror("");
        exit(SOCKET_ERROR); // correct it
    }
    return true;
}

bool Response::BadMethodResponse(ClientSocket client, int clientSocket, Request request, int &readCounter) {
    printValue("Method", "Bad request");

    std::stringstream response;

    fillHeaders(client, "", -1);
    std::string htmlText = "<!DOCTYPE html>\n"
                           "<html>\n"
                           "<head>\n"
                           "    <meta charset=\"utf-8\">\n"
                           "    <title>405 Method Not Allowed</title>\n"
                           "    <link href=\"https://fonts.googleapis.com/css2?family=Montserrat:wght@300&display=swap\" rel=\"stylesheet\">\n"
                           "    <link href=\"https://fonts.googleapis.com/css2?family=Silkscreen&display=swap\" rel=\"stylesheet\">\n"
                           "    <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-iYQeCzEYFbKjA/T2uDLTpkwGzCiq6soy8tYaI1GyVh/UjpbCx/TYkiZhlZB6+fzT\" crossorigin=\"anonymous\">\n"
                           "</head>\n"
                           "<body style=\"font-family: 'Montserrat';\">\n"
                           "    <div class=\"container mt-5 pt-5\">\n"
                           "        <div class=\"row justify-content-center\">\n"
                           "            <div class=\"col-4 text-center\">\n"
                           "                <h1 style=\"font-family: 'Silkscreen', cursive;\">405</h1>\n"
                           "                <h1>Method not allowed</h1>\n"
                           "            </div>\n"
                           "        </div>\n"
                           "    </div>\n"
                           "</body>\n"
                           "</html>";
    bodyMapPushBack("Content-Length", std::to_string(htmlText.length()));
    _status = std::make_pair(405, _statusCodes.find(405)->second);
    response << _httpVersion << " " << _status.first << " " << _status.second << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _body.begin(); it != _body.end(); it++) {
        response << it->first << ": " << it->second << "\r\n";
    }

    response << "\r\n";
    response << htmlText << "\r\n";
    std::cout << RED << "======= HTTP RESPONSE =======\n" << END << response.str() << RED << "======= ============= =======\n" << END << std::endl;
    if (send(clientSocket, response.str().c_str(), response.str().length(), MSG_NOSIGNAL) == SOCKET_ERROR) {
        printMsg(client.getServer()->getNb(), clientSocket, RED, "on descriptor ", " send message failure");
        perror("");
        exit(SOCKET_ERROR); // correct it
    }
    return true;
}

bool Response::POSTResponse(ClientSocket client, int clientSocket, Request request) {
    std::string contentType = request.getBody().count("Content-Type") ? request.getBody().find("Content-Type")->second : "";

    printValue("contentType", contentType);
    if (contentType.find("multipart/form-data") != std::string::npos) {
        POSTformdata(request, contentType, client, clientSocket);
    }
    return true;
}

std::string Response::postFileName(std::string body) {
    std::string fileName = "";

    std::string header = body.substr(0, body.find("\r\n\r\n"));
    if (!body.compare("--\r\n")) {
        return "";
    } else {
        int found = header.find("filename=");
        fileName = header.substr(found + 10);
        fileName = fileName.substr(0, fileName.find("\""));
        return fileName;
    }
}

std::string Response::postContent(std::string body) {
    std::string content = "";

    content = body.substr(body.find("\r\n\r\n") + 4);
    content = content.substr(0, content.find("\r\n"));
    return content;
}

void Response::POSTformdata(Request request, std::string contentType, ClientSocket client, int clientSocket) {
    std::string boundary = "";
    std::string fileName = "";
    std::list<std::string> lines = split(request.getMessage(), "\n");

    if (!contentType.empty()) {
        boundary = contentType.substr(contentType.find("boundary=") + 9);
    }

    if (boundary.size() < 1) {
        _status = std::make_pair(400, _statusCodes.find(400)->second);
        printMsg(client.getServer()->getNb(), clientSocket, RED, "on descriptor ", " can't read boundary");
        return;
    }

    if (request.getMessage().length() > request.getMaxBodySize()) {
        _status = std::make_pair(413, _statusCodes.find(413)->second);
        printMsg(client.getServer()->getNb(), clientSocket, RED, "on descriptor ", " payload too large");
        return;
    }

    _status = std::make_pair(100, _statusCodes.find(100)->second);
    std::string path = client.getServer()->getConfigList().count("root") ? client.getServer()->getConfigList().find("root")->second + "/downloads/" : "./resources/downloads/";
    if (isValidPath(path) != 1) {
        printMsg(client.getServer()->getNb(), clientSocket, VIOLET, "on descriptor ", " can't find path " + path + " to save file, files will be downloaded to the root directory");
        path = "./";
    }
    std::list<std::string> bodyList = splitStr(request.getMessage(), "--" + boundary.substr(0, boundary.length() - 1), "--");
    for (std::list<std::string>::const_iterator it = bodyList.begin(); it != bodyList.end(); it++) {
        fileName = postFileName(*it);
        if (!fileName.empty()) {
            int nb = 1;
            std::string tmpFileName = path + fileName;
            while (isValidPath(tmpFileName) == 0) {
                tmpFileName = path + fileName + "(" + std::to_string(nb) + ")";
                nb++;
            }
            fileName = tmpFileName;
            std::string content = postContent(*it);
            FILE * file;
            file = fopen(fileName.c_str(), "a");
            fwrite(content.c_str(), sizeof(char), content.length(), file);
            fclose(file);
        }
    }
}

bool Response::GETResponse(ClientSocket client, int clientSocket, Request request, int &readCounter) {
    std::stringstream response;
    static std::ifstream file;
    std::string fileName;
    static bool headerFlag;

    fileName = getFileName(client, request);
    fileName.erase(std::unique(fileName.begin(), fileName.end(), both_slashes()), fileName.end());
    file.open(fileName, std::ios::in | std::ios::binary | std::ios::ate);
    if (!headerFlag) {
        std::cout << RED << "FileName : " << END << fileName << std::endl;
        if (request.getBody().count("Transfer-Encoding")) {
            std::cout << request.getBody().find("Transfer-Encoding")->second << std::endl;
        }
        if (file.fail()) {
            printMsg(client.getServer()->getNb(), clientSocket, RED, "on descriptor ", " can't open input file: " + fileName);
            _status = std::make_pair(404, _statusCodes.find(404)->second);
        } else {
            _status = std::make_pair(200, _statusCodes.find(200)->second);
        }

        fillHeaders(client, fileName, file.tellg());
        response << _httpVersion << " " << _status.first << " " << _status.second << "\r\n";
        for (std::map<std::string, std::string>::const_iterator it = _body.begin(); it != _body.end(); it++) {
            response << it->first << ": " << it->second << "\r\n";
        }
        response << "\r\n";
        std::cout << RED << "======= HTTP RESPONSE =======\n" << END << response.str() << RED << "======= ============= =======\n" << END << std::endl;
        if (send(clientSocket, response.str().c_str(), response.str().length(), MSG_NOSIGNAL) == SOCKET_ERROR) {
            printMsg(client.getServer()->getNb(), clientSocket, RED, "on descriptor ", " send message failure");
            perror("");
            exit(SOCKET_ERROR); // correct it
        }
        headerFlag = true;
    }

    file.seekg(readCounter);
    /* порционная отправка ответа */
    std::string buff(READ_BUFFER_SIZE, '0');
    file.read(&buff[0], READ_BUFFER_SIZE);
    if (send(clientSocket, (char *)buff.c_str(), READ_BUFFER_SIZE, MSG_NOSIGNAL) == SOCKET_ERROR) {
        printMsg(client.getServer()->getNb(), clientSocket, RED, "on descriptor ", " failed to send file: " + fileName);
        file.close();
        file.clear();
        return false;
    }
    readCounter += READ_BUFFER_SIZE;
    if (file.eof()) {
        printMsg(client.getServer()->getNb(), clientSocket, "on descriptor ", " successfully send file: " + fileName);
        headerFlag = false;
        file.clear();
        file.close();
        readCounter = 0;
        return true;
    }
    file.close();
    return false;
}

void Response::bodyMapPushBack(std::string key, std::string value) {
    _body.insert(std::pair<std::string, std::string>(key, value));
}

void Response::initContentTypes() {
    _contentTypes["gif"] = "image/gif";
    _contentTypes["jpg"] = "image/jpeg";
    _contentTypes["jpeg"] = "image/jpeg";
    _contentTypes["png"] = "image/png";
    _contentTypes["svg"] = "image/svg+xml";
    _contentTypes["webp"] = "image/webp";
    _contentTypes["ico"] = "image/vnd.microsoft.icon";

    _contentTypes["css"] = "text/css";
    _contentTypes["csv"] = "text/csv";
    _contentTypes["html"] = "text/html";
    _contentTypes["htm"] = "text/html";
    _contentTypes["xml"] = "text/xml";
    _contentTypes["htm"] = "text/html";
    _contentTypes["pdf"] = "application/pdf";
    _contentTypes["mp3"] = "audio/mpeg";
    _contentTypes["mp4"] = "video/mp4";
}

void Response::initStatusCodes() {
    _statusCodes[100] = "Continue";
    _statusCodes[101] = "Switching Protocols";
    _statusCodes[102] = "Processing";
    _statusCodes[103] = "Checkpoint";
    _statusCodes[200] = "OK";
    _statusCodes[201] = "Created";
    _statusCodes[202] = "Accepted";
    _statusCodes[203] = "Non-Authoritative Information";
    _statusCodes[204] = "No Content";
    _statusCodes[205] = "Reset Content";
    _statusCodes[206] = "Partial Content";
    _statusCodes[207] = "Multi-Status";
    _statusCodes[300] = "Multiple Choices";
    _statusCodes[301] = "Moved Permanently";
    _statusCodes[302] = "Found";
    _statusCodes[303] = "See Other";
    _statusCodes[304] = "Not Modified";
    _statusCodes[305] = "Use Proxy";
    _statusCodes[306] = "Switch Proxy";
    _statusCodes[307] = "Temporary Redirect";
    _statusCodes[400] = "Bad Request";
    _statusCodes[401] = "Unauthorized";
    _statusCodes[402] = "Payment Required";
    _statusCodes[403] = "Forbidden";
    _statusCodes[404] = "Not Found";
    _statusCodes[405] = "Method Not Allowed";
    _statusCodes[406] = "Not Acceptable";
    _statusCodes[407] = "Proxy Authentication Required";
    _statusCodes[408] = "Request Timeout";
    _statusCodes[409] = "Conflict";
    _statusCodes[410] = "Gone";
    _statusCodes[411] = "Length Required";
    _statusCodes[412] = "Precondition Failed";
    _statusCodes[413] = "Request Entity Too Large";
    _statusCodes[414] = "Request-URI Too Long";
    _statusCodes[415] = "Unsupported Media Type";
    _statusCodes[416] = "Requested Range Not Satisfiable";
    _statusCodes[417] = "Expectation Failed";
    _statusCodes[418] = "I\'m a teapot";
    _statusCodes[422] = "Unprocessable Entity";
    _statusCodes[423] = "Locked";
    _statusCodes[424] = "Failed Dependency";
    _statusCodes[425] = "Unordered Collection";
    _statusCodes[426] = "Upgrade Required";
    _statusCodes[449] = "Retry With";
    _statusCodes[450] = "Blocked by Windows Parental Controls";
    _statusCodes[500] = "Internal Server Error";
    _statusCodes[501] = "Not Implemented";
    _statusCodes[502] = "Bad Gateway";
    _statusCodes[503] = "Service Unavailable";
    _statusCodes[504] = "Gateway Timeout";
    _statusCodes[505] = "HTTP Version Not Supported";
    _statusCodes[506] = "Variant Also Negotiates";
    _statusCodes[507] = "Insufficient Storage";
    _statusCodes[509] = "Bandwidth Limit Exceeded";
    _statusCodes[510] = "Not Extended";
}
