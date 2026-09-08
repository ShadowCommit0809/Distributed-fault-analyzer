#include "HttpServer.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

HttpServer::HttpServer(int port) : m_port(port) {
#if defined(_WIN32) || defined(_WIN64)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

HttpServer::~HttpServer() {
    stop();
#if defined(_WIN32) || defined(_WIN64)
    WSACleanup();
#endif
}

void HttpServer::get(const std::string& path, HttpHandler handler) {
    m_getRoutes[path] = handler;
}

void HttpServer::post(const std::string& path, HttpHandler handler) {
    m_postRoutes[path] = handler;
}

bool HttpServer::start() {
    if (m_running.load()) return true;

    m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_serverSocket == INVALID_SOCKET) {
        return false;
    }

    int opt = 1;
#if defined(_WIN32) || defined(_WIN64)
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<u_short>(m_port));

    if (bind(m_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
        return false;
    }

    if (listen(m_serverSocket, 20) == SOCKET_ERROR) {
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
        return false;
    }

    m_running = true;
    m_listenThread = std::thread(&HttpServer::listenLoop, this);
    return true;
}

void HttpServer::stop() {
    if (!m_running.load()) return;
    m_running = false;

    if (m_serverSocket != INVALID_SOCKET) {
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
    }

    if (m_listenThread.joinable()) {
        m_listenThread.join();
    }
}

void HttpServer::listenLoop() {
    while (m_running.load()) {
        sockaddr_in clientAddr;
#if defined(_WIN32) || defined(_WIN64)
        int clientLen = sizeof(clientAddr);
#else
        socklen_t clientLen = sizeof(clientAddr);
#endif
        SOCKET clientSock = accept(m_serverSocket, (sockaddr*)&clientAddr, &clientLen);
        if (clientSock == INVALID_SOCKET) {
            if (!m_running.load()) break;
            continue;
        }

        // Spawn detached thread to handle request concurrently
        std::thread([this, clientSock]() {
            this->handleClient(clientSock);
        }).detach();
    }
}

HttpRequest HttpServer::parseRequest(const std::string& raw) {
    HttpRequest req;
    std::istringstream stream(raw);
    std::string line;

    if (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream lineStream(line);
        std::string fullPath;
        lineStream >> req.method >> fullPath;

        size_t queryPos = fullPath.find('?');
        if (queryPos != std::string::npos) {
            req.path = fullPath.substr(0, queryPos);
            req.query = fullPath.substr(queryPos + 1);
        } else {
            req.path = fullPath;
        }
    }

    // Headers
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string val = line.substr(colon + 1);
            while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) val.erase(val.begin());
            req.headers[key] = val;
        }
    }

    // Body
    std::string body;
    char ch;
    while (stream.get(ch)) {
        body += ch;
    }
    req.body = body;

    return req;
}

void HttpServer::handleClient(SOCKET clientSock) {
    std::string rawData;
    char buffer[4096];
    int bytesRead = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        rawData.append(buffer, bytesRead);

        // Check if there is a Content-Length to read the complete body
        size_t clPos = rawData.find("Content-Length:");
        if (clPos == std::string::npos) {
            clPos = rawData.find("content-length:");
        }
        if (clPos != std::string::npos) {
            size_t headerEnd = rawData.find("\r\n\r\n");
            if (headerEnd != std::string::npos) {
                size_t valStart = clPos + 15;
                size_t lineEnd = rawData.find("\r\n", valStart);
                try {
                    int expectedLen = std::stoi(rawData.substr(valStart, lineEnd - valStart));
                    size_t bodyLen = rawData.length() - (headerEnd + 4);
                    while (bodyLen < static_cast<size_t>(expectedLen)) {
                        int more = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
                        if (more <= 0) break;
                        rawData.append(buffer, more);
                        bodyLen += more;
                    }
                } catch (...) {}
            }
        }

        HttpRequest req = parseRequest(rawData);

        HttpResponse res;
        if (req.method == "OPTIONS") {
            // CORS preflight
            res.statusCode = 204;
            res.statusMessage = "No Content";
            res.body = "";
        } else if (req.method == "GET" && m_getRoutes.count(req.path)) {
            res = m_getRoutes[req.path](req);
        } else if (req.method == "POST" && m_postRoutes.count(req.path)) {
            res = m_postRoutes[req.path](req);
        } else {
            res.statusCode = 404;
            res.statusMessage = "Not Found";
            res.body = "{\"error\": \"Route not found: " + req.path + "\"}";
        }

        std::string responseStr = res.serialize();
        send(clientSock, responseStr.c_str(), static_cast<int>(responseStr.length()), 0);

#if defined(_WIN32) || defined(_WIN64)
        shutdown(clientSock, SD_SEND);
#else
        shutdown(clientSock, SHUT_WR);
#endif
    }

    closesocket(clientSock);
}
