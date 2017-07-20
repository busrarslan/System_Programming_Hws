/*Busra Arslan*/
/*131044021   */
/*HW-2        */

/*Description:                                                           */
/*https://gist.github.com/semihozkoroglu/737691  adresinden yaralanildi. */
/* DirWalk fonksiyonunun calisma sekli icin.                             */
/*DirWalk fonksiyonum arguman olarak girilen ilk dosyadan baslayarak tum */
/*dosyalara bakmaktadir. her dosya icin aranan string sayisi ayri ayri   */
/* dosyaya ve console'a yazilir. Console'a en son tum dosyalarda gecen   */
/*aranan string sayisi da basilir.                                       */
/*memory leak kontrolu yapilmis ve no memory leak sonucu alinmistir.     */
/*Directory icerisindeki her turlu uzantili dosyanin okumasi yapilip     */
/*aranan string bulunmaktadir. Screenshot dosyanin icindedir. Program    */
/*istenildigi sekilde tam olarak calismaktadir                           */


#include <stdio.h>
#include <sys/wait.h> 
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>   
#include <sys/stat.h>
#include <unistd.h>
 
#define MAX_PATH_SIZE 1024

/**********************************function prototype*****************************/
int DirWalk(const char *path, int count_aranan, char aranan_str[], FILE* outp);


/**********************************MAIN*******************************************/
int main(int argc, char *argv[])
{
	FILE* outp;
	int i=0, j=0;
    int count_aranan= strlen(argv[2]);
    char aranan_str[count_aranan];
    
    outp=fopen( "gfd.log" ,"a");   /*output file*/                  
	
	if (argc != 3) {                   /*arguman sayisi kontrolu*/
		printf("Usage: dirtree <path>\n");
		exit(1);
	}
	
	for(j=0; j<count_aranan; ++j)   /*aranan string bir stringe atildi*/
	    aranan_str[j]=argv[2][j];
	
	
	i=DirWalk(argv[1],count_aranan,aranan_str,outp);
	printf("\n\nToplam Bulunan String Sayısı:%d \n\n", i);
	
	fclose(outp);  
    return 0;
} 


/*path >> actigi filelarda ilerledigi yol*/
int DirWalk(const char *path, int count_aranan, char aranan_str[], FILE* outp)
{
    DIR *dir;
	pid_t pid;
	FILE* inp;
    struct dirent *dosya;																	
	struct stat status;  /*Degisen fname ile birlikte status durumunu alir ve S_ISDIR fonksiyonunda kullanir*/
	
	char* str;
	
	/*Variables*/
	int ii,i=0, j=0,temp,Words=0,status1=0, c=0;
	int row_count=0, col_count=0, count=0;
	int n=0, t=0, k=0, bulma_sirasi=0, size=0;
	char fname[MAX_PATH_SIZE];
	char ch;
	
   
    /*Directory acma kontrolu*/
    if ((dir = opendir(path)) == NULL) {
		perror("opendir");
		exit(1);
	}
	while ((dosya = readdir(dir)) != NULL) 
	{	
	    /*Dosya adı "." veya ".." olduğunda strcmp sıfır değeri dönecektir.*/
		if ((strcmp(dosya->d_name, ".") != 0) && (strcmp(dosya->d_name, "..") != 0 )) 
		{          
					
			pid=fork();   /*Her file icin fork yapildi.*/
			
			if(pid==0)    /*fork kontrolu */
			{		
				sprintf(fname, "%s/%s", path, dosya->d_name); /*dosya ismini fname'e atma*/

				ii=strlen(fname);
				if( fname[ii-1] != '~'  )
				{		
					puts(fname);
					if (stat(fname, &status) == -1) 
					{                            
						perror("stat");                                  
						break;
					}
					if (S_ISDIR(status.st_mode))   /*dosya dizin mi diye bakar*/
					{							
						Words+=DirWalk(fname,count_aranan,aranan_str, outp);	/*DirWalk fonksiyonunun tekrar cagrilmasi*/
					}
					else
					{					
						inp=fopen(fname,"r");
						if(inp==NULL)
							printf("Hatalııı\n");
			
						while(!feof(inp))  /*dosyanin size'ini buluyor*/
                        {
                            fscanf(inp,"%c", &ch); 
                            ++size;
                        }   
                        rewind(inp);
               
                        /*Memoryden tek boyutlu string icin yer alma*/
                        str = (char*) malloc (sizeof(char) *size-3);
                         
                        /*Dosyadaki karakterleri dosya sonuna kadar okuyup stringe atmak*/
                        while(!feof(inp))
                        { 
                            fscanf(inp,"%c", &ch);       
                            *(str)=ch;
                           str[t+1]= *(str);
                            ++t;
                        }   
                        
                        
                        /*Search:Aranan stringi bulma*/
                        while(i+1< strlen(str)-3)
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
                                    /*row*/
                                    for(n=0; n< i+1; ++n)
                                    {
                                        if(str[n+1] == '\n'){
                                            temp=n+1;
                                            ++row_count;
                                        }
                                    }
                                    	
                                   /*column*/
                                   if(temp != 0)
                                       col_count= ((i+1) - temp - 1);
                                   else if(temp == 0)
                                       col_count= (k - count_aranan);             
                                   
                                    /*gfd.log dosyasina yazdirma*/
                                    fprintf(outp,"%d.  ", bulma_sirasi);
                                    fprintf(outp,"row: %d ", row_count+1);
                                    fprintf(outp,"col:%d \n", col_count+1);
                                    /*gfd.log terminale yazdirma*/
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
                        printf("Aranan stringin dosyada gectigi sayi: %d \n", count);   
                     
                        /*memory free edilir ve dosya kapatilir*/
                        free(str); 
                        fclose(inp);
                                            
                        
					    Words+=count;
					    count=0;
					    i=0;
					    t=0;
				
					}																								
					
				}
				 			
				exit(Words);					
			}
			else {
				wait(&status1);
				c+=WEXITSTATUS(status1);
			}

		}

	}
	

	closedir(dir);
	return c;   /*aranan stringin sayisi*/

}





