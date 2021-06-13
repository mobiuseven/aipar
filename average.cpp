#include <stdio.h>
#include <stdlib.h>
#include "imgreader.h"

unsigned char averaging(int x, int x1, int x2, int y, int y1, int y2, int width, int height, unsigned char **brightness){
  unsigned char unit[3][3] ;
  unsigned char sum = 0, ave = 0 ;
  int i = 0, j = 0 ;

  x1 = x + 1 ;
  x2 = x - 1 ;
  y1 = y + 1 ;
  y2 = y - 1 ;

  if(x1 > width-1){
    x1 = x ;
  }
  if(x2 < 0){
    x2 = x ;
  }
  if(y1 > height){
    y1 = y ;
  }
  if(y2 < 0){
    y2 = y ;
  }

  unit[0][0] = brightness[x2][y2] ;
  unit[1][0] = brightness[x][y2] ;
  unit[2][0] = brightness[x1][y2] ;
  unit[0][1] = brightness[x2][y] ;
  unit[1][1] = brightness[x][y] ;
  unit[1][2] = brightness[x1][y] ;
  unit[2][0] = brightness[x2][y1] ;
  unit[2][1] = brightness[x][y1] ;
  unit[2][2] = brightness[x2][y1] ;

  for(i=0; i<3; i++){
    for(j=0; j<3; j++){
      sum += unit[i][j]/9;
    }
  }
  // printf("%d\n", sum) ;
  //ave = sum / 9 ;
  return sum ;
}

int average(unsigned char **brightness, unsigned char hdr[], char fname[], int width, int height){
  FILE *fp ;
  int imgSize = width * height *3 ;
  int x = 0, x1 = 0, x2 = 0, y = 0,y1 = 0, y2 = 0 ;
  int i = 0 ;
  unsigned char *data ;

  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char)) ;
  fp = fopen(fname, "wb") ;
  if(fp==NULL){
    printf("ERROR: FAILED TO OPEN THE FILE.\n") ;
    return -1 ;
  }
  fwrite(hdr, 1, 54, fp) ;
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      data[i] = averaging(x, x1, x2, y, y1, y2, width, height, brightness) ;
      data[i+1] = averaging(x, x1, x2, y, y1, y2, width, height, brightness) ;
      data[i+2] = averaging(x, x1, x2, y, y1, y2, width, height, brightness) ;
      i += 3 ;
    }
  }
  fwrite(data, 1, imgSize, fp) ;
  free(data) ;
  return 0 ;
}


int main(void){
  char imgIN[] = "image_1_gray.bmp" ;
  char imgOUT[] = "image_1_ave.bmp" ;
  unsigned char hdr[54] ;
  unsigned char **brightness ;
  image bmp ;
  FILE *fp ;
  int i = 0 ;
  int imgRead, mkAve ;

  fp = fopen(imgIN, "rb") ;
  if(fp==NULL){
    printf("ERROR: FAILED TO OPEN THE FILE.\n") ;
    return -1 ;
  }
  bmp = readHdr(fp, hdr) ;
  brightness = (unsigned char **)malloc(bmp.width * sizeof(unsigned char *)) ;
  for(i=0; i<bmp.width; i++){
    brightness[i] = (unsigned char *)malloc(bmp.height * sizeof(unsigned char));
  }
  imgRead = readGrayData(bmp.width, bmp.height, fp, brightness) ;
  //fclose(fp) ;
  printf("Finished reading\n") ;
  mkAve = average(brightness, hdr, imgOUT, bmp.width, bmp.height) ;
  free(brightness) ;
  printf("Finished averaging\n") ;
  return 0 ;
}
