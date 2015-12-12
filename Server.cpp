/*
	SE3313 - Operating Systems
	Design Project: Multi-User Multi-Transaction Server
	Members: 	Brandon Baksh 250735326
				Tareq Tayeh 250725776
				Ahmad Chalabi 250727169
				Rami Shehto 250755398
	------------------------------------------------------------
	Part B: A Rock,Paper,Scissors Socket Server
*/

#include <iostream>
#include <mutex>
#include <vector>
#include <string>
#include "socket.h"
#include "Blockable.h"
#include "socketserver.h"
#include "thread.h"

void terminationThread(bool *terminate, Communication::SocketServer *server, std::vector<Communication::Socket *> *socket,const int id){
	std::string choice;
	std::cout<<"Server Online! Awaiting requests..."<<std::endl;
	while(!*terminate){
		std::cin>>choice;
		if(choice.compare("done")==0){
			*terminate = true;
		}
	}
	for(int i = 0; i<socket->size() ; i++){
		socket->at(i)->Close();
		std::cout<<"Now terminating server "<< i << "..."<<std::endl;
		delete socket->at(i);
	}
	server->Shutdown();
}

void communicationThread(bool *terminate, Communication::Socket *player1, Communication::Socket *player2, const int id){
	while(!*terminate){
		/* Variables to check socket status and handle game processing*/
		int write,read;
		Communication::ByteArray receivedMsg;
		std::string player1Response,player2Response	,player1Buffer,player2Buffer;
		int player1Choice,player2Choice;
		
		//I only want to execute futher than this when I know player 1 and player 2 are both waiting for the server to write to them...
		
		/* Handle player 1 */
		Communication::ByteArray sendToPlayer1("go");
		write = player1->Write(sendToPlayer1);
		std::cout<<"Thread: "<<id<<" Told player 1 to go"<<std::endl;

		/* Get a message from player 1 */
		read = player1->Read(receivedMsg);
		player1Response = receivedMsg.ToString();
		
		/* Handle player 2 */
		Communication::ByteArray sendToPlayer2("go");
		write = player2->Write(sendToPlayer2);
		std::cout<<"Thread: "<<id<<" Told player 2 to go"<<std::endl;
		
		/* Get a message from player 2 */
		read = player2->Read(receivedMsg);
		player2Response = receivedMsg.ToString();

		/* Process the input from player 1 and player 2 and determine a winner */
		player1Choice = player1Response[0] - '0';
		player2Choice = player2Response[0] - '0';
		std::cout<<"Thread: "<<id<<" Player 1: "<<player1Choice<<" Player 2: "<<player2Choice<<std::endl;
		switch(player1Choice){
			case 0:{
				if(player2Choice==0){
					player1Buffer=player2Buffer = "";
				}
				break;
			}
			case 1:{ //Player 1 chooses rock
				if(player2Choice==0){
					player1Buffer=player2Buffer = "";
				}else if(player2Choice==1){
					player1Buffer=player2Buffer = "tie";
				}else if(player2Choice==2){
					player1Buffer="lose";
					player2Buffer="win";
				}else if(player2Choice==3){
					player1Buffer="win";
					player2Buffer="lose";
				}
				break;
			}
			case 2:{//Player 1 chooses paper
				if(player2Choice==0){
					player1Buffer=player2Buffer = "";
				}else if(player2Choice==2){
					player1Buffer=player2Buffer = "tie";
				}else if(player2Choice==3){
					player1Buffer="lose";
					player2Buffer="win";
				}else if(player2Choice==1){
					player1Buffer="win";
					player2Buffer="lose";
				}
				break;
			}
			case 3:{//Player 1 chooses scissors
				if(player2Choice==0){
					player1Buffer=player2Buffer = "";
				}else if(player2Choice==3){
					player1Buffer=player2Buffer = "tie";
				}else if(player2Choice==1){
					player1Buffer="lose";
					player2Buffer="win";
				}else if(player2Choice==2){
					player1Buffer="win";
					player2Buffer="lose";
				}
				break;
			}
			default:{
				player1Buffer=player2Buffer="";
				break;
			}
		}
		
		/* Send messages back to player 1 and player 2*/
		Communication::ByteArray player1Outcome(player1Buffer);
		Communication::ByteArray player2Outcome(player2Buffer);
		write = player1->Write(player1Outcome);
		write = player2->Write(player2Outcome);
		std::cout<<"Thread: "<<id<<" Write 1: "<<player1Outcome.ToString()<<" Write 2: "<<player2Outcome.ToString()<<std::endl;
		if(player1Outcome.ToString().compare("")==0 || player2Outcome.ToString().compare("")==0){
			break;
		}
		// read = player1->Read(receivedMsg);
		// read = player2->Read(receivedMsg);
	}
	
	/* Terminate thread */
	player1->Close();
	player2->Close();
}

int main(){
	/* Create a Socket Server to accept connections */
	Communication::SocketServer *serverSocket;
	serverSocket = new Communication::SocketServer(1337);
	std::vector <std::thread *> threadVector;
	std::vector <Communication::Socket *>socketVector;
	bool terminate = false;
	int threadId = 0;
	
	/* Begin termination thread */
	std::thread* new_thread = new std::thread(terminationThread, &terminate, serverSocket, &socketVector,threadId);
	threadVector.push_back(new_thread);
	threadId++;
	
	while(!terminate){
		try{
			/* Create a regular Socket to communicate with client */
			Communication::Socket *player1 = new Communication::Socket(serverSocket->Accept());
			socketVector.push_back(player1);
			Communication::Socket *player2 = new Communication::Socket(serverSocket->Accept());
			socketVector.push_back(player2);
			
			/* Spawn a thread for the client that connected */
			std::thread* new_thread = new std::thread(communicationThread, &terminate, player1,player2,threadId);
			threadVector.push_back(new_thread);
			threadId++;
		}catch(int e){
			if(e == 2){
				break;
			}
			else{
				continue;
			}
		}		
	}	

	for(int i = 0 ; i < threadVector.size() ; i++){
		threadVector.at(i)->join();
		std::cout<<"Now terminating thread "<< i << "..."<<std::endl;
		delete threadVector.at(i);
	}
	std::cout<<"Server terminated! Good Bye!"<<std::endl;
	return 0;
}