/*Busra Arslan*/
/*131044021   */
/*HW-5        */


 
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



#define MAX_PATH_SIZE 1024
#define Size 1000
#define fifo_connect "connection"   //ana fifo baglantim icin


//global variables
char *aranan_str;       //void * functionda kullanmak icin arattigimiz string
int count_aranan;       //void * functionda kullanmak icin arattigimiz stringin size'i
int file;              //fifo icin
int total=0;           // aranan stringin total sayisini bulmak icin

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;     //MUTEX


//function prototype///////
void DirWalk(const char *path);
void* counterOfString(void *);
void signal_callback_handler(int signum);  //CTRL_C sinyali
	
struct timespec start, stop;    //timer hesabi icin


/////////////////////////////////////MAIN//////////////////////////////////////
int main(int argc, char *argv[])
{			
    
	struct timespec begin, end;
	
	int  j=0;

        
	if (argc != 3) {
	    printf("Usage: dirtree <path>\n");
	    exit(1);
	}
	
    count_aranan= strlen(argv[2]);
    aranan_str= (char*) malloc (sizeof(char) *count_aranan);  //aranan_str'ye yer alindi.
		
	for(j=0; j< count_aranan; ++j)   /*aranan string bir stringe atildi*/
	    aranan_str[j]=argv[2][j];



    //FIFO
    unlink(fifo_connect);   
    file = mkfifo(fifo_connect, 0666);    //fifo olusturuldu.
    
    if(file<0) {
         printf("Unable to create a fifo");
         exit(-1);
    }
   
    file = open(fifo_connect,O_RDWR);    //fifo okundu   ////FIFO
    if(file <1) {            
        perror("Error opening file");
        exit(1);
    }
    int a=0;
	write(file,&a,sizeof(a));
    

	//fonksiyonun çalışması süresini hesaplayan clock hesabi
	int n_sec;
	if( clock_gettime( CLOCK_REALTIME, &begin) == -1 ) 
	{				
		perror( "clock gettime" );
		return EXIT_FAILURE;
	}

//////////////////////////////////////
	DirWalk(argv[1]);	
		
	read(file,&a,sizeof(a));	

	printf("Tum filelardaki toplam string sayisi %d\n",a);    //aranan toplam string sayisi
	 
	close(file);        //fifo icin acilan dosya kapatildi.
	free(aranan_str);   //string free edildi.
//////////////////////////////////////
		
	if( clock_gettime( CLOCK_REALTIME, &end) == -1 )
    {			
        perror( "clock gettime" );
        return EXIT_FAILURE;
	}

	n_sec = ( end.tv_sec - begin.tv_sec )*1000000
             + ( end.tv_nsec - begin.tv_nsec );
         
	
	printf("\n\nProgramın Çalışma Süresi:%d nanosaniye\n",n_sec);
	
	
	
	return 0;
} 

/////////////////////////////////////END_MAIN//////////////////////////////////////


void DirWalk(const char *path)						
{    
	struct dirent *dosya;																	
	struct stat status;
	DIR *dir;
	FILE* inp;
	FILE *outp;
	FILE* inpipe;
	

	char backup[Size][Size];
	int i=0,status1=0,c=0,d=0;
	void *temp;
	int count;
	char fname[MAX_PATH_SIZE];
	pid_t pid;      //directoryler icin fork yapildiginda child kontrolu icin
	   
	//CTRL_C Sinyali                                    //sinyal
	signal(SIGINT, signal_callback_handler);   
	   
	
	   
    outp=fopen( "gfd.log" ,"a+");   /*output file*/
     
	if ((dir = opendir(path)) == NULL) {
		perror("opendir");
		exit(1);
	}
	

	while ((dosya = readdir(dir)) != NULL)
    {
		if (strcmp(dosya->d_name, ".") != 0 && strcmp(dosya->d_name, "..") != 0 ) 
		{          
		    /*girdiği klasörün içindeki dosyaları ve klasörlere bakıyor*/
		    /*eğer dosya görürse adını backup arrayinin içine atar klasörse fonksiyon*/
		    /*yeniden çağırılır.O klasörün içine girer ve aynı işlem yapılır*/
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
				    pid=fork();                        //fork yapildi   ////FORK
	         
				    if(pid==0)    //child'sa girer
				    {	
				       DirWalk(fname);	                    //recursive edildi string bulma fonksiyonu
				       exit(0);
				    }		
				    else{ 
				    
					    wait(NULL);  
				    }		
				}																		
			}					
		}
	}
  
    //////////////////////////////---- THREAD and MUTEX ----/////////////////////////////////////

	pthread_t thread_id[status1];		

	for(c=0;c<status1;c++)
	{	
		inp=fopen(backup[c],"r");
		if(inp==NULL)
			printf("Dosya acilamadi.!!Hatali\n");
			
        
	    pthread_create( &thread_id[c], NULL, counterOfString, inp);	  //string sayisini bulan fonksiyonu cagirma
        puts(backup[c]);   //path'i ekrana basar (en icteki dosyadan en genele dogru basar).
		fprintf(outp,"path>>%s\n",backup[c]);		  				  

	}	
	
	  
	for(d=0; d < status1; d++)
	{			              
        pthread_join( thread_id[d],&temp);
                    	
	    count += (long)(int)(intptr_t)temp;         
	}				

 
	closedir(dir);
	fclose(outp);

    read(file,&total,sizeof(total));     //read fifo  

	total+=count;                        // total >> aranan stringin toplam sayisi
			
	write(file,&total,sizeof(total));    ///write fifo
	
}



