/*/* Busra Arslan 
 * 131044021
 * Client.c
 * Midterm
 * IntegralGen.c
 * 
 ** //signal icin kaynak:http://www.csl.mtu.edu/cs4411.choi/www/Resource/signal.pdf
*   ///integralformulunde su siteden yararlanildi.Ve trapezoidal yöntemi kullanildi:*/
//http://www.codewithc.com/c-program-for-trapezoidal-method/
//clock_gettime fonksiyonu icin kitaptaki koddan yararlanildi.
//http://www.qnx.com/developers/docs/6.3.2/neutrino/lib_ref/c/clock_gettime.html

#include <stdio.h>
#include <sys/wait.h> 
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <math.h>
#include <matheval.h>



struct Client{

    char fi[1024],fj[1024];
    double time_interval;
    char operation[4];
    pid_t child_pid;
    double t0;    //clienta ilk baglandigi an
   
} Cli;
    
    
struct integralGen{

     double resolution; 
     int max_of_clients;
     double toplam_integral;
}integral;
    
    
//Global degiskenler
struct timespec start, stop;
pid_t str_pid[1024];   //sinyallerde kill yapabilmek icin global tanimladik
FILE *outp;    //server log filelar icin outputFİle
//sinyal fonksiyonu icin
char client_pid[1024];
int client_pid_size=0;
int client_sayisi=0;
    
    
    
