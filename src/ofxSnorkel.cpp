#include "ofxSnorkel.h"
#include "ofxLog.h"
ofxSnorkel ofxSnorkel::instance;

ofxSnorkel::ofxSnorkel() {
	OFXLOG("ofxSnorkel::ofxSnorkel()");
}

ofxSnorkel* ofxSnorkel::getInstance() {
	return &instance;
}

// somehow I get a "result too large" error when adding callback
// after the call to"snorkel_obj_set(http, snorkel_attrib_ipvers, IPVERS_IPV4, SOCK_SET)"
bool ofxSnorkel::setup(int nPort, std::string sDocRoot) {
	if(snorkel_init() != SNORKEL_SUCCESS) {
		OFXLOG("ofxSnorkel: ERROR: initializing sorkel");
		return false;
	}

	snorkel_obj_create(snorkel_obj_log, "ofxSnorkel.log");
	snorkel_debug (1);


	// create with doc root (when incorrect dir given; this will crash!
	http = snorkel_obj_create(snorkel_obj_server,2,sDocRoot.c_str());
	if(!http) {
		OFXLOG("ofxSnorkel: ERROR: creating http object. ");
	}

	// start listening on port XX
	if(snorkel_obj_set(http, snorkel_attrib_listener, nPort ,0) != SNORKEL_SUCCESS) {
		OFXLOG("ofxSnorkel: ERROR: binding http object on port: " << nPort);
		snorkel_obj_destroy(http);
		return false;
	}
	return true;
}
bool ofxSnorkel::start() {
	// start ip_v4
	if(snorkel_obj_set(http, snorkel_attrib_ipvers, IPVERS_IPV4, SOCK_SET) != SNORKEL_SUCCESS) {
		OFXLOG("ofxSnorkel: Error setting IP to v4 ");
		return false;
	}

	// start the http server
	if(snorkel_obj_start(http) != SNORKEL_SUCCESS) {
		OFXLOG("ofxSnorkel: Cannot start snorkel http server ");
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
        OFXLOG("ofxSnorkel: number of callbacks:" << call->size());
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
		OFXLOG("ofxSnorkel: Error, cannot set callback handler for: " << sURI);
		return false;
	}
	return true;
}

std::vector<ofxSnorkelCallback*>* ofxSnorkel::getCallbacks(std::string sURI) {
	std::map<std::string, std::vector<ofxSnorkelCallback*>* >::iterator it = callbacks.find(sURI);
	if(it == callbacks.end()) {
		OFXLOG("ofxSnorkel: not yet a callback added for: " << sURI);
		return NULL;
	}
	return (it->second);
}

void ofxSnorkel::handleURICallback(std::string sURI, snorkel_obj_t oHTTP, snorkel_obj_t oOutputStream) {
	OFXLOG("ofxSnorkel: handling an uri callback for: " << sURI);
	std::vector<ofxSnorkelCallback*>* uri_callbacks = getCallbacks(sURI);
	if(uri_callbacks == NULL) {
		OFXLOG("ofxSnorkel: error, no callbacks found for the uri: " << sURI);
		return;
	}
	std::vector<ofxSnorkelCallback*>::iterator it = uri_callbacks->begin();
	ofxSnorkelResponse response(oHTTP, oOutputStream);
	while(it != uri_callbacks->end()) {
		if((*it) != NULL) {
			(*it)->execute(&response);
		}
		else {
			OFXLOG("ofxSnorkel: error while executing a callback... ");
		}
 		++it;
	}
}

call_status_t ofxSnorkel::snorkelURICallback(snorkel_obj_t oHTTP, snorkel_obj_t oOutputStream) {
    OFXLOG("ofxSnorkel::snorkelURICallback()");
	char uri_val[1024];
	snorkel_obj_get(oHTTP, snorkel_attrib_uri,uri_val, 1024);
	ofxSnorkel* snorkel = ofxSnorkel::getInstance();
	std::string uri = uri_val;
	snorkel->handleURICallback(uri, oHTTP, oOutputStream);
	return HTTP_SUCCESS;
}
