# FIREWALL_Concurrent
This is the implementation of firewall in C++ using concurrency implemented on chat server with socket libraries and thread libraries

# WORKING OF THE MODEL:
# STEP 1:
Download all the files in one repository in the system and navigate to the repository in the terminal.

# STEP 2:
Now run this code in the directory in the terminal:
```
g++ -std=c++11 -o server main.cpp server.cpp client.cpp mythread.cpp server.h mythread.h client.h -lpthread
```
# STEP 3:
Execute this code using
```
./server
```
Now the code runs with this display saying server activated:
```
Running!
Mutex initialized.

```
# STEP 4:
Now open a new terminal using shift+ctrl+N, and then use telnet to connect to the server on the port 30666 as specified
in the server code,
```
telnet 127.0.0.1 30666
```
after connecting it shows,
```
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
```
Now a single client is connected to the server,

We can type the messages in the client side terminal and that will be displayed on the server side with <client.0>"message".

For now the firewall can block an IP , PORT , OR CENSORED MESSAGES.

# CENSORED MESSAGES
In the main.cpp we can see that censored messgae given is "hello"
Whenever a client types this message hello it will not be displayed on the server side terminal interface.

similar can be done for IP and PORT number restrictions when using differenct systems on the same LAN and just changing the 
IP and PORT number while using the telnet.


 

