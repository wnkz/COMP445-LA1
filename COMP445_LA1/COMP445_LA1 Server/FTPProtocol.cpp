#include "StdAfx.h"
#include "FTPProtocol.h"

const std::string FTPProtocol::R150_RETR = "150 Opening data connection for ";
const std::string FTPProtocol::R150_STOR = "150 Ok to send data.\n";

const std::string FTPProtocol::R215 = "215 UNIX Type: L8\n";
const std::string FTPProtocol::R220 = "220 COMP445 FTP Server\n";
const std::string FTPProtocol::R221 = "221 GoodBye !\n";
const std::string FTPProtocol::R226_RETR = "226 File send OK.\n";
const std::string FTPProtocol::R226_STOR = "226 File receive OK.\n";
const std::string FTPProtocol::R227 = "227 Entering Passive Mode";
const std::string FTPProtocol::R230 = "230 User logged on, proceed\n";

const std::string FTPProtocol::R451 = "451 Requested action aborted. Local error in processing.\n";

const std::string FTPProtocol::R501 = "501 Syntax error in parameters or arguments\n";
const std::string FTPProtocol::R502 = "502 Command not implemented.\n";
const std::string FTPProtocol::R530 = "530 Not logged in\n";
const std::string FTPProtocol::R550 = "550 Failed to open file.\n";

const std::string FTPProtocol::CUSER = "USER";
const std::string FTPProtocol::CLIST = "LIST";
const std::string FTPProtocol::CSYST = "SYST";
const std::string FTPProtocol::CPORT = "PORT";
const std::string FTPProtocol::CPASV = "PASV";
const std::string FTPProtocol::CPWD = "PWD";
const std::string FTPProtocol::CRETR = "RETR";
const std::string FTPProtocol::CSTOR = "STOR";
const std::string FTPProtocol::CQUIT = "QUIT";
