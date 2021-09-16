/*	
* Voice Buffer Plugin Test
*/

#include <Windows.h>


#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"

#include "plugin.h"
#include "ts3_functions.h"

#include "base.h"
#include <string>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <map>
#include <thread>
#include <chrono>
#include <time.h>

using namespace std;
static std::unique_ptr<PluginBase> plugin;
static struct TS3Functions ts3Functions;
/*
Initialise + required functions
*/
const char* ts3plugin_name()  {
    return plugin->name;
}

const char* ts3plugin_version() {
    return plugin->version;
}

int ts3plugin_apiVersion() {
    return 25;
}

const char* ts3plugin_author() {
    return plugin->author;
}

const char* ts3plugin_description() {
    return plugin->description;
}


std::map<anyID, uint64> clientVoice;
std::map<anyID, int> clientWhispering;
std::map<anyID, int> clientTalking;
//std::map<anyID, int> clientPrevInc;
std::map<anyID, int> clientPrevTalking;
std::map<anyID, int> talkStatusChanged;
//int updateSentPackets = 1;





void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
    ts3Functions = funcs;
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID,
                                        int status,
                                        int isReceivedWhisper,
                                        anyID clientID) {

    //anyID myID;
    //ts3Functions.getClientID(serverConnectionHandlerID, &myID);
    //if (clientID == myID) {
    //    ts3Functions.freeMemory(&myID);
    //    return;
    //}
    //ts3Functions.freeMemory(&myID);
    //get if client talking or no
    clientTalking[clientID] = status;
    talkStatusChanged[clientID] = 1;
}

void ts3plugin_onUpdateClientEvent(uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {


}


void ts3plugin_onConnectionInfoEvent(uint64 serverConnectionHandlerID, anyID clientID) {
    
    //try in thread

}





//void ts3plugin_onConnectionInfoEvent(uint64 serverConnectionHandlerID, anyID clientID) {
//    uint64 speechSent;
//    ts3Functions.getConnectionVariableAsUInt64(serverConnectionHandlerID, clientID, CONNECTION_BYTES_SENT_SPEECH, &speechSent);
//
//    //if talking, set current sent
//    if (clientTalking[clientID] != 0) {
//        clientVoice[clientID] = speechSent;
//        clientPrevInc[clientID] = 0;
//    }
//    else {
//        if (speechSent > clientVoice[clientID]) {
//            ts3Functions.printMessageToCurrentTab("inc out.");
//            if (clientPrevInc[clientID] == 1) {
                //char* nick;
                //ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_NICKNAME, &nick);
                //char msg[255];
                //sprintf_s(msg, sizeof(msg), "%s - whisper?", nick);
                //ts3Functions.printMessageToCurrentTab(msg);
                //ts3Functions.freeMemory(&nick);
//            }
//            clientPrevInc[clientID] = 1;
//        }
//        else {
//            clientPrevInc[clientID] = 0;
//        }
//        clientVoice[clientID] = speechSent;
//    }
//
//    ts3Functions.freeMemory(&speechSent);
//
//
//}


void checkPackets(uint64 serverConnectionHandlerID, anyID clientID) {



    uint64 speechSent;
    ts3Functions.getConnectionVariableAsUInt64(serverConnectionHandlerID, clientID, CONNECTION_BYTES_SENT_SPEECH, &speechSent);
    //if most recently we were talking
    
    int muteStatus;
    ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerID, CLIENT_OUTPUT_MUTED, &muteStatus);

    //if talk status changed or we are deafened, then update and return then update as normal.
    if (talkStatusChanged[clientID] == 1 || muteStatus == MUTEOUTPUT_MUTED) {
        talkStatusChanged[clientID] = 0;
        clientVoice[clientID] = speechSent;
        ts3Functions.freeMemory(&muteStatus);
        ts3Functions.freeMemory(&speechSent);
        clientPrevTalking[clientID] = 1;
        return;
    }
    else {

        ts3Functions.freeMemory(&muteStatus);

        //if they are talking, update as normal.

        if (clientTalking[clientID] == 1) {
            talkStatusChanged[clientID] = 0;
            clientVoice[clientID] = speechSent;
            ts3Functions.freeMemory(&speechSent);
            clientPrevTalking[clientID] = 1;
            return;
        }
        else {
            //if talk status has not changed and they are not talking, then we must check the speech packets.
            if (speechSent > clientVoice[clientID]) {

                //if (clientPrevTalking[clientID] == 1) {
                //    clientVoice[clientID] = speechSent;
                //    ts3Functions.freeMemory(&speechSent);
                //    clientPrevTalking[clientID] = 0;
                //    return;
                //}
                if (clientTalking[clientID] != 2) {
                    clientVoice[clientID] = speechSent;
                    ts3Functions.freeMemory(&speechSent);
                    clientPrevTalking[clientID] = 2;
                    return;
                }

                time_t current_time;
                struct tm time_info;
                char timeString[9];  // space for "HH:MM:SS\0"

                time(&current_time);
                localtime_s(&time_info, &current_time);

                strftime(timeString, sizeof(timeString), "%H:%M:%S", &time_info);

                char* nick;
                ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_NICKNAME, &nick);
                char msg[255];
                sprintf_s(msg, sizeof(msg), "[b][color=#000000]<%s> - %s is whispering.[/color][/b]", timeString, nick);
                ts3Functions.playWaveFile(serverConnectionHandlerID, "stop_talking.wav");
                
                //message current channel 
                //anyID myID;
                //uint64 myChannelID;
                //ts3Functions.getClientID(serverConnectionHandlerID, &myID);

                ///* Get own channel ID */
                //ts3Functions.getChannelOfClient(serverConnectionHandlerID, myID, &myChannelID);

                //ts3Functions.requestSendChannelTextMsg(serverConnectionHandlerID, msg, myChannelID, NULL);
                //ts3Functions.freeMemory(&myID);
                //ts3Functions.freeMemory(&myChannelID);


                ts3Functions.printMessageToCurrentTab(msg);
                

                ts3Functions.freeMemory(&nick);
                
            }
        }

    }



    talkStatusChanged[clientID] = 0;
    clientVoice[clientID] = speechSent;
    ts3Functions.freeMemory(&speechSent);


}

