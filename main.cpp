#include <iostream>
#include "mythread.h"
#include "server.h"
#include<vector>
#include<string>


 //!
 //!Actually allocate clients

using namespace std;

int main() {
  cout << "Running!" << endl;

  Server *s;
  s = new Server();
  vector<string> Ip;
  vector<int> ports; 
  vector<string> censored;
  censored.push_back("hello");

  //Main loop
  s->AcceptAndDispatch(Ip, ports, censored);

  return 0;
}
