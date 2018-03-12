#ifndef CLIENTMESSAGE_H
#define CLIENTMESSAGE_H

#include <regex>
#include <string>
#include <json/json.h>

#include "doormessage.h"

class Clientmessage
{
public:

    Clientmessage(bool isOpen,
                  Doormessage doormessage);
    Clientmessage();

    Clientmessage &operator=(const Clientmessage& rhs);

    static Clientmessage fromJson(const Json::Value &root);
    static Clientmessage fromString(const std::string &json);
    std::string toJson() const;

    bool isOpen() const;
    const Doormessage& doormessage() const;

private:
    bool _isOpen;
    Doormessage _doormessage;

    const static std::string _unlockButtonKey;
    const static std::string _lockButtonKey;
    const static std::string _emergencyUnlockKey;
    const static std::string _isOpenKey;
};

#endif
