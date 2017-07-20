/* Busra Arslan 
 * 131044021
 * fileShareServer.c
 * Final*/
 

////Librarys//////
#include <stdio.h>
#include <sys/wait.h> 
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/socket.h>   //socket
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <netinet/in.h>

//Kullanilan diger c dosyalari
#include "restart.h"
#include "uici.h"
#include "uiciname.h"

#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 50 /*maximum number of client connections*/
#define MAXSIZE 100000
#define MAX_PATH_SIZE 1024
#define Size 1000   



struct ClientServer{
    pid_t client_pid;  
    int choose;    //menu secimi 
    int time;
};

//Global Variables
struct ClientServer CS;
pid_t server_pid;
pid_t arr_client_pid[LISTENQ];    //signal de tum clientlari kill yapmak icin 
char arr_client_path[LISTENQ][LISTENQ];   //for sendFile
int arr_client_size=0;            //arr_client_pid 'nin size'ini tuttum.
int arr_client_path_size=0;  
char filename[MAX_PATH_SIZE];    //sendFile
char temp_arr[MAXSIZE];          //for sendFile
struct timespec start, stop;
char client_path[PATH_MAX + 1];         //for sendFile
char temp_arr2[MAXSIZE];   //for sendFile
 
 
//function prototype
void signal_callback_handler(int signum);
void listServer();
void logFileWrite(void);


