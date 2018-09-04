#include "RF24.h"
#include "linuxhal.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdio.h>

using namespace std;

const uint8_t pipes[][6] = {"MASTR","BRD01"};
bool radioNumber = 0;

int main(){
	LinuxHal hal("/dev/spidev0.0",32+31);
	RF24 radio(&hal);
    
    
    
	bool role_ping_out = true, role_pong_back = false;
  bool role = role_pong_back;

  cout << "RF24/examples/GettingStarted/\n";

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();


/********* Role chooser ***********/

  printf("\n ************ Role Setup ***********\n");
  string input = "";
  char myChar = {0};
  cout << "Choose a role: Enter 0 for pong_back, 1 for ping_out (CTRL+C to exit) \n>";
  getline(cin,input);

  if(input.length() == 1) {
	myChar = input[0];
	if(myChar == '0'){
		cout << "Role: Pong Back, awaiting transmission " << endl << endl;
	}else{  cout << "Role: Ping Out, starting transmission " << endl << endl;
		role = role_ping_out;
	}
  }
/***********************************/
  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.

    radio.setPALevel(RF24_PA_MAX);
	radio.setDataRate(RF24_2MBPS);
	radio.setChannel(90);
	radio.setRetries(5,15);
	radio.setPayloadSize(6);
	radio.setAddressWidth(5);
    

    
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
    
	
	radio.startListening();
	
	// forever loop
	while (1)
	{
		if (role == role_ping_out)
		{
			// First, stop listening so we can talk.
			radio.stopListening();

			// Take the time, and send it.  This will block until complete

			printf("Now sending...\n");
			uint64_t time = hal.ms();

			bool ok = radio.write( &time, 6 );

			if (!ok){
				printf("failed.\n");
			}
			// Now, continue listening
			radio.startListening();

			// Wait here until we get a response, or timeout (250ms)
			uint64_t started_waiting_at = hal.ms();
			bool timeout = false;
			while ( ! radio.available() && ! timeout ) {
				if (hal.ms() - started_waiting_at > 200 )
					timeout = true;
			}


			// Describe the results
			if ( timeout )
			{
				printf("Failed, response timed out.\n");
			}
			else
			{
				// Grab the response, compare, and send to debugging spew
				uint16_t data[3];
				radio.read( data, 6 );
                                
                                int v = data[0];
                              
				// Spew it
				printf("V=%d\n",v);
			}
			sleep(1);
		}

		//
		// Pong back role.  Receive each packet, dump it out, and send it back
		//

		if ( role == role_pong_back )
		{
			
			// if there is data ready
			if ( radio.available() )
			{
				// Dump the payloads until we've gotten everything
				unsigned long got_time;

				// Fetch the payload, and see if this was the last one.
				while(radio.available()){
					radio.read( &got_time, 8 );
				}
				radio.stopListening();
				
				radio.write( &got_time, 8);

				// Now, resume listening so we catch the next packets.
				radio.startListening();

				// Spew it
				printf("Got payload(%d) %lu...\n",sizeof(unsigned long), got_time);
				
				hal.delay(925); //Delay after payload responded to, minimize RPi CPU time
				
			}
		
		}

	} // forever loop

	
	return 0;
}
