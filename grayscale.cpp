#include <stdio.h>
#include <stdlib.h>
#include "imgreader.h"

int grayscale(char outfname[], pixels **pixel, unsigned char hdr[], int width, int height){
  FILE *fp ;
  unsigned char *data ;
  int i = 0 , x = 0, y = 0 ;
  int imgSize = width * height * 3 ;

  data = (unsigned char *)malloc(width * height * 3 * sizeof(char)) ;
  fp =fopen(outfname, "wb") ;
  if(fp == NULL){
    printf("ERROR: CAN'T OPEN FILE\n") ;
    return -1 ;
  }
  fwrite(hdr, 1, 54, fp) ;
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      data[i] = pixel[x][y].b*0.3 + pixel[x][y].g*0.59 + pixel[x][y].r*0.11 ;
      data[i+1] = pixel[x][y].b*0.3 + pixel[x][y].g*0.59 + pixel[x][y].r*0.11 ;
      data[i+2] = pixel[x][y].b*0.3 + pixel[x][y].g*0.59 + pixel[x][y].r*0.11 ;
      i += 3 ;
    }
  }
  fwrite(data, 1, imgSize, fp) ;
  free(data) ;
  fclose(fp) ;
  return 0 ;
}

int main(void){
  char fname[] = "image_11.bmp" ;
  char grayfname[] = "image_11_gray.bmp" ;
  unsigned char hdr[54] ;
  image img ;
  FILE *fp ;
  pixels **pixel ;
  int i ,imgRead, mkgray ;

  fp = fopen(fname, "rb") ;
  if(fp==NULL){
    printf("ERROR: FAILED TO OPEN THE FILE\n") ;
    return -1 ;
  }

  img = readHdr(fp, hdr) ;
  //printf("height=%d, width=%d\n", img.height, img.width) ;
  pixel = (pixels **)malloc(img.width * sizeof(pixels *)) ;
  for(i=0; i<img.width; i++){
    pixel[i] = (pixels *)malloc(img.height * sizeof(pixels)) ;
  }

  imgRead = readData(img.width, img.height, fp, pixel) ;
  printf("Finished reading\n") ;

  mkgray = grayscale(grayfname, pixel, hdr, img.width, img.height) ;

  free(pixel) ;

  printf("Finished grayscaling\n") ;
  return 0 ;
}
