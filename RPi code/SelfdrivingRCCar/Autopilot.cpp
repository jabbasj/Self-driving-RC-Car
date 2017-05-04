#include "all_includes.h"



Autopilot::Autopilot() {

	m_Print("Autopilot created");
	
	READ_CONFIG();

	STARTED = FOLLOWING_PATH = TURN_INCOMING = STOP_INCOMING = OBSTACLE_DETECTED = false;
	speed_count = time_of_last_speed_update = last_total_distance = turn_angle = speed = turn_dist = map_turn_angle = stop_dist = obstacle_dist = 0;
	
	INIT_TRANSITIONS();
	
	//if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
	if(gpioInitialise() < 0){
		printf("setup gpio failed !");
	}
	
	if(wiringPiSetup() == -1) { //when initialize wiring failed,print messageto screen
		printf("setup gpio failed !");
	}
	m_SpeedEncoder.SetInterrupt();
	gpioSetMode(MUSIC_PIN, PI_OUTPUT);
	m_Driver.Initialize();

}

Autopilot::~Autopilot() {
	delete xbee;
	//delete m_ConnectionManager;
	//m_Print("Accelerometer destroyed");
}


void Autopilot::INIT_TRANSITIONS() {

	m_Print("Transitions defined");

	// Rest --- (start) --> Starting
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::REST, States::STARTING, Triggers::start, [&]{return (!STARTED || !FOLLOWING_PATH); }, [&]{start_autopilot(); SEND_SpeakMessage("STARTING AUTOPILOT"); std::cout << "STARTING..\n"; } },
	});
	
	// Starting --- (maintain) --> Cruising
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::STARTING, States::CRUISING, Triggers::maintain, [&]{return QUESTION_goForward(); }, [&]{ std::cout << "CRUISING..\n";} },
	});
	
	// Cruising --- (turn_ahead) --> Turning
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::CRUISING, States::TURNING, Triggers::turn_ahead, [&]{return TURN_INCOMING; }, [&]{ 
			
			std::string turnmsg;
			if (map_turn_angle > 0) {
				turnmsg = "RIGHT ";
			} else {
				turnmsg = "LEFT ";
			}
			turnmsg += "TURN IN " + std::to_string(turn_dist) + " cm";
			SEND_SpeakMessage(turnmsg);
			std::cout << "TURNING..\n";} },
	});
	
	// Cruising --- (stop_ahead) --> Stopping
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::CRUISING, States::STOPPING, Triggers::stop_ahead, [&]{return STOP_INCOMING; }, [&]
			{ 
				std::string stopmsg = "STOPPING IN " + std::to_string(stop_dist) + " cm";
				SEND_SpeakMessage(stopmsg); 
				std::cout << "STOPPING..\n"; } },
	});
	
	// Turning --- (turn_complete) --> Starting
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::TURNING, States::STARTING, Triggers::turn_complete, [&]{return !TURN_INCOMING; }, [&]{ SEND_SpeakMessage("TURN COMPLETE"); std::cout << "STARTING..\n";} },
	});
	
	
	// Turning --- (stop_ahead) --> Stopping
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::TURNING, States::STARTING, Triggers::stop_ahead, [&]{return !TURN_INCOMING; }, [&]{ SEND_SpeakMessage("STOPPING"); std::cout << "STOPPING..\n";} },
	});
	
	// Stopping --- (stop_ahead) --> Turning
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::STOPPING, States::TURNING, Triggers::turn_ahead, [&]{return !STOP_INCOMING; }, [&]{ SEND_SpeakMessage("TURNING"); std::cout << "TURNING..\n";} },
	});
	
	
	// Stopping --- (stop_complete) --> Starting
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::STOPPING, States::STARTING, Triggers::stop_complete, [&]{return !STOP_INCOMING; }, [&]{ m_Driver.SetForward(); std::cout << "STARTING..\n";} },
	});
	
	
	// Cruising --- (avoid_obstacle) --> AVOIDING_COLLISION
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::CRUISING, States::AVOIDING_COLLISION, Triggers::avoid_obstacle, [&]{return OBSTACLE_DETECTED; }, [&]{ SEND_SpeakMessage("AVOIDING COLLISION"); wave(MUSIC_FREQ, 3); std::cout << "AVOIDING_COLLISION..\n";} },
	});
	
	// Cruising --- (avoid_obstacle) --> AVOIDING_COLLISION
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::TURNING, States::AVOIDING_COLLISION, Triggers::avoid_obstacle, [&]{return OBSTACLE_DETECTED; }, [&]{ SEND_SpeakMessage("AVOIDING COLLISION"); wave(MUSIC_FREQ, 3); std::cout << "AVOIDING_COLLISION..\n";} },
	});
	
	// AVOIDING_COLLISION --- (obstacle_cleared) --> Cruising
	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::AVOIDING_COLLISION, States::CRUISING, Triggers::obstacle_cleared, [&]{return !OBSTACLE_DETECTED; }, [&]{ m_Driver.SetForward(); std::cout << "CRUISING..\n";} },
	});
	
}


