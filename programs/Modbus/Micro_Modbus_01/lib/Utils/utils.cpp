
#define _XOPEN_SOURCE

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<iostream>
#include<utils.h>

using namespace std;


void CharCopyRtrimInc(char** sourcePacketPtr, char* dstPacketPtr, uint8 dataLen)
{
	CharCopyInc(sourcePacketPtr, dstPacketPtr, dataLen, true);
}

void CharCopyInc(char** sourcePacketPtr, char* dstPacketPtr, uint8 dataLen)
{
	CharCopyInc(sourcePacketPtr, dstPacketPtr, dataLen, false);
}


void CharCopyInc(char** sourcePacketPtr, char* dstPacketPtr, uint8 dataLen, bool trimData)
{
	uint8 bLoopIndex;
	char* src = *sourcePacketPtr;

	for (bLoopIndex = 0; bLoopIndex < dataLen; bLoopIndex++) {
		if (src[bLoopIndex] == '~') {
			dstPacketPtr[bLoopIndex] = 0;
		}
		else {
			dstPacketPtr[bLoopIndex] = src[bLoopIndex];
		}
	}

	if (trimData) {
		for (bLoopIndex = dataLen - 1; bLoopIndex > 0; bLoopIndex--) {
			if (dstPacketPtr[bLoopIndex] != ' ') {
				break;
			}
			dstPacketPtr[bLoopIndex] = '\0';
		}
	}

	//Null termination
	dstPacketPtr[dataLen] = 0;

	*sourcePacketPtr += dataLen;
}

uint32 HexToUint32Inc(char** source_ptr, uint8 data_len)
{
	uint32 result = 0x00;

	result = HexToUint32(*source_ptr, data_len);

	*source_ptr += data_len;

	return result;
}

uint32 HexToUint32(char* source_ptr, uint8 data_len)
{
	uint32 result = 0x00;
	uint8* dst = (uint8*)&result + ((data_len / 2) - 1); // point to LSB

	uint8 buff = 0x00;
	uint8 loopIndex = 0;

	for (loopIndex = 0; loopIndex < data_len; loopIndex += 2) {
		buff = 0x00;
		buff = HexstringToHex(source_ptr[loopIndex + 0]) << 4;
		buff |= HexstringToHex(source_ptr[loopIndex + 1]);

		*dst = buff;
		dst--;
	}

	return result;
}
 
uint8 HexstringToHex(uint8 source)
{
	if ((source > 0x40) && (source < 0x47)) {
		return 0x0A + source - 0x41;
	}
	else if ((source > 0x60) && (source < 0x67)) {
		return 0x0A + source - 0x61;
	}
	else if ((source > 0x30) && (source < 0x3A)) {
		return source - 0x30;
	}
	return 0;
}

double RoundOff( double x, int numDecimals);

void Split( string strParameters, vector<string> *pTargetArray, char cSeparator) {
	bool bEnd = false;
	string strPair;
	int ipos1 = 0,ipos2 = 0;

	while ( !bEnd ) {
		if ( (ipos2 = strParameters.find( cSeparator,ipos1)) != string::npos ) {
			strPair.assign( strParameters, ipos1, ipos2 - ipos1);
		} else {
			strPair.assign(strParameters,ipos1,strParameters.length() - ipos1);
			bEnd = true;
		}
		if ( !strPair.empty() ) {
			pTargetArray->push_back( strPair);
		}
		ipos1 = ipos2 + 1;
	}
}

char *TrimRight(char *lcp_string) {
	char lcp_tr[1024]={0x00};
	char *lcp_trim;
	memset( lcp_tr, 0x00, 1024);
	strcpy( lcp_tr, lcp_string);
	int li_count = strlen( lcp_string) - 1;
	while( lcp_tr[li_count] == ' '  ) li_count--;
	lcp_tr[li_count+1] = '\0';
	lcp_trim = lcp_tr;

	return lcp_trim;
}

string Trim( string Data, char Separator)
{
	char l_pTrim[ Data.length() + 1];

	memset( l_pTrim, 0x00, Data.length() + 1);

	int l_iIndex = 0;

	while( Data.c_str()[l_iIndex] == Separator ) l_iIndex++;

	if ( l_iIndex < Data.length() )
	{
		strcpy( l_pTrim, Data.c_str() + l_iIndex);
	}

	l_iIndex = strlen( l_pTrim) - 1;

	while( l_iIndex >= 0 && l_pTrim[l_iIndex] == Separator )
	{
		l_pTrim[l_iIndex] = '\0';
		l_iIndex--;
	}
	
	return l_pTrim;
}

char *Replace( char *str, char *find, char *rep, int count) {
	if ( count > 20 ) return 0;

	char tmp[1024] = {0};

	int il = strlen( find);
	char *fl = strstr( str, find);
	if ( fl == NULL ) {
		return 0;
	} else {
		Replace( fl + il, find, rep, count++);
	}
	strncpy( tmp, str, fl-str);
	strcat( tmp, rep);
	strcat( tmp, fl+il);

	strcpy( str, tmp);
	return str;
}

string ToString( int Num) {
	char l_cBuff[32];

	sprintf( l_cBuff, "%d", Num);

	return l_cBuff;
}

string ToString( uint Num) {
	char l_cBuff[32];

	sprintf( l_cBuff, "%u", Num);

	return l_cBuff;
}

string ToString( long Num) {
	char l_cBuff[32];

	sprintf( l_cBuff, "%ld", Num);

	return l_cBuff;
}

string ToString( ulong Num) {
	char l_cBuff[32];

	sprintf( l_cBuff, "%u", Num);

	return l_cBuff;
}

string ToString( double Num) {
	char l_cBuff[32];

	sprintf( l_cBuff, "%0.2f", Num);

	return l_cBuff;
}

string ToString( double Num, int Precision) {
	char l_cBuff[32];

	sprintf( l_cBuff, "%0.2f", Num);

	return l_cBuff;
}

int ToInt( string Value) {
	// cout<<"ToInt. Value:"<<Value.c_str()<<", Result:"<<atoi( Value.c_str())<<endl;
	return atoi( Value.c_str());
}

uint ToUint( string Value) {
	// cout<<"ToUInt. Value:"<<Value.c_str()<<", Result:"<<strtoul( Value.c_str(), NULL, 0)<<endl;
	return (uint)strtoul( Value.c_str(), NULL, 0);
}

long ToLong( string Value) {
	// cout<<"ToLong. Value:"<<Value.c_str()<<", Result:"<<strtol( Value.c_str(), NULL, 0)<<endl;
	return strtol( Value.c_str(), NULL, 0);
}

ulong ToUlong( string Value) {
	// cout<<"ToUlong. Value:"<<Value.c_str()<<", Result:"<<strtoul( Value.c_str(), NULL, 0)<<endl;
	return strtoul( Value.c_str(), NULL, 0);
}

double ToDouble( string Value) {
	double l_eBuff = strtod( Value.c_str(), NULL);
	return l_eBuff;
}

double ToDouble( string Value, int Precision) {
	double l_eBuff = strtod( Value.c_str(), NULL);
	return RoundOff( l_eBuff, 2);
}

double RoundOff( double x, int numDecimals)
{
	double l_eFinalResult = 0.0;
	char l_cBuff[32];

	sprintf( l_cBuff, "%.*f", numDecimals, x);

	sscanf( l_cBuff, "%lf", &l_eFinalResult);
	
	return l_eFinalResult;
}








