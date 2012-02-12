#pragma once
#include <string>
class FTPProtocol
{
public:
	static const unsigned int CMD_MAX_LENGTH = 4096;

	static const std::string R150_RETR;
	static const std::string R150_c;
	static const std::string R150_STOR;

	static const std::string R215;
	static const std::string R215_c;
	static const std::string R220;
	static const std::string R220_c;
	static const std::string R221;
	static const std::string R221_c;
	static const std::string R226_c;
	static const std::string R226_RETR;
	static const std::string R226_STOR;
	static const std::string R227;
	static const std::string R227_c;
	static const std::string R230;
	static const std::string R230_c;

	static const std::string R451;
	static const std::string R451_c;

	static const std::string R501;
	static const std::string R502;
	static const std::string R530;
	static const std::string R550;
	static const std::string R501_c;
	static const std::string R502_c;
	static const std::string R530_c;
	static const std::string R550_c;

	static const std::string CUSER;
	static const std::string CLIST;
	static const std::string CSYST;
	static const std::string CPORT;
	static const std::string CPASV;
	static const std::string CPWD;
	static const std::string CRETR;
	static const std::string CSTOR;
	static const std::string CQUIT;
};
