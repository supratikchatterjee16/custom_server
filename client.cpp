namespace networking{
template <class Protocol>
class Client{
private:
	struct sockaddr_in serverIP;
	int socket_fd;
	Protocol proto;
public:
	Client(){
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		serverIP.sin_family = AF_INET;
	}
	Client(int family, int stream){
		socket_fd = socket(family, stream, 0);
		serverIP.sin_family = family;
	}
	~Client(){}
	void setProtocol(Protocol p){proto = p;}
	void connectTo(std::string address, int port){
		serverIP.sin_port = port;
		serverIP.sin_addr.s_addr = inet_addr(address.c_str());
		connect(socket_fd, (struct sockaddr*) &serverIP, sizeof(serverIP));
	}
	void startCommunicating(){
		proto.handle(SocketStream(socket_fd));
	}
};
}