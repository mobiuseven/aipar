#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "imgreader.h"

unsigned char enclose(unsigned char **brightness, double **ssd, double minSsd, int x, int y, int minX, int minY){
  int i, j ;
  unsigned char tmp ;

  if(x+1 == minX || x == minX ||  x-1 == minX || y-1 == minY || y == minY || y+1 == minY){
    tmp = 255 ;
  }else{
    tmp = brightness[x][y] ;
  }

  return tmp ;
}

double calcSsd(unsigned char **brightness, image bmp, unsigned char **brightnessTmp, image bmpTmp, int x, int y){
  int i, j ;
  int height = bmp.height, width = bmp.width ;
  int heightTmp = bmpTmp.height, widthTmp = bmpTmp.width ;
  double ssd = 0;
  unsigned char **tmp ;
  
  tmp = (unsigned char **)malloc(width * sizeof(unsigned char *)) ;
  if(tmp == NULL){
    printf("MALLOC FAILED\n") ;
    return -1 ;
  }
  for(i=0; i<width; i++){
    tmp[i] = (unsigned char *)malloc(height * sizeof(unsigned char)) ;
    if(tmp[i] == NULL){
      printf("MALLOC FAILED\n") ;
      return -1 ;
    }
  }

  //clone original image
  for(j=0; j<height; j++){
    for(i=0; i<width; i++){
      tmp[i][j] = brightness[i][j] ;
    }
  }
  //calculate ssd
  for(j=0; j<heightTmp; j++){
    for(i=0; i<widthTmp; i++){
      if(i+x >= width || j+y >= height){
        ssd += pow(0 - brightnessTmp[i][j], 2.0) ;
      } else{
        ssd += pow(tmp[i+x][j+y] - brightnessTmp[i][j], 2.0) ;
      }
    }
  }
  
  free(tmp) ;
  return ssd ;
}

int tmpMatch(unsigned char hdr[], unsigned char **brightness, image gray, unsigned char **brightnessTmp, image tmp, char fname[] ){
  FILE *fp ;
  int height = gray.height, width = gray.width ;
  int heightTmp = tmp.height, widthTmp = tmp.width ;
  int imgSize = width * height *3 , imgSizeTmp = widthTmp * heightTmp * 3 ;
  int x, y, minX, minY ;
  int i ;
  unsigned char *data;
  double **ssd;
  double minSsd = -DBL_MAX ;

  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char )) ;
  ssd = (double **)malloc(width * sizeof(double *)) ;
  if(ssd == NULL || data == NULL){
    printf("MALLOC FAILED\n") ;
    return -1 ;
  } 
  for(i=0; i<width; i++){
    ssd[i] = (double *)malloc(height * sizeof(double)) ;
    if(ssd[i] == NULL){
      printf("MALLOC FAILED\n") ;
      return -1 ;
    }
  }

  //calculate ssd
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      ssd[x][y] = calcSsd(brightness, gray, brightnessTmp, tmp, x, y) ;
      //printf("%f, %d, %d\n", ssd[x][y], x, y) ;
      if(ssd[x][y]<minSsd){
        minSsd = ssd[x][y] ;
        minX = x ;
        minY = y ;
      }
    }
  }
  printf("SSD Calculation Finished!\n") ;

  i = 0 ;
  //enclose
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      //b
      data[i] = enclose(brightness, ssd, minSsd, x, y, minX, minY);
      //g
      data[i+1] = enclose(brightness, ssd, minSsd, x, y, minX, minY) ;
      //r
      data[i+2] = enclose(brightness, ssd, minSsd, x, y, minX, minY) ;
      i += 3 ;
    }
  }
  
  //open targert file
  fp = fopen(fname, "wb") ;
  //if failed to open the file
 if(fp==NULL){
    printf("ERROR: FAILED TO WRITE THE FILE.\n") ;
    return -1 ;
  }
  //write the header
  fwrite(hdr, 1, 54, fp) ;
  //write the new image data
  fwrite(data, 1, imgSize, fp) ;
  free(data) ;
  for(i=0; i<width; i++){
    free(ssd[i]) ;
  }
  free(ssd) ;
  fclose(fp) ;
  return 0 ;
}



int main(void){
  char imgIN[] = "image_1_gray.bmp" ;
  char imgINTmp[] = "image_1_tmp.bmp" ;
  char imgOUT[] = "image_1_tmpmatch.bmp" ;
  unsigned char hdr[54], hdrTmp[54] ;
  unsigned char **brightness, **brightnessTmp ;
  image bmp, bmpTmp ;
  FILE *fp, *fpTmp ;
  int i = 0 ;
  
  //open the gray image and tmplate image
  fp = fopen(imgIN, "rb") ;
  fpTmp = fopen(imgINTmp, "rb") ;
  if(fp==NULL || fpTmp==NULL){
    if(fp = NULL){
      printf("ERROR: FAILED TO READ %s\n", imgIN) ;
    }else{
      printf("ERROR: FAILED TO READ %s.\n", imgINTmp) ;
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
  bmpTmp = readHdr(fpTmp, hdrTmp) ;
  brightnessTmp = (unsigned char **)malloc(bmpTmp.width * sizeof(unsigned char *)) ;
  if(brightnessTmp == NULL){
    printf("MALLOC FAILED\n") ;
    return -1 ;
  }
  for(i=0; i<bmpTmp.width; i++){
    brightnessTmp[i] = (unsigned char *)malloc(bmpTmp.height * sizeof(unsigned char));
    if(brightnessTmp[i] == NULL){
      printf("MALLOC FAILED\n") ;
      return -1 ;
    }
  }
  //get image data
  readGrayData(bmpTmp.width, bmpTmp.height, fpTmp, brightnessTmp) ;
  //close gray image
  fclose(fpTmp) ;
  printf("Reading Finished Template Image\n") ;

  //apply template matching
  tmpMatch(hdr, brightness, bmp, brightnessTmp, bmpTmp, imgOUT) ;
  printf("Tmplate Matching Finished!!\n") ;
  
  //free memory
  for(i=0; i<bmp.width; i++){
    free(brightness[i]) ;
  }
  free(brightness) ;
  for(i=0; i<bmpTmp.width; i++){
    free(brightnessTmp[i]);
  }
  free(brightnessTmp) ;

  printf("Finished\n") ;
  
  return 0 ;
}
