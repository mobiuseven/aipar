#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "imgreader.h"

unsigned char binarize(unsigned char brightness){
  unsigned char threshould = 100 ;

  if(brightness > threshould){
    return 255 ;
  }else{
    return 0 ;
  }
}

unsigned char subtract(unsigned char brightnessFront, unsigned char brightnessBack){
  int tmp ;

  tmp = (int)brightnessFront - (int)brightnessBack ;
  if(tmp < 0){
    tmp = -tmp ;
  }
  return (unsigned char)tmp ;
}


int backgroundSubtract(unsigned char hdr[], unsigned char **brightness, image front, unsigned char **brightnessBack, image back, char fname[] ){
  FILE *fp ;
  int heightFront = front.height, widthFront = front.width ;
  int heightBack = back.height, widthBack = back.width ;
  int imgSize = widthFront * heightFront *3 ;
  int x, y ;
  int i ;
  unsigned char *data;
  unsigned char **sub ;

  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char )) ;
  sub = (unsigned char **)malloc(widthFront * sizeof(unsigned char *)) ;
  if(sub == NULL || data == NULL){
    printf("MALLOC FAILED\n") ;
    return -1 ;
  } 
  for(i=0; i<widthFront; i++){
    sub[i] = (unsigned char *)malloc(heightFront * sizeof(unsigned char)) ;
    if(sub[i] == NULL){
      printf("MALLOC FAILED\n") ;
      return -1 ;
    }
  }

  //subtract
  for(y=0; y<heightFront; y++){
    for(x=0; x<widthFront; x++){
      sub[x][y] = subtract(brightness[x][y], brightnessBack[x][y]) ;
      //printf("%d %d %d\n", sub[x][y], x, y) ;
    }
  }
  printf("Subtraction Finished!\n") ;
  i = 0 ;

  //binarize
  for(y=0; y<heightFront; y++){
    for(x=0; x<widthFront; x++){
      //b
      data[i] = binarize(sub[x][y]);
      //g
      data[i+1] = binarize(sub[x][y]) ;
      //r
      data[i+2] = binarize(sub[x][y]) ;
      i += 3 ;
    }
  }
  printf("Binarize Finished\n") ;
  
  //open targert file
  fp = fopen(fname, "wb") ;
  //if failed to open the file
  if(fp==NULL){
    printf("ERROR: FAILED TO OPEN THE FILE.\n") ;
    return -1 ;
  }
  //write the header
  fwrite(hdr, 1, 54, fp) ;
  //write the new image data
  fwrite(data, 1, imgSize, fp) ;
  //printf("b\n") ;
  //free(data) ;
  //printf("a\n") ;
  //free(sub) ;
  //printf("c\n") ;
  fclose(fp) ;
  return 0 ;
}



int main(void){
  char imgIN[] = "image_1_gray.bmp" ;
  char imgIN2[] = "image_1_gray2.bmp" ;
  char imgOUT[] = "image_1_bkgsub.bmp" ;
  unsigned char hdr[54], hdr2[54] ;
  unsigned char **brightness, **brightness2 ;
  image bmp, bmp2 ;
  FILE *fp, *fp2 ;
  int i = 0 ;
  
  //open the front image and back image
  fp = fopen(imgIN, "rb") ;
  fp2 = fopen(imgIN2, "rb") ;
  if(fp==NULL || fp2==NULL){
    if(fp = NULL){
      printf("ERROR: FAILED TO READ %s\n", imgIN) ;
    }else{
      printf("ERROR: FAILED TO READ %s.\n", imgIN2) ;
    }
    return -1 ;
  }

  //read gray image
  //copy header info
  bmp = readHdr(fp, hdr) ;
  brightness = (unsigned char **)malloc(bmp.width * sizeof(unsigned char *)) ;
  if(brightness == NULL){
    printf("MALLOC FAILED\n") ;
    return -1 ;
  }
  for(i=0; i<bmp.width; i++){
    brightness[i] = (unsigned char *)malloc(bmp.height * sizeof(unsigned char));
    if(brightness[i] == NULL){
      printf("MALLOC FAILED\n") ;
      return -1 ;
    }
  }
  //get image data
  readGrayData(bmp.width, bmp.height, fp, brightness) ;
  //close gray image
  fclose(fp) ;
  printf("Reading Finished Gray Image\n") ;

  //read template image
  //copy header info
  bmp2 = readHdr(fp2, hdr2) ;
  brightness2 = (unsigned char **)malloc(bmp2.width * sizeof(unsigned char *)) ;
  if(brightness2 == NULL){
    printf("MALLOC FAILED\n") ;
    return -1 ;
  }
  for(i=0; i<bmp2.width; i++){
    brightness2[i] = (unsigned char *)malloc(bmp2.height * sizeof(unsigned char));
    if(brightness2[i] == NULL){
      printf("MALLOC FAILED\n") ;
      return -1 ;
    }
  }
  //get image data
  readGrayData(bmp2.width, bmp2.height, fp2, brightness2) ;
  //close gray image
  fclose(fp2) ;
  printf("Reading Finished Gray Image\n") ;

  //apply background subtraction
  backgroundSubtract(hdr, brightness, bmp, brightness2, bmp2, imgOUT) ;
  printf("Background Subtraction Finished!!\n") ;
  
  //free memory
  free(brightness) ;
  free(brightness2) ;

  printf("Finished\n") ;
  
  return 0 ;
}
