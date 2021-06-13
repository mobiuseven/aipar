typedef struct{
  int width ;
  int height ;
} image;

typedef struct{
  unsigned char r ;
  unsigned char g ;
  unsigned char b ;
} pixels;


//function to get header info from image
image readHdr(FILE *fp, unsigned char hdr[]){
  image img ;

  //read header
  fread(hdr, 1, 54, fp) ;
  //calculate the width of image
  img.width = hdr[18] + hdr[19]*256 + hdr[20]*256*256 + hdr[21]*256*256*256 ;
  //calculate the height of image
  img.height = hdr[22] + hdr[23]*256 + hdr[24]*256*256 + hdr[25]*256*256*256 ;
  return img ;
}


//function to read color image
int readData(int width, int height, FILE *fp, pixels **bmpPixels){
  int imgSize = width * height * 3 ;
  int i = 0 ;
  int x = 0 , y = 0 ;
  unsigned char *data ;

  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char)) ;

  fread(data, 1, imgSize, fp) ;
  for(y=0; y<height; y++){
    for(x=0; x<width; x++) {

      //blue
      bmpPixels[x][y].b = data[i] ;
      i++ ;
      //green
      bmpPixels[x][y].g = data[i] ;
      i++ ;
      //red
      bmpPixels[x][y].r = data[i] ;
      i++ ;
    }
  }
  free(data) ;
  return 0 ;
}


//function to read gray image
int readGrayData(int width, int height, FILE *fp, unsigned char **brightness){
  int imgSize = width * height * 3 ;
  long int i = 0 ;
  int x = 0, y = 0 ;
  unsigned char *data ;

  data = (unsigned char *)malloc(imgSize * sizeof(unsigned char)) ;
  
  //read image data
  fread(data, 1, imgSize, fp) ;

  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      brightness[x][y] = data[i] ;
      i +=3 ;
    }
  }
  free(data) ;
  return 0 ;
}


//function to write image
/*
int writeImage(FILE *fp, usigned char hdr[], usigned char data, int imgSize){
  
  //write header
  fwrite(fp, 1, 54, hdr) ;
  //write image data
  fwrite(data, 1, imgSize, fp) ;

  return 0 ;
}
*/