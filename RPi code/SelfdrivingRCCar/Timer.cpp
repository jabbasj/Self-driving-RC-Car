#include "all_includes.h"


Timer::Timer() {
	m_Print("Timer created");

}


Timer::~Timer() {

	//m_Print("Timer destroyed");

}

long Timer::timePoint() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	
}
