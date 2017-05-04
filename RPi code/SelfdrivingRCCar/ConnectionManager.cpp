#include "all_includes.h"



#ifndef USE_XBEE
ConnectionManager::ConnectionManager() {

	finished = true;
	//m_Print("Connection Manager created");
}
#endif


ConnectionManager::~ConnectionManager() {

	//m_Print("Connection Manager destroyed");
	my_MSG disc;
	disc.type = "DISCONNECT";
	
	sendData(disc);	

	closeConnection();
}

void ConnectionManager::error(char *msg) {
	perror(msg);
	std::cout << msg << std::endl;
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
	
	messages_to_send.clear();
	messages_received.clear();
	temp.clear();
	
	std::cout << "\nInitializing Connection...\n";
	
#ifndef USE_XBEE
	finished = true;
	
	printf("using port #%d\n", PORT_NUM);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error(const_cast<char *>("ERROR opening socket"));
		return false;
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT_NUM);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
		error(const_cast<char *>("ERROR on binding"));
		return false;
	}
	
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	//--- wait on a connection ---
	printf("waiting for new client...\n");
	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen)) < 0) 
	{
		error(const_cast<char *>("ERROR on accept"));
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

void ConnectionManager::closeConnection() {
	
	std::cout << "Closing Connection...\n";	
	finished = true;
	
#ifndef USE_XBEE
	close(newsockfd);
	close(sockfd);
#endif
	
}

void ConnectionManager::deserialize(char* to_deserialize, my_MSG * result) {
	
	std::string delim = "&&&";
	std::string to_deserial = to_deserialize;
	
	try {
		to_deserial.erase(0, 0 + delim.length());
		size_t pos = to_deserial.find(delim);	
		
		result->type = to_deserial.substr(0, pos);
		to_deserial.erase(0, pos + delim.length());
		pos = to_deserial.find(delim);	
		
		result->unique_id = stoi(to_deserial.substr(0, pos));
		to_deserial.erase(0, pos + delim.length());
		pos = to_deserial.find(delim);
		
		result->message = to_deserial.substr(0, pos);
		to_deserial.erase(0, pos + delim.length());
		pos = to_deserial.find(delim);
		
		result->num = stoi(to_deserial.substr(0, pos));
		
	} catch (...) {
		std::cout << "Error deserialize\n";
	}
		
}

void ConnectionManager::serialize(char* result, my_MSG * to_serialize) {
	try {
		memset(result, '\0', BUFLEN);
		char pattern[] = "&&&";
		size_t i = 0;
		size_t j = 0;
		std::string data;
		
		memcpy(result + i, pattern, strlen(pattern)); i+= strlen(pattern); //start
		for (;j < to_serialize->type.size();j++) { //type
			result[i++] = to_serialize->type[j];
		}
		memcpy(result + i, pattern, strlen(pattern)); i+= strlen(pattern);
		
		data = std::to_string(to_serialize->unique_id);//unique_id
		for (j = 0;j < data.size();j++) { 
			result[i++] = data[j];
		}
		memcpy(result + i, pattern, strlen(pattern)); i+= strlen(pattern);
		
		for (j = 0;j < to_serialize->message.size();j++) { //message
			result[i++] = to_serialize->message[j];
		}
		memcpy(result + i, pattern, strlen(pattern)); i+= strlen(pattern);
		
		data = std::to_string(to_serialize->num);//num
		for (j = 0;j < data.size();j++) { 
			result[i++] = data[j];
		}
		memcpy(result + i, pattern, strlen(pattern)); i+= strlen(pattern); //finish
		
	} catch (...) {
		std::cout <<  "Error serialize()\n";
	}
}

void ConnectionManager::sender() {
	
	std::cout << "Sender Started...\n";
	
	try {
		
		while(!finished) {
				
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
	catch( std::exception e) {
		std::cout << "Sender failed! " << e.what();
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
		std::cout << "Listener failed! " << e.what();
	}
	std::cout << "Listener Finished...\n";
}


void ConnectionManager::sendData(my_MSG msg) {
	
	int n = 5;
	char buffer[BUFLEN];
		
	serialize(buffer, &msg);

#ifndef USE_XBEE
	if ((n = write(newsockfd, buffer, BUFLEN) < 0))
		error(const_cast<char *>("ERROR writing to socket"));
#else
	try {
		*this << buffer;
		
	} catch (xbee_err err) {
		
		printf("send<<: %d - %s\n", err, xbee_errorToStr(err));
		
		if (err != XBEE_ETIMEOUT ) {
			n = -1;
		}
	} catch (libxbee::xbee_etx &etx_err) {
		std::cout << "TX error: " << etx_err.ret << " retVal: " << etx_err.retVal << "\n";
	}
	
	
#endif
		
	if (n < 0) {
		finished = true;
		
		my_MSG disc_msg;
		disc_msg.type = "DISCONNECT";
		
		mut_recv.lock();
		messages_received.push_back(disc_msg);
		mut_recv.unlock();
		
		delay(1000);
	}
}

my_MSG ConnectionManager::getData() {
	
	char buffer[BUFLEN];
	my_MSG result;
	int n;

	if ((n = read(newsockfd, buffer, BUFLEN)) < 0)
		error(const_cast<char *>("ERROR reading from socket"));
		
	if (n < 0) {
		result.type = "DISCONNECT";
		finished = true;
		delay(1000);
		return result;
	}
		
	deserialize(buffer, &result);
	
	return result;
	
}

void ConnectionManager::mySend(my_MSG msg) {
	mut_send.lock();
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

void ConnectionManager::printMsg(my_MSG msg) {
	
	std::cout << "type::" << msg.type << std::endl;
	std::cout << "id::" << msg.unique_id << std::endl;
	std::cout << "msg::" << msg.message << std::endl;
	std::cout << "num::" << msg.num << std::endl;
	
}
