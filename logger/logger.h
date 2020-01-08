#ifndef __LOGGER__
#define __LOGGER__

#include<time.h>
#include<mutex>
#include<fstream>
#include<iostream>
#include<stdlib.h>
namespace networking{
class Logger{
private:
	std::ostream out;
	std::ofstream of;//needs to be given a global scope for ostream
	std::string spacer;
	std::string delim;
	std::string seperator;
	int level;
	time_t clock;
	std::mutex mtx;
	void print_log(std::string mode, std::string message){
		mtx.lock();
		time(&clock);
		out << mode << " " <<ctime(&clock) << spacer << message << delim << seperator;
		mtx.unlock();
	}
public:
	Logger():out(std::cout.rdbuf()){
		level = 0;
		spacer = "";
		delim = "\n";
		seperator = "\n";
	}
	~Logger(){}
	void setLevel(int level){
		level = level;
	}
	void redirect(std::string filename){
		of.open(filename, std::fstream::out);
		out.rdbuf(of.rdbuf());
	}	
	void setDelimiter(std::string d){delim = d;}
	void setSpacer(std::string s){spacer = s;}
	void setSeperator(std::string s){seperator = s;}
	
	void error(std::string args){
		if(level > 4) return;
		print_log("ERROR", args);
	}
	void critical(std::string args){
		if(level > 3) return;
		print_log("CRITICAL", args);
	}
	void warning(std::string args){
		if(level > 2) return;
		print_log("WARNING", args);
	}
	void info(std::string args){
		if(level > 1) return;
		print_log("INFO", args);
	}
	void debug(std::string args){
		if(level > 0) return;
		print_log("DEBUG", args);
	}
} logger;
}
#endif
