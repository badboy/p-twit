#ifndef CCONFIG_H_
#define CCONFIG_H_

#include "includes.h"
#include <string>

class Config {
  private:
	std::string file;
	bool loaded;
	TiXmlDocument doc;

	void set_base64_code();
  public:
	std::string user_nick;
	std::string user_password;
	std::string base64_code;

	int wlan_connection;

	Config(std::string _file);
	~Config();

	int load();

	bool is_loaded();
	bool save();
};

#endif
