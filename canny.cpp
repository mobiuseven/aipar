#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "imgreader.h"

double calcTan(int x, int y, int width, int height, unsigned char **brightness){
  int unit[3][3] ;
  int sobelX[3][3] = {{-1, 0, 1},
                      {-2, 0, 2},
                      {-1, 0, 1}} ;
  int sobelY[3][3] = {{-1, -2, -1,},
                      {0, 0, 0,}, 
                      {1, 2, 1}} ;
  int i, j, k ;
  int sumX = 0, sumY = 0 ;
  int x1, x2, y1, y2 ;
  double tan ;
  
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

  unit[0][0] = (int) brightness[x2][y2] ;
  unit[0][1] = (int) brightness[x][y2] ;
  unit[0][2] = (int) brightness[x1][y2] ;
  unit[1][0] = (int) brightness[x2][y] ;
  unit[1][1] = (int) brightness[x][y]  ;
  unit[1][2] = (int) brightness[x1][y] ;
  unit[2][0] = (int) brightness[x2][y1] ;
  unit[2][1] = (int) brightness[x][y1] ;
  unit[2][2] = (int) brightness[x2][y1] ;

  //sobel
  //apply filter for horizontal (sobelX)
  for(i=0; i<3; i++){
    for(j=0; j<3; j++){
      for(k=0; k<3; k++){
        sumX+= unit[i][k] * sobelX[k][j] ;
      }
    }
  }
  //apply filter for virtical (sobelY)
  for(i=0; i<3; i++){
    for(j=0; j<3; j++){
      for(k=0; k<3; k++){
        sumY += unit[i][k] * sobelY[k][j] ;
      }
    }
  }
  if(sumX){
    tan = sumY / sumX ;
  }else{        //if sumX = 0
    tan = 3 ;
  }

  return  tan ;
}

unsigned char sobelFilter(int x, int y, int width, int height, unsigned char **brightness){
  int unit[3][3] ;
  int sobelX[3][3] = {{-1, 0, 1},
                      {-2, 0, 2},
                      {-1, 0, 1}} ;
  int sobelY[3][3] = {{-1, -2, -1,},
                      {0, 0, 0,}, 
                      {1, 2, 1}} ;
  int i, j, k ;
  int sum = 0, sumX = 0, sumY = 0 ;
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

  unit[0][0] = (int) brightness[x2][y2] ;
  unit[0][1] = (int) brightness[x][y2] ;
  unit[0][2] = (int) brightness[x1][y2] ;
  unit[1][0] = (int) brightness[x2][y] ;
  unit[1][1] = (int) brightness[x][y]  ;
  unit[1][2] = (int) brightness[x1][y] ;
  unit[2][0] = (int) brightness[x2][y1] ;
  unit[2][1] = (int) brightness[x][y1] ;
  unit[2][2] = (int) brightness[x2][y1] ;

  //sobel
  //apply filter for horizontal (sobelX)
  for(i=0; i<3; i++){
    for(j=0; j<3; j++){
      for(k=0; k<3; k++){
        sumX+= unit[i][k] * sobelX[k][j] ;
      }
    }
  }
  //apply filter for virtical (sobelY)
  for(i=0; i<3; i++){
    for(j=0; j<3; j++){
      for(k=0; k<3; k++){
        sumY += unit[i][k] * sobelY[k][j] ;
      }
    }
  }
  sum = sqrt(pow(sumX, 2) + pow(sumY, 2)) ;
  if(sum < 0){
    sum = 0 ;
  }else if(sum > 255){
    sum = 255 ;
  }

  return  (unsigned char) sum;
}

unsigned char suppression(int x, int y, int width, int height, unsigned char **g, double **tan){
  int dir ;
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

  //define the dirction of the gradient
  if(tan[x][y] <= -2.4142){
    dir = 90 ;
  }else if(tan[x][y] <= -0.4142){
    dir = 135 ;
  }else if(tan[x][y] <= 0.4142){
    dir = 0 ;
  }else if(tan[x][y] < 2.4142){
    dir = 45 ;
  }else{
    dir = 90 ;
  }
  //supress the max
  switch(dir){
    case 0:
      if((g[x][y] < g[x1][y]) || (g[x][y] < g[x2][y])){
        g[x][y] = 0 ;
      }
      break ;
    case 45:
      if((g[x][y] < g[x1][y2]) || (g[x][y] < g[x2][y1])){
        g[x][y] = 0 ;
      }
      break ;
    case 90:
      if((g[x][y] < g[x][y1]) || g[x][y] < g[x][y2]){
        g[x][y] = 0 ;
      }
      break ;
    case 135:
      if((g[x][y] < g[x1][y1]) || (g[x][y] < g[x2][y2])){
        g[x][y] = 0 ;
      }
      break ;
    default:
      g[x][y] = g[x][y] ;
  }

  return g[x][y] ;
}