//CTRL-C icni signal fonksiyonu
void signal_callback_handler(int signum)
{

    signal(signum, SIG_IGN);
    printf("\nCTRL-C sinyali alindi.Programdan 2 sn sonra cikilacak\n");
    
    sleep(2);
    exit(0);
}


//String dosyada kac kere gectigini bulan ,row ve col degerlerini 
//output dosyasina yazan fonksiyon
void* counterOfString(void *inp)
{	
    FILE *outp;
    FILE *inpipe;
	int size=0,t=0,i=0,j=0,k=0,n=0;
	int count=0,bulma_sirasi=0,temp=0;
	int row_count=0, col_count=0;
	char ch;
	int fds[2];
	int total_temp;
	char* str;


    pthread_mutex_lock( &mutex1 );       //Lock//////////////////
    
    
    outp=fopen( "gfd.log" ,"a+");   /*output file*/ 
    

	//Tum elemanlari tek boyutlu bir stringe atandi
    while(!feof(inp))
    {
        fscanf(inp,"%c", &ch); 
        ++size;
    }   
    rewind(inp);
    
    //Memoryden tek boyutlu string icin yer alma
    str = (char*) malloc (sizeof(char) *size+3);

    //Dosyadaki karakterleri dosya sonuna kadar okuyup stringe atmak
    while(!feof(inp))
    {
        fscanf(inp,"%c", &ch);       
        *(str)=ch;
        str[t+1]= *(str);
        ++t;
    }   

    fprintf(outp,"\n");
    //Search:Aranan stringi, stringin elamaninin 
    //ilk harfinin gectigi yeri bulma            
    while(i+1< size-2)
    {
        k=i;
        
        if(str[i+1] == aranan_str[0])
        {            
            while((aranan_str[j] == str[k+1]) && (j< count_aranan))
            {
                ++j;  ++k;                  
            } 
            if(j == count_aranan)
            {
                ++count;
                ++bulma_sirasi;
                //row
                for(n=0; n< i+1; ++n)
                {
                    if(str[n+1] == '\n'){
                        temp=n+1;
                        ++row_count;
                    }
                }
               //column
               if(temp != 0)
                   col_count= ((i+1) - temp - 1);
               else if(temp == 0)
                   col_count= (k - count_aranan);             
               
                //gfd.log dosyasina yazdirma
                fprintf(outp,"%d.  ", bulma_sirasi);
                fprintf(outp,"row: %d ", row_count+1);
                fprintf(outp,"col:%d \n", col_count+1);
                //gfd.log terminale yazdirma
                printf("%d.  ", bulma_sirasi);
                printf("row: %d ", row_count+1);
                printf("col:%d \n", col_count+1);
            }
       
            
            row_count=0;
            j=0;  k=0;
        }
        ++i;
    }

    fprintf(outp,"Aranan stringin dosyada gectigi sayi: %d \n", count); 
    printf("Aranan stringin dosyada gectigi sayi: %d \n\n", count);   
    

    total_temp += count;
	//printf("%d", total_temp);

    i=0;  //while'da arttirilan size degeri icin her islem gordugu dosyadan once sifirlanir
    t=0;  //stringin size'i
  
  
    pthread_mutex_unlock( &mutex1 );         //Unlock////////////////////
    
    //memory free edilir ve dosyalar kapatilir
    close(fds[0]);
    free(str); 	   
    fclose(inp);
    fclose(outp);

    //Bu kaynaktan yararlanildi
    //http://sorucevap.netgez.com/sizet-vs-intptrt
	return (void*)(intptr_t) total_temp;
	
}


