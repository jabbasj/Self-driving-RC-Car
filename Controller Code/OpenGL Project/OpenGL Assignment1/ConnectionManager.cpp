#include "all_headers.h"

#ifndef USE_XBEE
ConnectionManager::ConnectionManager() {
	finished = true;
}
#endif

ConnectionManager::~ConnectionManager() {

	my_MSG disc;
	disc.type = "DISCONNECT";

	sendData(disc);

	closeConnection();
}



void ConnectionManager::mySend(my_MSG msg) {
	mut_send.lock();
	//std::cout << "Time point: " << timePoint() << "ms\n";
	temp.push_back(msg);
	mut_send.unlock();
}

std::vector<my_MSG> ConnectionManager::myReceive() {

	std::vector<my_MSG> msgs;

	if (messages_received.size() > 0) {

		mut_recv.lock();
		msgs = messages_received;
		messages_received.clear();
		mut_recv.unlock();

	}

	return msgs;
}


#ifdef USE_XBEE
void ConnectionManager::xbee_conCallback(libxbee::Pkt **pkt) {
	
	char buff[BUFLEN];
	memset(buff, '\0', BUFLEN);

	int i;
	if ((*pkt)->size() < BUFLEN) {
		for (i = 0; i < (*pkt)->size(); i++) {
			buff[i] = (**pkt)[i];
		}
	}

	my_MSG msg;
	deserialize(buff, &msg);
	
	mut_recv.lock();
	messages_received.push_back(msg);
	mut_recv.unlock();

	if (msg.type == "DISCONNECT") {
		finished = true;
	}
}
#endif


bool ConnectionManager::initializeConnection() {

	std::cout << "\nInitializing connection...\n";

	messages_to_send.clear();
	messages_received.clear();
	temp.clear();

#ifndef USE_XBEE
	finished = true;

	if (!loadTargetIP()) {
		return false;
	}

	WSAStartup(MAKEWORD(2, 2), &wsa);
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error(const_cast<char *>("ERROR opening socket"));
		return false;
	}

	if ((server = gethostbyname(target_ip.c_str())) == NULL) {
		error(const_cast<char *>("ERROR, no such host\n"));
		return false;
	}

	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(target_port);
	serv_addr.sin_addr.S_un.S_addr = inet_addr(target_ip.c_str());


	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		error(const_cast<char *>("ERROR connecting"));
		return false;
	}

#endif

	finished = false;

	std::thread s(&ConnectionManager::sender, this);
	s.detach();

#ifndef USE_XBEE //using callback instead
	std::thread l(&ConnectionManager::listener, this);
	l.detach();
#endif

	return true;
}

bool ConnectionManager::loadTargetIP() {

	std::ifstream input_file("targetconfig.txt");
	std::string line;

	try {
		if (input_file.is_open()) {

			while (getline(input_file, line)) {

				//ip:343.213.123.57,port:432432
				if (line.find("ip:") != std::string::npos) {

					target_ip = line.substr(line.find("ip:") + std::strlen("ip:"), line.find(",port:") - line.find("ip:") - std::strlen("ip:"));
					target_port = std::stoi(line.substr(line.find("port:") + std::strlen("port:"), -1));
				}
			}

			input_file.close();
		}
	}
	catch (...) {
		std::cout << "error loadTargetIP\n";
		return false;
	}

	return true;
}

void ConnectionManager::deserialize(char* to_deserialize, my_MSG* result) {

	std::string to_deserial = to_deserialize;
	/*	^^^type(str)^^^unique_id(int)^^^message(str)^^^num(int)^^^	*/
	std::regex r("&&&(.*)&&&(.*)&&&(.*)&&&(.*)&&&");
	std::smatch match_result;
	std::regex_match(to_deserial, match_result, r);

	if (!match_result.empty() && match_result[0].length() > 0 && match_result.size() == 5) {
		result->type = match_result[1];
		result->unique_id = stoi(match_result[2]);
		result->message = match_result[3];
		result->num = stoi(match_result[4]);
	}
	else {
		std::cout << "Error deserialize()\n";
	}
}

