#include <netinet/in.h>
#include <cstdlib>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <thread>

#define PORT 8080

class Controller
{
public:
    Controller() = default;
    ~Controller() = default;
    int result(char* s);
private:
    int m_sum(int, int);
    int m_mul(int, int);
    int m_diff(int, int);
    int m_div(int, int);
};

int Controller::result(char* s) {

    int action = s[0] - '0';
    int a = s[1] - '0';
    int b = s[2] - '0';
    switch (action) {
        case 0:
            return this->m_sum(a, b);
        case 1:
            return this->m_diff(a, b);
        case 2:
            return this->m_mul(a, b);
        case 3:
            return this->m_div(a, b);
    }
    return 0;
}

int Controller::m_sum (int a, int b) {
    return a + b;
}

int Controller::m_mul (int a, int b) {
    return a * b;
}

int Controller::m_div (int a, int b) {
    return a / b;
}

int Controller::m_diff (int a, int b) {
    return a - b;
}

////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread, result;
    sockaddr_in address{};
    int opt = 1;
    int addr_len = sizeof(address);
    char buffer[64] = { 0 };
    Controller con;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket failed" << "\n";
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        std::cerr << "setsockopt";
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        std::cerr << "bind failed";
        exit(EXIT_FAILURE);
    }
    std::cout << "listening...\n";
    if (listen(server_fd, 3) < 0) {
        std::cerr << "listen";
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                             (socklen_t *) &addr_len)) < 0) {
        std::cerr << "accept";
        exit(EXIT_FAILURE);
    }
    while(true){
        valread = read(new_socket, buffer, 64);
        result  = con.result(buffer);
        std::cout << "result: " <<  result << "\n";
        send(new_socket, std::to_string(result).c_str(), strlen(std::to_string(result).c_str()), 0);

    }

    close(new_socket);

    shutdown(server_fd, SHUT_RDWR);
    return 0;
}