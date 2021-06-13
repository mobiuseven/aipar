#define THRESHOLD 100
#define BLACK 0x00
#define WHITE 0xff
#include <stdio.h>
#include <stdlib.h>
#include "imgreader.h"

int binarize(unsigned char **brightness, unsigned char hdr[], char filename[], int width, int height){
  FILE *fp ;
  unsigned char *data ;
  int imgSize = width * height * 3 ;
  int i = 0, x = 0, y = 0 ;

  //printf("a\n") ;
  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char)) ;
  //printf("b\n") ;
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      if(brightness[x][y]< THRESHOLD){
	      data[i] = BLACK ;
	      data[i+1] = BLACK ;
	      data[i+2] = BLACK ;
	      //printf("cb\n") ;
      }else{
	      data[i] = WHITE ;
	      data[i+1] = WHITE ;
	      data[i+2] = WHITE ;
	      //printf("cw\n") ;
      }
      i+=3 ;
    }
  }
  //printf("binary\n") ;

  fp=fopen(filename, "wb") ;
  if(fp==NULL){
    printf("ERROR: FAILED TO OPEN THE FILE") ;
    return -1 ;
  }
  fwrite(hdr, 1, 54, fp) ;
  fwrite(data, 1, imgSize, fp) ;
  free(data) ;
  fclose(fp) ;

  return 0 ;
}

int main(void) {
  char imgIN[] = "image_1_gray.bmp" ;
  char imgOUT[] = "image_1_binary.bmp" ;
  unsigned char hdr[54] ;
  FILE *fp ;
  image bmp ;
  unsigned char **brightness ;
  int i, imgRead, mkBin ;

  fp = fopen(imgIN, "rb") ;
  if(fp==NULL){
    printf("ERROR: FAILED TO OPEN THE FILE\n") ;
    return -1 ;
  }
  bmp = readHdr(fp, hdr) ;
  brightness = (unsigned char **)malloc(bmp.width * sizeof(unsigned char *)) ;
  for(i=0; i<bmp.width; i++){
    brightness[i] = (unsigned char *)malloc(bmp.height * sizeof(unsigned char)) ;
  }
  imgRead = readGrayData(bmp.width, bmp.height, fp, brightness) ;
  printf("Finished reading\n") ;
  mkBin = binarize(brightness, hdr, imgOUT, bmp.width, bmp.height) ;

  free(brightness) ;
  printf("Finished Binarizing\n") ;
  return 0 ;
}
