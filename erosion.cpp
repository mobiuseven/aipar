#include <stdio.h>
#include <stdlib.h>
#include "imgreader.h"

unsigned char filter(int x, int y, int width, int height, unsigned char **brightness){
  unsigned char unit[9] ;
  int i ;
  int x1, x2, y1, y2 ;
 
  //define the other pixels
  x1 = x + 1 ;
  x2 = x - 1 ;
  y1 = y + 1 ;
  y2 = y - 1 ;

  //exceptions
  //left edge
  if(x1 > width-1){
    x1 = x ;
  }
  //right edge
  if(x2 < 0){
    x2 = x ;
  }
  //top
  if(y1 > height){
    y1 = y ;
  }
  //bottom
  if(y2 < 0){
    y2 = y ;
  }

  unit[0] = brightness[x2][y2] ;
  unit[1] = brightness[x][y2] ;
  unit[2] = brightness[x1][y2] ;
  unit[3] = brightness[x2][y] ;
  unit[4] = brightness[x][y]  ;
  unit[5] = brightness[x1][y] ;
  unit[6] = brightness[x2][y1] ;
  unit[7] = brightness[x][y1] ;
  unit[8] = brightness[x2][y1] ;

  //dilation
  for(i=0; i<9; i++){
    if(i==4){ //skip if you are checking featured pixel
      continue ;
    }
    if(!unit[i]){
      unit[4] = 0 ;
    }
  }
  
  return  unit[4];
}

int erosion(unsigned char **brightness, unsigned char hdr[], char fname[], int width, int height){
  FILE *fp ;
  int imgSize = width * height *3 ;
  int x = 0,  y = 0 ;
  int i = 0 ;
  unsigned char *data ;

  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char)) ;

  //open targert file
  fp = fopen(fname, "wb") ;
 
  //if failed to open the file
 if(fp==NULL){
    printf("ERROR: FAILED TO WRITE THE FILE.\n") ;
    return -1 ;
  }

 //write the header
  fwrite(hdr, 1, 54, fp) ;
  //generate new image
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      data[i] = filter(x, y, width, height, brightness) ;
      data[i+1] = filter(x, y, width, height, brightness) ;
      data[i+2] = filter(x, y, width, height, brightness) ;
      i += 3 ;
    }
  }
  
  //write the new image data
  fwrite(data, 1, imgSize, fp) ;
  free(data) ;
  return 0 ;
}


int main(void){
  char imgIN[] = "image_1_binary.bmp" ;
  char imgOUT[] = "image_1_erosion.bmp" ;
  unsigned char hdr[54] ;
  unsigned char **brightness ;
  image bmp ;
  FILE *fp ;
  int i = 0 ;
  int imgRead, mkErosion ;
  
  //open the gray image
  fp = fopen(imgIN, "rb") ;
  if(fp==NULL){
    printf("ERROR: FAILED TO READ THE FILE.\n") ;
    return -1 ;
  }

  //copy header info
  bmp = readHdr(fp, hdr) ;

  brightness = (unsigned char **)malloc(bmp.width * sizeof(unsigned char *)) ;
  for(i=0; i<bmp.width; i++){
    brightness[i] = (unsigned char *)malloc(bmp.height * sizeof(unsigned char));
  }

  //get image data
  imgRead = readGrayData(bmp.width, bmp.height, fp, brightness) ;

  //close gray image
  fclose(fp) ;

  printf("Finished reading\n") ;

  //apply median filter
  mkErosion = erosion(brightness, hdr, imgOUT, bmp.width, bmp.height) ;
  
  //free the memory
  free(brightness) ;
  
  printf("Finished erosion\n") ;
  
  return 0 ;
}
