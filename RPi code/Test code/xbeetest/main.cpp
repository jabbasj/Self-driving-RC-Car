/*
	libxbee - a C/C++ library to aid the use of Digi's XBee wireless modules
	          running in API mode.
	Copyright (C) 2009 onwards  Attie Grande (attie@attie.co.uk)
	libxbee is free software: you can redistribute it and/or modify it
	under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	libxbee is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	GNU Lesser General Public License for more details.
	You should have received a copy of the GNU Lesser General Public License
	along with libxbee. If not, see <http://www.gnu.org/licenses/>.

g++-4.8 -Wall -c "%f"
g++-4.8 -I/home/pi/projects/xbeetest -L/home/pi/projects/xbeetest -Wall -lpthread -lxbeep -lxbee -o "%e" "%f"

*/

#include <iostream>
#include <string.h>
#include <unistd.h>

#include <xbeep.h>


//Call backs
/* ========================================================================== */

class myConnection: public libxbee::ConCallback {
	public:
		explicit myConnection(libxbee::XBee &parent, std::string type, struct xbee_conAddress *address = NULL): libxbee::ConCallback(parent, type, address) {};
		void xbee_conCallback(libxbee::Pkt **pkt);
		std::string myData;
};

void myConnection::xbee_conCallback(libxbee::Pkt **pkt) {
	std::cout << "Callback!!\n";
	int i;
	for (i = 0; i < (*pkt)->size(); i++) {
		std::cout << (**pkt)[i];
	}
	std::cout << "\n";

	std::cout << myData;

	/* if you want to keep the packet, then you MUST do the following:
	      libxbee::Pkt *myhandle = *pkt;
	      *pkt = NULL;
	   and then later, you MUST delete the packet to free up the memory:
	      delete myhandle;
	   if you do not want to keep the packet, then just leave everything as-is, and it will be free'd for you */
}

/* ========================================================================== */

int main(int argc, char *argv[]) {

	/* get available libxbee modes */
	try {
		std::list<std::string> modes = libxbee::getModes();
		std::list<std::string>::iterator i;
		
		std::cout << "Available libxbee modes:\n";
		for (i = modes.begin(); i != modes.end(); i++) {
			std::cout << "  " << *i;
		}
		std::cout << "\n";
	} catch (xbee_err ret) {
		std::cout << "Error while retrieving libxbee modes...\n";
	}


	try {

		/* setup libxbee */
		libxbee::XBee xbee("xbee1", "/dev/ttyUSB0", 9600);
		std::cout << "Running libxbee in mode '" << xbee.mode() << "'\n";

		
		/* get available connection types */
		try {
			std::list<std::string> types = xbee.getConTypes();
			std::list<std::string>::iterator i;
			
			std::cout << "Available connection types:\n";
			for (i = types.begin(); i != types.end(); i++) {
				std::cout << "  " << *i;
			}
			std::cout << "\n";
		} catch (xbee_err ret) {
			std::cout << "Error while retrieving connection types...\n";
		}

		
		/* make a connection */
		struct xbee_conAddress addr;
		memset(&addr, 0, sizeof(addr));
		addr.addr16_enabled = 1;
		addr.addr16[0] = 0x00;
		addr.addr16[1] = 0x00;

		myConnection con(xbee, "16-bit Data", &addr); /* with a callback */
		con.myData = "Testing, 1... 2... 3...\n";

		/* send data */
		//con.Tx("NI"); /* like this */
		//con << "NI";    /* or like this */
		con.Tx("THIS IS A TEST THIS IS A TEST THIS IS A TEST THIS IS A TEST");

		/* NOTE: if you're trying to set a configuration option, then you'll need to provide
		         the RAW value, not the ASCII character. To do this, see the following example:
		             con << "D4" + std::string(1, 0x05);
		         where "D4" will query or set the configuration of pin 4, and 0x05 will set it
		         to be an output, with a logic 'high' value
		         this is the C++ equivelant to calling the following from C:
		             xbee_conTx(con, NULL, "D4%c", 0x05);                                      */

		usleep(100000000);

	} catch (xbee_err err) {
		std::cout << "Error " << err << ": " << xbee_errorToStr(err) << "\n";
	}
	
	return 0;
}
