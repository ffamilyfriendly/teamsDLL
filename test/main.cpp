#include "helpers.h"
#include <iostream>

/*
  !!!WARNING!!!
  this code is really messy and easy to break. It is just meant for testing / debugging
*/

json config = utils::config();

int login(teams::client *c) {
   try {
      c->login(config["client_id"],config["token"]);
    } catch(std::exception &e) {
      std::cout << "could not log in. " << e.what() << std::endl;
      return 1;
    }
    std::cout << "logged in!" << std::endl;
    return 0;
}

int getTeams(teams::client *c) {
  std::map<std::string,teams::team> teamList = c->getTeams();
  std::cout << "got " << teamList.size() << " team(s)!" << std::endl;
  for(std::pair<std::string, teams::team> element : teamList) {
    std::string id = element.first;
    teams::team team = element.second;
    std::cout << "team with id \"" << id << "\" was created at " << team.createdDateTime << std::endl;
  }
  return 0;
}

int team(teams::client *c,std::vector<std::string> args) {
  if(args.size() < 3) return 1;
  teams::team _team = c->getTeam(args[1]);
  if(args[2] == "channels") {
  std::map<std::string,teams::team::channel> teamList = _team.getChannels();
  std::cout << "got " << teamList.size() << " channel(s)!" << std::endl;
  for(std::pair<std::string, teams::team::channel> element : teamList) {
    std::string id = element.first;
    teams::team::channel chan = element.second;
    std::cout << "channel with id \"" << id << "\" " << (chan.email == "" ? "does not have an email" : "has email " + chan.email) << std::endl;
  }
  } else if(args.size() >= 4 and args[2] == "channel") {
    std::cout << "channel" << std::endl;
    teams::team::channel chan = _team.getChannel(args[3]);
    if(args.size() >= 5 and args[4] == "remove") {
      std::cout << "remove" << std::endl;
      chan.remove();
    }
  } else if(args[2] == "archive") {
    _team.archive();
  }
  return 0;
}


int clear() {
  system("clear");
  return 0;
}

int help() {
std::cout << "command - Usage\n###############\nlogin - login\ngetTeams - getTeams" << std::endl;
return 0;
}



int handleInputs(teams::client *c) {
  std::cout << "run: ";
  std::string cmd;
  std::getline(std::cin,cmd);
  std::vector<std::string> args;
  utils::split(cmd,args,' ');
  
  if(args.front() == "login") {
    return login(c);
  } else if(args.front() == "getTeams") {
    return getTeams(c);
  } else if(args.front() == "clear") {
    return clear();
  } else if(args.front() == "team") {
    return team(c,args);
  } else {
    std::cout << "no such command \"" << args.front() << "\"." << std::endl;
    return help();
  }
}

int main() {
  teams::client *c = new teams::client();
  bool keep_running = true;

  while(keep_running) {
    int state = handleInputs(c);
    if(state != 0) {
      std::cout << "function exited with code " << state << "." << std::endl;
    }
  }
}