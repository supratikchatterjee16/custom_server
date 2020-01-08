class SocketStreamReader{
private:
	int socket_fd;
	char buffer[2048];
	int buffer_position;
	int buffer_read_size;
	std::string delimiter;
	bool EOS;
	void setbackTo(int pos){buffer_position = pos;}//sets buffer_position to required position
	int getNextBuffer(){
		buffer_position = 0;
		buffer_read_size = read(socket_fd, buffer, sizeof(buffer) -1);
		return buffer_read_size;
	}
public:
	SocketStreamReader(int socket){
		socket_fd = socket;
		delimiter = "\r\n";
	}
	~SocketStreamReader(){}
	std::string readAll(){//unsafe due to being able to overload the RAM
		std::string temp = "";
		if(EOS) return temp;
		while(getNextBuffer() > 0){
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
			getNextBuffer();
		}
		line.append(buffer);
		line.erase(0, buffer_position);
		int original_position = buffer_position;
		buffer_position += line.find_first_of(delimiter);
		line = line.substr(0, buffer_position - original_position);
		buffer_position += delimiter.size();
		return line;
	}
	std::string readRemaining(){//reads the remaining(unsafe)
		std::string temp(buffer);
		temp.erase(0, buffer_position);
		while(getNextBuffer() > 0){
			temp.append(buffer);
			if(buffer_read_size < sizeof(buffer)) break;
		}
	}
	const char* readNext(int size){//reads the next size number of bytes
		std::string temp(buffer);
		temp.erase(0, buffer_position);
		while(getNextBuffer() > 0){
			temp.append(buffer);
			if(buffer_read_size < sizeof(buffer)) break;
		}
		return temp.c_str();
	}
};
 

class SocketStreamWriter{
private:
	int socket_fd;
	std::string delimiter;
public:
	SocketStreamWriter(int socket){
		socket_fd = socket;
		delimiter = "\r\n";
	}
	~SocketStreamWriter(){}
	void send(char* bytes, int length){write(socket_fd, bytes, length);}
	void send(std::string message){write(socket_fd, message.c_str(), message.length());}
};

class SocketStream{
private:
	SocketStreamWriter sw;
	SocketStreamReader sr;
	struct sockaddr_in address;
	int socket_fd;
public:
	SocketStream(int socket): sw(socket), sr(socket){
		socket_fd = socket;
		socklen_t addr_size = sizeof(struct sockaddr_in);
		getpeername(socket_fd, (struct sockaddr *)&address, &addr_size);
	}
	int getClientPort(){
		return address.sin_port;
	}
	std::string getClientAddress() {
		std::string s(inet_ntoa(address.sin_addr));
		return s;
	}
	SocketStreamReader getStreamReader(){return this->sr;}
	SocketStreamWriter getStreamWriter(){return this->sw;}
};