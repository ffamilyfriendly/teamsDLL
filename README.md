# docs

## common
#### properties
- **string** url
> the base API url
#### functions
- **string** generic_get(**string** partial_path);
> makes a get request to the url defined in properties + the partial path that is passed. Returns data from request

## client : common
#### properties
- **string** tenant
- **string** token (private)
#### functions
- **void** login(**string** id,**string** secret)
> get token from microsoft servers
- **map\<string,team\>** getTeams()
> gets all teams the application has access to
- **team** getTeam(**string** id)
> gets a team by ID

## channel : common
#### properties
- id
- displayName
- email
- membershipType
- *parent
#### functions
- none

## team : common
#### properties
- **string** id;
- **string** createdDateTime;
- **string** displayName;
- **string** mail;  
#### functions
- **map\<string,channel\> getChannels()**
> returns a list of all channels in team

# example
```cpp
#include "main.h"
#include <iostream>

int main() {
    teams::client *c = new teams::client();
    c->login("id","token");
    teams::team due_archive = c->getTeam("team_id");
    due_archive.archive();
}
```