void Autopilot::MAIN_LOOP() {
	
	std::cout << "MAIN_LOOP started...\n";
	

	while (true) {	
		
		while (!CONNECTION_ACTIVE || !STARTED || !FOLLOWING_PATH || fsm.state() == States::REST);
		
		float dist = m_SpeedEncoder.GetDistance() * 1000;
		last_disp = dist - last_total_disp; //* cos(m_Driver.servo_angle * 3.14 /180.0);
		last_total_disp = dist;
		
		OBSTACLE_DETECTED  = m_ObstacleDetection.CheckObstacle(&obstacle_dist);
								
		if (STOP_INCOMING) {
			stop_dist -= last_disp/1000.0;
		}
		
		if (TURN_INCOMING) {
			turn_dist -= last_disp/1000.0;
		}
		
		if (last_disp > 0) {
			SEND_DispMessage(last_disp);
		}
		
		switch(fsm.state()) {
			
			case States::REST:
				std::cout << "REST..\n";
				break;
				
			case States::STARTING:
				
				m_Driver.SetStraight();
				fsm.execute(Triggers::maintain);
				break;
			
			case States::CRUISING:
				
				maintain();
				
				if (QUESTION_obstacleFound()) {
					
					fsm.execute(Triggers::avoid_obstacle);
				}
				
				if (TURN_INCOMING) {
					
					fsm.execute(Triggers::turn_ahead);
				}
				
				if (STOP_INCOMING) {
					
					fsm.execute(Triggers::stop_ahead);
				}
		
			
				break;
				
			case States::TURNING: /*turn ahead*/
			
				if (QUESTION_turnAhead()) {
					
					if (execute_turn() == false) {
						//turn complete
						TURN_INCOMING = false;
						fsm.execute(Triggers::turn_complete);
						break;
					}
					
				} else {
					maintain();
				}
				
				if (QUESTION_obstacleFound()) {
					
					fsm.execute(Triggers::avoid_obstacle);
				}
				
				if (STOP_INCOMING) {
					
					fsm.execute(Triggers::stop_ahead);
				}
				
				break;
				
			case States::STOPPING:
			
				if (QUESTION_stopIncoming()) {
					execute_stop();
					
					delay(2000);
					STOP_INCOMING = false;
					
					fsm.execute(Triggers::stop_complete);					
					break;
					
				} else {
					maintain();
				}
			
				break;
				
			case States::AVOIDING_COLLISION:
			
				execute_stop();
				delay(5000);
			
				fsm.execute(Triggers::obstacle_cleared);
			
				break;
				
			default:
				std::cout << "UNEXPECTED STATE: " << GetCurrentState() << "\n";
				Stop();
				return;
		}
		//std::cout << "turn_angle: " << turn_angle << ", " << "speed: " << speed << ", " << "last_disp: " << last_disp << "\n";
		//delay(25);
	}
	
	std::cout << "MAIN_LOOP finished!\n";
	
}

