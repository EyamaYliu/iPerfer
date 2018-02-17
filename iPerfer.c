// iPerfer Program that designed to test connection bandwidth
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
//#define PORT 8080

int main(int argc, char const *argv[])
{


    if (argc != 4 && argc != 8 )
    {
	printf("Error:Missing or additional arguments\n");

	exit(1);
    }

    int serverportNum = atoi(argv[3]);   
    int clientportNum = atoi(argv[5]); 
    int dataTiming = atoi(argv[7]);  
    //Server mode
    if (argc == 4 && strcmp(argv[1],"-s") == 0 && strcmp(argv[2],"-p") == 0 && serverportNum != 0)
    {
		
	if(serverportNum >= 65535 || serverportNum <= 1024)
	{
		printf("Error: port number must be in the range 1024 to 65535\n");
		exit(1);
	}

	    int server_fd, new_socket, valread,datasize;
	    struct sockaddr_in address;
	    int opt = 1;
	    int addrlen = sizeof(address);
	    char buffer[1000] = {0};
	    char FinishTest[10];
	    char *acknowledgement = "RECEIVED";
	    time_t start,end;
	    double elapsed;
      

	    // Creating socket file descriptor
	    server_fd = socket(AF_INET, SOCK_STREAM, 0);
	    if (server_fd == 0)
	    {
		perror("socket failed");
		exit(EXIT_FAILURE);
	    }
	      

	    address.sin_family = AF_INET;
	    address.sin_addr.s_addr = INADDR_ANY;
	    address.sin_port = htons( serverportNum );
	      
	    // Forcefully attaching socket to the port 8080
	    if (bind(server_fd, (struct sockaddr *)&address, 
		                         sizeof(address))<0)
	    {
		perror("bind failed");
		exit(EXIT_FAILURE);
	    }
	    if (listen(server_fd, 3) < 0)
	    {
		perror("listen");
		exit(EXIT_FAILURE);
	    }
	    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
		               (socklen_t*)&addrlen))<0)
	    {
		perror("accept");
		exit(EXIT_FAILURE);
	    }
	    //Read whats coming
	
	    //Time when receiving start   
	    start = time(NULL);
	     while(buffer[valread-1] == 0)
	    {	
		//Keep reading for FIN sign
		valread = read( new_socket , buffer, 1000);
	        datasize += valread;
		//printf("%d kB received.\n",valread);	
	    }
	     //Send acknowledgement
	    send(new_socket , acknowledgement , strlen(acknowledgement) , 0 );

	    //Stop time clock
	    end = time(NULL);
	    elapsed = difftime(end,start);
	
	    float sizeofdata = (datasize-1)*8/1000;
	    float rate = sizeofdata/(elapsed*1000);
	    printf("\nServer Summary: \n     This transmission went for %g seconds.\n     There are total of %g KB sent.\n     Transmission rate is %g Mbps.\n ",elapsed,sizeofdata,rate);
 
  	    return 0;
	    

    }//Client mode condition
	else if(argc == 8 && strcmp(argv[1],"-c") == 0 && strcmp(argv[2],"-h") == 0 &&  strcmp(argv[4],"-p") == 0 && strcmp(argv[6],"-t") == 0 && clientportNum != 0)
	{
	    
	    	
	    if(clientportNum >= 65535 || clientportNum <= 1024)
	    {
		printf("Error: port number must be in the range 1024 to 65535\n");		
  	    	exit(1);
	    }

	    char server_hostname[20];
	    strcpy(server_hostname,argv[3]);

	 	
	    struct sockaddr_in address;
	    int sock = 0, valread,valsent;
	    struct sockaddr_in serv_addr;
	    char *SendFinIndicator = "FIN";
	    char buffer[1000] = {0};
	    int ZeroArrayData[1000] = {0};
	    int sizeofData = 1000;
	    int terminate = 1;
	    time_t start,end,s2,e2;
	    double elapsed,elapsed2;

	    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    {
		printf("\n Socket creation error \n");
		return -1;
	    }
	  
	    memset(&serv_addr, '0', sizeof(serv_addr));
	  
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(clientportNum);
	      
	    // Convert IPv4 and IPv6 addresses from text to binary form
	    if(inet_pton(AF_INET, server_hostname, &serv_addr.sin_addr)<=0) 
	    {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	    }
	  
	    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	    {
		printf("\nConnection Failed \n");
		return -1;
	    }

	    //Sending Test data for certain seconds
	    start = time(NULL);
	    int count = 0;
	    while(terminate)
	    {
	        end = time(NULL);
		elapsed = difftime(end,start);
	  	if (elapsed >= dataTiming) //seconds 
			terminate = 0;
		else
		{
		    	valsent = send(sock , ZeroArrayData , sizeofData , 0 );
			count += valsent;
		}
            }
	    s2 = time(NULL);
	    float sizeofdata = count*8/1000;
	    //Sending Finish indicator
	    send(sock , SendFinIndicator , strlen(SendFinIndicator) , 0 );

	    //Reading Acknowledgement
	    valread = read(sock, buffer,1000);
	    e2 = time(NULL);
	    elapsed2 = difftime(e2,s2);
	    float rate = sizeofdata/((elapsed+elapsed2)*1000);//in Megabyte per secound

	    printf("\nClient Summary: \n     This transmission went for %g seconds.\n     There are total of %g KB sent.\n     Transmission rate is %g Mbps.\n ",elapsed+elapsed2,sizeofdata,rate);
	    return 0;
   	}//Wrong argument, exit
    else
    {
	printf("Some arguments are going wrong.\n");
	exit(1);
    }


	

}
