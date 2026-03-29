#include "Commands.hpp"

Command::Command(Client& client) : client(client), paramsVec(client.getParamsVec()) {}

Command::~Command() {}
