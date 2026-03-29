#include "Exceptions.hpp"
#include "Server.hpp"

//BaseException
BaseException::BaseException(std::string msg) : message(msg) {}

void BaseException::cleanup() {}

const char* BaseException::what() const throw()
{
	return message.c_str();
}
BaseException::~BaseException() throw() {}

//InitException
InitException::InitException(std::string msg) : BaseException(msg) {}
void InitException::cleanup()
{
	Server::closeServerSocket();
}
//RunException
RunException::RunException(std::string msg) : BaseException(msg) {}
void RunException::cleanup()
{
	Server::cleanResources();
}
