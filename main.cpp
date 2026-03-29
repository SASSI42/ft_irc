#include <iostream>
#include "Server/Server.hpp"

int main(int arc, char **arg)
{
	signalHandlers();

	try
	{
		Server::parse(arc, arg);
		Server::init();
		Server::run();
		Server::cleanResources();
	}
	catch(BaseException& e)
	{
		e.cleanup();
		std::cerr << e.what() << '\n';
		return 1;
	}
	return 0;
}