int threadEnabled = 1;
void idleCheck() {
    while (threadEnabled) {
        //check if connected to a server.
        uint64 serverConnectionHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
        int connectionStatus;
        ts3Functions.getConnectionStatus(serverConnectionHandlerID, &connectionStatus);
        if (connectionStatus != STATUS_CONNECTION_ESTABLISHED) {
            ts3Functions.freeMemory(&connectionStatus);
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }
        ts3Functions.freeMemory(&connectionStatus);
        //if sent voice packets increase
        //and they aren't talking
        //then they are whispering
        //could check for recieved packets to see the target(s)

        //get channel clients
        anyID myID;
        uint64 myChannelID;
        ts3Functions.getClientID(serverConnectionHandlerID, &myID);
        
        /* Get own channel ID */
        ts3Functions.getChannelOfClient(serverConnectionHandlerID, myID, &myChannelID);
        
        anyID* channelClients;
        ts3Functions.getChannelClientList(serverConnectionHandlerID, myChannelID, &channelClients);
        


        //loop through all clients
        for (int i = 0; channelClients[i]; ++i) {
            //skip if it is own client
            if (channelClients[i] == myID) continue;

            
            //check if the clientID is not found in our maps.
            if (clientVoice.find(channelClients[i]) == clientVoice.end()) {
                //client is not found in our maps, we must add them.
                clientVoice.insert(pair<anyID, uint64>(channelClients[i], 0));
                clientWhispering.insert(pair<anyID, int>(channelClients[i], 0));
                clientTalking.insert(pair<anyID, int>(channelClients[i], 0));
                clientPrevTalking.insert(pair<anyID, int>(channelClients[i], 0));
                talkStatusChanged.insert(pair<anyID, int>(channelClients[i], 1));
                ts3Functions.printMessageToCurrentTab("adding");

            }
            else {
                //client is found in our maps, continue.
            }

            /*Get the client's variables and connection info.*/

            ts3Functions.requestClientVariables(serverConnectionHandlerID, channelClients[i], NULL);
            if (ts3Functions.requestConnectionInfo(serverConnectionHandlerID, channelClients[i], NULL) == ERROR_ok) {
                checkPackets(serverConnectionHandlerID, channelClients[i]);
            }
            else {
                ts3Functions.printMessageToCurrentTab("failed the connection request");
            }
        }


        /*Free Memory after use.*/
        ts3Functions.freeMemory(&myID);
        ts3Functions.freeMemory(&myChannelID);
        ts3Functions.freeMemory(&channelClients);
        this_thread::sleep_for(chrono::milliseconds(250));
    }
    return;
}

thread* ct;
int ts3plugin_init() {
    //init stuff
    thread checkThread(idleCheck);
    checkThread.detach();
    return 0;
}
void ts3plugin_shutdown() {
    threadEnabled = 0;
    plugin.release();
}