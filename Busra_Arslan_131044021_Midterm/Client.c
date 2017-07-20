/* Busra Arslan 
 * 131044021
 * Client.c
 * Midterm
 * //signal icin kaynak:http://www.csl.mtu.edu/cs4411.choi/www/Resource/signal.pdf
*/

#include <stdio.h>
#include <sys/wait.h> 
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include<math.h>


struct odev{

    char fi[1024],fj[1024];
    double time_interval;
    char operation[4];
    pid_t child_pid;
   
} ok;
    
    
struct integralGen{

     double resolution; 
     int max_of_clients;
     double toplam_integral;
}integral;
 
FILE *outp;   
    
//function prototype
void signal_callback_handler(int signum);

    
int main(int argc, char **argv)
{
    //   signal(SIGINT, signal_callback_handler);

	int fifo_client,fifo_client_pid, fifo_math,i;
	int arr[4];	
	char mathServerName[255];
	int choice=atoi(argv[2]);
	int t=0,s=0, size=0,size2=0;
	int j=0;
	char fi_dosya[7]; //fi.txt gibi tum .txt dosyalarimin adinin uzunlugu 7 dir.
	char fj_dosya[7];
	char fileName[1024]; //output dosyalari icin string tutuldu
	
	
	strcpy(fi_dosya, argv[1]);
	strcpy(fj_dosya, argv[2]);
	
	
	if (argc != 5) {                   /*arguman sayisi kontrolu*/
	    printf("Usage: dirtree <path>\n");
	    exit(1);
	}
		
    //signal durumunda log dosyalarina yazilcak dosya
    sprintf(fileName,"logClient.log" );
    outp= fopen(fileName, "a+");  //log dosyalari icin output
	
	
	//Alinan argumanlari structlara atama ve checkler
       //birinci dosya icin
	if(strcmp(fi_dosya , "f1")==0 || strcmp(fi_dosya , "f1.txt")==0)
        strcpy(ok.fi,"f1.txt");
    else if(strcmp(fi_dosya , "f2")==0 || strcmp(fi_dosya , "f2.txt")==0)
	    strcpy(ok.fi,"f2.txt"); 
    else if(strcmp(fi_dosya , "f3")==0 || strcmp(fi_dosya , "f3.txt")==0)
	    strcpy(ok.fi,"f3.txt");     
	else if(strcmp(fi_dosya , "f4")==0 || strcmp(fi_dosya , "f4.txt")==0)
	    strcpy(ok.fi,"f4.txt");  
	else if(strcmp(fi_dosya , "f5")==0 || strcmp(fi_dosya , "f5.txt")==0)
        strcpy(ok.fi,"f5.txt");    
	else if(strcmp(fi_dosya , "f6")==0 || strcmp(fi_dosya , "f6.txt")==0)
	    strcpy(ok.fi,"f6.txt");     
	   
	//ikinci dosya icin    
    if(strcmp(fj_dosya , "f1")==0 || strcmp(fj_dosya , "f1.txt")==0)
	    strcpy(ok.fj,"f1.txt");   
    else if(strcmp(fj_dosya, "f2")==0 || strcmp(fj_dosya , "f2.txt")==0)
	    strcpy(ok.fj,"f2.txt"); 
    else if(strcmp(fj_dosya , "f3")==0 || strcmp(fj_dosya , "f3.txt")==0)
	    strcpy(ok.fj,"f3.txt");
    else if(strcmp(fj_dosya , "f4")==0 || strcmp(fj_dosya , "f4.txt")==0)
	    strcpy(ok.fj,"f4.txt");
    else if(strcmp(fj_dosya , "f5")==0 || strcmp(fj_dosya , "f5.txt")==0)
	    strcpy(ok.fj,"f5.txt");
    else if(strcmp(fj_dosya , "f6")==0 || strcmp(fj_dosya , "f6.txt")==0)
	    strcpy(ok.fj,"f6.txt"); 
	    
	    
	ok.time_interval= atoi(argv[3]);
	strcpy(ok.operation,argv[4]);
	
	
    
    //printf("%s\n", ok.fi);
     //printf("%s\n", ok.fj);


    
	fifo_client=open("connection",O_RDWR);
	 
	 
	if(fifo_client < 0) {
	    printf("Error in opening file");
	    exit(0);
    }  
	

	write(fifo_client, &ok, sizeof(struct odev));
	
	
	//client FIFO>>>>>>>>>>><
	//her client geldiginde onlari child olarak dusunup fifo yapar.
	char temp_pid[1024];
	ok.child_pid= getpid();   //child pid'lerini verir
	sprintf(temp_pid, "%d", ok.child_pid);
		
	int file = mkfifo(temp_pid,0666); 
	if(file<0) {
		 printf("Unable to create a fifo1");
		 exit(-1);
	}

	fifo_client_pid=open(temp_pid,O_RDWR);
	

	while(1){
	    read(fifo_client, &integral, sizeof(struct integralGen));
        signal(SIGINT, signal_callback_handler);   //signal
        //sleep(1);
 
	    printf("\nfinal integration is %f",integral.toplam_integral);
	}

		

	fflush(stdout);
	close(fifo_client);
	close(fifo_client_pid);
	//unlink(temp_pid);
	unlink("connection");
		
    return 0;
}




//CTRL-C icni signal fonksiyonu
void signal_callback_handler(int signum)
{
    char  c;
    int k=0;

        
    signal(signum, SIG_IGN);
    printf("\nCTRL-C sinyali alindi.Client Killled!!!\n"); 
    
    kill(ok.child_pid, SIGINT);    //kill

    printf("Client 2 sn sonra kapatilacak.\n");
    
    fprintf(outp,"\nfinal integration is %f\n",integral.toplam_integral);
    
    sleep(2);
    
    exit(0);

}


