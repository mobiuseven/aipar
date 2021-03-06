#include <stdio.h>
#include <stdlib.h>
#include "imgreader.h"


unsigned char search(int x, int y, int width, int height, unsigned char **brightness, unsigned char *lableNo){
  int i, j, count = 1;
  int i1, i2, j1, j2 ;
  
  
  brightness[y*width+x] = lableNo ;
  //classify
  while(true){
    for(i=0; i<height; i++){
      for(j=0; j<width; j++){
        
        //define other pixels
	      i1 = x + 1 ;
	      i2 = x - 1 ;
	      j1 = y + 1 ;
	      j2 = y - 1 ;
	
	      //exceptions
	      //left edge
	      if(i1 > width-1){
	        i1 = x ;
	      }
	      //right edge
	      if(i2 < 0){
	        i2 = x ;
      	}
	      //top
	      if(j1 > height){
	        j1 = y ;
	      }
	      //bottom
	      if(j2 < 0){
	        j2 = y ;
	      }
	
	      //if the pixel is already labled
	      if(brightness[i*width+j] == lableNo) {
	        //check right
	        if(brightness[i*width+j+1]){
	          brightness[i*width+j1] = lableNo ;
	          count++ ;
	        }  
	        //check top right
	        if(brightness[i2*width+j1]){
	          brightness[i2*width+j+1] = lableNo ;
	          count++ ;
	        }
	        //check top
	        if(brightness[i2*width+j]){
	          brightness[i2*width+j] = lableNo ;
	          count++ ;
	        }
	        //check top left
	        if(brightness[i2*width+j2]){
	          brightness[i2*width+j2] = lableNo ;
	          count++ ; 
	        }
	        //check left
	        if(brightness[i*width+j2]){
	          brightness[i*width+j2] = lableNo ;
	          count++ ;
	        }
	        //check bottom left
	        if(brightness[i1*width+j2]){
	          brightness[i1*width+j2] = lableNo ;
	          count++ ;
	        }
	        //check bottom
	        if(brightness[i1*width]+j){
	          brightness[i1*width+j] = lableNo ;
	          count++ ;
	        }
	        //check bottom right
	        if(brightness[i1*width+j1]){
	          brightness[i1*width+j1] = lableNo ;
	          count++ ;
	        }
        }     
      }
    }
    if(!lableNo) break ;
  }
}


int lable(unsigned char **brightness, unsigned char hdr[], char fname[], int width, int height){
  FILE *fp ;
  int imgSize = width * height *3 ;
  int x = 0,  y = 0 ;
  int i, j ;
  unsigned char *data ;
  unsigned char lableNo = 1;
  unsigned char *lableNo2 ;
  
  lableNo2 = &lableNo ;
  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char)) ;
  
  //copy image
  /*
    for(y=0; y<height; y++){
    for(x=0; x<width; x++){
    data[y*width+x] = brightness[y*width+x] ;
    }
    }
  */
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
      if(brightness[y*width+x]){
        if(lableNo >= 255){
          printf("Too many lables (over 255)\n") ;
          return -1 ;
        }
      }
      data[y*width+x] = search(x, y, width, height, brightness, lableNo2) ;
      lableNo++ ;
      lableNo2 = &lableNo ;
    }
  }
  
  //write the new image data
  fwrite(data, 1, imgSize, fp) ;
  free(data) ;
  return 0 ;
}


int main(void){
  char imgIN[] = "image_1_binary.bmp" ;
  char imgOUT[] = "image_1_lable.bmp" ;
  unsigned char hdr[54] ;
  unsigned char **brightness ;
  image bmp ;
  FILE *fp ;
  int i = 0 ;
  int imgRead, mkLable ;
  
  //open gray image
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
  
  //apply gaussian filiter
  mkLable = lable(brightness, hdr, imgOUT, bmp.width, bmp.height) ;
  
  free(brightness) ;
  printf("Finished gaussian filtering\n") ;
  return 0 ;
}
