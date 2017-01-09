#include "all_includes.h"

//****************************************
//				GLOBAL Variables
//****************************************
Autopilot m_Autopilot;


//****************************************
//				MAIN
//****************************************
int main() {

	m_Autopilot.ready_to_start = true;

	m_Print(m_Autopilot.GetCurrentState());

	m_Autopilot.Start();

	m_Print(m_Autopilot.GetCurrentState());

	m_Pause();
	return 0;
}



//****************************************
//				GLOBAL Functions
//****************************************

void m_Print(const char msg[]) {

#ifdef VERBOSE
	std::cout << msg << std::endl;
#endif

}

//TODO: write this function for raspberry pi
void m_Log(const char msg[]) {

#ifdef LOGFILE
	//log msg into text file
#endif

}


void m_Pause() {
	std::cout << "Press enter to continue ...";
	std::cin.get();
}