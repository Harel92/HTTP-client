README
Name: Harel Madmoni
Id: 203382080

Exercise 2 - HTTP client

The program get HTTP request from the user and sends the request to the web server.
receives the reply from the server and display it on screen.


===Description ===

Program files:

client.c

// compile: gcc -Wall client.c -o client
// run: ./client "[-p<text>] [-r n <pr1=value1 pr2=value2 ...>] <URL>\n"

-----------------------------
Commands:
HTTP://HOST:[PORT]/Filepath. default PORT is 80 if not given.

    HOST= server name.
    PORT= default PORT is 80 if not given.
    Filepath= path inside the host.

-p BODY. 

    -p= send POST method instead of default GET method.
     BODY= any word.  

-r n pr1=value1 pr2=value2....

    -r= add parameters to send.
     n= number of parameters.
     All parameters format is 'name'='value', and seperated by one space.
    

	
		
------------------



