#include "../src/main.h"
#include <iostream>

int main() {
    teams::client *c = new teams::client();
    //login with client ID and secret. keep the secret secret... duh
    std::cout << "logging in...\n";
    try {
      c->login("client id","client token");
    } catch(std::exception) {
      std::cout << "could not log in.\n";
      return 1;
    }
    std::cout << "logged in!\n";

    std::cout << "fetching teams...\n";
    std::map<std::string,teams::team> teamList = c->getTeams();
    std::cout << "got " << teamList.size() << " team(s)!" << std::endl;

    for(std::pair<std::string, teams::team> element : teamList) {
      std::string id = element.first;
      teams::team team = element.second;
      std::cout << "team with id \"" << id << "\" was created at " << team.createdDateTime << std::endl;
    }
}