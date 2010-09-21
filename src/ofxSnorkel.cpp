#include "ofxSnorkel.h"

ofxSnorkel ofxSnorkel::instance;

ofxSnorkel::ofxSnorkel() {
	std::cout << "ofxSnorkel()" << std::endl;
}

ofxSnorkel* ofxSnorkel::getInstance() {
	return &instance;
}

// somehow I get a "result too large" error when adding callback
// after the call to"snorkel_obj_set(http, snorkel_attrib_ipvers, IPVERS_IPV4, SOCK_SET)"
bool ofxSnorkel::setup(int nPort, std::string sDocRoot) {
	if(snorkel_init() != SNORKEL_SUCCESS) {
		std::cout << "ofxSnorkel: Error initializing sorkel";
		return false;
	}

	snorkel_obj_create(snorkel_obj_log, "ofxSnorkel.log");
	snorkel_debug (1);


	// create with doc root (when incorrect dir given; this will crash!
	http = snorkel_obj_create(snorkel_obj_server,2,sDocRoot.c_str());
	if(!http) {
		std::cout << "Error creating http object: "  <<std::endl;
	}

	// start listening on port XX
	if(snorkel_obj_set(http, snorkel_attrib_listener, nPort ,0) != SNORKEL_SUCCESS) {
		std::cout << "ofxSnorkel: Error binding http object on port: " << nPort <<std::endl;
		snorkel_obj_destroy(http);
		return false;
	}

/* TEST
	if(snorkel_obj_set(
					http
					,snorkel_attrib_uri
					,GET
					,"/index.html"
					,encodingtype_text
					,call_cb) != SNORKEL_SUCCESS)
	{
		perror("callback not working!");
		//std::cout << "Test callback not working." << std::endl;
	}
	else {
		std::cout << "Callback WORKS!!!" << std::endl;
	}
	//
*/

	return true;
}
bool ofxSnorkel::start() {
	// start ip_v4
	if(snorkel_obj_set(http, snorkel_attrib_ipvers, IPVERS_IPV4, SOCK_SET) != SNORKEL_SUCCESS) {
		std::cout << "ofxSnorkel: Error setting IP to v4 " <<std::endl;
		return false;
	}

	// start the http server
	if(snorkel_obj_start(http) != SNORKEL_SUCCESS) {
		std::cout << "ofxSnorkel: Cannot start snorkel http server " <<std::endl;
		snorkel_obj_destroy(http);
		return false;
	}
	return true;
}

bool ofxSnorkel::addURICallback(std::string sURI, ofxSnorkelCallback* pCallback, encodingtype_t nEncodingType) {
	// add to the callback lists for the given uri.
	std::vector<ofxSnorkelCallback*>* call = getCallbacks(sURI);
	if(call == NULL) {
		std::vector<ofxSnorkelCallback*>* uri_callbacks = new std::vector<ofxSnorkelCallback*>();
		uri_callbacks->push_back(pCallback);
		callbacks[sURI] = uri_callbacks;
	}
	else {
		call->push_back(pCallback);
		std::cout << "ofxSnorkel: number of callbacks:" << call->size() << std::endl;
	}

	// request for a uri callback on snorkel.
	if(snorkel_obj_set(
					http
					,snorkel_attrib_uri
					,GET
					,sURI.c_str()
					,nEncodingType
					,ofxSnorkel::snorkelURICallback) != SNORKEL_SUCCESS)
	{
		std::cout << "ofxSnorkel: Error, cannot set callbackhandler for: " << sURI << std::endl;
		return false;
	}
	return true;
}

std::vector<ofxSnorkelCallback*>* ofxSnorkel::getCallbacks(std::string sURI) {
	std::map<std::string, std::vector<ofxSnorkelCallback*>* >::iterator it = callbacks.find(sURI);
	if(it == callbacks.end()) {
		std::cout << "ofxSnorkel: not yet a callback added for: " << sURI << std::endl;
		return NULL;
	}
	return (it->second);
}

void ofxSnorkel::handleURICallback(std::string sURI, snorkel_obj_t oHTTP, snorkel_obj_t oOutputStream) {
	std::cout << "ofxSnorkel: handling an uri callback for: " << sURI << std::endl;
	std::vector<ofxSnorkelCallback*>* uri_callbacks = getCallbacks(sURI);
	if(uri_callbacks == NULL) {
		std::cout << "ofxSnorkel: error, no callbacks found for the uri: " << sURI << std::endl;
		return;
	}
	std::vector<ofxSnorkelCallback*>::iterator it = uri_callbacks->begin();
	ofxSnorkelResponse response(oHTTP, oOutputStream);
	while(it != uri_callbacks->end()) {
		if((*it) != NULL) {
			(*it)->execute(&response);
		}
		else {
			std::cout << "hmm NULL? " << std::endl;
		}
 		++it;
	}
	std::cout << "Execute callbacks" << std::endl;
}

call_status_t ofxSnorkel::snorkelURICallback(snorkel_obj_t oHTTP, snorkel_obj_t oOutputStream) {
	//snorkel_printf(oOutputStream, "<html><body>test</body></html>\r\n");



	char uri_val[1024];
	snorkel_obj_get(oHTTP, snorkel_attrib_uri,uri_val, 1024);
	ofxSnorkel* snorkel = ofxSnorkel::getInstance();
	std::string uri = uri_val;
	snorkel->handleURICallback(uri, oHTTP, oOutputStream);
	return HTTP_SUCCESS;
}
