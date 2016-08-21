#include <fstream>
#include <iostream>
#include "gvoice.h"
#define GOOGLE_VOICE_INFOS ".gvoice"

int send_text_msg( std::string message ){
	GoogleVoice gv;
	std::ifstream file( GOOGLE_VOICE_INFOS );
	std::string username, password;

	int status;
	

	if(gv.Init()){
		std::cout << "GoogleVoice() failed to initialize.\n";
		return -1;
	}

	getline( file, username );
	getline( file, password );
	file.close();

	status = gv.Login( username, password );

	if((gv.debug & 1) || status )
		std::cout << "GoogleVoice login returned "<< status << " \n";
	
	message = google_voice_build_msg(); 
	status = gv.SendSMS( "1234567", message );

	if((gv.debug & 2 ) || status )
		std::cout << "GoogleVoice.sendSMS() returned " << status << "\n";
	if(gv.debug & 1 && !status )
		std:cout << "SMS sent successfully\n";
	
	return status;
}
