// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from piggy.djinni

#pragma once

#include <string>

namespace ns_piggy_native {

class ServerMessageGetter {
public:
    virtual ~ServerMessageGetter() {}

    static std::string try_login(const std::string & host_and_port, const std::string & user_name, const std::string & user_password);

    static std::string try_register(const std::string & host_and_port, const std::string & user_name, const std::string & user_password);

    static std::string try_status(const std::string & host_and_port, const std::string & user_name, const std::string & user_token);
};

}  // namespace ns_piggy_native