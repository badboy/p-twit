#include "CConfig.h"
#include "helpers.h"
#include "base64.h"

Config::Config(std::string _file)
{
	user_nick = "";
	user_password = "";
	base64_code = "";
	wlan_connection = 1;

	file = _file;
	load();
}

int Config::load()
{
	loaded = FALSE;
	if(file.size() <= 0) return 0;
	if(!doc.LoadFile(file.c_str())) return 0;
	TiXmlElement* root = doc.RootElement();
	if(!root)
		return 0;
	if (strcmp(root->Value( ), "psptwitter") != 0)
		return 0;
	for(TiXmlElement* elem = root->FirstChildElement( ); elem; elem = elem->NextSiblingElement())
	{
		if(strcmp(elem->Value(), "account") == 0)
		{
			TiXmlElement *_nick = elem->FirstChildElement("username");
			TiXmlElement *_pw = elem->FirstChildElement("password");

			if(_nick != NULL) user_nick = textValue(_nick);
			if(_pw != NULL) user_password = textValue(_pw);
			set_base64_code();
		}

		if(strcmp(elem->Value(), "config") == 0)
		{
			TiXmlElement *_wlan = elem->FirstChildElement("wlan_connection");
			if(_wlan != NULL) wlan_connection = atoi(textValue(_wlan));
		}
	}
	loaded = TRUE;
	return 1;
}

void Config::set_base64_code()
{
	if(user_nick.size() > 0 && user_password.size() > 0)
	{
		std::string tmp = user_nick + ":" + user_password;
		base64_code = base64_encode((const unsigned char*)tmp.c_str(), tmp.size());
	}
}

Config::~Config() {}

bool Config::is_loaded()
{
	return loaded;
}

bool Config::save()
{
	if(!loaded) return FALSE;

	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	
	TiXmlElement * root = new TiXmlElement( "psptwitter" );
	doc.LinkEndChild( root );
	
	TiXmlElement * user = new TiXmlElement( "account" );
	root->LinkEndChild( user );

	TiXmlElement * nick = new TiXmlElement( "username" );
	user->LinkEndChild( nick );
	TiXmlText * nickname = new TiXmlText( user_nick.c_str() );
	nick->LinkEndChild( nickname );
	TiXmlElement * pw = new TiXmlElement( "password" );
	user->LinkEndChild( pw );
	TiXmlText * password = new TiXmlText( user_password.c_str() );
	pw->LinkEndChild( password );

	char buf[10];
	TiXmlElement * config = new TiXmlElement( "config" );
	root->LinkEndChild( config );

	TiXmlElement * wlan = new TiXmlElement( "wlan_connection" );
	config->LinkEndChild( wlan );
	sprintf(buf, "%d", wlan_connection);
	TiXmlText * wlan_val = new TiXmlText( buf );
	wlan->LinkEndChild( wlan_val );

	set_base64_code();
	return doc.SaveFile(file.c_str());
}

