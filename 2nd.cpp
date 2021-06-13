#include <stdio.h>
#include <stdlib.h>
#include "imgreader.h"

unsigned char gaussFilter(int x, int y, int width, int height, unsigned char **brightness){
  unsigned char unit[3][3] ;
  unsigned char sum = 0 ;
  int i, j, tmp ;

  //define pixels
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

  unit[0][0] = brightness[x2][y2] ;
  unit[1][0] = brightness[x][y2] ;
  unit[2][0] = brightness[x1][y2] ;
  unit[0][1] = brightness[x2][y] ;
  unit[1][1] = brightness[x][y]  ;
  unit[1][2] = brightness[x1][y] ;
  unit[2][0] = brightness[x2][y1]  ;
  unit[2][1] = brightness[x][y1] ;
  unit[2][2] = brightness[x2][y1] ;

  //gaussian filter
  for(i=0; i<3; i++){
    for(j=0; j<3; j++){
      tmp = i + j ;
      if(tmp%2){
	sum += unit[i][j] / 8 ;
      }else{
	if(i==2 && j==2){
	  sum += unit[i][j] / 4 ;
	}else{
	  sum += unit[i][j] / 16 ;
	}
      }
    }
  }
 
  
  return sum ;
}


unsigned char medianFilter(int x, int y, int width, int height, unsigned char **brightness){
  unsigned char unit[9] ;
  unsigned char sum = 0 ;
  int x1, x2, y1, y2 ;
  int i, j, tmp ;

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
  unit[6] = brightness[x2][y1]  ;
  unit[7] = brightness[x][y1] ;
  unit[8] = brightness[x2][y1] ;

  //median filter
  //sort
  for(i=0; i<9; i++){
    for(j=i+1; j<9; j++){
      if(unit[i] > unit[j]){
	tmp = unit[i] ;
	unit[i] = unit[j] ;
	unit[j] = tmp ;
      }
    }
  }
 
  //printf("%d\n", sum) ;
  
  return unit[4] ;
}

int gauss(unsigned char **brightness, unsigned char hdr[], char fname[], int width, int height){
  FILE *fp ;
  int imgSize = width * height *3 ;
  int x = 0, y = 0 ;
  int i = 0 ;
  unsigned char *data ;

  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char)) ;
  
  //open the target file
  fp = fopen(fname, "wb") ;

  //if failed to open the file
  if(fp==NULL){
    printf("ERROR: FAILED TO WRITE THE FILE.\n") ;
    return -1 ;
  }

  //write the header
  fwrite(hdr, 1, 54, fp) ;

  //generate the new image
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      data[i] = gaussFilter(x, y, width, height, brightness) ;
      data[i+1] = gaussFilter(x, y, width, height, brightness) ;
      data[i+2] = gaussFilter(x, y, width, height, brightness) ;
      i += 3 ;
    }
  }

  //write the new image data
  fwrite(data, 1, imgSize, fp) ;
  free(data) ;
  return 0 ;
}

int median(unsigned char **brightness, unsigned char hdr[], char fname[], int width, int height){
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

  //generate the new image
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
  char imgIN[] = "image_1_gray.bmp" ;
  char imgGaussOUT[] = "image_1_gauss.bmp" ;
  char imgMedianOUT[] = "image_1_gauss.bmp" ;
  unsigned char hdr[54] ;
  unsigned char **brightness ;
  image bmp ;
  FILE *fp ;
  int i = 0 ;
  int imgRead, mkGauss ,mkMedian;

  //read gray image
  fp = fopen(imgIN, "rb") ;
  if(fp==NULL){
    printf("ERROR: FAILED TO READ THE FILE.\n") ;
    return -1 ;
  }
  //read header info
  bmp = readHdr(fp, hdr) ;
  brightness = (unsigned char **)malloc(bmp.width * sizeof(unsigned char *)) ;
  for(i=0; i<bmp.width; i++){
    brightness[i] = (unsigned char *)malloc(bmp.height * sizeof(unsigned char));
  }
  //read bitmap data
  imgRead = readGrayData(bmp.width, bmp.height, fp, brightness) ;
  printf("Finished reading\n") ;
  //apply gaussian filiter
  mkGauss = gauss(brightness, hdr, imgGaussOUT, bmp.width, bmp.height) ;
  //apply median filiter
  mkMedian = median(brightness, hdr, imgMedianOUT, bmp.width, bmp.height) ;

  free(brightness) ;

  printf("Finished gaussian filtering\n") ;
  return 0 ;
}
