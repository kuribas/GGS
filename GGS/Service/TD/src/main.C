// $Id: main.C 9037 2010-07-06 04:05:44Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.


#include "types.h"
#include <cstdlib>

#include "ODKStream.h"

tdstream tds;	
string sMyLogin;

void CopyrightNotice();

void Error(const string &s)
{
  cerr << "error: " << s << endl;
  exit(-1);
}

int main(int argc, char** argv) {

  // CopyrightNotice();

  int err;
  bool   logging  = false;
  string server   = "localhost";
  int    port     = 5000;
  //string login    = "/td"; must use global sMyLogin
  string password = "";

  sMyLogin = "/td";
  
  if (argc<2) {
  error:
    cout << "usage: " << argv[0] << " [-lo(gging)] [-s server] [-p port] [-l login] -pw password" << endl; 
    exit(-1);
  }

  for (int argi=1; argi < argc; ++argi) {

    string opt = argv[argi];

    if (opt == "-l") {

      if (argi == argc-1) goto error;
      sMyLogin = argv[argi+1];
      if (sMyLogin == "") Error("empty login");
      argi++;
      
    } else if (opt == "-pw") {

      if (argi == argc-1) goto error;      
      password = argv[argi+1];
      if (password == "") Error("empty password");
      argi++;

    } else if (opt == "-s") {

      if (argi == argc-1) goto error;      
      server = argv[argi+1];
      if (server == "") Error("empty server");
      argi++;

    } else if (opt == "-p") {

      if (argi == argc-1) goto error;            
      if (sscanf(argv[argi+1], "%d", &port) != 1 || port < 1024) Error("port?");
      argi++;

    } else if (opt == "-lo") {

      logging = true;
      
    } else goto error;
  }

  if (password == "") goto error;

  
  // Connect(server, port,logging)
  if( (err=tds.Connect(server.c_str(),port,logging)) ) {
    cerr << tds.ErrText(err) << "\n";
    return err;
  }

#if 0  
  
  // Login(name, password)
  if (argc<4) {
    cout << "Login: ";
    cin >> sMyLogin;
  }
  else
    sMyLogin=argv[3];

  // Login(name, password)
  string sPassword;
  if (argc<5) {
    cout << "Password: ";
    cin >> sPassword;
  }
  else
    sPassword=argv[4];

#endif
  
  if( (err=tds.Login(sMyLogin.c_str(),password.c_str())) ) {
    cerr << tds.ErrText(err) << "\n";
    tds.Disconnect();
    return err;
  }

  tds.Process();			// receive, parse, and pass on messages

  return 0;
}

void CopyrightNotice() {
  ifstream ifs("copying.txt");
  if (!ifs.good()) {
    cerr << "No copyright notice, aborting\n";
    cout << "No copyright notice, aborting\n";
  }
  string s;

  getline(ifs, s, '\0');
  cout << s;
}
