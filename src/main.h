#include <iostream>
#include <curl/curl.h>
#include <map>

//json parser https://github.com/nlohmann/json
#include "../lib/json.hpp"
using json = nlohmann::json;

namespace teams {

//this class contains functions and props used by all classes
struct generic_returnType {
    std::string data;
    long http_code;
};

class common {
public:
    //funcs
    generic_returnType generic_get(std::string partial_path);
    generic_returnType generic_post(std::string partial_path,std::string body = "{}");
    //props
    std::string url = "https://graph.microsoft.com/beta/";

};

class team : common {
    public:
    // CLASS
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

    //functions
    void archive(bool shouldSetSpoSiteReadOnlyForMembers = false); //archive team with this function
    std::map<std::string,teams::team::channel> getChannels(); 
    teams::team::channel getChannel(std::string id);
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
    std::map<std::string,teams::team> getTeams(); //get all teams
    teams::team getTeam(std::string id);
    //props
    std::string tenant = "704ac63f-12c4-4186-94dc-84cc0643f0c8";
    private:
    std::string token;
};
}