void ConnectionManager::serialize(char* result, my_MSG* to_serialize) {
	try {
		memset(result, '\0', BUFLEN);
		char pattern[] = "&&&";
		size_t i = 0;
		size_t j = 0;
		std::string data;

		memcpy(result + i, pattern, strlen(pattern)); i += strlen(pattern); //start
		for (; j < to_serialize->type.size(); j++) { //type
			result[i++] = to_serialize->type[j];
		}
		memcpy(result + i, pattern, strlen(pattern)); i += strlen(pattern);

		data = std::to_string(to_serialize->unique_id); //unique_id
		for (j = 0; j < data.size(); j++) {
			result[i++] = data[j];
		}
		memcpy(result + i, pattern, strlen(pattern)); i += strlen(pattern);

		for (j = 0; j < to_serialize->message.size(); j++) { //message
			result[i++] = to_serialize->message[j];
		}
		memcpy(result + i, pattern, strlen(pattern)); i += strlen(pattern);

		data = std::to_string(to_serialize->num); //num
		for (j = 0; j < data.size(); j++) {
			result[i++] = data[j];
		}
		memcpy(result + i, pattern, strlen(pattern)); i += strlen(pattern); //finish
	}
	catch (...) {
		std::cout << "Error serialize()\n";
	}
}

void ConnectionManager::sender() {
	std::cout << "Sender Started...\n";

	try {
		while (!finished)
		{
			if (temp.size() > 0) {
				printf("Sending (%d) pending messages...\n", temp.size());

				mut_send.lock();
				messages_to_send = temp;
				temp.clear();
				mut_send.unlock();
			}
			while (messages_to_send.size() > 0 && !finished) {

				my_MSG msg_to_send = messages_to_send.front();

				sendData(msg_to_send);

				messages_to_send.erase(messages_to_send.begin());
			}			
		}
	}
	catch (std::exception e) {
		std::cout << e.what();
	}

	std::cout << "Sender Finished...\n";
}

void ConnectionManager::listener() {

	std::cout << "Listener Started...\n";

	try {

		while (!finished) {

			my_MSG msg = getData();

			printMsg(msg);

			mut_recv.lock();
			messages_received.push_back(msg);
			mut_recv.unlock();

			if (msg.type == "DISCONNECT" || finished) {
				break;
			}
		}
	}
	catch (std::exception e) {
		std::cout << e.what();
	}

	std::cout << "Listener finished...\n";
}

void ConnectionManager::error(char *msg) {
	perror(msg);
	printf("failed with error code : %d\n", WSAGetLastError());
}

void ConnectionManager::sendData(my_MSG msg) {
	int n = 5;

	char buffer[BUFLEN];
	serialize(buffer, &msg);

#ifndef USE_XBEE
	if ((n = send(sockfd, buffer, BUFLEN, 0)) < 0)
		error(const_cast<char *>("ERROR writing to socket"));
#else

	try {
		*this << buffer;
	}
	catch (xbee_err err) {
		printf("send<<: %d - %s\n", err, xbee_errorToStr(err));

		if (err != XBEE_ETIMEOUT) {
			//n = -1;
		}
	}
	catch (libxbee::xbee_etx &etx_err) {

		std::cout << "TX error<<: " << etx_err.ret << " retVal: " << etx_err.retVal << "\n";		
	}
#endif

	if (n <= 0) {
		finished = true;

		my_MSG disc_msg;
		disc_msg.type = "DISCONNECT";

		mut_recv.lock();
		messages_received.push_back(disc_msg);
		mut_recv.unlock();

		Sleep(1000);
	}
}

my_MSG ConnectionManager::getData() {
	char buffer[BUFLEN];
	my_MSG result;
	int n;

	if ((n = recv(sockfd, buffer, BUFLEN, 0)) < 0)
		error(const_cast<char *>("ERROR reading from socket"));

	if (n <= 0) {
		result.type = "DISCONNECT";
		finished = true;

		Sleep(1000);

		return result;
	}

	deserialize(buffer, &result);

	return result;
}

void ConnectionManager::closeConnection()
{
	std::cout << "Closing connecting...\n";
	finished = true;

	my_MSG disc;
	disc.type = "DISCONNECT";
	sendData(disc);

#ifndef USE_XBEE
	closesocket(sockfd);
	WSACleanup();
#endif
	return;
}

void ConnectionManager::printMsg(my_MSG msg) {
	std::cout << "type::" << msg.type << std::endl;
	std::cout << "id::" << msg.unique_id << std::endl;
	std::cout << "msg::" << msg.message << std::endl;
	std::cout << "num::" << msg.num << std::endl << std::endl;
}