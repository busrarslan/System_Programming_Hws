
// Busra Arslan 
// * 131044021
 //* client.c
 //* Final
 

//////Librarys///////
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
#include <sys/socket.h> //socket
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <limits.h>   
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//Kullanilan diger c dosyalari
#include "restart.h"
#include "uici.h"
#include "uiciname.h"

////Defines///////////////
#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 50 /*maximum number of client connections*/
#define MAXSIZE 100000
#define MAX_PATH_SIZE 1024
#define Size 1000    
  
struct ClientServer{
    pid_t client_pid; 
    int choose;     //menu secimi
    int time;
};


struct ClientServer CS;
pid_t arr_client_pid[LISTENQ];  //signal de tum clientlari kill yapmak icin 
int arr_client_size=0;      //arr_client_pid 'nin size'ini tuttum. 
char backup[Size][Size];      //sendFile
int status1;

char filename[MAX_PATH_SIZE];  //sendFile
char temp_arr[MAXSIZE];   //for sendFile
char temp_arr2[MAXSIZE];   //for sendFile

    
//////////Function Prototype//////////////////
void signal_callback_handler(int signum);
void listLocal();
void sendFile();
void help();
void logFileWrite(void);



/////////////////////////////MAIN///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    /*Variables*/
    int port_number;
    int sockfd, n=0;
    struct sockaddr_in servaddr;            //for ip_number check
    char recvline[MAXLINE];
    int s;
    int a=0;
    int t=0;
    //for sendFile
    char client_path[PATH_MAX + 1];
    char* path;

    

    /*arguman sayisi kontrolu*/
	if (argc != 3) {                  
	    printf("Usage: dirtree <path>\n");
	    exit(1);
	}

           
    port_number = (u_port_t)atoi(argv[2]);  //port number atama

    //client pid'si
    CS.client_pid = getpid();
    printf("pid %d\n", (int)CS.client_pid); 
    
          

    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(argv[1]);
    servaddr.sin_port =  htons(port_number); //convert to big-endian order
    
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)));
     
    printf("connection\n");   

    ////////////////////CLİENT MENU BOLUMU///////////////////////////////////
    
    while(1) 
    {

        //signal
        signal(SIGINT, signal_callback_handler);  
        //sleep(1);
            
        /////////////server'a struct gondermek//////////////////////////////
        if ((send(sockfd,&CS, sizeof ( struct ClientServer ),0)) == -1 ) {
            fprintf(stderr, "Failure Sending Message\n");
            close(sockfd);
            exit(1);
        }
        
         //<filename> <clientid>  for sendFile
        path = getcwd( client_path, PATH_MAX + 1 );
        if ((send(sockfd, client_path , sizeof (client_path),0)) == -1 ) {
            fprintf(stderr, "Failure Sending Message\n");
            close(sockfd);
            exit(1);
        }  
              
       
    
        //Komut secimi//////////////////////////////////////////
        printf("___________________________________\n");
        printf("Final projesi sunumuna hosgeldiniz\n");
        printf("Client islemleri icin secim yapmalisiniz.Secim yapacaginiz komutun numarasini giriniz\n");
        
        printf("__________SECENEKLER___________\n");
        printf("_______________________________\n");
        printf("[1] listLocal<<<<<<<<<<<<<\n");
        printf("[2] listServer<<<<<<<<<<<<\n");
        printf("[3] lsClients<<<<<<<<<<<<<\n");
        printf("[4] sendFile<<<<<<<<<<<<<<\n");
        printf("[5] help<<<<<<<<<<<<<<<<<<\n"); 

        ///////////////////////////////////////CHOOSE/////////////////////////////////////
        printf("Client islemleri icin secim yapmalisiniz\n");
        scanf("%d",&CS.choose);
        
         /////////////server'a struct gondermek//////////////////////////////
        if ((send(sockfd,&CS, sizeof ( struct ClientServer ),0)) == -1 ) {
            fprintf(stderr, "Failure Sending Message\n");
            close(sockfd);
            exit(1);
        }
        //printf("%d secim\n", CS.choose);
   
        if(CS.choose== 2)             //listServer
        {  
              printf("listServer\n");
        
        }
        else if(CS.choose == 1)          //clientta islem goruyor
        {        
              printf("listLocal\n");
              listLocal();
        }
        else if(CS.choose == 3)         //clientta islem goruyor
        {  

             printf("lsClients\n");
            
             if(recv(sockfd,  arr_client_pid, sizeof ( arr_client_pid ),0) > 0);
             if(recv(sockfd,  &arr_client_size, sizeof (arr_client_size ),0) > 0);
             
             printf("<<<<clients pid>>>>>>>\n");
             for(t=0; t< arr_client_size; ++t)
                printf("client pid:  %d \n",  arr_client_pid[t]);
             
            
        }         
        else if(CS.choose == 5){            //clientta islem goruyor
             printf("help\n");
             help();
        }  
        else if(CS.choose == 4)                 //sendFile
        {  
            int secim;
            int q=0,a,b;

            sendFile();
            //mini menu
            printf("sendFile'a hosgeldiniz\n");
            printf("[1] <filename> \n");
            printf("[2] <filename> <clientid>\n");
            printf("Dosya gondermek istediginiz sekle gore 1 veya 2 giriniz\n");
            
            scanf("%d", &secim);
          
            //secimi servera gonderme
            if ((send(sockfd,&secim, sizeof (secim ),0)) == -1 ) {
                fprintf(stderr, "Failure Sending Message\n");
                close(sockfd);
                exit(1);
            }
                
                
            if(secim == 1)
            {   
                FILE *inp;
                int w=0;
                int iii,temp=0;
                char buff[PATH_MAX + 1];
                char* path;
                pid_t clientid;
                
                printf("filename adi giriniz: \n");
                scanf("%s", filename);
                 
                /*for(a=0; a<status1; ++a)
                {
                    puts(backup[a]);
                }*/     
               
                path = getcwd( buff, PATH_MAX + 1 );
                //puts(path);
                strncat(buff, "/", 1);
                strncat(buff, filename, strlen(filename));
                puts(buff);  //pathi ekrana yazar for check
                
                 //printf("status >> %d\n", status1);   
                 for(iii=0; iii<status1; ++iii)
                 {
                    if(strcmp(backup[iii],path) == 0)
                        temp=1;
                 }   
                
                 
                 if ((send(sockfd, &temp, sizeof (temp),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(sockfd);
                        exit(1);
                 }  
                 
                 
                 if(temp==0)      
                    printf("Filename dosya dizininde bulunamadi\n");
                 else if(temp == 1)
                 {
                    char ch;
                     printf("Dosyayi serverda olusturdu.Ve icine yazdi\n");
                    inp= fopen(filename,"r");
                    //dosya icerigini iki boyutlu arraye kopyaladim
                    while(!feof(inp))
                    {
                        fscanf(inp,"%c", &ch); 
                        temp_arr[w] = ch;
                        ++w;          
                    } 
                    if ((send(sockfd, &w, sizeof (w),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(sockfd);
                        exit(1);
                    }  
                      
                    if ((send(sockfd, temp_arr, sizeof ( temp_arr),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(sockfd);
                        exit(1);
                    }  
                    if ((send(sockfd, filename, sizeof ( filename),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(sockfd);
                        exit(1);
                    } 
                     
               
                    fclose(inp);    
              
                 }
                
              }
              else if(secim == 2)
              {
                   FILE *inp2;
                   int yy;
                   char ch2;
                   pid_t clientid;
                   printf("filename adi giriniz: \n");
                   scanf("%s", filename); 
                   printf("clientid giriniz: \n");
                   scanf("%d", &clientid);  
                   
                   inp2= fopen(filename,"r");
                   
                   if ((send(sockfd, filename, sizeof ( filename),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(sockfd);
                        exit(1);
                    } 
                    
                   if ((send(sockfd, &clientid, sizeof (clientid),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(sockfd);
                        exit(1);
                   }
                   while(!feof(inp2))
                    {
                        fscanf(inp2,"%c", &ch2); 
                        temp_arr2[yy] = ch2;
                        ++yy;          
                    }  
                    
                   if ((send(sockfd, &yy, sizeof (yy),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(sockfd);
                        exit(1);
                    }  
                      
                    if ((send(sockfd, temp_arr2, sizeof ( temp_arr2),0)) == -1 ) {
                        fprintf(stderr, "Failure Sending Message\n");
                        close(sockfd);
                        exit(1);
                    } 
                    else 
                        printf("başarılar ikinci clientinizdan alinan dosya ilk clientinizda olusturuldu\n"); 

              }
                    
        }
        else 
            exit(0);
        
    }   
        

   
   
    return 0;


   
}


//Clientin bulundugu localdeki dosyalari listeler
void listLocal()
{
    char* path;
    char buff[PATH_MAX + 1];
    int  i=0;
    char fname[MAX_PATH_SIZE];
   
      
    struct dirent *dosya;																	
	struct stat status;
	DIR *dir;
	
    pid_t pid;      //directoryler icin fork yapildiginda child kontrolu icin


    path = getcwd( buff, PATH_MAX + 1 );
    //puts(buff);
    if( path != NULL ) {
        printf( "My working directory is %s.\n", path );
    }


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
				       listLocal();	     //recursive edildi string bulma fonksiyonu
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


void sendFile()
{
    char* path;
    char buff[PATH_MAX + 1];
    int  i=0;
    char fname[MAX_PATH_SIZE];
   
      
    struct dirent *dosya;																	
	struct stat status;
	DIR *dir;
	
    pid_t pid;      //directoryler icin fork yapildiginda child kontrolu icin


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
				       listLocal();	     //recursive edildi string bulma fonksiyonu
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



void help()
{
    printf("Help bolumune hosgeldiniz. Fonksiyonlar icin bilgilendirme almak icin bu bolumden yararlaniniz \n");
    
    printf("listLocal  4>> to list the local files in the directory client program started\n");
    printf("listServer >> to list the files in the current scope of the server-client\n");
    printf("lsClients  >> lists the clients currently connected to the server with their respective clientids\n");
    printf("sendFile <filename> <clientid> or file <filename> (if file exists) \n");
    printf("help    >> displays the available comments and their usage\n");

}




//CTRL-C icni signal fonksiyonu  //kill fonksiyonu cagrildi log dosyalarina yazdirildi
void signal_callback_handler(int signum)
{
    char  c;
    //int k=0;
       
    signal(signum, SIG_IGN);
    printf("\nCTRL-C sinyali alindi.Client killed!!!\n"); 
    
    logFileWrite();  //logFile yazdirma
    
    //client kill
    kill(CS.client_pid, SIGINT);
    
     
    sleep(1);
    
    exit(0);

}


void logFileWrite(void)
{

    FILE *outp;
    char str[20];
    
    sprintf(str,"logClient_%d.log", (int)CS.client_pid);
    
    //client_pid 'sine gore log dosyasi aciliyor
    outp= fopen(str, "a+" );
    
    fprintf(outp, "client_pid >> %d\n", (int)CS.client_pid);
     
    fprintf(outp,"\nClient_pid_%d Kill!!!\n", (int)CS.client_pid); 

   

    fclose(outp);
}