//function prototype//////////
void signal_callback_handler(int signum);
double trapezoidals_int(void *f, double xmin, double xmax);
int stringMaxSizeBulma(int size1,int size2,int size3,int size4,int size5,int size6);

    
int main(int argc, char **argv)
{
    //signal(SIGINT, signal_callback_handler);
    FILE *f1_inp, *f2_inp, *f3_inp, *f4_inp, *f5_inp, *f6_inp;	
	struct timespec begin,end;
	int fifo_server,choice;
	char ch;
	char mathServerName[255];
	int file2;
	int size1=0,size2=0,size3=0,size4=0,size5=0,size6=0;
	char *str_1, *str_2, *str_3, *str_4, *str_5, *str_6; //dosyadan Cliunan veriler icin
	//for integral
    char *temp_str, *ilk_str, *ikinci_str;
    int temp_str_size=0;
    int size=0;
    
    //pipe icin variables
    int fds[2];     //pipe icin Cliuma yazma modu arrayi
   
	int i=0;                 //str_pid icin
	char fileName[1024];
       
   	
	if (argc != 3) {                   /*arguman sayisi kontrolu*/
	    printf("Usage: dirtree <path>\n");
	    exit(1);
	}
  
    //signal durumunda log dosyalarina yazilcak dosya
    sprintf(fileName,"logIntegralGen.log");
    outp= fopen(fileName, "a+");
  
    signal(SIGINT, signal_callback_handler);   //signal
    sleep(1);
  
	f1_inp= fopen("f1.txt","r");
    f2_inp= fopen("f2.txt","r");
    f3_inp= fopen("f3.txt","r");
    f4_inp= fopen("f4.txt","r");
    f5_inp= fopen("f5.txt","r");
    f6_inp= fopen("f6.txt","r");
	
	
	//Alinan argumanlari structlara atama
	if(argv[1] > 0)
	    integral.resolution=(double)atoi(argv[1]);

	integral.max_of_clients= atoi(argv[2]);
	
	//////////////////////////////////////////////////////////////////
	//6 adet .txt dosyayi Cliuma//
	//f1.txt dosya acma kontrolu
    if(f1_inp == NULL){
        perror("Error in file opening\n");
        exit(1);
    }
    
	while(!feof(f1_inp))
	{
	    fscanf(f1_inp,"%c", &ch);
        ++size1;
    }   
    str_1= (char*) malloc (sizeof(char) *size1);
    rewind(f1_inp);
    fgets(str_1, size1, f1_inp);
	//puts(str_1);
	
	
	//f2.txt dosya acma kontrolu
    if(f2_inp == NULL){
        perror("Error in file opening\n");
        exit(1);
    }
    
	while(!feof(f2_inp))
	{
	    fscanf(f2_inp,"%c", &ch); 
        ++size2;
    }   
    str_2= (char*) malloc (sizeof(char) *size2);
    rewind(f2_inp);
    fgets(str_2, size2, f2_inp);
	//puts(str_2);
	
	
	//f3.txt dosya acma kontrolu
    if(f3_inp == NULL){
        perror("Error in file opening\n");
        exit(1);
    }
    
    while(!feof(f3_inp))
	{
	    fscanf(f3_inp,"%c", &ch); 
        ++size3;
    }   
    str_3= (char*) malloc (sizeof(char) *size3);
    rewind(f3_inp);
    fgets(str_3, size3, f3_inp);
	//puts(str_3);
	
	
	//f4.txt dosya acma kontrolu
    if(f4_inp == NULL){
        perror("Error in file opening\n");
        exit(1);
    }
    
    while(!feof(f4_inp))
	{
	    fscanf(f4_inp,"%c", &ch); 
        ++size4;
    }   
    str_4= (char*) malloc (sizeof(char) *size4);
    rewind(f4_inp);
    fgets(str_4, size4, f4_inp);
	//puts(str_4);
	
	
	//f5.txt dosya acma kontrolu
    if(f5_inp == NULL){
        perror("Error in file opening\n");
        exit(1);
    }
    
    while(!feof(f5_inp))
	{
	    fscanf(f5_inp,"%c", &ch); 
        ++size5;
    }   
    str_5= (char*) malloc (sizeof(char) *size5);
    rewind(f5_inp);
    fgets(str_5, size5, f5_inp);
	//puts(str_5);
	
	
	//f6.txt dosya acma kontrolu
    if(f6_inp == NULL){
        perror("Error in file opening\n");
        exit(1);
    }
    
    while(!feof(f6_inp))
	{
	    fscanf(f6_inp,"%c", &ch); 
        ++size6;
    }   
    str_6= (char*) malloc (sizeof(char) *size6);
    rewind(f6_inp);
    fgets(str_6, size6, f6_inp);
	//puts(str_6);
	//////////////////////////////////////////////////////////////////
	
	
    ilk_str= (char*) malloc (sizeof(char) *temp_str_size);	
    ikinci_str= (char*) malloc (sizeof(char) *temp_str_size);
    
    temp_str_size= stringMaxSizeBulma(size1,size2, size3,size4,size5,size6);
    temp_str= (char*) malloc (sizeof(char) *temp_str_size*2); //iki dosya oldugu icin iki ile carptim
    
    signal(SIGINT, signal_callback_handler);   //signal
    sleep(1);
  
    
       
	//unlink(argv[1]);
	/////////////////////////FIFO////////////////////////////
	unlink("connection");
	
	file2 = mkfifo("connection", 0666); 
	if(file2<0) {
				 printf("Unable to create a fifo1");
				 exit(-1);
	}
	

	printf("fifos server and child created successfuly");
	
 
	while(1)
	{
		signal(SIGINT, signal_callback_handler);   //signal
        sleep(1);
  	
  	
		fifo_server = open("connection",O_RDWR);
	
	    if(fifo_server<1) {
		    printf("Error opening file");
	     }

           
        //İntegral hesabi icin <<<<to>>>>'in zamani bulma
        if( clock_gettime( CLOCK_REALTIME, &begin) == -1 ) {					
	        perror( "clock gettime" );
	        return EXIT_FAILURE;
        }
        
        
	    read(fifo_server,&Cli,sizeof(struct Client));	 ////Cliuma
	
	    /////CLOCKKKK//////////////TIMER////////////////
		if( clock_gettime( CLOCK_REALTIME, &end) == -1 ) {	
	        perror( "clock gettime" );
	        return EXIT_FAILURE;
		}
				
		//her clientin baglanma ilk baglanma suresi 
	    Cli.t0 = ( end.tv_sec - begin.tv_sec )*1000 + ( end.tv_nsec - begin.tv_nsec )/1000000000;
	   
	    //fflush(stdout);
        //printf("t0>> clienta ilk baglanma suresi\n%.1f", t0);
        
        
        //ilk dosya hangisi kontrolu
        if(strcmp(Cli.fi,"f1.txt")==0)
            strcpy(ilk_str, str_1);
        else if(strcmp(Cli.fi,"f2.txt")==0)
            strcpy(ilk_str, str_2);
        else if(strcmp(Cli.fi,"f3.txt")==0)
            strcpy(ilk_str, str_3);
        else if(strcmp(Cli.fi,"f4.txt")==0)
            strcpy(ilk_str, str_4);
        else if(strcmp(Cli.fi,"f5.txt")==0)
            strcpy(ilk_str, str_5);
        else if(strcmp(Cli.fi,"f6.txt")==0)
            strcpy(ilk_str, str_6);

            
        //ikinci dosya hangisi kontrolu
        if(strcmp(Cli.fj,"f1.txt")==0)
            strcpy(ikinci_str, str_1);
        else if(strcmp(Cli.fj,"f2.txt")==0)
            strcpy(ikinci_str, str_2);
        else if(strcmp(Cli.fj,"f3.txt")==0)
            strcpy(ikinci_str, str_3);
        else if(strcmp(Cli.fj,"f4.txt")==0)
            strcpy(ikinci_str, str_4);
        else if(strcmp(Cli.fj,"f5.txt")==0)
            strcpy(ikinci_str, str_5);
        else if(strcmp(Cli.fj,"f6.txt")==0)
            strcpy(ikinci_str, str_6);

        //puts(ilk_str);
        //puts(ikinci_str);
       
        signal(SIGINT, signal_callback_handler);   //signal
        sleep(1);
  
       //parantezler koyarak islem onceligi kontrolunu sagladim
        strcat(temp_str,"(");
        strncat(temp_str,ilk_str,strlen(ilk_str)-1);   //sondaki \n i almamasi icin
        size= strlen(temp_str);
        strcat(temp_str, ")");
        strcat(temp_str,Cli.operation);
        strcat(temp_str,"(");
        strncat(temp_str,ikinci_str,strlen(ikinci_str)-1);  //sondaki \n i almamasi icin
        size=strlen(temp_str);
        strcat(temp_str, ")");
        
        
        puts(temp_str);
        ////////////////
        void *f;
        
        f= evaluator_create(temp_str);
        puts(temp_str);
        
        
        //Bir pipe oluşturuluyor.max_of_client icin parent childe haber versin diye.
	    if(pipe(fds)==-1)          //pipe check        
        {
            perror("Failed to create pipe");
            return;
        }
        
        pid_t	pid;                    //fork
        if ((pid = fork()) < 0)
           printf("NULL");
           
           
	    //printf("%s",Cli.fi);
        // printf("%s",Cli.fj); 
              
	    double toplam= Cli.time_interval;
        
        if (pid == 0)
	    {
	           
            signal(SIGINT, signal_callback_handler);   //signal
            sleep(1);
  
  
	        //max_client kontrolu ve gelen client sayisini ekrana basma
	        close (fds[0]); //Cliuma kısmı kapatıldı
	        ++client_sayisi;                                           ///////pipe
	        write(fds[0], &client_sayisi, sizeof(client_sayisi)); //parente haber verdik 
        	//printf("GelenClientSayisi%d\n", client_sayisi);
	        //client_sayisinin max_of_clients'i asmamasi icin kontrol
            if(client_sayisi > integral.max_of_clients) 
            {
               perror("max_client'dan buyuk deger geldi.Cikiyor\n");
               exit(EXIT_FAILURE);    
            }
			 
			///////////////////////////////////////////
			//Her gelen client icin 	
			char temp_pid[1024];
	        sprintf(temp_pid, "%d", Cli.child_pid);
	        pid_t fifo_server_;
	        fifo_server_= open(temp_pid,O_WRONLY);
	       
	        //printf("temp pid: %s\n", temp_pid); 
        	

	    	str_pid[i]=Cli.child_pid; //str_pid sinyalde
	    	++i;
		
			
			
	               
	        if(strcmp(Cli.operation,"+")==0  ||  strcmp(Cli.operation,"-")==0  ||  strcmp(Cli.operation,"*")==0 )
            {
                 while(1)
                 {
                       
                    signal(SIGINT, signal_callback_handler);   //signal
                    //sleep(1);
              
  
                    integral.toplam_integral= trapezoidals_int(f, Cli.t0, Cli.time_interval);
                    Cli.t0= Cli.time_interval;
                    Cli.time_interval += toplam;
     
                    write(fifo_server,&integral,sizeof(struct integralGen));    ///yazma
    
	              }
	        }
     
            else if(strcmp(Cli.operation,"/")==0 )
            {
                if(atoi(ikinci_str) == 0)
                {
                    perror("Operatoru bolme islemi olan bu ifade de payda 0 oldugu icin islem yapilamaz\n");
                    integral.toplam_integral= -1;
                    write(fifo_server,&integral,sizeof(struct integralGen));     //yazma
                    exit(1);
                }
                else{
                    while(1)
                    {
                       
                        signal(SIGINT, signal_callback_handler);   //signal
                        //sleep(1);
     
                        integral.toplam_integral= trapezoidals_int(f, Cli.t0, Cli.time_interval);
                        Cli.t0= Cli.time_interval;
                        Cli.time_interval += toplam;
     
                        write(fifo_server,&integral,sizeof(struct integralGen));    ///yazma
                    }
                }
            }
		
	        exit(EXIT_FAILURE);
	        //close(new_pid);
	    }
	    else{
         	close (fds[1]);    //yazma modu kapatildi
            read(fds[0], &client_sayisi, sizeof(client_sayisi)); //parente haber verdik  //pipe read
            wait(NULL);
	    }
	

	    fflush(stdout);
	   	
										
	    //sleep(1);
			
										
    }	
    	
    close(fifo_server);
	unlink("fifo_server");
	free(temp_str);
	free(ilk_str);
	free(ikinci_str);
	free(str_1);    //mallocla yer alinan dosya Cliumalarinin 
	free(str_2);    //yapildi stringlere alinan yerler free edildi
	free(str_3);
	free(str_4);
	free(str_5);
	free(str_6);

	fclose(f1_inp);   //Cliunan dosyalar kapatilir
	fclose(f2_inp);
	fclose(f3_inp);
	fclose(f4_inp);
	fclose(f5_inp);
	fclose(f6_inp);

	

    return 0;
}