unsigned char hystresisThreshold(int x, int y, int width, int height, unsigned char **sg, unsigned char ht, unsigned char lt){
  int i, j ;
  int x1, x2, y1, y2 ;
  unsigned char tmp[8] ;
  unsigned char edge ;

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
  /*  for debug
  printf("x=%d, y=%d\n", x, y) ;
  for(j=0; j<3; j++){
    for(i=0; i<3; i++){
      printf("%4d", sg[i][j]) ;
    }
    //printf("\n") ;
  }
  printf("\n") ;
  */

  tmp[0] = sg[x2][y2] ;        //bottom right
  tmp[1] = sg[x][y2] ;         //bottom
  tmp[2] = sg[x1][y2] ;        //bottom left
  tmp[3] = sg[x2][y] ;         //left
  tmp[4] = sg[x1][y] ;         //right
  tmp[5] = sg[x2][y1] ;        //top left
  tmp[6] = sg[x][y1] ;         //top
  tmp[7] = sg[x1][y1] ;        //top right

  if(sg[x][y] <= lt){
    edge = 0 ;
  }else if(sg[x][y] <= ht){
    for(i=0; i<8; i++){
      if(tmp[i] > ht){
        edge = 255 ;
      }
    }
  }else{
    edge = 255 ;
  }
  
  return edge ;
}

int canny(unsigned char **brightness, unsigned char hdr[], char fname[], int width, int height){
  FILE *fp ;
  int imgSize = width * height *3 ;
  int x,  y;
  int i ;
  unsigned char *data;
  unsigned char **g, **sg;
  unsigned char lt = 50;
  unsigned char ht = 150 ;
  double **tan ;

  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char )) ;
  g = (unsigned char **)malloc(width * sizeof(unsigned char *)) ;
  sg = (unsigned char **)malloc(width * sizeof(unsigned char *)) ;
  tan = (double **)malloc(width * sizeof(double *)) ;
  for(i=0; i<width; i++){
    g[i] = (unsigned char *)malloc(height * sizeof(unsigned char)) ;
    sg[i] = (unsigned char *)malloc(height * sizeof(unsigned char)) ;
    tan[i] = (double *)malloc(height * sizeof(double)) ;
  }

  //apply sobel filter
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      g[x][y] = sobelFilter(x, y, width, height, brightness) ;
      tan[x][y] = calcTan(x, y, width, height, brightness) ;
      //printf("%d, %d\n", g[x][y], tan[x][y]) ;
    }
  }
  printf("Solbel Filter Finished!\n") ;
  //non-maximum supression
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      sg[x][y] = suppression(x, y, width, height, g, tan) ;
      //printf("%d %d %4d\n", x, y, sg[x][y]) ;     for debug
    }
  }
  printf("Supression Finished\n") ;
  for(i=0; i<width; i++){
    free(g[i]) ;
    free(tan[i]) ;
  }
  free(g) ;
  free(tan) ;
  i = 0 ;
  //hysteresis threshold
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      data[i] = hystresisThreshold(x, y, width, height, sg, ht, lt) ;
      data[i+1] = data[i] ;
      data[i+2] = data[i] ;
      i += 3 ;
    }
  }
  printf("Hystresis Finished\n") ;


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
  for(i=0; i<width; i++){
    free(sg[i]) ;
  }
  free(sg) ;
  free(data) ;
  fclose(fp) ;
  return 0 ;
}

unsigned char gaussFilter(int x, int y, int width, int height, unsigned char **brightness){
  unsigned char unit[3][3] ;
  unsigned char sum = 0 ;
  int x1, x2, y1, y2 ;
  int i, j, tmp ;

  //define other pixels
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

  //apply gaussian filiter
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

  //wirte the header
  fwrite(hdr, 1, 54, fp) ;

  //generate new image
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      data[i] = gaussFilter(x, y, width, height, brightness) ;
      data[i+1] = gaussFilter(x, y, width, height, brightness) ;
      data[i+2] = gaussFilter(x, y, width, height, brightness) ;
      i += 3 ;
    }
  }

  //wirte new image data
  fwrite(data, 1, imgSize, fp) ;
  free(data) ;
  fclose(fp) ;
  return 0 ;
}

int main(void){
  char imgIN[] = "image_1_gray.bmp" ;
  char imgOUTGauss[] = "image_1_gauss.bmp" ;
  char imgOUT[] = "image_1_canny.bmp" ;
  unsigned char hdr[54] ;
  unsigned char **brightness ;
  image bmp ;
  FILE *fp, *fp1 ;
  int i = 0 ;
  
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
  readGrayData(bmp.width, bmp.height, fp, brightness) ;
  //close gray image
  fclose(fp) ;

  printf("Reading Finished\n") ;
  //apply gaussian filiter
  gauss(brightness, hdr, imgOUTGauss, bmp.width, bmp.height) ;
  printf("Gaussian Filiter Finished!!\n") ;
  //free memory
  for(i=0; i<bmp.width; i++){
    free(brightness[i]) ;
  }
  free(brightness) ;
 
  fp1 = fopen(imgOUTGauss, "rb") ;
  if(fp1==NULL){
    printf("ERROR: FAILED TO READ THE FILE\n") ;
    return -1 ;
  }
  bmp = readHdr(fp1, hdr) ;
  brightness = (unsigned char **)malloc(bmp.width * sizeof(unsigned char *)) ;
  for(i=0; i<bmp.width; i++){
    brightness[i] = (unsigned char *)malloc(bmp.height * sizeof(unsigned char));
  }
  //get image data
  readGrayData(bmp.width, bmp.height, fp, brightness) ;
  //close gaussian image
  fclose(fp1) ;
  printf("Reading Finished!\n") ;
  //apply median filter
  canny(brightness, hdr, imgOUT, bmp.width, bmp.height) ;

  //free memory
  free(brightness) ;
  
  printf("Finished\n") ;
  
  return 0 ;
}
