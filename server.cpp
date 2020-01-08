namespace networking{
template <class Protocol>
class ServerHandler{
private:
	struct sockaddr_in clientIP;
	int client_socket_fd;
	bool keep_live;
	Protocol proto;
public:
	ServerHandler(){}
	~ServerHandler(){}
	void handle(int socket){
		client_socket_fd = socket;
		proto.handle(SocketStream(client_socket_fd));
	}
	void setProtocol(Protocol p){proto = p;}
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
	ServerHandler<Protocol> serverhandle;
	bool keep_live;
	void handle(int socket_fd){
		serverhandle.handle(socket_fd);
		serverhandle.closeConnection();
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
	void setProtocol(Protocol p){
		serverhandle.setProtocol(p);
	}
	void shutdown(){
		keep_live = false;
	}
};
}