//Cliunan dosyalardaki verileri attigi strinlerin max sizeini bulup ona gore birlestirdigi stringde 
//mallocla yer ayırmak icin bu max sizei kullandim.
int stringMaxSizeBulma(int size1,int size2,int size3,int size4,int size5,int size6)
{
    int i;
    int max=0;
    char arr[6]={size1,size2,size3,size4,size5,size6};
    
    for(i=0;i<6;i++) {    
 
        if (arr[i]>=max) { 
            max=arr[i];
        }   
    }

    return max;
}


//CTRL-C icni signal fonksiyonu  //kill fonksiyonu cagrildi log dosyalarina yazdirildi
void signal_callback_handler(int signum)
{
    char  c;
    int k=0;
       
    signal(signum, SIG_IGN);
    printf("\nCTRL-C sinyali alindi.Tum Clientlar Kapatildi!!!\n"); 
    fprintf(outp,"CTRL-C sinyali alindi.Tum Clientlar Kapatildi!!!\n");
    fprintf(outp,"Tum clientlar kapatildigi icin serverda Kill olur.\n");
    
    for(k=0; k< client_pid_size; ++k)
        kill(client_pid[k], SIGINT);     //kill

    printf("Clientlarin hepsi kapandigi icin Server Killed 2 sn sonra.\n");
    printf("Clientlarin hepsi kapandigi icin Server Killed oldu.\n");
    
     
    sleep(2);
    
    exit(0);

}


//////////////////////////INTEGRALL (TRAPEZ)//////////////////////////
double trapezoidals_int(void *f, double xmin, double xmax)
{
    
    int i,n;
    double h,y[20],so,se,ans,x[20];

    h=(xmax-xmin)/integral.resolution;  //aralik sayisi >>h
    n=(xmax-xmin)/h;           //DElta X >> n
    if(n%2==1)    //tam sayi mi diye kontrol degilse tam sayi yapiyor DElta X i
    {
        n=n+1;
    }
    h=(xmax-xmin)/n;
   // printf("\nrefined value of n and h are:%d  %f\n",n,h);
   //printf("\n Y values \n");
   for(i=0; i<=n; i++)
   {
       x[i]=xmin+i*h;
       y[i]=evaluator_evaluate_x(f,x[i]);
       // printf("\n%f\n",y[i]);
   }
    so=0;
    se=0;
    for(i=1; i<n; i++)
    {
        if(i%2==1)
        {
            so=so+y[i];
        }
        else
        {
            se=se+y[i];
        }
    }
    ans=h/3*(y[0]+y[n]+4*so+2*se);
    
    return ans;

}


