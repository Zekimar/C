#include <stdio.h>
//struct to represent tiff tag
typedef struct{
	unsigned short identifier;
	unsigned short datatype;
	int numOfItems;
	int value;
} TIFF;

//struct to represent header 
typedef struct{
	char exif[4];
	char endian[2];
	unsigned short count;
} HEADER;

//struct to represent data we need
typedef struct{
	char manufacturer[50];
	char model[50];
	int width;
	int height;
	int iso;
	unsigned int exposure1;
	unsigned int exposure2;
	unsigned int fstop1;
	unsigned int fstop2;
	unsigned int focal1;
	unsigned int focal2;
	char date[50];
}  DATA;

int main(int argc, char* argv[]){
	//if there was a command line argument, proceed with program
	if( argc == 2 ) {
		//get file from command line arg
		printf("The file opened is %s\n", argv[1]);
		FILE* f = fopen(argv[1], "rb");
		if(f == NULL){
			printf("that file doesn't exist!\n");
			return 1;
		}
		
		//initialize structures
		HEADER header = {"", "", 0};
		TIFF tiff = {0,0,0,0};
		DATA data = {"", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, ""};
		
		//print error message when exif string not present
		fseek(f, 6, SEEK_SET);
		fread(&header.exif, 4, 1, f);
		if (strcmp(header.exif, "Exif") != 0){
			printf("ERROR: Exif string not present.\n");
			return 1;
		}
		
		//print error message when endianness is MM
		fseek(f, 12, SEEK_SET);
		fread(&header.endian, 2, 1, f);
		if (strcmp(header.endian, "MM") == 0){
			printf("ERROR: MM format not supported.");
			return 1;
		}
		
		//get counting variable for number of tiff tags
		fseek(f, 20, SEEK_SET);
		fread(&header.count, 2, 1, f);
		fseek(f, 22, SEEK_SET);
		
		
		int i;
		int returnset;
		//loop through tiff tags
		for (i = 0; i < header.count; i++){
			//variable to go back to the tags when we read the values
			long pos = ftell(f);
			
			//read tiff tags
			fread(&tiff, 12, 1, f);
			
			//gather info from relevant tags
			if (tiff.identifier == 0x010F){
				tiff.value = tiff.value + 12;
				fseek(f, tiff.value, SEEK_SET);
				fread(&data.manufacturer, tiff.numOfItems, 1, f);
				pos = pos + 12;
				fseek(f, pos, SEEK_SET);
			}else if (tiff.identifier == 0x0110){
				tiff.value = tiff.value + 12;
				fseek(f, tiff.value, SEEK_SET);
				fread(&data.model, tiff.numOfItems, 1, f);
				pos = pos + 12;
				fseek(f, pos, SEEK_SET);
			}else if (tiff.identifier == 0x8769){
				tiff.value = tiff.value + 12;
				fseek(f, tiff.value, SEEK_SET);
				//get new count variable and end loop as per instructions
				fread(&header.count, 2, 1, f);
				break;
			}
		}
		//loop through second batch of tiff tags
		int j;
		for (j = 0; j < header.count; j++){
			long pos2 = ftell(f);
			
			fread(&tiff, 12, 1, f);
			
			if (tiff.identifier == 0xA002){
				data.width = tiff.value;
			}else if (tiff.identifier == 0xA003){
				data.height = tiff.value;
			}else if (tiff.identifier == 0x8827){
				data.iso = tiff.value;
			}else if (tiff.identifier == 0x829a){
				tiff.value = tiff.value + 12;
				fseek(f, tiff.value, SEEK_SET);
				fread(&data.exposure1, 4, 1, f);
				fread(&data.exposure2, 4, 1, f);
				pos2 = pos2 + 12;
				fseek(f, pos2, SEEK_SET);
			}else if (tiff.identifier == 0x829d){
				tiff.value = tiff.value + 12;
				fseek(f, tiff.value, SEEK_SET);
				fread(&data.fstop1, 4, 1, f);
				fread(&data.fstop2, 4, 1, f);
				pos2 = pos2 + 12;
				fseek(f, pos2, SEEK_SET);
			}else if (tiff.identifier == 0x920A){
				tiff.value = tiff.value + 12;
				fseek(f, tiff.value, SEEK_SET);
				fread(&data.focal1, 4, 1, f);
				fread(&data.focal2, 4, 1, f);
				pos2 = pos2 + 12;
				fseek(f, pos2, SEEK_SET);
			}else if (tiff.identifier == 0x9003){
				tiff.value = tiff.value + 12;
				fseek(f, tiff.value, SEEK_SET);
				fread(&data.date, tiff.numOfItems, 1, f);
				pos2 = pos2 + 12;
				fseek(f, pos2, SEEK_SET);
			}
			
			
		}
		//print results
		printf("Manufacturer: \t%s\n", data.manufacturer);
		printf("Model: \t%s\n", data.model);
		printf("Exposure Time: \t%d/%d second\n", data.exposure1, data.exposure2);
		printf("F-Stop: \t%d/%d\n", data.fstop1, data.fstop2);
		printf("ISO: \tISO %d\n", data.iso);
		printf("Date Taken: \t%s\n", data.date);
		printf("Focal Length: \t%d/%d mm\n", data.focal1, data.focal2);
		printf("Width: \t%d pixels\n", data.width);
		printf("Height: \t%d pixels\n", data.height);
		fclose(f);
	}
	else if( argc > 2 ) {
		printf("Too many arguments supplied.\n");
	}	
	else {
		printf("One argument expected.\n");
	}
	
	return 0;
}