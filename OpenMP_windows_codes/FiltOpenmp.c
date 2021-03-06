
/************************** OpenMP IMPLEMENTATION *******************************************/
/****************** IMPLEMENTATION SUITABLE FOR MICROSOFT WINDOWS ***********************/
/*********************** Image Averaging Filter Benchmark *******************************/
//FILES NEEDED: bmp file of 8 bits (The repository has a sample file)
//Find files in: Sample Data/lady1000.bmp//

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned char* readBMP(char* filename, unsigned char info[54], unsigned char end[1024], int *size)
{
    int i;
    unsigned char* data;

    FILE* f = fopen(filename, "rb");

    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    *size = width * height;

    data = (unsigned char*) malloc(*size);
    fread(data, sizeof(unsigned char), *size, f); // read the rest of the data at once
    fread(end, sizeof(unsigned char), 1024, f);
    fclose(f);

    return data;
}

void writeBMP(char* filename, unsigned char* data, unsigned char info[54], unsigned char end[1024], int size){

     //Put everything again in a bmp file
    int i;
    FILE* newf = fopen(filename,"wb");
    fwrite(info,sizeof(unsigned char),54,newf);
    fwrite(data,sizeof(unsigned char),size,newf);
    fwrite(end,sizeof(unsigned char),1024,newf);
    fclose(newf);

}

/*****************************AVERAGING FILTER*******************************************/
void promediador(unsigned char *imagen,long TX,long TY, int tam,unsigned char *copia,float DesE){

    int i,j,mi,mj;
	float acum=0;
	acum=0;

#pragma omp parallel shared(copia,imagen,tam) private(i,j,mi,mj,acum)
{

#pragma omp for
for(i=0;i<=TY-tam;i++)
   for(j=0;j<=TX-tam;j++){
      for(mi=i;mi<i+tam;mi++)
         for(mj=j;mj<j+tam;mj++){
             acum+=imagen[(mi*TX)+mj];
         }
		 acum=(acum/((tam*tam)-1));
		 if(acum>DesE)
            copia[((i+tam/2)*TX)+(j+tam/2)]=(unsigned char)(acum);
         acum=0;
   }
}

}
void copy_image(unsigned char* original, unsigned char *copia, int size){
    int i;
    for(i=0;i<size;i++)
        copia[i] = original[i];
}

int main(int argc, char *argv[])
{
    unsigned char* image;
    unsigned char header[54];
    unsigned char end[1024];
    unsigned char* final_image;
    float DesE = 20.0;
    int size;
    int window = 30;
    int TX,TY,p=4;
    double startT,stopT;
    int nt=4;

    if(isdigit(argv[1])){
       p = atoi(argv[1]);
    if(p<1 || p>20)
        printf("# of threads should be between 1 and 20 - it runs 4 threads -> default");
    else
        nt = p;
    }
    omp_set_num_threads(nt);

    startT = clock();
    image = readBMP("lady1000.bmp",header,end,&size);

    //applying filter
    //extracting size
    TX = *(int*)&header[18];
    TY = *(int*)&header[22];

    final_image = (unsigned char*) malloc(size*sizeof(unsigned char));

    //copy image
    copy_image(image,final_image,size);
    //applying filter
    promediador(image,TX,TY,window,final_image,DesE);
    //write final result
    writeBMP("exit.bmp",final_image,header,end,size);

    stopT = clock();
    printf("%d processors; %f secs\n",p,(stopT-startT)/CLOCKS_PER_SEC);
}

