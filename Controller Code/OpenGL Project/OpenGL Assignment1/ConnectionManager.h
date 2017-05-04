#include "all_headers.h"


class ConnectionManager 
#ifdef USE_XBEE
	: public libxbee::ConCallback 
#endif

{

	friend class Autopilot;

public:

#ifdef USE_XBEE
	explicit ConnectionManager(libxbee::XBee &parent, std::string type, struct xbee_conAddress *address = NULL) : libxbee::ConCallback(parent, type, address) { finished = true; };
	void xbee_conCallback(libxbee::Pkt **pkt);
	std::string myData;
#else

	ConnectionManager();
#endif
	~ConnectionManager();

protected:

	std::string target_ip;
	int target_port;

	std::vector<my_MSG> messages_to_send;
	std::vector<my_MSG> messages_received;
	std::vector<my_MSG> temp;
	std::mutex			mut_send;			//mutex for temp (which is a holder for messages_to_send)
	std::mutex			mut_recv;			//mutex for messages_received 

	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	WSADATA wsa;
	bool finished;

	bool loadTargetIP();
	bool initializeConnection();
	void closeConnection();
	void deserialize(char*, my_MSG*);
	void serialize(char*, my_MSG*);
	void sender();
	void listener();
	void error(char *msg);
	void sendData(my_MSG msg);
	my_MSG getData();

	void mySend(my_MSG msg);
	std::vector<my_MSG> myReceive();
	void printMsg(my_MSG msg);

	__int64 timePoint() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
};