/*--Author£∫¿Ó∫ÍΩ‹--*/

#include "rtpHandler.h"

rtpHandler *rtpHandler::instance = new rtpHandler;

rtpHandler * rtpHandler::getInstance()
{
	return instance;
}

void rtpHandler::unpackRTP(string rtpPacket)
{
}

void rtpHandler::getMedia(vector<int>& img)
{
	
}

rtpHandler::rtpHandler()
{
	
}