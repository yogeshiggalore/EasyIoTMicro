/*******************************************************************************
* Project Name:Micro_SD_02
*
* Version: 1.0
*
* Description:
* In this project esp32 uses sd card to read,write and store files 
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
*******************************************************************************/

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

/* sd card related functions */
void ListDirectory(fs::FS &fs, const char * dirname, uint8_t levels);
void CreateDirectory(fs::FS &fs, const char * path);
void RemoveDirectory(fs::FS &fs, const char * path);
void ReadFile(fs::FS &fs, const char * path);
void WriteFile(fs::FS &fs, const char * path, const char * message);
void AppendFile(fs::FS &fs, const char * path, const char * message);
void RenameFile(fs::FS &fs, const char * path1, const char * path2);
void DeleteFile(fs::FS &fs, const char * path);
void TestFileIO(fs::FS &fs, const char * path);
uint8_t ExistFile(fs::FS &fs, const char * path);

/* card type veriable */
uint8_t ui8CardType;

/* sd card size */
uint64_t ui64CardSize;

void setup(){
	/* start serial with baudrate 9600 */
	Serial.begin(9600);
	
	/* start sdcard  module if fails stop exceution */
	if(!SD.begin()){
		Serial.println("Card Mount Failed");
		while(1);
	}
	
	/* Get SD card Type */
	ui8CardType = SD.cardType();
	
	if(ui8CardType == CARD_NONE){
		Serial.println("No SD card attached");
		while(1);
	}else if(ui8CardType == CARD_MMC){
		Serial.println("SD Card Type: MMC");
	} else if(ui8CardType == CARD_SD){
		Serial.println("SD Card Type: SDSC");
	} else if(ui8CardType == CARD_SDHC){
		Serial.println("SD Card Type: SDHC");
	} else {
		Serial.println("SD Card Type: UNKNOWN");
	}
	
	/* get size of sd card */
	ui64CardSize = SD.cardSize() / (1024 * 1024);
	Serial.printf("SD Card Size: %lluMB\n", ui64CardSize);
 
	/* list all directory present in sd card */
	ListDirectory(SD, "/", 0);

	/* create directory "mydir" */
	CreateDirectory(SD, "/mydir");

	/* list all directory level 0*/
	ListDirectory(SD, "/", 0);

	/* remove directory "mydir" */
	RemoveDirectory(SD, "/mydir");

	/* list directory level 0*/
	ListDirectory(SD, "/", 2);
	
	/* write a file "hello.txt" with content "Hello" */
	WriteFile(SD, "/hello.txt", "Hello ");

	/* append to file "hello.txt" with content "World!\n" */
	AppendFile(SD, "/hello.txt", "World!\n");

	/* read a file "hello.txt" */
	ReadFile(SD, "/hello.txt");

	/* delete file "foo.txt" */
	DeleteFile(SD, "/foo.txt");
	
	/* rename file "hello.txt" to "foo.txt" */
	RenameFile(SD, "/hello.txt", "/foo.txt");

	/* read a file "foo.txt" */
	ReadFile(SD, "/foo.txt");
	
	/* test input output file of "test.txt" */
	TestFileIO(SD, "/test.txt");

	ExistFile(SD,"/foo.txt");
	ExistFile(SD,"/yogesh.txt");
}
 
void loop(){
 
}

/* function to check file exist */
uint8_t ExistFile(fs::FS &fs, const char * path){

	Serial.printf("Checking file %s presence? \n", path);

	if(fs.exists(path)){
		Serial.println("file exits");
		return true;
	}else{
		Serial.println("file not exits");
		return false;
	}
}