//activates every 100 ms, calculates average instantaneous speed
void Autopilot::CalculateSpeed(bool * active) {
	
	std::cout << "Calculate Speed loop started...\n";
	
	while (1) {
		
		while (!(*active));
		
		speed_count++;
		float dist = m_SpeedEncoder.GetDistance();
		float delta_dist = dist - last_total_distance;
		float new_speed = delta_dist / (SPEED_CONTROL_TIME / 1000.0);
		speed = (new_speed + speed) / speed_count;
		//std::cout << "Speed: " << speed << "\n";		
		
		last_total_distance = dist;
		
		delay(SPEED_CONTROL_TIME);
	}
	
	std::cout << "Calculate Speed loop finished!\n";
}

void Autopilot::SEND_SpeakMessage(std::string state) {
	
	
	my_MSG msg;
	msg.type = "SPEAK";
	msg.message = state;
	
	m_ConnectionManager->mySend(msg);	
}


bool Autopilot::execute_turn() {	
	
	delay(100);
	maintain();
	m_Driver.SetAngle(map_turn_angle);
	
	if (turn_dist <= -TURN_TRAVEL_DISTANCE) {
		return false;
	}
	
	delay(200);
	
	return true;	
}


bool Autopilot::execute_stop() {
	
	m_Driver.ImmediateStop();
	
	return false;
}

void Autopilot::maintain() {
	
	if (QUESTION_adjustAngle()) {
	//	long current_pt = m_Timer.timePoint();
		
		turn_angle = m_LaneDetection.GetAngle(); //ADJUST_SERVO, ADD TIMER HERE
		m_Driver.SetAngle(turn_angle);
		
	//	long other_pt = m_Timer.timePoint();
	//	long diff_pt = other_pt - current_pt;
	//	std::cout << "Servo (1) loop took : " << diff_pt << " (ms)\n";
		
	}
	
	//ADJUST_MOTOR, ADD TIMER HERE
	if (QUESTION_goForward() && QUESTION_speedUp()) 
	{	
		long time_pt = m_Timer.timePoint(); //ms
		
		if ((time_pt - time_of_last_speed_update) >= SPEED_CONTROL_TIME) {
		
			//std::cout << "ACCELERATING, CONSTANT: " << CONSTANT_FORWARD_SPEED << ", Curr Speed: " << speed << "cm/s\n";

			time_of_last_speed_update = time_pt;
			
			if (fsm.state() == States::TURNING) {
				m_Driver.Accelerate(true);				
			} else {
				m_Driver.Accelerate();
			}
			
		}

//long diffPT = time_of_last_speed_update-time_pt;
//std::cout << "MOTOR (1) loop took : " << diffPT << " (ms)\n";

	} 
	else if (QUESTION_slowDown()) 
	{
		long _time_pt = m_Timer.timePoint(); //ms
		
		if ((_time_pt - time_of_last_speed_update) >= SPEED_CONTROL_TIME) {
		
			//std::cout << "DECELERATING, CONSTANT: " << CONSTANT_FORWARD_SPEED << ", Curr Speed: " << speed << "cm/s\n";

			time_of_last_speed_update = _time_pt;
			m_Driver.Decelerate();
		}		
	}

}

void Autopilot::start_autopilot() {	

	m_Print("Starting autopilot");	

	m_SpeedEncoder.Reset();	
	FOLLOWING_PATH = true;
	STARTED = true;
	
	
	time_of_last_speed_update = m_Timer.timePoint();
	m_Driver.SetStraight();
	m_Driver.SetForward();
	m_Driver.Accelerate();
}


