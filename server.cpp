#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<map>
#include<cstring>
#include<thread>

#include "logger/logger.h"

class SocketStreamScanner{
private:
	int socket_fd;
	struct sockaddr_in address;
	char buffer[2048];
	int buffer_position;
	int buffer_read_size;
	std::string delimiter;
	bool EOS;
	void setbackTo(int pos){buffer_position = pos;}//sets buffer_position to required position
public:
	SocketStreamScanner(int socket){
		socket_fd = socket;
		delimiter = "\r\n";
		struct sockaddr_in addr;
		socklen_t addr_size = sizeof(struct sockaddr_in);
		getpeername(socket_fd, (struct sockaddr *)&address, &addr_size);
	}
	~SocketStreamScanner(){}
	std::string readAll(){//unsafe due to being able to overload the RAM
		std::string temp = "";
		if(EOS) return temp;
		while((buffer_read_size = read(socket_fd, buffer, sizeof(buffer) - 1)) > 0){
			temp.append(buffer);
			if(buffer_read_size < sizeof(buffer)) break;
		}
		EOS = true;
		return temp;
	}
	std::string readNext(){//returns a string upto the next found delimiter
		std::string line = "";
		if(EOS) return line;
		if(buffer_position >= buffer_read_size){//If buffer has been read till the end read next bytes
			buffer_read_size = read(socket_fd, buffer, sizeof(buffer) - 1);
			buffer_position = 0;
		}
		line.append(buffer);
		line.erase(0, buffer_position);
		int original_position = buffer_position;
		buffer_position += line.find_first_of(delimiter);
		line = line.substr(0, buffer_position - original_position);
		buffer_position += delimiter.size();
		return line;
	}
	int getClientPort(){
		return address.sin_port;
	}
	std::string getClientAddress() {
		std::string s(inet_ntoa(address.sin_addr));
		return s;
	}
};

template <class Protocol>
class Client{
private:
	struct sockaddr_in serverIP;
	
};

template <class Protocol>
class ServerHandler{
private:
	struct sockaddr_in clientIP;
	int client_socket_fd;
	bool keep_live;
	Protocol proto;
public:
	ServerHandler(){}
	ServerHandler(int socket){
		client_socket_fd = socket;
	}
	~ServerHandler(){}
	void setProtocol(Protocol p){
		proto = p;
	}
	void handle(){
		proto.handle(SocketStreamScanner(client_socket_fd));
	}
	void closeConnection(){
		close(client_socket_fd);
	}
};

template <class Protocol>
class Server{
private:
	struct sockaddr_in serverIP;//Client IP is not a property of the Server socket
	int server_socket_fd;
	std::string protocol;
	bool keep_live;
	void handle(int socket_fd){
		ServerHandler<Protocol> c(socket_fd);
		c.handle();
		c.closeConnection();
	}
public:
	Server(){
		server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		memset(&serverIP, '0', sizeof(serverIP));
		serverIP.sin_family = AF_INET;
		serverIP.sin_addr.s_addr = htonl(INADDR_ANY);
		serverIP.sin_port = htons(8080);
		bind(server_socket_fd, (struct sockaddr*) &serverIP, sizeof(serverIP));
		std::string protocol = "";
		keep_live = true;
	}
	~Server(){}
	void start(int capacity = 20){
		listen(server_socket_fd, capacity);
		int client_socket_fd;
		logger.info("Server is live");
		while(keep_live){
			client_socket_fd = accept(server_socket_fd, (struct sockaddr*)NULL, NULL);
			std::thread handlerThread([this, client_socket_fd]{this->handle(client_socket_fd);});
			handlerThread.join();
		}
	}
	void shutdown(){
		keep_live = false;
	}
};
