#ifndef LOGIC_H
#define LOGIC_H

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

#include "config.h"
#include "clientmessage.h"
#include "door.h"
#include "logger.h"
#include "request.h"
#include "response.h"

/* The "Logic" class
 *
 * This class is initilized by all settings.
 *
 * It handles incoming requests and allows modifications of the door
 */
class Logic
{
public:

    Logic(const std::string &ldapUri,
          const std::string &bindDN,
          const std::string &webPrefix,
          const std::string &serDev,
          const unsigned int baudrate,
          const std::string &logfile_scripts);
    ~Logic();

    // Parse incoming JSON Requests
    Response request(const Request &request);

    // Send direct command to door without credential checks
    enum class DoorCommand { Lock, Unlock };
    Response processDoor(const DoorCommand &doorCommand);

    // Returns the current Token
    Clientmessage getClientMessage();

private:

    // Internal lock wrapper
    Response _lock();
    // Internal unlock wrapper
    Response _unlock();

    // Checks if incoming credentials against LDAP
    Response _checkLDAP(const std::string &user,
                        const std::string &password);

    void _doorCallback(Doormessage doormessage);

    Logger &_logger;

    // The door
    Door _door;

    // stop indicator for the thread
    bool _run = true;
    // General mutex for concurrent data access
    mutable std::mutex _mutex = {};

    Doormessage _doormessage = {};

    // The URI of the ldap server
    const std::string _ldapUri;
    // LDAP bindDN
    const std::string _bindDN;
    // Prefix of the website
    const std::string _webPrefix;
};

#endif