bool Autopilot::QUESTION_goForward() {
	
	if (CONNECTION_ACTIVE && FOLLOWING_PATH && STARTED) {
		return true;
	}
	
	return false;	
}

bool Autopilot::QUESTION_goBackward() {
	
	return false;
}

bool Autopilot::QUESTION_turnAhead() {
	
	if (TURN_INCOMING && turn_dist <= TURN_DIST_THRESHOLD) {
		return true;
	}
	
	return false;
}

bool Autopilot::QUESTION_obstacleFound() {
	
	if (OBSTACLE_DETECTED && obstacle_dist <= OBSTACLE_THRESHOLD) {
		return true;
	}
	
	return false;
}

bool Autopilot::QUESTION_adjustAngle() {
	
	if (fsm.state() != States::TURNING) {
		return true;
	}
	
	return false;
}

bool Autopilot::QUESTION_stopIncoming() {
	
	if (STOP_INCOMING && stop_dist <= STOP_DIST_THRESHOLD) {
		return true;
	}
	
	return false;
}

bool Autopilot::QUESTION_speedUp() {
	
	if (speed < CONSTANT_BACKWARD_SPEED && !QUESTION_slowDown()) {
		return true;
	}
	
	return false;
}

bool Autopilot::QUESTION_slowDown() {
	
	if (fsm.state() == States::TURNING) {
		if (QUESTION_turnAhead() /*&& speed > TURN_SPEED_THRESHOLD*/) {
			return false;
		}
	}
	
	if (fsm.state() == States::STOPPING) {
		return true;
	}
	
	
	if (speed > CONSTANT_FORWARD_SPEED) {
		return true;
	}
	
	return false;
}



void Autopilot::Start() {

	if (!CONNECTION_ACTIVE) {
		
		#ifdef USE_XBEE
			std::cout << "Try xbee...\n";
			
			xbee = new libxbee::XBee("xbee1", "/dev/ttyUSB0", 115200);
			try {
				
				struct xbee_conAddress addr;
				memset(&addr, 0, sizeof(addr));
				
				addr.addr16_enabled = 1;
				addr.addr16[0] = 0x00;
				addr.addr16[1] = 0x00;
				
				m_ConnectionManager = new ConnectionManager(*xbee, "16-bit Data", &addr);
				
			} catch (xbee_err err) {
				printf("xbee_setup(): %d - %s \n", err, xbee_errorToStr(err));
				return;
			}
		#else
			m_ConnectionManager = new ConnectionManager();
		#endif
		
		
		CONNECTION_ACTIVE = m_ConnectionManager->initializeConnection();
		
		if (CONNECTION_ACTIVE) {
			SEND_SpeakMessage("SMART CAR IS ALIVE");
			std::cout << "Opened communication with controller!\n";
			
		} else {
			std::cout << "Controller not found!\n";
		}
	}
	
	if (!STARTED && CONNECTION_ACTIVE && !FOLLOWING_PATH) {
		
		THREAD_laneDetection = std::thread([this] {this->m_LaneDetection.Loop(&CONNECTION_ACTIVE, &m_Driver.servo_angle);});
		THREAD_laneDetection.detach();
		
		THREAD_obstacleDetection = std::thread([this] {this->m_ObstacleDetection.Loop(&CONNECTION_ACTIVE);});
		THREAD_obstacleDetection.detach();
		
		THREAD_handleReceivedMessages = std::thread(&Autopilot::HANDLE_ReceivedMessages, this);
		THREAD_handleReceivedMessages.detach();
		
		THREAD_calculateSpeed = std::thread([this] {this->CalculateSpeed(&CONNECTION_ACTIVE);});
		THREAD_calculateSpeed.detach();
			
		THREAD_mainLoop = std::thread(&Autopilot::MAIN_LOOP, this);
		THREAD_mainLoop.join();
		
	}
}


