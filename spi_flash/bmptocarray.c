#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma pack(1)
 
struct BitMap
{
   unsigned short int Type;
   unsigned int Size;
   unsigned short int Reserved1, Reserved2;
   unsigned int Offset;
} Header; 
 
struct BitMapInfo
{
   unsigned int Size;
   int Width, Height;
   unsigned short int Planes;
   unsigned short int Bits;
   unsigned int Compression;
   unsigned int ImageSize;
   int xResolution, yResolution;
   unsigned int Colors;
   unsigned int ImportantColors;
} InfoHeader;
 
struct Pixels
{
   unsigned char Blue,Green,Red;
};
 
int main(int agrc, char *argv[]) {
 
   int i=0, j=0;
   int size_spix; // size of struct Pixel
   int padding = 0;
   char temp[4];
 
   FILE *BMP_in = fopen (argv[1], "rb");
   if (BMP_in == NULL) {
      printf("\nCannot open file\n");
      exit(1);
   }
 
 
 
   printf("\nSize of header: %d", sizeof(Header));
   printf("\nSize of type: %d", sizeof(Header.Type));
   printf("\nSize of size: %d", sizeof(Header.Size));
   printf("\nSize of reserve1: %d", sizeof(Header.Reserved1));
   printf("\nSize of reserve2: %d", sizeof(Header.Reserved2));
   printf("\nSize of Offset: %d\n", sizeof(Header.Offset));
    
   printf("\nSize of info header: %d\n\n", sizeof(InfoHeader));
 
    
   fread(&Header, sizeof(Header), 1, BMP_in);
   fread(&InfoHeader, sizeof(InfoHeader), 1, BMP_in);
    
   printf("\nType: %hd", Header.Type);
   printf("\nSize: %d", Header.Size);
   printf("\nReserved 1: %hd", Header.Reserved1);
   printf("\nReserved 2: %hd", Header.Reserved2);
   printf("\nOffset: %d", Header.Offset);
   printf("\n\nSize: %d", InfoHeader.Size);
   printf("\nWidth: %d", InfoHeader.Width);
   printf("\nHeight: %d", InfoHeader.Height);
   printf("\nPlanes: %hd", InfoHeader.Planes);
   printf("\nBits per pixel: %d", InfoHeader.Bits);
   printf("\nCompression: %d", InfoHeader.Compression);
   printf("\nImageSize: %d", InfoHeader.ImageSize);
   printf("\nxResolution: %d", InfoHeader.xResolution);
   printf("\nyResolution: %d", InfoHeader.yResolution);
   printf("\nColors: %d", InfoHeader.Colors);
   printf("\nImportantColors: %d", InfoHeader.ImportantColors);
   printf("\n");
 
 
 
   padding = InfoHeader.Width % 4;
   if(padding != 0 ) {
      padding = 4 - padding;
   }
    
   size_spix = sizeof(struct Pixels);
   struct Pixels **pixel_arrayp;
 
   printf("\nSize_spix: %d\n", size_spix);
    
   /*allocate memory*/
   pixel_arrayp = (struct Pixels **)calloc(InfoHeader.Height,sizeof(struct Pixel*));
 
   for(i=0;i<InfoHeader.Height; i++) {
      pixel_arrayp[i] = (struct Pixels *)calloc(InfoHeader.Width,size_spix);
   }
    
   printf("\nReading Pixels Info...\n");
   for(i=0; i < InfoHeader.Height; i++) {
      for(j=0; j < InfoHeader.Width; j++) {
          
         //printf("%d", fread(&pixel_arrayp[i][j],size_spix ,1,BMP_in)); //return 1
         //printf("%d", fread(&pixel_arrayp[i][j], 1, size_spix, BMP_in)); //return 3
         fread(&pixel_arrayp[i][j], 3,1,  BMP_in);
          
         printf("y=%d  x=%d   ", i+1, j+1);
         printf("[%d][%d][%d]\n",pixel_arrayp[i][j].Red, pixel_arrayp[i][j].Green,pixel_arrayp[i][j].Blue);      
      }
      if(padding != 0) {
         printf("Padding: %d\n", padding);
         fread(&temp, padding, 1,  BMP_in);            
      }
   }
 
 
   fclose(BMP_in);
 
   FILE *BMP_out = fopen(argv[2], "wb");
   if (BMP_out == NULL) {
      printf("\nCannot open file\n");
      exit(1);
   }

#if 0
   printf("\nWriting Header...\n");
   //save to new bmp file
   fwrite(&Header, sizeof(Header), 1,  BMP_out);
   fwrite(&InfoHeader, sizeof(InfoHeader), 1, BMP_out);
    
   printf("\nWriting Pixels...\n");
   for(i=0; i < InfoHeader.Height ; i++) {
      for(j=0; j < InfoHeader.Width; j++) {   
         fwrite(&pixel_arrayp[i][j], size_spix, 1, BMP_out);
      }
      if(padding != 0) {
         //printf("Padding: %d\n", padding);
         fwrite(&temp, padding, 1,BMP_out);
      }
   }
#else
   printf("\nWriting Pixels...\n");
   fwrite("unsigned char picture[] = {\n\r", 1, sizeof("unsigned char picture[] = {\n\r"), BMP_out);
	for(i=0; i < InfoHeader.Height ; i++)
	{
		for(j=0; j < InfoHeader.Width; j++)
		{   
			char buff[256];
			sprintf(buff, "0x%x, 0x%x, 0x%x, ", pixel_arrayp[i][j].Red, pixel_arrayp[i][j].Green, pixel_arrayp[i][j].Blue);
			fwrite(buff, 1, strlen(buff), BMP_out);
		}
	}
	fwrite("\n\r};\n\r", 1, sizeof("\n\r};\n\r"), BMP_out);
#endif
   printf("\nDone writing...\n");
 
  // for(i=0; i<InfoHeader.Height; i++) {
  //    free(pixel_arrayp[i]);
  // }
   //free(pixel_arrayp);
   //printf("\nFree...\n");
   fclose(BMP_out);
   
   return 0;
}