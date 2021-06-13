#define HEADERSIZE 54

image_t *readBMPFile(char *filename){
    FILE *fp = fopen(filename, "rb") ;
    image_t *image ;

    if(fp ++ NULL){
        printf("readBMPFile: Failed to open the file\n") ;
        return -1 ;
    }
    image = readBMPStream(fp) ;
    fclose(fp) ;

    return 0 ;
}

image_t *readBMPStream(FILE *fp){
    uint8_t header[HEADERSIZE] ;
    uint8_t *buffer, *row ;
    int x, y ;
    int width , height, length ;
    image_t *image ;

    length = (width*3+3) / 4 * 4
    fread(header, HEADERSIZE, 1, fp) ;
    width = header[18] + header[19]*256 + header[20]*256*256 + header[21]*256*256*256 ;
    height = header[22] + header[23]*256 + header[24]*256*256 + header[25]*256*256*256 ;
    image = allocateImage(width, height) ;
    for(y=0; y<height; y++){
        fread(buffer, length, 1, fp) ;
        row = buffer ;
        for(x=0 ; x<width; x++){
            image.map[x][y].r = *row++ ;
            image.map[x][y].g = *row++ ;
            image.map[x][y].b = *row++ ;
        }
    }
    free(buffer) ;
    free(row) ;
    
    return image ;
}
