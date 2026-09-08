#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

struct HttpRequest {
    std::string method;
    std::string path;
    std::string query;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
};

struct HttpResponse {
    int statusCode{200};
    std::string statusMessage{"OK"};
    std::string contentType{"application/json"};
    std::string body;

    std::string serialize() const {
        std::string res = "HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage + "\r\n";
        res += "Content-Type: " + contentType + "\r\n";
        res += "Content-Length: " + std::to_string(body.length()) + "\r\n";
        res += "Access-Control-Allow-Origin: *\r\n";
        res += "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n";
        res += "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
        res += "Connection: close\r\n\r\n";
        res += body;
        return res;
    }
};

using HttpHandler = std::function<HttpResponse(const HttpRequest&)>;

class HttpServer {
private:
    int m_port;
    std::atomic<bool> m_running{false};
    SOCKET m_serverSocket{INVALID_SOCKET};
    std::thread m_listenThread;

    std::unordered_map<std::string, HttpHandler> m_getRoutes;
    std::unordered_map<std::string, HttpHandler> m_postRoutes;

public:
    explicit HttpServer(int port = 8081);
    ~HttpServer();

    void get(const std::string& path, HttpHandler handler);
    void post(const std::string& path, HttpHandler handler);

    bool start();
    void stop();

private:
    void listenLoop();
    void handleClient(SOCKET clientSock);
    HttpRequest parseRequest(const std::string& raw);
};

#endif // HTTP_SERVER_HPP
