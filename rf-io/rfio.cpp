#include "RF24.h"
#include "linuxhal.h"
#include <string>
#include <iostream>
#include <inttypes.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <vector>
using namespace std;

#define COMMAND_SET_ADDRESS 0
#define COMMAND_CONFIGURE_GPIO 1
#define COMMAND_WRITE_GPIO 2
#define COMMAND_READ_DIGITAL 3
#define COMMAND_READ_ANALOG 4
#define COMMAND_WRITE_POWER 5
#define COMMAND_PING 6

int main(int argc, const char * argv[]){
    if(argc != 2){
        cout << "usage: "<<argv[0]<<" <ADDRESS>"<< endl;
        return 1;
    }
    const char * addr = argv[1];

    cout << "Connecting to RF-IO on Address "<<addr<<endl;

    LinuxHal hal("/dev/spidev0.0",32+31);
    RF24 comm(&hal);

    comm.setPALevel(RF24_PA_MAX);
    comm.setDataRate(RF24_2MBPS);
    comm.setChannel(90);
    comm.setRetries(5,15);
    comm.setPayloadSize(6);
    comm.setAddressWidth(5);

    comm.openWritingPipe((const unsigned char *)addr);
    comm.openReadingPipe(1,(const unsigned char *)"MASTR");

    srand(hal.ms());

    uint8_t txdata[6];
    uint8_t rxdata[6];

    bool pingOk = false;

    while(!pingOk){
        cout << "Attempting to ping IoBoard @ "<<addr<<"... ";

        comm.stopListening();
        txdata[0] = COMMAND_PING;
        for(int i = 0; i < 5; i++){
            txdata[i+1] = rand() & 0xff;
        }
        comm.write(txdata,6);
        comm.startListening();

        uint64_t started_waiting_at = hal.ms();
        bool timeout = false;
        while ( ! comm.available() && ! timeout ) {
            if (hal.ms() - started_waiting_at > 500 )
                timeout = true;
        }
        if(timeout){
            cout << "Failed"<<endl;
            hal.delay(1000);
            continue;
        }

        bool bad = false;

        comm.read(rxdata,6);
        for(int i = 0; i < 6; i++){
            if(rxdata[i] != txdata[i])bad = true;
        }

        if(bad){
            cout << "Failed: Invalid response"<<endl;
            hal.delay(1000);
            continue;
        }

        pingOk = true;
        cout << "Ok."<<endl;
    }

    comm.stopListening();

    cout << "Enter Command >";

    while(true){
        std::string input;
        vector<string> args;

        getline(cin,input);
        boost::algorithm::split(args,input, boost::is_any_of(" "));
        if(args.size() == 0)continue;
        string cmd = args[0];

        if(cmd == "setaddr"){
            if(args.size() != 2 || args[1].length() != 5){
                cout << "Invalid Arg"<<endl;
                continue;
            }
            const char * cstr = args[1].c_str();

            txdata[0] = COMMAND_SET_ADDRESS;
            for(int i = 0; i < 5; i++){
                txdata[i+1] = cstr[i];
            }
            bool ok = comm.write(txdata,6);
            if(!ok)cout << "TX Failed!"<<endl;
        }
        else if(cmd == "gpiodir"){
            if(args.size() != 3){
                cout << "Invalid Arg"<<endl;
                continue;
            }
            int port = std::stoi(args[1]);
            int dir = args[2]=="in" ? 0 : 1;

            txdata[0] = COMMAND_CONFIGURE_GPIO;
            txdata[1] = port;
            txdata[2] = dir;
            bool ok = comm.write(txdata,6);
            if(!ok)cout << "TX Failed!"<<endl;
        }
        else if(cmd == "gpiowrite"){
            if(args.size() != 3){
                cout << "Invalid Arg"<<endl;
                continue;
            }
            int port = std::stoi(args[1]);
            int val = std::stoi(args[2]);

            txdata[0] = COMMAND_WRITE_GPIO;
            txdata[1] = port;
            txdata[2] = val;

            bool ok = comm.write(txdata,6);
            if(!ok)cout << "TX Failed!"<<endl;

        }
        else if(cmd == "gpioread"){
            if(args.size() != 2){
                cout << "Invalid Arg"<<endl;
                continue;
            }

            int port = std::stoi(args[1]);

            txdata[0] = COMMAND_WRITE_GPIO;
            txdata[1] = port;

            bool ok = comm.write(txdata,6);
            if(!ok){
                cout << "TX Failed!"<<endl;
                continue;
            }

            comm.startListening();

            uint64_t started_waiting_at = hal.ms();
            bool timeout = false;
            while ( ! comm.available() && ! timeout ) {
                if (hal.ms() - started_waiting_at > 500 )
                    timeout = true;
            }
            if(timeout){
                cout << "Failed: Got no response!"<<endl;
                continue;
            }

            comm.read(rxdata,8);
            comm.stopListening();

            int state = rxdata[2];

            cout << "IoBoard returned: "<<state<<endl;
        }
        else if(cmd == "agpioread"){
            if(args.size() != 2){
                cout << "Invalid Arg"<<endl;
                continue;
            }

            int port = std::stoi(args[1]);

            txdata[0] = COMMAND_WRITE_GPIO;
            txdata[1] = port;

            bool ok = comm.write(txdata,6);
            if(!ok){
                cout << "TX Failed!"<<endl;
                continue;
            }

            comm.startListening();

            uint64_t started_waiting_at = hal.ms();
            bool timeout = false;
            while ( ! comm.available() && ! timeout ) {
                if (hal.ms() - started_waiting_at > 500 )
                    timeout = true;
            }
            if(timeout){
                cout << "Failed: Got no response!"<<endl;
                continue;
            }

            comm.read(rxdata,8);
            comm.stopListening();

            int state = rxdata[2];
            state |= rxdata[3]<<8;
            cout << "IoBoard returned: "<<state<<endl;

        }
        else if(cmd == "pwrwrite"){
            if(args.size() != 3){
                cout << "Invalid Arg"<<endl;
                continue;
            }

            int port = std::stoi(args[1]);
            int val = std::stoi(args[2]);

            txdata[0] = COMMAND_WRITE_GPIO;
            txdata[1] = port;
            txdata[2] = val;

            bool ok = comm.write(txdata,6);
            if(!ok)cout << "TX Failed!"<<endl;

        }else{
            cout << "Available Commands: "<<endl;
            cout << "setaddr <5Characters>" << endl;
            cout << "gpiodir <port> <in/out>" << endl;
            cout << "gpiowrite <port> <0/1>" << endl;
            cout << "agpioread <port>" << endl;
            cout << "pwrwrite <port> <0-255>" << endl;
        }
    }



    return 1;
}
