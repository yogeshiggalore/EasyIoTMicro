#include<iostream>
#include<config.h>
#include<utils.h>

using namespace std;

int main( int argc, char* argv[])
{
	string l_sBuff = "";

	CConfig l_oConfig;

	l_oConfig.ReadConfigFile( "InstaConfig.cfg", l_sBuff);

	cout<<l_oConfig.GetConfig( "InstaCredit")->GetParam( "InstanceName").GetValue()<<endl;
	cout<<l_oConfig.GetConfig( "InstaCredit")->GetConfig( "INClient")->GetConfig( "Log")->GetParam( "SplitFile").GetValue()<<endl;
	// cout<<l_oConfig.getConfig(1)->getConfig( "Log")->getParam( "Name").getValue()<<endl;
	// cout<<l_oConfig.getConfig(1)->getConfig( 1)->getParam( "Name").getValue()<<endl;
	exit( EXIT_SUCCESS);
}