void Autopilot::HANDLE_ReceivedMessages() {
	
	std::cout << "HANDLE_ReceivedMessages started...\n";
	
	do {		
		std::vector<my_MSG> received_msgs = m_ConnectionManager->myReceive();
	
		my_MSG * msg;
		
		for (size_t k = 0; k < received_msgs.size(); k++) {
			
			msg = &received_msgs[k];
			
			if (msg->type == "DISCONNECT") {

				Stop();
				
				CONNECTION_ACTIVE = false;
				
				#ifndef USE_XBEE
					m_ConnectionManager->closeConnection();				
					sleep(2);				
					CONNECTION_ACTIVE = m_ConnectionManager->initializeConnection();
				#endif
				
			} else if (msg->type == "TURN") {
				
				TURN_INCOMING = true;
				map_turn_angle = msg->num;
				turn_dist = msg->unique_id;
				//tim
				fsm.execute(Triggers::turn_ahead);
				
			} else if (msg->type == "STOP_SIGN") {
				
				STOP_INCOMING = true;
				stop_dist = msg->num;
				fsm.execute(Triggers::stop_ahead);
				
			} else if (msg->type == "START") {				
				
				fsm.execute(Triggers::start);
				
				#ifdef USE_XBEE
				CONNECTION_ACTIVE = true;
				#endif
				
			} else if (msg->type == "FINISH") {
				
				std::cout << "Arrived.\n";
				Stop();
				
			} else if (msg->type == "COMMAND") {
				
				//std::cout << "Command Time point: " << m_Timer.timePoint() << "ms\n";
				
				if (msg->message == "LEFT") {
				
					m_Driver.TurnLeft();
					
				} else if (msg->message == "RIGHT") 
				{
					m_Driver.TurnRight();
				} 
				else if (msg->message == "FASTER") 
				{
					if (m_Driver.MotorDuty < FWD_DUTY_AVG) {
						m_Driver.SetForward();
					}
					
					m_Driver.Accelerate(true);
				}
				 else if (msg->message == "SLOWER") 
				{
					m_Driver.Decelerate(true);
				} else if (msg->message == "STOP" ) {
					
					m_Driver.ImmediateStop();
				}
				
			} else {
				std::cout << "UNKNOWN MSG:: " << msg->type << "\n";
			}			
		}
		
	} while (true);	

	std::cout << "HANDLE_ReceivedMessages finished...\n";
}

void Autopilot::SEND_DispMessage(int disp) {
	
	my_MSG disp_msg;
	
	disp_msg.type = "DISP";
	disp_msg.num = disp; //cm * 1000
	
	m_ConnectionManager->mySend(disp_msg);
}

void Autopilot::Stop() {
	
	std::cout << "STOP\n";

	m_Driver.ImmediateStop();

	STARTED = false;
	FOLLOWING_PATH = false;
	
	std::cout << "Hall sensor distance travelled: " << m_SpeedEncoder.GetDistance() << "\n";
	m_SpeedEncoder.Reset();
	fsm.reset();
	
	STARTED = FOLLOWING_PATH = TURN_INCOMING = STOP_INCOMING = OBSTACLE_DETECTED = false;
	speed_count = last_total_disp = time_of_last_speed_update = last_total_distance = turn_angle = speed = turn_dist = map_turn_angle = stop_dist = obstacle_dist = 0;
}

const char* Autopilot::GetCurrentState() {

	return stringify_state(fsm.state());
}

const char* Autopilot::stringify_state(States state) {

	switch (state) {
	case States::REST:
		return "REST";
	case States::STARTING:
		return "STARTING";
	case States::CRUISING:
		return "CRUISING";
	case States::RECOVERING:
		return "RECOVERING";
	case States::TURNING:
		return "TURNING";
	case States::STOPPING:
		return "STOPPING";
	case States::STOPPED:
		return "STOPPED";
	case States::AVOIDING_COLLISION:
		return "AVOIDING_COLLISION";
	default:
		return "?????unknown state??????";
	}
}

