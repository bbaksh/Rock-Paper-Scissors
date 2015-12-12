/*
	SE3313 - Operating Systems
	Design Project: Multi-User Multi-Transaction Server
	Members: 	Brandon Baksh 250735326
				Tareq Tayeh 250725776
				Ahmad Chalabi 250727169
				Rami Shehto 250755398
	------------------------------------------------------------
	Part 2: A Rock,Paper,Scissors Client
*/

#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include "socket.h"
#include "Blockable.h"
#include "socketserver.h"
#include "thread.h"
int inArray(std::string userInput){
	if(userInput.empty()){
		return -1;
	}
	std::string validChoices[4] = {"done","rock","paper","scissors"};
	boost::algorithm::to_lower(userInput);
	for(int i = 0 ; i < 4 ; ++i){
		if(userInput.compare(validChoices[i])==0){
			return i;
		}
	}
	return -1;
}
int main(){
	/* Create a socket to connect to server */
	Communication::Socket mySocket("127.0.0.1",1337);
	
	/* Declare some variables to check socket status */
	int open,write,read, choice = -1, win = 0, loss = 0, tie=0;
	std::string myMessage;
	bool terminate = true;
	
	/* Open the socket */
	open = mySocket.Open();
	Communication::ByteArray msgIn;
	while(terminate){
		/* Initial read to synchronize players */
		read = mySocket.Read(msgIn);
		if(msgIn.ToString().compare("go")!=0){
			std::cout<<"Initial read of :"<<msgIn.ToString()<<std::endl;
			terminate = false;
			break;
		}
		
		/* Write to the socket */		
		while(choice==-1){
			// std::cin.ignore(INT_MAX);
			// std::cin.clear();
			std::cout<<"Please enter your choice ('rock','paper','scissors','done'): "<<std::endl;
			std::cin>>myMessage;
			choice = inArray(myMessage);
		}
		
		Communication::ByteArray msgOut(std::to_string(choice));
		write = mySocket.Write(msgOut);
		std::cout<<"Message Sent: "<<myMessage<<std::endl;
		if(myMessage.compare("done")==0){
			terminate = false;
			break;
		}
		myMessage="";
		choice=-1;
		
		/* Read the response from the server */

		read = mySocket.Read(msgIn);
		
		std::cout<<"Message Received: "<<msgIn.ToString()<<std::endl;
		if(msgIn.ToString().compare("")==0){
			terminate = false;
			break;
		}else if(msgIn.ToString().compare("win")==0){
			win++;
		}else if(msgIn.ToString().compare("tie")==0){
			tie++;
		}else{
			loss++;
		}
		std::cout<<"You have "<<win<<" games won and "<<loss<<" games lost and "<< tie<<" games tied.."<<std::endl;
		// Communication::ByteArray clear("");
		// write = mySocket.Write(clear);
	}
	/* Close the socket when we are done */
	mySocket.Close();
	
	std::cout<<"Bye World!"<<std::endl;
	return 0;
}


/* 
	Main:
		- Double accept
		- Two sockets sent to a thread (clients share a thread)
		
	ThreadFnc:
		- Game loop outside 
		- 2 inner loops for players
		- each inner loop follows write->read->write
		- some sort of "move proccessing" and "rules"
		- on client...read->write->read
	
	ControlFnc:
		- Handles ending all games and closing server

*/