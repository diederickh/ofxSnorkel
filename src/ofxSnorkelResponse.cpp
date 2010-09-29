#include "ofxSnorkelResponse.h"
#include "ofxLog.h"

ofxSnorkelResponse::ofxSnorkelResponse(snorkel_obj_t oHTTP, snorkel_obj_t oOutputStream)
:http(oHTTP)
,output_stream(oOutputStream)
{
    OFXLOG("ofxSnorkelResponse() ");
}

std::string ofxSnorkelResponse::getParameter(std::string sName) {
	char buf[1024];
	snorkel_obj_get(http, snorkel_attrib_query, sName.c_str(), buf, 1024);
	std::string result = buf;
	return buf;
}

bool ofxSnorkelResponse::sendFile(std::string sFile) {
	OFXLOG("ofxSnorkelResponse::sendFile(): - begin, file=" << sFile);
	std::vector<char> writable(sFile.size() + 1);
	std::copy(sFile.begin(), sFile.end(), writable.begin());
	//snorkel_file_stream(output_stream, &writable[0],0,SNORKEL_USE_SENDFILE);
	snorkel_file_stream(output_stream, &writable[0],0,SNORKEL_FILE_SEND);
	OFXLOG("ofxSnorkelResponse::sendFile() - done, file=" << sFile);
	return true;
	//snorkel_obj_set(output_stream, snorkel_attrib_header,"Content-Type","image/jpeg");
}