/////////////////////////////MAIN///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
    FILE *outp;   //for sendFile
    int port_number=0;
    int listenfd, connfd, n=0;
    pid_t childpid;   //fork icin kullanildi
    struct timespec begin,end;
    int l;   
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;
    
    //server pid
    server_pid = getpid();
    
       
    if (argc != 2) {                   /*arguman sayisi kontrolu*/
	    printf("Usage: dirtree <path>\n");
	    exit(1);
	}
	
	printf("__________________________________________________\n");
	printf("_____________  CSE 244 SİSTEM PROGRAMLAMA_________\n"); 
	printf("_____________	  FİNAL PROJESİ __________________\n");
	printf("_________________ Busra ARSLAN____________________\n");
	printf("____________________131044021_____________________\n");
    printf("__________________________________________________\n");
    
    //aldigi argumani port_number'a atadi.
    port_number = (u_port_t) atoi(argv[1]);        


    //Create a socket for the soclet
    //If sockfd<0 there was an error in the creation of the socket
    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) 
    {
        perror("Problem in creating the socket");
        exit(2);
    }


    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port_number);
   

    //bind the socket
    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
   

    //listen to the socket by creating a connection queue, then wait for clients
    listen (listenfd, LISTENQ);

    
    printf("%s\n","Server running...waiting for connections.");
   
    while(1)
    {
        
        //signal
        signal(SIGINT, signal_callback_handler);  
        //sleep(1);
        
        clilen = sizeof(cliaddr);
        //accept a connection
            
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

        printf("%s\n","Connection.Received request...");
            

        //client baglanma zamani
        if( clock_gettime( CLOCK_REALTIME, &begin) == -1 ) {					
            perror( "clock gettime" );
            return EXIT_FAILURE;
        }
        
        
   
        //////////////////////////////////////////////////////////////////
        //clienttan client_pid almak
        if((n = recv(connfd, &CS, sizeof (struct ClientServer ),0)) > 0);
          
        //client_id alindi
        arr_client_pid[arr_client_size] = CS.client_pid;

        ++arr_client_size;
        //printf("%dsize\n", arr_client_size);
        //////////////////////////////////////////////////////////////////
        ////CLOCKKKK//////////////TIMER////////////////
        if( clock_gettime( CLOCK_REALTIME, &end) == -1 ) {	
            perror( "clock gettime" );
            return EXIT_FAILURE;
        }
	
        //her clientin baglanma suresi 
        CS.time = (  - begin.tv_sec )*1000 + (  - begin.tv_nsec )/1000000000;
        printf("Clientin baglanma suresi>> %d  microsaniye\n", (int)CS.time);
        
        //////////////////////////////////////////////////////////////////
        //client_path'i client'tan almak
        if((n = recv(connfd, client_path, sizeof (client_path ),0)) > 0);
        
        strcpy(arr_client_path[arr_client_path_size] , client_path);
        
         ++arr_client_path_size;
        ///////////////////////////////////////////////////////////////////
        
        //FORK//
        if ( (childpid = fork ()) == 0 )  //child process
        {   
             printf ("%s\n","Child created");
             //close listening socket
             close (listenfd);
             
        
             while(1)
             {             
               
                if( recv(connfd, &CS, sizeof (struct ClientServer ),0) > 0);
       
                
                 //server menu
                 if(CS.choose == 2)
                 { 
         
                    printf("listServer\n");
                    listServer();
                    continue;
                 
                 }
                 else if(CS.choose == 3)         //lsClientsclientta islem goruyor
                 {  
                 
                    printf("lsClients\n");
                   
                    //printf("%dsize\n", arr_client_size);
                    if ((send(connfd, arr_client_pid, sizeof (  arr_client_pid ),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(connfd);
                        exit(1);
                    }
                    if ((send(connfd, &arr_client_size, sizeof ( arr_client_size),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(connfd);
                        exit(1);
                    }
                    continue;
                    
                 } 
                         
                 else if(CS.choose == 4)   //sendFile
                 {
                     int w, ii=0,a;
                     int secim, temp;
                     pid_t clientid;
                     char temp_client_path[MAXLINE];
                     printf("sendFile\n");
                     
                     if( recv(connfd, &secim, sizeof (secim ),0) > 0);
                     
                     if(secim == 1)  //<filename>  modu
                     {
                         if( recv(connfd, &temp, sizeof (temp ),0) > 0);
                         if(temp == 1)
                         {
                            
                              if( recv(connfd, &w, sizeof (w),0) > 0);
                  
                             
                              if( recv(connfd, temp_arr, sizeof (temp_arr),0) > 0);
                              
                                         
                              if( recv(connfd, filename, sizeof (filename ),0) > 0);
                              outp=fopen(filename,"w");
                             
                             
                               for(ii=0; ii< w; ++ii)
                                  fprintf(outp,"%c", temp_arr[ii]);
             
                                
                              fclose(outp);
                          }  
                       } 
                       if(secim == 2)  //<filename> <clientid> modu
                       {
                            FILE *outp2;
                            int yy;
                   
                            int jj,pp, temp_jj=0;
                            
                            if( recv(connfd, filename, sizeof (filename ),0) > 0);
                              outp=fopen(filename,"w");
                             
                             if( recv(connfd, &clientid, sizeof (clientid ),0) > 0);
                              outp=fopen(filename,"w");
                              
                        
                             for(jj=0; jj<arr_client_size; ++jj)
                             {
                                if(arr_client_pid[jj] == clientid)
                                    temp_jj = jj;
                             }
                             
                             strcpy(temp_client_path, arr_client_path[temp_jj]);
                            
                            
                             strncat(temp_client_path, "/", 1);
                             strncat(temp_client_path, filename, strlen(filename));
                             
                             outp2= fopen(temp_client_path,"w");  //pathi acar ve oraya yazar
                             
                              if( recv(connfd, &yy, sizeof (yy),0) > 0);
                  
                             
                              if( recv(connfd, temp_arr2, sizeof (temp_arr2),0) > 0);
                              //puts(temp_arr2);
                          
                
                             for(pp=0; pp< yy; ++pp)
                                fprintf(outp2,"%c", temp_arr2[pp]);
                             
                             fclose(outp2);
                      }
                      continue;
                 }
                 else{
                    continue;
                }
                
             }
            
             exit(0);
             
        }
    }
    
    //close socket of the server
    close(connfd);


    
    return 0;

}



//Server'in bulundugu localdeki dosyalari listeler
void listServer()
{
    char* path;
    char buff[PATH_MAX + 1];
    int  i=0;
    char backup[Size][Size];
    int status1=0;

    char fname[MAX_PATH_SIZE];
    struct dirent *dosya;																	
	struct stat status;
	DIR *dir;
	
    pid_t pid;      //directoryler icin fork yapildiginda child kontrolu icin


    //path bulur >> getcwd()
    path = getcwd( buff, PATH_MAX + 1 );  
    //puts(buff);
    
    if( path != NULL ) {
        printf( "My working directory is %s.\n", path );
    }
	

    //return EXIT_SUCCESS;

    //path kontrolu
    if ((dir = opendir(path)) == NULL) {
		perror("opendir");
		exit(1);
	}
	

	while ((dosya = readdir(dir)) != NULL)
    {
		if (strcmp(dosya->d_name, ".") != 0 && strcmp(dosya->d_name, "..") != 0 ) 
		{          
		    //girdiği klasörün içindeki dosyaları ve klasörlere bakıyor
		    //eğer dosya görürse adını backup arrayinin içine atar klasörse fonksiyon
		    //yeniden çağırılır.O klasörün içine girer ve aynı işlem yapılır
			i=strlen(dosya->d_name);
			sprintf(fname, "%s/%s", path, dosya->d_name);   //path >> fname
						
			if( dosya->d_name[i-1] != '~'  )
			{							
				if(dosya->d_type==DT_REG)   //file 
				{		
					strcpy(backup[status1],fname);   //file adi backup'a kopyalaniyor.
					puts(backup[status1]);
					status1++;
				}
				if(stat(fname, &status) == -1) 
				{                            
					perror("stat");                                  
					break;
				}
				if(S_ISDIR(status.st_mode))    //Directory
				{
				    pid=fork();               //fork yapildi   ////FORK
	         
				    if(pid==0)    //child'sa girer
				    {	
				       listServer();	     //recursive edildi string bulma fonksiyonu
				       exit(0);
				    }		
				    else{ 
				    
					    wait(NULL);  
				    }		
				}																		
			}					
		}
	}

}





//CTRL-C icni signal fonksiyonu  //kill fonksiyonu cagrildi log dosyalarina yazdirildi
void signal_callback_handler(int signum)
{
    char  c;
    int k=0;
       
    signal(signum, SIG_IGN);
    printf("\nCTRL-C sinyali alindi.Tum Clientlar Kapatildi!!!\n"); 
    printf("\nServer log dosylarina yazdirildi.Tum Clientlar Kill!!!\n"); 
    printf("\nServer kill!!!\n"); 
    
    logFileWrite();  //logFile yazdirma
    
    //tum client kill
    for(k=0; k< arr_client_size; ++k)
        kill(arr_client_pid[k], SIGINT);    

    //server kill
    kill(server_pid, SIGINT);
     
    sleep(1);
    
    exit(0);

}


void logFileWrite(void)
{

    FILE *outp;
    char str[20];
    
    sprintf(str,"logServer_%d.log", (int)server_pid);
    
    //server_pid 'sine gore log dosyasi aciliyor
    outp= fopen(str, "a+" );
    
    fprintf(outp, "server_pid >> %d\n", (int)server_pid);
   
     
    fprintf(outp,"\nServer log dosylarina yazdirildi.Tum Clientlar Kill!!!\n"); 
    fprintf(outp,"\nServer kill!!!\n"); 
   

    fclose(outp);
}