/* function for listing all directory present in sdcard */
void ListDirectory(fs::FS &fs, const char * dirname, uint8_t levels){
	Serial.printf("Listing directory: %s\n", dirname);
	
	/* open directory */
	File root = fs.open(dirname);

	/* if fails to open return */
	if(!root){
		Serial.println("Failed to open directory");
		return;
	}

	/* if given name is not directory then return */
	if(!root.isDirectory()){
		Serial.println("Not a directory");
		return;
	}
	
	/* open next file in directory mentioned */
	File file = root.openNextFile();
	while(file){
		/* check is directory */
		if(file.isDirectory()){
			Serial.print(" DIR : ");
			Serial.println(file.name());
			if(levels){
				/* repeat and read all files in that directory */
				ListDirectory(fs, file.name(), levels -1);
			}
		}else{
			/* display file name and file size */
			Serial.print(" FILE: ");
			Serial.print(file.name());
			Serial.print(" SIZE: ");
			Serial.println(file.size());
		}

		/* open next file in directory */
		file = root.openNextFile();
	}
}
 
/* function to create new directory */
void CreateDirectory(fs::FS &fs, const char * path){
	Serial.printf("Creating Directory: %s\n", path);
	/* create a directory in given path */
	if(fs.mkdir(path)){
		Serial.println("Directory created");
	}else {
		Serial.println("Directory create failed");
	}
}
 
/* function to remove directory */ 
void RemoveDirectory(fs::FS &fs, const char * path){
	
	Serial.printf("Removing Directory: %s\n", path);
	if(fs.rmdir(path)){
		Serial.println("Directory removed");
	}else{
		Serial.println("Remove directory failed");
	}
}
 
/* function to read file */ 
void ReadFile(fs::FS &fs, const char * path){
	Serial.printf("Reading file: %s\n", path);
	File file = fs.open(path);
	if(!file){
		Serial.println("Failed to open file for reading");
		return;
	}

	Serial.print("Read from file: ");
	while(file.available()){
		Serial.write(file.read());
	}
}

/* function to write file */ 
void WriteFile(fs::FS &fs, const char * path, const char * message){
	Serial.printf("Writing file: %s\n", path);

	File file = fs.open(path, FILE_WRITE);
	if(!file){
		Serial.println("Failed to open file for writing");
		return;
	}
	
	if(file.print(message)){
		Serial.println("File written");
	}else{
		Serial.println("Write failed");
	}
}

/* function to append data to file */ 
void AppendFile(fs::FS &fs, const char * path, const char * message){
	Serial.printf("Appending to file: %s\n", path);

	File file = fs.open(path, FILE_APPEND);
	if(!file){
		Serial.println("Failed to open file for appending");
		return;
	}

	if(file.print(message)){
		Serial.println("Message appended");
	}else{
		Serial.println("Append failed");
	}
}

/* function rename file */ 
void RenameFile(fs::FS &fs, const char * path1, const char * path2){
	Serial.printf("Renaming file %s to %s\n", path1, path2);
	if(fs.rename(path1, path2)) {
		Serial.println("File renamed");
	}else{
		Serial.println("Rename failed");
	}
}

/* function to delete file */ 
void DeleteFile(fs::FS &fs, const char * path){
	Serial.printf("Deleting file: %s\n", path);
	if(fs.remove(path)){
		Serial.println("File deleted");
	}else{
		Serial.println("Delete failed");
	}
}
 
/* function to test input and output file */ 
void TestFileIO(fs::FS &fs, const char * path){
	File file = fs.open(path);
	static uint8_t buf[512];
	size_t len = 0;
	uint32_t start = millis();
	uint32_t end = start;
	
	if(file){
		len = file.size();
		size_t flen = len;
		start = millis();
		while(len){
			size_t toRead = len;
			if(toRead > 512){
				toRead = 512;
			}

			file.read(buf, toRead);
			len -= toRead;
		}

		end = millis() - start;
		Serial.printf("%u bytes read for %u ms\n", flen, end);
		file.close();
	}else {
		Serial.println("Failed to open file for reading");
	}
 
	file = fs.open(path, FILE_WRITE);
	if(!file){
		Serial.println("Failed to open file for writing");
		return;
	}
 
	size_t i;
	start = millis();
	for(i=0; i<2048; i++){
		file.write(buf, 512);
	}
	end = millis() - start;
	Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
	file.close();
}

