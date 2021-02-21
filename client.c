
#include <stdio.h>
#include <stdlib.h>
#include <string.h>        
#include <unistd.h>        	  
#include <sys/types.h>     	          
#include <netinet/in.h>    	          
#include <sys/socket.h>   	         
#include <netdb.h>   
      	          
#define  BUFLEN 100
#define MAX_PORT 6
#define MAX_PORT_NUM 65535
#define HTTP_SIZE 7
#define COMMAND 256
#define BODY_LENGTH 10

void error(char* str){
    perror(str);
    exit(EXIT_FAILURE) ;
}
//checks if a string include only digits
int isDigit(char* str){

    for(int i=0; i<strlen(str) ;i++){
        if(str[i]<='9' && str[i]>='0')
            continue;
    return -1;
    }
    return 0;
}


int main(int argc, char *argv[])
{
    int rc;                 // connect 
    int sockfd;             // socket
    int freehost=0;         // if host has been initialized
    int isPost=0;           // if -p  has been initialized
    int isR=0;              // if -r  has been initialized
    int R_param=0;          // -r number of parameters
    char* command;          // command to send to server
    char** Rwords;          // array for -r words
    char rbuf[BUFLEN];      // buffer 

    char* port=(char*)calloc(MAX_PORT,sizeof(char));       //array to hold port(MAX SIZE=65535)
    if(port==NULL){
        fprintf(stderr, "Cannot allocate memory \n");
        exit(EXIT_FAILURE) ;
    }
    char* body;                                            
    char* temp_host;                                        
    char* host=NULL;                      
    char* temp_port;
    char* path;
    int size=argc-1;

    struct sockaddr_in serv_addr;
    struct hostent *server;


    if (argc < 2) {
        fprintf(stderr, "Missing server name \n");
        free(port);
        exit(EXIT_FAILURE) ; 
    }

    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){ 
        free(port);
        error("socket failed");
    }

    for(int i=1; i<argc; i++){

        // -p handler
        if(strcmp(argv[i],"-p")==0 && isPost==0){
            if(i+1<argc){
                body=argv[i+1];
                size=size-2;
                isPost=1;
                i++;     
            }
        }

        // -r handler
        else if(strcmp(argv[i],"-r")==0 && isR==0){
            if(i+1<argc){
                if(isDigit(argv[i+1])==-1)
                    break;
                R_param=atoi(argv[i+1]);
                if(R_param<0)
                    break;    

                i+=2;
                Rwords=(char**)calloc(R_param,sizeof(char*));
                if(Rwords==NULL){
                    fprintf(stderr, "Cannot allocate memory \n");
                    free(port);
                    if(freehost==1)
                         free(host); 
                    exit(EXIT_FAILURE) ;
                }
                isR=1;
                int temp=R_param;
                int index=0;
                int j=i;
                while(temp>0){
                    if(argv[j]==NULL)
                        break;
                    else if(strchr(argv[j],'=')==NULL)
                        break;
                    else if(strcmp(strchr(argv[j],'='),"=")==0 || strcmp(strchr(argv[j],'='),argv[j])==0)
                        break;    
                    Rwords[index]=argv[j];
                    index++;
                    temp--;
                    j++;
                }
                if(temp==0)
                    size=size-2-R_param; 

                i+=R_param-1;           
            }         
        }
        // host+port handler
        else if(strstr(argv[i],"http://")!=NULL && freehost==0){
            temp_host=strstr(argv[i],"http://");
            temp_host+=HTTP_SIZE;
            size--;

            if(strstr(temp_host,":")==NULL){
                port[0]='8';
                port[1]='0';
                port[2]='\0';
            }
            else
            { 
                temp_port=strstr(temp_host,":");
                temp_port++;
                if(temp_port[0]=='/')
                    size=-1;
                int h;
                for(h=0; h< MAX_PORT-1; h++){
                    if(temp_port[h]=='\0' || temp_port[h]=='/')
                        break;    
                    port[h]=temp_port[h];   
                }
                port[MAX_PORT-1]='\0';
           
                temp_port+=h;
                if(temp_port[0]!='\0' && temp_port[0]!='/')
                    size=-1;
                       
            }
            
            host=(char*)calloc(strlen(temp_host)+1,sizeof(char));
            if(host==NULL){
                    fprintf(stderr, "Cannot allocate memory \n");
                    free(port);
                    if(isR==1)
                        free(Rwords);
                    exit(EXIT_FAILURE) ;
                }
            freehost=1;
            path=strstr(temp_host,"/");
            for(int i=0; i<strlen(temp_host); i++){
                if(temp_host[i]=='\0' || temp_host[i]=='/' || temp_host[i]==':')
                    break;
                host[i]=temp_host[i];
                
            }    
        }
        // undefined word
        else{
            fprintf(stderr, "Usage: client [-p<text>] [-r n <pr1=value1 pr2=value2 ...>] <URL>\n");
            if(freehost==1)
                free(host); 
            if(isR==1)
                free(Rwords);       
            free(port);
            exit(EXIT_FAILURE) ;
        }   

    }

    // size=0 means all words are valid and there is no unnecessary words

    if(size!=0 || isDigit(port)==-1 || atoi(port)>MAX_PORT_NUM || host==NULL){
        fprintf(stderr, "Usage: client [-p<text>] [-r n <pr1=value1 pr2=value2 ...>] <URL>\n");
        if(freehost==1)
            free(host); 
        if(isR==1)
            free(Rwords);       
        free(port);
        exit(EXIT_FAILURE) ;
    }


    // connect to server
    server = gethostbyname(host);
    if (server == NULL) {
        herror("ERROR, no such host\n");
        if(freehost==1)
            free(host);
        if(isR==1)
            free(Rwords);    
        free(port);  
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(atoi(port));

    rc = connect(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (rc < 0){ 
        free(host);
        free(port);
        if(isR==1)
            free(Rwords);
        error("connect failed:");
    }

    // command handler
    int path_size;
    if(path==NULL)
        path_size=0;
    else
        path_size=strlen(path);
    
    int command_size=strlen(host)+path_size+COMMAND;
    command=(char*)calloc(command_size,sizeof(char));
    if(command==NULL){
        fprintf(stderr, "Cannot allocate memory \n");
        free(port);
        free(host);
        if(isR==1)
            free(Rwords);
        exit(EXIT_FAILURE) ;
    }


    if(isPost==1)
        strcat(command,"POST ");
    else
        strcat(command,"GET ");


    if(path==NULL)
        strcat(command,"/");
    else
    strcat(command,path);

    if(isR==1 && R_param!=0){
        strcat(command,"?");
        for(int i=0;i<R_param;i++){
            strcat(command,Rwords[i]);
            if(i+1!=R_param)
                strcat(command,"&");
        }
    }    

    strcat(command," HTTP/1.0\r\nHost: ");
    strcat(command,host);
    if(isPost==1){
         strcat(command,"\r\nContent-length:");
         char body_length[BODY_LENGTH];
         sprintf(body_length,"%ld",strlen(body));
         strcat(command,body_length);
    }
    strcat(command,"\r\n\r\n");
    if(isPost==1){
        strcat(command,body);
    }

    printf("HTTP request =\n%s\nLEN = %ld\n",command,strlen(command));


	// send and then receive messages from the server
	write(sockfd, command, strlen(command)+1) ;

    int bytes_read=0;

    do{
	    rc = read(sockfd, rbuf,  BUFLEN) ;
        bytes_read+=rc;
        rbuf[rc]='\0';
        if(rc > 0)
	    	  printf("%s", rbuf) ;
	    else if(rc<0)
              error("read() failed") ;
    }while(rc>0);

    printf("\nTotal received response bytes: %d\n",bytes_read);

    close(sockfd);
    free(host);
    free(port);
    free(command);
    if(isR==1)
        free(Rwords);

    return EXIT_SUCCESS;
}
