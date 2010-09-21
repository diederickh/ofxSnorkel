#ifndef OFXSNORKELRESPONSEH
#define OFXSNORKELRESPONSEH

#include <string>
#include <iostream>
#include <vector>

#include "snorkel.h"
class ofxSnorkelResponse {
public:
	ofxSnorkelResponse(snorkel_obj_t oHTTP, snorkel_obj_t oOutputStream);
	std::string getParameter(std::string sName);
	bool sendFile(std::string sFile);
private:
	snorkel_obj_t http;
	snorkel_obj_t output_stream;
};

#endif
