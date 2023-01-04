#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include<string>
#include<vector>
#include<defs.h>

using namespace std;

void Split( string strParameters, vector<string> *pTargetArray, char cSeparator);
string Trim( string Data, char Separator = ' ');

char *TrimRight(char *lcp_string);
char *Replace( char *str, char *find, char *rep, int count = 0);

void CharCopyRtrimInc(char** sourcePacketPtr, char* dstPacketPtr, uint8 dataLen);
void CharCopyInc(char** sourcePacketPtr, char* dstPacketPtr, uint8 dataLen);
void CharCopyInc(char** sourcePacketPtr, char* dstPacketPtr, uint8 dataLen, bool trimData);

uint32 HexToUint32Inc(char** source_ptr, uint8 data_len);
uint32 HexToUint32(char* source_ptr, uint8 data_len);
uint8 HexstringToHex(uint8 source);

string ToString( int);
string ToString( uint);
string ToString( long);
string ToString( ulong);
string ToString( double);
string ToString( double, int Preceision);

int ToInt( string);
uint ToUint( string);
long ToLong( string);
ulong ToUlong( string);
double ToDouble( string);
double ToDouble( string, int Precision);

double RoundOff( double x, int numDecimals);