/*Expected in CONFIG_FILENAME
 * 
 *  OBSTACLE_THRESHOLD:___
	CONSTANT_FORWARD_SPEED:___
	CONSTANT_BACKWARD_SPEED:___
	WHEEL_RADIUS:___
	PORT_NUM:___
*/
bool Autopilot::READ_CONFIG() {
	
	std::cout << "\nREADING CONFIG: " << CONFIG_FILENAME << "...\n";
	
	std::ifstream input_file(CONFIG_FILENAME);
	std::string line;
	
	std::vector<std::string> configs = {"OBSTACLE_THRESHOLD:", 
										"CONSTANT_FORWARD_SPEED:",
										"CONSTANT_BACKWARD_SPEED:",
										"WHEEL_RADIUS:",
										"PORT_NUM:",
										"MAX_DELAY:",
										"MOTOR_FREQ:",
										"MotorEnable:",
										"ServoMotorEnable:",
										"TRIG:",
										"ECHO:",
										"MAX_ATTEMPTS:",
										"MAX_RANGE:",
										"SIG:",
										"MAX_TURN_ANGLE:",
										"LOOP_INTERVAL_TIME:",
										"Kp:",
										"Ki:",
										"Kd:",
										"ADJUST_ANGLE_SENSITIVITY:",
										"TURN_DIST_THRESHOLD:",
										"STOP_DIST_THRESHOLD:",
										"FWD_DUTY_MAX:",
										"REVERSE_DUTY_MAX:",
										"TURN_DUTY_MAX:",
										"SPEED_CONTROL_TIME:",
										"TURN_TRAVEL_DISTANCE:",
										"OBSTACLE_CONSECUTIVE_COUNT:",
										"MUSIC_PIN:",
										"MUSIC_FREQ:"
										};

	try {
		if (input_file.is_open()) {
			
			while (getline(input_file, line)) {
				
				std::cout << line << std::endl;
				
				for (size_t i = 0; i < configs.size(); i++) {
					
					std::string config = configs[i];					
					size_t pos = line.find(config);
														
					if (pos != std::string::npos) {
						
						configs.erase(configs.begin() + i);
						
						if (config == "OBSTACLE_THRESHOLD:") {				
							
							OBSTACLE_THRESHOLD = std::stoi(line.substr(config.length(), -1));
							
						} else if (config == "CONSTANT_FORWARD_SPEED:") {
							CONSTANT_FORWARD_SPEED = std::stoi(line.substr(config.length(), -1));
							
						} else if (config == "CONSTANT_BACKWARD_SPEED:") {						
							CONSTANT_BACKWARD_SPEED = std::stoi(line.substr(config.length(), -1));
							
						} else if (config == "WHEEL_RADIUS:") {							
							WHEEL_RADIUS = std::stof(line.substr(config.length(), -1));
							WHEEL_CIRCUMFERENCE = 2 * 3.14 * WHEEL_RADIUS / 4.0;

							
						} else if (config == "PORT_NUM:") {
							PORT_NUM = std::stoi(line.substr(config.length(), -1));

						} 
			
						else if (config == "MAX_DELAY:") {
							MAX_DELAY = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "MOTOR_FREQ:") {
							MOTOR_FREQ = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "MotorEnable:") {
							MotorEnable = std::stoi(line.substr(config.length(), -1));
						}

						
						else if (config == "ServoMotorEnable:") {
							ServoMotorEnable = std::stoi(line.substr(config.length(), -1));
						}

						else if (config == "TRIG:") {
							TRIG = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "ECHO:") {
							ECHO = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "MAX_ATTEMPTS:") {
							MAX_ATTEMPS = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "MAX_RANGE:") {
							MAX_RANGE = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "SIG:") {
							SIG = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "MAX_TURN_ANGLE:") {
							MAX_TURN_ANGLE = std::stof(line.substr(config.length(), -1));
						}
						
						else if (config == "LOOP_INTERVAL_TIME:") {
							LOOP_INTERVAL_TIME = std::stof(line.substr(config.length(), -1));
						}
						
						else if (config == "Kp:") {
							Kp = std::stof(line.substr(config.length(), -1));
						}
						else if (config == "Ki:") {
							Ki = std::stof(line.substr(config.length(), -1));
						}
						
						else if (config == "Kd:") {
							Kd = std::stof(line.substr(config.length(), -1));
						}
						
						else if (config == "ADJUST_ANGLE_SENSITIVITY:") {
							ADJUST_ANGLE_SENSITIVITY = std::stof(line.substr(config.length(), -1));
						}
						
						else if (config == "TURN_DIST_THRESHOLD:") {
							TURN_DIST_THRESHOLD = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "STOP_DIST_THRESHOLD:") {
							STOP_DIST_THRESHOLD = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "FWD_DUTY_MAX:") {
							FWD_DUTY_MAX = std::stoi(line.substr(config.length(), -1));
							FWD_DUTY_MIN = FWD_DUTY_MAX * 0.90;
							FWD_DUTY_AVG = FWD_DUTY_MAX * 0.95;							
						}
						
						else if (config == "MUSIC_PIN:") {
							MUSIC_PIN = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "REVERSE_DUTY_MAX:") {
							REVERSE_DUTY_MAX = std::stoi(line.substr(config.length(), -1));
							//REVERSE_DUTY_MIN = REVERSE_DUTY_MAX + 2;
							//REVERSE_DUTY_AVG = REVERSE_DUTY_MAX + 1;
							//FWD_DUTY_AVG = REVERSE_DUTY_MAX + (FWD_DUTY_MAX - REVERSE_DUTY_MAX) / 2.0f;
						}						
											
						else if (config == "TURN_DUTY_MAX:") {
							TURN_DUTY_MAX = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "SPEED_CONTROL_TIME:") {
							SPEED_CONTROL_TIME = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "TURN_TRAVEL_DISTANCE:") {
							TURN_TRAVEL_DISTANCE = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "OBSTACLE_CONSECUTIVE_COUNT:") {
							OBSTACLE_CONSECUTIVE_COUNT = std::stoi(line.substr(config.length(), -1));
						}
						
						else if (config == "MUSIC_FREQ:") {
							MUSIC_FREQ = std::stoi(line.substr(config.length(), -1));
						}
						
						else {
							std::cout << "CONFIG: " << config << " from LINE: " << line << " NOT SET\n";
						}							
						
						break;
					} 
					
				}
			}
			
			input_file.close();
			
			if (configs.size() == 0) {
				std::cout << "\nConfig read successful!\n";
				return true;
			}			
		}		
		
	} catch (...) {
		std::cout << "\nERROR READ_CONFIG\n";
		return false;
	}
	
	std::cout << "\nUNEXPECTED config values..\n";
	for (size_t i = 0; i < configs.size(); i ++) {
		std::cout << configs[i];
	}
	
	return false;
}


void Autopilot::wave(int delay_us, int seconds) {
	
	gpioPulse_t pulse[2];
	
	pulse[0].gpioOn = (1<<MUSIC_PIN);
	pulse[0].gpioOff = 0;
	pulse[0].usDelay = 0.5* delay_us;
	
	pulse[1].gpioOn = 0;
	pulse[1].gpioOff = (1<<MUSIC_PIN);
	pulse[1].usDelay = 0.5 * delay_us;
	
	gpioWaveAddNew();
	gpioWaveAddGeneric(2, pulse);
	
	int id = gpioWaveCreate();
		
	if (id >= 0) {
			
		gpioWaveTxSend(id, PI_WAVE_MODE_REPEAT);
		
		gpioSleep(PI_TIME_RELATIVE, seconds, 0);
		
		gpioWaveTxStop();
	}	
}
