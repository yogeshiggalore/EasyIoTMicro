#include <string>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <utils.h>
#include <config.h>

using namespace std;

CParameter::CParameter()
{
	this->m_sName = "";
	this->m_sValue = "";
}

CParameter::CParameter( string Name, string Value)
{
	this->m_sName = Name;
	this->m_sValue = Value;
}

CParameter::~CParameter()
{
	this->m_sName = "";
	this->m_sValue = "";
}

string CParameter::GetName( void)
{
	return this->m_sName;
}

void CParameter::SetName( string Value)
{
	this->m_sName = Value;
}

string CParameter::GetValue( void)
{
	return this->m_sValue;
}
void CParameter::SetValue( string Value)
{
	this->m_sValue = Value;
}

CConfig::CConfig()
{
	this->m_sName = "";
	this->m_iSettingsCount = 0;
	this->m_iParameterCount = 0;
}

CConfig::CConfig( string Name)
{
	this->m_sName = Name;
	this->m_iSettingsCount = 0;
	this->m_iParameterCount = 0;
}

string CConfig::GetName()
{
	return this->m_sName;
}

int CConfig::AddParameter( string Name, string Value)
{
	if ( this->m_iParameterCount >= MAX_PARAMETERS_PER_SETTING )
	{
		return -1;
	}

	this->m_oParameters[ this->m_iParameterCount].SetName( Name);

	this->m_oParameters[ this->m_iParameterCount].SetValue( Value);

	this->m_iParameterCount++;

	return this->m_iParameterCount;
}

int CConfig::AddParameter( CParameter Value)
{
	if ( this->m_iParameterCount >= MAX_PARAMETERS_PER_SETTING )
	{
		return -1;
	}

	this->m_oParameters[ this->m_iParameterCount] = Value;

	this->m_iParameterCount++;

	return this->m_iParameterCount;
}

int CConfig::AddConfig( CConfig* Value)
{
	if ( this->m_iSettingsCount >= MAX_SETTINGS_PER_SETTING )
	{
		return -1;
	}

	this->m_oSettings[ this->m_iSettingsCount] = Value;

	this->m_iSettingsCount++;

	return this->m_iSettingsCount;
}

CParameter CConfig::GetParam( int Index)
{
	CParameter l_oParam;

	if ( Index > 0 && Index <= this->m_iParameterCount )
	{
		l_oParam = this->m_oParameters[Index - 1];
	}

	return l_oParam;
}

CParameter CConfig::GetParam( string Name)
{
	CParameter l_oParam;

	for( int l_iIndex = 0; l_iIndex < this->m_iParameterCount; l_iIndex++)
	{
		if ( this->m_oParameters[l_iIndex].GetName() == Name )
		{
			l_oParam = this->m_oParameters[l_iIndex];

			break;
		}
	}
	return l_oParam;
}

CConfig* CConfig::GetConfig( int Index)
{
	CConfig* l_oConfig = NULL;

	if ( Index > 0 && Index <= this->m_iSettingsCount )
	{
		l_oConfig = this->m_oSettings[Index - 1];
	}

	return l_oConfig;
}

CConfig* CConfig::GetConfig( string Name)
{
	CConfig* l_oConfig = NULL;

	for( int l_iIndex = 0; l_iIndex < this->m_iSettingsCount; l_iIndex++)
	{
		if ( this->m_oSettings[l_iIndex]->GetName() == Name )
		{
			l_oConfig = this->m_oSettings[l_iIndex];

			break;
		}
	}
	return l_oConfig;
}

bool CConfig::ReadConfigFile( string FileName, string& Error)
{
	fstream l_oFile;
	string l_sBuff = "";
	string l_sVarName = "";
	string l_sVarValue = "";
	int l_iLineCount = 0;

	char l_cVarName[1024];
	char l_cValue[4096];
	int l_iCurrentConfig = 0;
	CConfig*  l_pconfig[24];

	try
	{
		l_pconfig[l_iCurrentConfig] = this;

		l_oFile.open( FileName.c_str(), fstream::in);

		if ( ! l_oFile )
		{
			cout<<"Error reading configuration file"<<endl;

			return false;
		}

		while( getline( l_oFile, l_sBuff) )
		{
			++l_iLineCount;
			l_sBuff = Trim( l_sBuff);

			if ( l_sBuff.length() == 0 ) continue;
			if ( l_sBuff.substr( 0, 1) == "#" ) continue;

			if ( l_sBuff == "}" )
			{
				if ( l_iCurrentConfig == 0 )
				{
					cout<<"Error in Configuration file. Line NO: "<<l_iLineCount<<endl;
					break;
				}
				else
				{
					l_iCurrentConfig--;
				}
			}
			else
			{
				memset( l_cVarName, 0, 1024);
				memset( l_cValue, 0, 4096);

				if ( sscanf( l_sBuff.c_str(), "%[^=\n]=%[^\t\n]", l_cVarName, l_cValue) < 1 )
				{
					cout<<"Invalid Data Format in Configuration File. Line NO: "<<l_iLineCount<<endl;
				}
				else
				{
					l_sVarName = Trim( l_cVarName);
					l_sVarValue = Trim( l_cValue);

					if ( strcmp( l_sVarValue.c_str(), "{") == 0 )
					{
						l_pconfig[l_iCurrentConfig + 1] = new CConfig( l_sVarName);
						l_pconfig[l_iCurrentConfig]->AddConfig( l_pconfig[l_iCurrentConfig + 1]);
						l_iCurrentConfig++;
					}
					else
					{
						l_pconfig[l_iCurrentConfig]->AddParameter( l_sVarName, l_sVarValue);
					}
				}
			}
		}
		return true;
	}
	catch(...)
	{
		cout<<"Error reading configuration file."<<endl;
		return false;
	}
}


