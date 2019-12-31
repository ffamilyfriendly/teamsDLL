#include "main.h"

CURL *curl = curl_easy_init();
struct curl_slist *list = NULL;

//ngl stole this from SO. Thanks quantum physicist https://stackoverflow.com/a/36401787
size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size*nmemb;
    try
    {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }
    return newLength;
}

//NON CLASS-BOUND FUNCTIONS
teams::team genTeam(json js) {
    teams::team t;
        t.id = js["id"];
        t.createdDateTime = js["createdDateTime"];
        t.displayName = js["displayName"];
        t.mail = js["mail"];
        t.mailEnabled = js["mailEnabled"];
    return t;
}

teams::team::channel genChannel(json js,teams::team *parent) {
    teams::team::channel c;
        c.id = js["id"];
        c.displayName = js["displayName"];
        c.email = js["email"];
        c.membershipType = js["membershipType"];
        c.parent = parent;
    return c;
}
//ENDOF NON CLASS-BOUND FUNCTIONS

// COMMON CLASS FUNCTIONS
teams::generic_returnType teams::common::generic_get(std::string partial_path) {
    CURL *handle = curl_easy_duphandle(curl);
    std::string url = this->url + partial_path;
    curl_easy_setopt(handle,CURLOPT_URL,url.c_str());
    //data
    std::string return_data;
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &return_data);
    curl_easy_perform(handle); 
    teams::generic_returnType returnThis;
    returnThis.data = return_data;
    long http_code = 0;
    curl_easy_getinfo(handle,CURLINFO_RESPONSE_CODE,&http_code);
    returnThis.http_code = http_code;
    curl_easy_cleanup(handle);
    return returnThis;
}

teams::generic_returnType teams::common::generic_post(std::string partial_path,std::string body) {
    std::string url = this->url + partial_path;
    CURL *handle = curl_easy_duphandle(curl);
    curl_easy_setopt(handle,CURLOPT_URL,url.c_str());
    curl_easy_setopt(handle,CURLOPT_CUSTOMREQUEST,"POST");
    curl_easy_setopt(handle,CURLOPT_POSTFIELDS,body);

    //data
    std::string return_data;
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &return_data);
    curl_easy_perform(handle); 
    teams::generic_returnType returnThis;
    returnThis.data = return_data;
    long http_code = 0;
    curl_easy_getinfo(handle,CURLINFO_RESPONSE_CODE,&http_code);
    returnThis.http_code = http_code;
    curl_easy_cleanup(handle);
    return returnThis;
}
// ENDOF COMMON CLASS FUNCTIONS

// CLIENT FUNCTIONS
void teams::client::login(std::string id, std::string secret) {

    //construct login url
    std::string login_url = "https://login.microsoftonline.com/" + this->tenant + "/oauth2/v2.0/token"; 
    
    //build request
    curl_mime *mime;
    mime = curl_mime_init(curl);

    //create parts
    curl_mimepart *cid = curl_mime_addpart(mime);
    curl_mimepart *cli_secret = curl_mime_addpart(mime);
    curl_mimepart *grant = curl_mime_addpart(mime);
    curl_mimepart *scope = curl_mime_addpart(mime);
    
    //id
    curl_mime_data(cid, id.c_str(), CURL_ZERO_TERMINATED);
    curl_mime_name(cid, "client_id");
    //secret
    curl_mime_data(cli_secret, secret.c_str(), CURL_ZERO_TERMINATED);
    curl_mime_name(cli_secret, "client_secret"); 
    //grant
    curl_mime_data(grant, "client_credentials", CURL_ZERO_TERMINATED);
    curl_mime_name(grant, "grant_type");
    //scope
    curl_mime_data(scope, "https://graph.microsoft.com/.default", CURL_ZERO_TERMINATED);
    curl_mime_name(scope, "scope");
    
    //write data to string
    std::string return_data;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &return_data);

    //post
    curl_easy_setopt(curl, CURLOPT_MIMEPOST,mime);
    curl_easy_setopt(curl,CURLOPT_URL,login_url.c_str());
    curl_easy_perform(curl); 
    auto js = json::parse(return_data);
    curl_easy_reset(curl);
    //This way of error handling is dumb but itworks
    if(js.contains("access_token")) {
        //set token
        this->token = js["access_token"];

        // HEADERS
        std::string auth_header = "Authorization: Bearer " + this->token;
        list = curl_slist_append(list,auth_header.c_str());
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,list);
    } else if(js.contains("error")) {
        throw std::runtime_error(js["error_description"]);
    } else {
        throw std::exception();
    }
}

std::map<std::string,teams::team> teams::client::getTeams() {   
    teams::generic_returnType generic_return;
    generic_return = this->generic_get("groups?$filter=resourceProvisioningOptions/Any(x%3Ax%20eq%20'Team')");
    auto js = json::parse(generic_return.data);
    std::map<std::string,teams::team> teams_list;
    for(auto& e : js["value"]) {
        teams_list[e["id"]] = genTeam(e);
    }
    return teams_list;
}

teams::team teams::client::getTeam(std::string id) {
    std::string url = "/groups/" + id;
    teams::generic_returnType generic_return;
    generic_return = this->generic_get(url);
    auto js = json::parse(generic_return.data);
    return genTeam(js);
} 
//ENDOF CLIENT FUNCTIONS

//TEAM FUNCTIONS

std::map<std::string,teams::team::channel> teams::team::getChannels() {
    std::string url = "/teams/" + this->id + "/channels";
    teams::generic_returnType generic_return;
    generic_return = this->generic_get(url);
    auto js = json::parse(generic_return.data);
    std::map<std::string,teams::team::channel> chan_list;
    for(auto& e : js["value"]) {
        chan_list[e["id"]] = genChannel(e,this);
    }
    return chan_list;
}

teams::team::channel teams::team::getChannel(std::string id) {
    std::string url = "/teams/" + this->id + "/channels/" + id;
    teams::generic_returnType generic_return;
    generic_return = this->generic_get(url);
    auto js = json::parse(generic_return.data);
    return genChannel(js,this);
}


//archives a team. Makes team read only but not gone
 void teams::team::archive(bool shouldSetSpoSiteReadOnlyForMembers) {
    std::string url = "/teams/" + this->id + "/archive";
    teams::generic_returnType generic_return;
    generic_return = this->generic_post(url,"{\"shouldSetSpoSiteReadOnlyForMembers\":false}");
    if(generic_return.http_code != 202) {
        throw std::runtime_error("could not archive team");
    }
 }
 //ENDOF TEAM FUNCTIONS

// CHANNEL
void teams::team::channel::remove() {
    std::string url = this->parent->url + "teams/" + this->parent->id + "/channels/" + this->id;
    CURL *delete_handle = curl_easy_duphandle(curl); //create temp handle
    curl_easy_setopt(delete_handle,CURLOPT_CUSTOMREQUEST,"DELETE");
    std::string return_data;
    curl_easy_setopt(delete_handle,CURLOPT_URL,url.c_str());
    curl_easy_setopt(delete_handle, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(delete_handle, CURLOPT_WRITEDATA, &return_data);
    curl_easy_perform(delete_handle);
    curl_easy_cleanup(delete_handle); //clean up temp delete handle
    std::cout << return_data << std::endl;
}