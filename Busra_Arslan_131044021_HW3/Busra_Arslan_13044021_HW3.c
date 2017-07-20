/*Busra Arslan*/
/*131044021   */
/*HW-3        */

#include <stdio.h>
#include <sys/wait.h> 
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
 
#define MAX_PATH_SIZE 1024
#define Size 1000

//function prototype
void DirWalk(const char *path, int count_aranan, char aranan_str[], int *total);

void signal_callback_handler(int signum);


int main(int argc, char *argv[])
{
    int total=0;
	int i=0,j=0;
    int count_aranan= strlen(argv[2]);
    char aranan_str[count_aranan];
  
	              
	//signal(SIGINT, signal_callback_handler);
	if (argc != 3) {                   /*arguman sayisi kontrolu*/
		printf("Usage: dirtree <path>\n");
		exit(1);
	}
	
	for(j=0; j<count_aranan; ++j)   /*aranan string bir stringe atildi*/
	    aranan_str[j]=argv[2][j];

   
	DirWalk(argv[1], count_aranan,aranan_str, &total);	
	printf("Aranan stringin toplam sayisi>>> %d\n", total);

    return 0;
} 




void DirWalk(const char *path, int count_aranan, char aranan_str[], int *total)								
{
    struct dirent *dosya;																	
	struct stat status;
    DIR *dir;
	pid_t *pid;
	FILE* inp;
	FILE* outp;
    FILE* inpipe;
	
	/////////////*Variables*///////////////
	int ii=0, c=0;
	int i=0,temp,status1=0,p=0,j=0;
	int row_count=0, col_count=0, count=0;
	int n=0, t=0, k=0, bulma_sirasi=0, size=0;
    char fname[MAX_PATH_SIZE];
	int fds[2];       //pipe için bir dizi
	char backup[Size][Size];
	char ch;
	char* str;       //dosyadan okudugum characterleri attigim string


   signal(SIGINT, signal_callback_handler);
   outp=fopen( "gfd.log" ,"a+");   /*output file*/  
	
	if ((dir = opendir(path)) == NULL) {
		perror("opendir");
		exit(1);
	}
	
	sleep(1);   //bekleme suresi

      
	while ((dosya = readdir(dir)) != NULL)
    {	
		if (strcmp(dosya->d_name, ".") != 0 && strcmp(dosya->d_name, "..") != 0 ) 
		{    	      
			ii=strlen(dosya->d_name);
			sprintf(fname, "%s/%s", path, dosya->d_name);
								
			if( dosya->d_name[ii-1] != '~' )
			{		
				if(dosya->d_type==DT_REG)
				{		
					strcpy(backup[status1],fname);
					//puts(backup[status1]);
					status1++;
				}
				if (stat(fname, &status) == -1) {                            
					perror("stat");                                  
					break;
				}
				if (S_ISDIR(status.st_mode))
			    {
					DirWalk(fname,count_aranan,aranan_str,total);		//recursive>>> fonksiyon cagirma
				}																		
			}
				
		}
	}	
	
	
    pid=(pid_t *)malloc(sizeof(pid_t)*(status1));	
					
	for(p=0; p<status1; p++)
	{	
	    //Bir pipe oluşturuluyor.
		if(pipe(fds)==-1)          //pipe check        
        {
            perror("Failed to create pipe");
            return;
        }
		pid[p]=fork();
		
		
		if(pid[p]==0)
		{	
			//sleep(2);
			close (fds[0]); //Okuma kısmı kapatıldı

			inpipe = fdopen(fds[1], "w"); //Yazma modunda pipe çalışıyor.////////////////////////
						
			inp=fopen(backup[p],"r");
			
			if(inp==NULL)               //dosya acarken hata kontrolu
				printf("Could not open file\n");
								
			puts(backup[p]);   //path'i ekrana basar (en icteki dosyadan en genele dogru basar).
			fprintf(outp,"\npath>>%s\n",backup[p]);
								
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
            
            //memory free edilir ve dosya kapatilir
            free(str); 	   
            fclose(inp);

	        fprintf(outp,"Aranan stringin dosyada gectigi sayi: %d \n", count); 
            printf("Aranan stringin dosyada gectigi sayi: %d \n", count);   
         
                 
			write(fds[1], &count, sizeof(count)); //parente haber verdik
			
		    count=0;
		    i=0;
		    t=0;
	  
     									
			close (fds[1]); //dosya yazıldıktan sonra kapanıyor.
			exit(0);
								
		}
		else
	    {	
			close (fds[1]);

			inpipe = fdopen(fds[0], "a"); // Okuma modunda pipe çalışıyor.//////////////////

	        read(fds[0], &count, sizeof(count)); //parente haber verdik
	        
	        *total += count;
	        count=0;
			
			close (fds[0]); //işi biten dosya kapanıyor.

					
		}
									       
	}
   
   
	wait(NULL);									
			
   	free(pid);	
	fclose(outp);
	closedir(dir);
}

//CTRL-C icni signal fonksiyonu
void signal_callback_handler(int signum)
{
    char  c;

    signal(signum, SIG_IGN);
    printf("\nCTRL-C sinyali alindi.Tum Pipe ve Fifolar kapatildi\n");
    
    sleep(3);
    exit(0);

}


