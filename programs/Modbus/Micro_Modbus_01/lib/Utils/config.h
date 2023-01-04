#ifndef _CONFIG_H
#define _CONFIG_H

#include <iostream>
#include <string>

using namespace std;

const int MAX_PARAMETERS_PER_SETTING = 128;
const int MAX_SETTINGS_PER_SETTING = 128;

class CParameter
{
public:
	CParameter();

	CParameter( string, string);

	~CParameter();

	string GetName( void);
	void SetName( string);

	string GetValue( void);
	void SetValue( string);

private:
	string m_sName;
	string m_sValue;
};

class CConfig {
protected:
public:
	CConfig();

	CConfig( string Name);

	string GetName( void);
	int AddParameter( string Name, string Value);

	int AddParameter( CParameter Value);

	int AddConfig( CConfig* );

	CParameter GetParam( int Index);

	CParameter GetParam( string Name);

	CConfig* GetConfig( int Index);

	CConfig* GetConfig( string Name);

	bool ReadConfigFile( string FileName, string& Error);
private:

	string m_sName;

	int m_iParameterCount;

	int m_iSettingsCount;

	CParameter m_oParameters[MAX_PARAMETERS_PER_SETTING];

	CConfig* m_oSettings[MAX_SETTINGS_PER_SETTING];

};

#endif

