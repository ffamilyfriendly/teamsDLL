#include "helpers.h"
#include <sstream>
#include <fstream>

size_t utils::split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

json utils::config() {
    std::ifstream t("config.json");
    if(!t.good()) {
        std::cout << "NOT GOOD >:(" << std::endl;
        return json::parse("{\"error\":\"file does not exist\"}");
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string epicly = buffer.str();
    t.close();
    return json::parse(epicly);
}