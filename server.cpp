#include<bits/stdc++.h>
#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <sstream>

using namespace std;

//Actually allocate clients
vector<Client> Server::clients;
int serverSock, clientSock;
struct sockaddr_in Server::serverAddr;
struct sockaddr_in Server::clientAddr;
char buff[256];
string Server::ipaddr;
thread Server::IP_check;
thread Server::Port_check;
thread Server::censor_check;
vector<string> Ip;
vector<int> ports;
vector<string> censored;

void checkIP(string current_Ip, vector<string> Ip, bool* valid){
  for(auto ptr = Ip.begin(); ptr<Ip.end(); ptr++){
    if(*valid == false)
      return;
    if((*ptr).compare(current_Ip) == 0){
      *valid = false;
      break;
    }
  }
}

void checkPORT(int current_port, vector<int> ports, bool* valid){
  for(auto ptr = ports.begin(); ptr<ports.end(); ptr++){
    if(*valid == false)
      return;
    if(*ptr == current_port){
      *valid = false;
      break;
    }
  }
}

void checkCENSORED(string message, vector<string> censored, bool* valid){
  stringstream text(message);
  string word;
  while(text>>word){
    for(auto ptr = censored.begin(); ptr<censored.end(); ptr++){
      if(*valid == false)
        return;
      if((*ptr).compare(word) == 0){
        *valid = false;
        break;
      }
    }
  }
}

Server::Server() {

  //Initialize static mutex from MyThread
  MyThread::InitMutex();

  //For setsock opt (REUSEADDR)
  int yes = 1;

  //Init serverSock and start listen()'ing
  serverSock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serverAddr, 0, sizeof(sockaddr_in));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(PORT);

  //Avoid bind error if the socket was not close()'d last time;
  setsockopt(serverSock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));

  if(bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(sockaddr_in)) < 0)
    cerr << "Failed to bind";

  listen(serverSock, 5);
}

/*
	AcceptAndDispatch();

	Main loop:
		Blocks at accept(), until a new connection arrives.
		When it happens, create a new thread to handle the new client.
*/
void Server::AcceptAndDispatch(vector<string> IpList, vector<int> portsList, vector<string> censoredList) {
  
  Client *c;
  MyThread *t;

  socklen_t cliSize = sizeof(sockaddr_in);
  Ip = IpList;
  ports = portsList;
  censored = censoredList;

  while(1) {

          c = new Client();
	  t = new MyThread();

	  //Blocks here;
          c->sock = accept(serverSock, (struct sockaddr *) &clientAddr, &cliSize);

	  if(c->sock < 0) {
	    cerr << "Error on accept";
	  }
	  else {
	    t->Create((void *) Server::HandleClient, c);
	  }
  }
}

//Static
void *Server::HandleClient(void *args) {

  //Pointer to accept()'ed Client
  Client *c = (Client *) args;
  char buffer[256-25], message[256];
  int index;
  int n;

  //Add client in Static clients <vector> (Critical section!)
  MyThread::LockMutex((const char *) c->name);
  
    //Before adding the new client, calculate its id. (Now we have the lock)
    c->SetId(Server::clients.size());
    sprintf(buffer, "Client n.%d", c->id);
    c->SetName(buffer);
    cout << "Adding client with id: " << c->id << endl;
    Server::clients.push_back(*c);

  MyThread::UnlockMutex((const char *) c->name);

  while(1) {
    memset(buffer, 0, sizeof buffer);
    n = recv(c->sock, buffer, sizeof buffer, 0);

    //Client disconnected?
    if(n == 0) {
      cout << "Client " << c->name << " diconnected" << endl;
      close(c->sock);
      
      //Remove client in Static clients <vector> (Critical section!)
      MyThread::LockMutex((const char *) c->name);

        index = Server::FindClientIndex(c);
        cout << "Erasing user in position " << index << " whose name id is: " 
	  << Server::clients[index].id << endl;
        Server::clients.erase(Server::clients.begin() + index);

      MyThread::UnlockMutex((const char *) c->name);

      break;
    }
    else if(n < 0) {
      cerr << "Error while receiving message from client: " << c->name << endl;
    }
    else {
      //Message received. Send to all clients.
      bool valid = true;
      ipaddr = inet_ntoa(((struct sockaddr_in)clientAddr).sin_addr);
      snprintf(message, sizeof message, "<%s>: %s", c->name, buffer);
      Server::IP_check = thread(checkIP, ipaddr, Ip, &valid);
      Server::Port_check = thread(checkPORT, clientAddr.sin_port, ports, &valid);
      Server::censor_check = thread(checkCENSORED, message, censored, &valid);

      IP_check.join();
      Port_check.join();
      censor_check.join();
      if(valid==true){
        cout << "Will send to all: " << message << endl;
        Server::SendToAll(message,c->id);
      }    
    }
  }

  //End thread
  return NULL;
}

void Server::SendToAll(char *message,int id) {
  int n;

  //Acquire the lock
  MyThread::LockMutex("'SendToAll()'");
 
    for(size_t i=0; i<clients.size(); i++) { 
      if(Server::clients[i].id != id)
      {
       n = send(Server::clients[i].sock, message, strlen(message), 0);
       cout << n << " bytes sent." << endl;
      }
    }
   
  //Release the lock  
  MyThread::UnlockMutex("'SendToAll()'");
}

void Server::ListClients() {
  for(size_t i=0; i<clients.size(); i++) {
    cout << clients.at(i).name << endl;
  }
}

/*
  Should be called when vector<Client> clients is locked!
*/
int Server::FindClientIndex(Client *c) {
  for(size_t i=0; i<clients.size(); i++) {
    if((Server::clients[i].id) == c->id) return (int) i;
  }
  cerr << "Client id not found." << endl;
  return -1;
}
