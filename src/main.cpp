#include "main.h"
#include <curl/curl.h>
//json parser https://github.com/nlohmann/json
#include "../lib/json.hpp"
using json = nlohmann::json;

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
    if(js["access_token"] != "") {
        //set token
        this->token = js["access_token"];
    } else {
        throw std::exception();
    }
}

std::string teams::client::generic_get(std::string partial_path) {
    std::string url = this->url + partial_path;
    std::string auth_header = "Authorization: Bearer " + this->token;

    // HEADERS
    list = curl_slist_append(list,auth_header.c_str());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,list);
    curl_easy_setopt(curl,CURLOPT_URL,url.c_str());

    //data
    std::string return_data;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &return_data);
    curl_easy_perform(curl); 
    return return_data;
}

std::map<std::string,teams::team> teams::client::getTeams() {    
    auto js = json::parse(this->generic_get("groups?$filter=resourceProvisioningOptions/Any(x%3Ax%20eq%20'Team')"));

    //initialize array
    std::map<std::string,teams::team> teams_list;
    teams::team return_array[js["value"].size()-1];
    for(int i = 0; i < js["value"].size()-1; i++) {
        teams::team t;
        t.createdDateTime = js["value"][i]["createdDateTime"];
        t.displayName = js["value"][i]["displayName"];
        t.mail = js["value"][i]["mail"];
        t.mailEnabled = js["value"][i]["mailEnabled"];

        teams_list[js["value"][i]["id"]] = t;
    }
    return teams_list;
}