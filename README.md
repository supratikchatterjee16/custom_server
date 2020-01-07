# Networking Lib

This is a library made to leverage the template system to work with any protocol imaginable.  
It is intended to be as customizable as imaginable, offering a Reader stream to work with the bytes.  
It is basically a stand-in for the low level sockets implementations one might have to make.  

## A Brief Sample

```C++
#include "path/to/folder/with/headerfile/networking.h"
class YourProtocol{
public:
	void handle(SocketStreamScanner sss){
		//do something with the SocketStreamScanner and return the response.
		//Both the appropriate documentations for the SocketStreamScanner and the output structure
		//Will be provided when they become well defined.
	}
}

int main(int argc, char* argv[]){
	Server<YourProtocol> s;
	s.start(); //See. isn't that simple.
	//Currently it's being developed to be as thread safe as possible.
}
```
