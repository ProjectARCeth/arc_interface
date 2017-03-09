#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char**argv){
	std::string str = "usb";
	std::fstream f;
	f.open("/dev/ttyS0"); 
	while (f >> str){std::cout << str;}
}