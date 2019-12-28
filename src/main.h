#include <iostream>
#include <map>

namespace teams {

class team {
    public:
    //functions
    void archive(); //archive team with this function
    //std::map<std::string,user> getMembers(); //TODO: add this function & user class // get all users of group  
    //props
    std::string createdDateTime;
    std::string displayName;
    std::string mail;    
    bool mailEnabled; 
};

class client {
    public:
    //functions
    void login(std::string id, std::string secret); //will be used to get token
    std::string generic_get(std::string partial_path);
    std::map<std::string,team> getTeams(); //get all teams
    //props
    std::string tenant = "704ac63f-12c4-4186-94dc-84cc0643f0c8";
    std::string url = "https://graph.microsoft.com/beta/"; //beta version is needed to support getTeams() function

    private:
    std::string token;
};
}

