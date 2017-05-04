#ifndef __CONNECTION_MANAGER__
#define __CONNECTION_MANAGER__

#include "all_includes.h"


class ConnectionManager 
#ifdef USE_XBEE
		: public libxbee::ConCallback
#endif
{
	friend class Autopilot;

public:

#ifdef USE_XBEE

	explicit ConnectionManager(libxbee::XBee &parent, std::string type, 
							struct xbee_conAddress * address = NULL) :libxbee::ConCallback(parent, type, address) { finished = true; };
	void xbee_conCallback(libxbee::Pkt **pkt);
	std::string myData;
#else
	ConnectionManager();
#endif
	~ConnectionManager();


//protected:

	std::vector<my_MSG> messages_to_send;
	std::vector<my_MSG> messages_received;
	std::vector<my_MSG> temp;
	std::mutex mut_send;
	std::mutex mut_recv;
	bool finished;
	
	int sockfd, newsockfd, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	
	bool initializeConnection();
	void closeConnection();
	void deserialize(char*, my_MSG *);
	void serialize(char*, my_MSG *);
	void sender();
	void listener();
	void error(char *msg);
	void sendData(my_MSG msg);
	my_MSG getData();
	
	void mySend(my_MSG msg);
	std::vector<my_MSG> myReceive();
	void printMsg(my_MSG msg);
	
};


#endif //__CONNECTION_MANAGER__
