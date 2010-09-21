#ifndef OFXSNORKELH
#define OFXSNORKELH

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "snorkel.h"
#include "ofxSnorkelResponse.h"

class ofxSnorkelCallback {
public:
	virtual call_status_t execute(ofxSnorkelResponse* pResponse) = 0;
};

class ofxSnorkel {
public:
	static ofxSnorkel* getInstance();
	bool setup(int nPort, std::string sDocRoot = ".");
	bool start();

	// must be called before ofxSnorkel::start() (see snorkel.h line 325)
	bool addURICallback(std::string sURI, ofxSnorkelCallback* pCallback, encodingtype_t nEncodingType = encodingtype_text);

private:
	ofxSnorkel();
	std::vector<ofxSnorkelCallback*>* getCallbacks(std::string sURI);
	static call_status_t snorkelURICallback(snorkel_obj_t oHTTP, snorkel_obj_t oOutputStream);
	static ofxSnorkel instance;
	snorkel_obj_t http;
	std::map<std::string, std::vector<ofxSnorkelCallback*>* > callbacks;
	void handleURICallback(std::string sURI, snorkel_obj_t oHTTP, snorkel_obj_t oOutputStream);
};

#endif
