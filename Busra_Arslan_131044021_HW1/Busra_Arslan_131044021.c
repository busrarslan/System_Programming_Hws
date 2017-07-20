/* Busra ARSLAN                                                                   */
/* 131044021                                                                      */
/*Sistem Programlama HW01                                                         */

/*Stringi search eden algoritmam:                                                 */                                                   
/*Uzunluk kadar okuma yapiyor. Aranan stringin ilk elemanini kontrol eden ilk if'e*/
/*girip aranan stringin uzunlugu kadar ilerleyip tum karakterleri check ediyor.   */
/*eger tum harfler esitse aranan stringdir diyor ve count(buldugu sayi)'u         */
/* arttiriyor. Bulunan stringin row ve col degerini '\n' kontrolu yaparak buldum. */
/*Bu sekilde bulmamin nedeni okudugum dosyayi tek boyutlu stringe atmam.Bu yüzden */
/*'\n' kontrolu yaparak istenen ifadeleri buldum.                                 */

/*Notttt!!!Bulunan stringin row ve col degerleri 1'den baslayarak buldum.         */
/*Ekrana ilk buldugu row ve col'lari daha sonra kac tane buldugunu basiyor.       */

/*--------------------------------------------------------------------------------*/
/*                              Includes                                          */
/*--------------------------------------------------------------------------------*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


/*--------------------------------------------------------------------------------*/
/*                              MAIN                                              */
/*--------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{

    FILE *inp;
    FILE *outp;
    
    char ch;
    int  i=0, j=0, size=0; /*size: mallocla kac karaktere yer ayrildigi*/
    int k=0, t=0, n=0;
    int count=0, temp=0;   /*count: string kac kere gectigi*/
    int count_aranan=0;   /*aranan stringin size'i*/
    int row_count=0;      
    int col_count=0;
    int bulma_sirasi=0;  /*ilk buldugu yerden son buldugu yere numaralandirma*/

    char *str;   /*dosyadaki karakterleri attigim string*/
    
    outp=fopen( "gff.log" ,"a+");   /*output file*/
 
 
    /*FILE FOR CHECK*/
	if(argc > 3)
	{
		printf("\nFazla argüman girdiniz\n");
		return -1;
	}
	if(argc < 3)
	{
		printf("\nAz argüman girdiniz\n");
		return -1;
	}
	if((inp=fopen( argv[1] ,"r")) == NULL)
	{
		perror("Dosya Açılamadı\n"); 
		return -1;
	}
	
	
	count_aranan= strlen(argv[2]);


	/*Size bulmak icin dosyayi okuma*/
	/*Tum elemanlari tek boyutlu bir stringe atandi*/
	while(!feof(inp))
	{
	    fscanf(inp,"%c", &ch); 
        ++size;
    }   
    rewind(inp);
    
    /*Memoryden tek boyutlu string icin yer alma*/
    str = (char*) malloc (sizeof(char) *size);
	 
    /*Dosyadaki karakterleri dosya sonuna kadar okuyup stringe atmak*/
	while(!feof(inp))
	{
        fscanf(inp,"%c", &ch);       
        *(str)=ch;
        str[t+1]= *(str);
        ++t;
    }   

            
    /*Search:Aranan stringi, stringin elamaninin */
    /*ilk harfinin gectigi yeri bulma            */
    while(i+1< strlen(str)-3)
    {
        k=i;
        
        if(str[i+1] == argv[2][0])
        {            
            while((argv[2][j] == str[k+1]) && (j< count_aranan))
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
               
                /*gff.log dosyasina yazdirma*/
                fprintf(outp,"%d.  ", bulma_sirasi);
                fprintf(outp,"row: %d ", row_count+1);
                fprintf(outp,"col:%d \n", col_count+1);
                /*gff.log terminale yazdirma*/
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
    fclose(outp);
    
    return 0;

}







