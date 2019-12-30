#include <iostream>
#include <curl/curl.h>
#include <map>

namespace teams {

//this class contains functions and props used by all classes
class common {
public:
    //funcs
    std::string generic_get(std::string partial_path);
    std::string generic_post(std::string partial_path,curl_mime *m);
    //props
    std::string url = "https://graph.microsoft.com/beta/";

};

class channel {
    public:
    //props
    std::string id;
    std::string displayName;
    std::string email;
    std::string membershipType;
    team *parent;
    //funcs
    void remove();
    

};

class team : common {
    public:
    //functions
    void archive(bool shouldSetSpoSiteReadOnlyForMembers = false); //archive team with this function
    std::map<std::string,channel> getChannels(); 
    //props
    std::string id;
    std::string createdDateTime;
    std::string displayName;
    std::string mail;    
    bool mailEnabled; 
};

class client : common {
    public:
    //functions
    void login(std::string id, std::string secret); //will be used to get token
    std::map<std::string,team> getTeams(); //get all teams
    team getTeam(std::string id);
    //props
    std::string tenant = "704ac63f-12c4-4186-94dc-84cc0643f0c8";
    private:
    std::string token;
};
}

