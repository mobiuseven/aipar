#define HEADER_SIZE 54 //DEAFULT HEADER SIZE

typedef struct{
  uint8_t r ;
  uint8_t g ;
  uint8_t b ;
} pixels_t ;


typedef struct{
  uint32_t width ;
  uint32_t height ;
  pixels_t **map ;
} image_t ;


image_t *allocateImage(unit32_t width, unit32_t height){
  int i ;
  image_t *image ;

  image.width = width ;
  image.height = height ;
  image.map = (pixels **)malloc(width * sizeof(pixels_t *)) ;
  for(i=0; i<height; i++){
    image.map[i] = (pixels *)malloc(height * sizeof(pixels_t)) ;
  }

  return image ;
}


void freeImage(image_t image){
  for(i=0; i<height; i++){
    free(image.map[i]) ;
  }
  free(image.map) ;
  free(image) ;
}

image_t *cloneImage(image_t image){
  int i ;
  image_t *newImage = allocateImage(image.width, image.height) ;
  for(i=0; i<image.height; i++){
    memcpy(newImage.map[i], image.map[i], image.height*sizeof(pixels_t)) ;
  }

  return newImage ;
}