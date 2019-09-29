/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <stdlib.h>
#include <ctype.h>
#include <chrono>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <grpcpp/grpcpp.h>

#include <openssl/md5.h>

#include <mysql++/mysql++.h>
#include <bcrypt/BCrypt.hpp>

#include <md5Utils.h>
#include <piggy_base64.h>
#include <openssl_rsa.h>


#ifdef BAZEL_BUILD
#include "examples/protos/piggy.grpc.pb.h"
#else

#include "piggy.grpc.pb.h"
#include "../../../mysqlpp/include/mysql++/result.h"

#endif


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using piggy::HelloRequest;
using piggy::HelloReply;
using piggy::RegisterReply;
using piggy::RegisterRequest;

using piggy::Greeter;

using namespace piggy;
using namespace mysqlpp;
using namespace std;
using namespace std::chrono;

#define ERR_CODE_OK 0
#define ERR_CODE_USER_EXIST 10001
#define ERR_CODE_SYSTEM_ERROR 10002
#define ERR_CODE_USER_NAME_EMPTY 10003
#define ERR_CODE_USER_PSW_EMPTY 10004
#define ERR_CODE_USER_PSW_OR_USER_NAME_ERROR 10005
#define ERR_CODE_TOKEN_OUTDATED 10006

std::string servercert = "-----BEGIN CERTIFICATE-----\n"
                         "MIIFUjCCAzoCAQEwDQYJKoZIhvcNAQELBQAwbzELMAkGA1UEBhMCVVMxCzAJBgNV\n"
                         "BAgMAkNBMRUwEwYDVQQHDAxTYW5GcmFuY2lzY28xDzANBgNVBAoMBkdvb2dsZTEQ\n"
                         "MA4GA1UECwwHeW91dHViZTEZMBcGA1UEAwwQd3d3Lm15c2VydmVyLmNvbTAeFw0x\n"
                         "OTA5MjUxOTA4MDlaFw0yMDA5MjQxOTA4MDlaMG8xCzAJBgNVBAYTAlVTMQswCQYD\n"
                         "VQQIDAJDQTEVMBMGA1UEBwwMU2FuRnJhbmNpc2NvMQ8wDQYDVQQKDAZHb29nbGUx\n"
                         "EDAOBgNVBAsMB3lvdXR1YmUxGTAXBgNVBAMMEHd3dy5teXNlcnZlci5jb20wggIi\n"
                         "MA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQDUR4x8u/w6wmIs9BHsWbAWcZyW\n"
                         "nacQdxZh+ko1z7d26ybkfwUrw3KSeuwntEsOWNKrkt0ZvNs4OTDFshbEgkr6mkmp\n"
                         "PTgd9PAZ0os4Hu7EMBy2E9el8XEt9FC/aqgbg9Mvj68s2Xc7I2BcgmIt6BQ48FRH\n"
                         "54gVGf6vH6WFiSVStI4Nwcrhnf/EtPVcQwRHGYQu7jPnYBScDrz3w4Gnzxrexo35\n"
                         "PqEcqZAVcQMEHASjqU6sDNhdLPzVoyRMpkofe6ykuE5E1qqbWGo7B+Rf7BfrFQ1R\n"
                         "Tu8PYeOfN9GE46tnAAscsdAX/5AOL+kgx3ZCcMefJpizJOWxa8cG2dfOhiBTsfzu\n"
                         "ipIqdR+kfLXJ4UcstegDsmr86vuP6KCCICidsjNK3WBX67w5huIDcnBx1LsReUhm\n"
                         "9oFk+u2GC5uwnE9ksyubUJaJ+kKSh5Jnl7GFhtXX0C/ziM+CE1hEy1tXKMk+n0QK\n"
                         "OJXAV/nxXCqVrm6oYJN4uSAKSzUJF8hzEyK4zm5h7sLjiYPvy7DMYpYNlPC4wHM7\n"
                         "iYVDxKcE/VBOX1VjjNqmwlQYyb0D2jmXlJ9Es0DpUwMFmvZjVsatdarAcKl/I0gq\n"
                         "6kKYofx8NMV9wP9rVhLnbLc8AKmC8TQl18SwYRbamJqbzyNFaHd/zDqWRRQP4cOi\n"
                         "oWmJ57qQ2vUXEvWtRQIDAQABMA0GCSqGSIb3DQEBCwUAA4ICAQDT45/aFYUvd28v\n"
                         "vHnw23bObJdF0X7l28ecB6W9WAt2gKBWQr0S2uhW0qfmUsVZ96h4x8KyPF0YCDD4\n"
                         "tz2SL+Z84WiUZbg1uiY2FZsAjJTb+2lsWbJWSudSMCvyElDnMw6Wv5W1nIFBdE5f\n"
                         "+GaVWrv7dNW8uAUaBt3CtmYZ08EAtB/rtAMLUe+ufi0dh0BVST5UM8XZK/66kcV/\n"
                         "9QSwclrV4FH+Hu8bJxwf4cOqBSThe0AZtiH8quCTyDgf0CdNNywWSVwyDFBng7/4\n"
                         "PZNAHNoWFe6Pa/nVmR0TvSgxIZwTWGTEnH5qT9yv/D5nY3eHhgDD77lDjA5Dg2ib\n"
                         "OEleXojDY0/0bQiqiRmJUbaPmNfbinmGLzWniAXeLHX4GfZkiRQx/STJ9bbdH3lu\n"
                         "QsBrYRNsmwby4+Lrlk6OkauVq32qwznYQtnMkS51+LdUf+QOrQJr9maTm0AbQtsS\n"
                         "y4mBMU44utP+yKq4GKO5UYbrWC/V+MmyHDdNMKv8nHndwcnoEEHJ+J0RAvN/fJdw\n"
                         "YV5Vbf/0S1qmybYNizgdKT3tKpvzDHLyPTV2ujlGVv1I9D0tVu5Eh2jE4vL9mQaO\n"
                         "+1yL1PeHgVBfi2Km4nRr/Xdp20G3l2eyrR7nOBZ2SHKHVre+OXxY4emkEWkMmLAO\n"
                         "lBEws9a1sDU+m49mH4/NX7qanxiTCw==\n"
                         "-----END CERTIFICATE-----";

std::string serverkey = "-----BEGIN RSA PRIVATE KEY-----\n"
                        "MIIJKQIBAAKCAgEA1EeMfLv8OsJiLPQR7FmwFnGclp2nEHcWYfpKNc+3dusm5H8F\n"
                        "K8NyknrsJ7RLDljSq5LdGbzbODkwxbIWxIJK+ppJqT04HfTwGdKLOB7uxDActhPX\n"
                        "pfFxLfRQv2qoG4PTL4+vLNl3OyNgXIJiLegUOPBUR+eIFRn+rx+lhYklUrSODcHK\n"
                        "4Z3/xLT1XEMERxmELu4z52AUnA6898OBp88a3saN+T6hHKmQFXEDBBwEo6lOrAzY\n"
                        "XSz81aMkTKZKH3uspLhORNaqm1hqOwfkX+wX6xUNUU7vD2HjnzfRhOOrZwALHLHQ\n"
                        "F/+QDi/pIMd2QnDHnyaYsyTlsWvHBtnXzoYgU7H87oqSKnUfpHy1yeFHLLXoA7Jq\n"
                        "/Or7j+iggiAonbIzSt1gV+u8OYbiA3JwcdS7EXlIZvaBZPrthgubsJxPZLMrm1CW\n"
                        "ifpCkoeSZ5exhYbV19Av84jPghNYRMtbVyjJPp9ECjiVwFf58Vwqla5uqGCTeLkg\n"
                        "Cks1CRfIcxMiuM5uYe7C44mD78uwzGKWDZTwuMBzO4mFQ8SnBP1QTl9VY4zapsJU\n"
                        "GMm9A9o5l5SfRLNA6VMDBZr2Y1bGrXWqwHCpfyNIKupCmKH8fDTFfcD/a1YS52y3\n"
                        "PACpgvE0JdfEsGEW2piam88jRWh3f8w6lkUUD+HDoqFpiee6kNr1FxL1rUUCAwEA\n"
                        "AQKCAgEAqmoV+jzfrDxInFYD3X3SwvHKB4/ggnNX4KgR00lBu5Hj0yO1fwCUhctw\n"
                        "sh8m8gmUdnCcVwEp6+TdppfldzQyBSS/ZcHjvwjXh6NiaFdMzurg4brkMC+38HBm\n"
                        "MoTpF82oSFJNCvA5q5wmy51QPBJHyb4FqwPQV0BbGTOKqb0D3XVdZOXifWVd4why\n"
                        "wPJ8CKgPWa2YtXKImQ+r3CtzdsfohldXunPw2xW0ucHT7gSgJNLSeYSGXaa0Ar3J\n"
                        "ck88yjF603GyQ8khhTxha6pWSDs8qFhqLI/1kp+/IB6rcMdk3ndGdG6648kiWIFV\n"
                        "4h6+bXAkkTtm8sn4K4qeaH2RphY+8/tw0VjQYSXzY/BRD6xwAhz0G6lSdutW/AdS\n"
                        "zrcB2dBGac1mA1QGQkeKOBbJCzWlyDV9RE/+XtRmm+KLho8QrHwLHjT3yyX7QeK/\n"
                        "XnDzlroXumU2It+G7VaUMZRL4cB2jH8+uKP3N01MCT6NO18cXD+CX8shZBieJXxx\n"
                        "+DSErzPSnT1jC66HoBycZEFN4LQqkVFRqZDS42ri9Fgy1KLI63q+YuayAORLgNrN\n"
                        "58N0FbNwp6ZKouli9CRnDpWdRDoSgN+jjYdTc0fwaFbYX78QjeCtWTneHotUcjGj\n"
                        "y9xN29hg63etzh0qwFNpHOH2U0iLbMrnyi2tVnlJZz93yVxa3zECggEBAOmCHa8K\n"
                        "wGZG+ki1Zb/duoHgcgX7waVnw8t7IoY8pnKlSIrqeSz5RFboIEgo0RRJ8GopdG6P\n"
                        "B7Os1xoBQZX9ducp0AYjZg8UkalOKQmYV/dOC2QpnBNbFeyBJK3ikUaj915a+RC9\n"
                        "4RFDEiRC3TytvBmgunsMXI7Z51Mph64UdgpHca9lu2iDZ7eddbGItLAD6V1GDR7z\n"
                        "2WrZs1s5+aeozZKpTfcqB2+nI6segV1Acb4omWW2RqPQc9B7Auxo1VNwAeftNHzq\n"
                        "raFXcfFe1yA25/hNVVHalvYjDrKINIYH3OAEtJqgmsPB2CNhHuBm8KOfykGiK6vB\n"
                        "yGDVz0XFCvl3Du8CggEBAOi5+GEivakl9rHxe3qnA/B2O+ldEjcMagIFF+3EQb5C\n"
                        "Yl/GMPylnY4eDUdaNKPJhasIoj0x4O8E0CR2rKH4pzg+eDOrBMnO2SdJW37PUQiy\n"
                        "V8E2eUwbKOsZpmi6oGo7K3iFTItcB/irgzP7IqxM4uCFFzOejXmz86+aZp+2rXYT\n"
                        "gky2242IUFCpg0YNZVc8d0ciilQ90dQFBsMe0BZL/4wtbT6Ev8A1B+C2wsBVftvq\n"
                        "47JujDmCPgrdFLluA15a0Lz6ekbNK9tkwGxjGJGPwPYiPtNWcZLA2xkPGb5zbfkY\n"
                        "0zZHDOITS0vGmOvjlNHni9ZpLnOuoDFuW9Mf2Y2IhwsCggEARu26HIh0hty6e8l1\n"
                        "/cDZPUgy+nRNjvOk+rMNep8DpEFmdqPgaPPSeV+LJsAXOkV3XJf5APg7S62NrR2C\n"
                        "kY/rRS5jPOoVvzDtSfMu7fYzijBPr34gJZT+ZNeZj6QJot59blDN5PL4Yajw4sjY\n"
                        "sOzI/TPgUS4hE1dHN/CegRc//8FR3AhX09hOcezHakTvNaQKh8WWiLdjbFxyaZyD\n"
                        "pBTXVNxOjWxb0+N+JyVeADhqFGv6pHWooWvZMhObgLobSnv0aEGaNvOGCEMqnjnD\n"
                        "uMpUzgyXhQST5HIjrcFHywqqYfWr8mgkep/wClce7OT35YvNAQDEqSh0uE1Z54SB\n"
                        "3+L1SwKCAQEA2UwLZmcRv8X6udtH+BXJz9QVzKdiQpQMxyYKWa9N02AYimR7iOr6\n"
                        "WaAlteG8FNItRK0jDxBrfcF4txM3zWRmflWV1mhwaB4DFHOiQp1AhGZBGd7LEwyp\n"
                        "ABtLDa5z6bylsjqEZfrgVoeOUhLDJS4FCK48I65/WotzlRsNh0MCY6WDnC2Agn4s\n"
                        "v6QHNnl7jiCfHoRZtJ6kNNscnUM/+BGt4izoO9+nSsg5OtBqtzsc4CpJwGkH9Qpu\n"
                        "86XJm/nkm00gadNWKJWZxZglMw6z+fXHrgn7r0ifcu8vpA49c15D6LoNPDjLBqwN\n"
                        "OguM7XPpmLQiFWjQPqNmX8Eza8XpVAWOUQKCAQBGi8PGKl6WRW99gy450hUtRDI5\n"
                        "9xFP+sja1uz5ORWg+rwB1lXG1bZKnvGJo/Nmy4Egn7IW9+W9pyreaEkIZoxBH0v1\n"
                        "YOMMmxDXzmKGj6+cNgJNw091Ca7ibRhUGfU59xsJwRrBI3kxieH/6+hJjFDRUNLg\n"
                        "4PL4Dazx1pgfbe/PVguGghDSIpX5Ao2IF6GPxrhoYVfYu7QxWJLDoMImONnhAzej\n"
                        "7v2IyD7P2dof3N3QlucLoVWGJrTYrqALQmHAY6WXyjbNGuQN5ZTapZHe6WLNcwYr\n"
                        "gzd8mnb4vs2QGHA7R8eoWY5WgSPS4Hc4mMLyQ0qoAr8bbCXeHyAmla3lzPnx\n"
                        "-----END RSA PRIVATE KEY-----";

unsigned char server_private_key[] = "-----BEGIN RSA PRIVATE KEY-----\n"
                                 "MIIEogIBAAKCAQEAuC24gUsxt7HggDw1UPTEqekujh1Snd2h63j/+48ao6W4DYn1\n"
                                 "sAKFrjJWkeOiNsKJZmpFtPGnqxxXooyRSl1diMLVLkZqBLLd7xdqpr8J2e4IqzsU\n"
                                 "VdxZuNvROKYPpVM0AQwFf8Xd2qneWTkhM+PDdH4JM2Z+0G1LkyHO3g1ACJnUZqPg\n"
                                 "UrzuOxBVHMsQvmx94vdFmUldebwPAG1aPW42eJ29kxvvYDxRhFTEC7P6Rq5Rf+6J\n"
                                 "dgrj8WnP/iculGVb109PyuvfiI6KRjMh8DbD2l0YRIU5ua9uG4wp7BKLzpEaxsx9\n"
                                 "5MKDK1iUHL8sj9lh0ArtFDHoigVWb1FA4BQPcQIDAQABAoIBAASb4Eob/5qBMQUE\n"
                                 "fLER7vnxg+C6ix64NR0XYhCBfEEsHHb6RNOqvH2ndNMhxyVnqNiPQD9O7MszYl4G\n"
                                 "9nvN6lFJztvhlzWW5EbfZyPVFAIIGt9uEwwy/2vI1Gt+fFW+x0ZfbqPL58lZBt9k\n"
                                 "tpf90x5PKRvILNQ6Z/G+ntafrkKIzBHKHN7QzPN9vuXrj5JuBT56YG2rQeA4RVW3\n"
                                 "xY59abeyuXPsMw0W8bTCPgyHjdNUX7I+e+8ypX86w+y630aQs5gsSZtbfiKIHPGu\n"
                                 "Y7NM4Cy3e5t0rJtIQ2Kb+THZ+6mNFacieRp95sQRtlfVisaZSMVlb2hbU13SpDj4\n"
                                 "k8G4VyECgYEA2WwOa7wMaKMLIJC0WiPJzcgwiR4CmmBqhxdos0pqDDH8imoEXK1i\n"
                                 "a+6FRuYoKSirATqTlx+EPYrW/x9SQkmyDr6WaKqCW21FdEmIRQ0+1bt4VRfMFchK\n"
                                 "GH4x/tWCePyacdKJoNr0afZysUDpp74gJ9uJcSWFHLHHp5syJCZ1UKsCgYEA2Nul\n"
                                 "5hUuD4nc3w7Tq62+NkVLRxbMc+IJe7yDhEE51I+dRhp5I1sgBDoIfpZPgf5zO0Hk\n"
                                 "mMmio5w66K0RfO0LoKDbl0KU/YvzAHpGHs7l56DR+uk+wAwb3UiJBzaI+AtKc9jE\n"
                                 "sw207TETtuj21BhFMCu7bUqod/9Dm4hyCuIwuFMCgYASqGs0Y5bNA/DYgpJ3icqv\n"
                                 "lhZULfcbOFrr7N5XRHOFSGTbm+1z5o0jpfxZ8TZREeRa3qP7vAClJ1z717w/ZAOb\n"
                                 "sE9H/BnjrqTfLdA73L8fqE7TPo+M7vsszueLbtcbetHmIqMc4q1GypddDqz+cBpi\n"
                                 "UTyiMQ4TQE5Z27+ZypMWsQKBgGdWlAHGSgLBkW0B5cBDtQarkqm3pYn7RsWeGZXm\n"
                                 "dyZhoXKMnvlILUOWXAHmotCxbjvotIRSYZxRkIjcIB9F3mqW/M1hvxnrnMq3eb6o\n"
                                 "nUn5itZkW7CslIKoVBQJ2s9Iohv+Sg2BFJyK7HgXH7gGMMI5tMDp+Mnl7FOre5zQ\n"
                                 "GKNdAoGARYRV1uhwFepW604xOmpZc+1WRmZjjNcO9CAHE+Tb4qKaWU/CP/6qzW1Q\n"
                                 "wCvUojwnQxK/yP02D1oupKtpGRpPsfy+rfSQp4Wu/HhYlz7j5QLQP6isHOuZUt1z\n"
                                 "9AVz1RN3Wv+rEhOFvkOsO/OEmFRZSxEL1kxH01B3GWennSRsjZc=\n"
                                 "-----END RSA PRIVATE KEY-----";


bool isCharPointerEmpty(const char* str) {
    if (str == NULL) {
        return true;
    }

    return str[0] == '\0';
}

std::string getDecryptedPwd(const std::string user_password) {
    if (user_password.empty()) {
        return "";
    }

    std::string bbb = base64_decode(user_password);

    // 将加密后的string转回unsigned char*
    unsigned char* abcd = (unsigned char*)(bbb.c_str());
    unsigned char decrypted[2048]={};
    int decrypted_length = private_decrypt(abcd, 256, server_private_key, decrypted);

    std::string encrypted_string(decrypted, decrypted + decrypted_length);

    return encrypted_string;
}

bool isUserTokenExistInDB(string user_name, string user_token) {
    try {
        Connection conn(false);
        conn.connect("single_login", "localhost", "root", "12345678");
        Query query = conn.query();

        query << "select * from t_user_token where user_name='" + user_name + "'" + " and" + " user_token='" +
        user_token + "'";

        StoreQueryResult ares = query.store();

        if (isCharPointerEmpty(query.error())) {
            cout << "Name: " << user_name << " query token existence success" << endl;
        } else {
            cout << "Name: " << user_name << " query token existence fail" << endl;
            return false;
        }

        cout << "Name: " << user_name << " - token row count = " << ares.num_rows() <<
        ", query error msg = " << query.error() << endl;

        if (ares.num_rows() == 1) {
            cout << "Name: " << user_name << " - token exist in db" << endl;
            return true;
        } else {
            return false;
        }

    } catch (BadQuery er) {
        cerr << "Error: " << er.what() << endl;
        return false;
    } catch (const BadConversion& er) {
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return false;
    } catch (const Exception& er) {
        cerr << "Error: " << er.what() << endl;
        return false;
    }
}

int saveUserTokenToDB(string user_name, string user_token) {
    try {
        Connection conn(false);
        conn.connect("single_login", "localhost", "root", "12345678");
        Query query = conn.query();

        string saveTokenStr = "insert into t_user_token values('" + user_name + "', '" +
                user_token + "') ON DUPLICATE KEY UPDATE" + " user_token='" + user_token + "'";

        cout << "save token sql = "<< saveTokenStr << endl;


        query << saveTokenStr;

        query.store();

        if (isCharPointerEmpty(query.error())) {
            cout << "Save token ok = " << user_token << endl;
            return ERR_CODE_OK;
        } else {
            cout << "Save token fail = "<< user_token << ", error = " << query.error() << endl;
            return ERR_CODE_SYSTEM_ERROR;
        }

    } catch (BadQuery er) { // handle any connection or
        // query errors that may come up
        cerr << "Error: " << er.what() << endl;
        return ERR_CODE_SYSTEM_ERROR;
    } catch (const BadConversion& er) {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return ERR_CODE_SYSTEM_ERROR;
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return ERR_CODE_SYSTEM_ERROR;
    }
}

bool isUserExistInDB(string user_name) {
    try {
        Connection conn(false);
        conn.connect("single_login", "localhost", "root", "12345678");
        Query query = conn.query();

        query << "select * from t_user where user_name='" + user_name + "'";

        StoreQueryResult ares = query.store();

        if (isCharPointerEmpty(query.error())) {
            cout << "Name: " << user_name << " query existence success" << endl;
        } else {
            cout << "Name: " << user_name << " query existence fail" << endl;
            return true;
        }

        cout << "Name: " << user_name << " - row count = " << ares.num_rows() << ", query error msg = " << query.error() << endl;

        if (ares.num_rows() == 1) {
            cout << "Name: " << user_name << " - exist in db" << endl;
            return true;
        } else {
            return false;
        }

    } catch (BadQuery er) { // handle any connection or
        // query errors that may come up
        cerr << "Error: " << er.what() << endl;
        return true;
    } catch (const BadConversion& er) {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return true;
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return true;
    }
}

bool isUserAndPasswordExistInDB(string user_name, string password) {
    try {
        BCrypt bcrypt;

        Connection conn(false);
        conn.connect("single_login", "localhost", "root", "12345678");
        Query query = conn.query();

        string query_msg = "select * from t_user where user_name='" + user_name + "'";

        cout << "Check name pwd exist query = " + query_msg << endl;

        query << query_msg;

        StoreQueryResult ares = query.store();

        if (isCharPointerEmpty(query.error())) {
            cout << "Name: " << user_name << ", pwd = " << password << " query existence success" << endl;
        } else {
            cout << "Name: " << user_name << ", pwd = " << password << " query existence fail" << endl;
            return false;
        }

        cout << "Name: " << user_name << ", pwd = " << password << " - row count = " << ares.num_rows() << ", query error msg = " << query.error() << endl;

        if (ares.num_rows() == 1) {
            cout << "Name: " << user_name << ", pwd = " << password << " - exist in db" << endl;

            mysqlpp::String s = ares[0]["user_password"];
            string pwd_bcrypt = std::string(s.data(),s.length());

            cout << "Name: " << user_name << ", pwd_bcrypt = " << pwd_bcrypt << endl;

            if (bcrypt.validatePassword(password, pwd_bcrypt)) {
                cout << "Name: " << user_name << ", pwd_bcrypt = " << pwd_bcrypt << ", pwd check success" << endl;
                return true;
            }

            cout << "Name: " << user_name << ", pwd_bcrypt = " << pwd_bcrypt << ", pwd check fail" << endl;
            return false;

        } else {
            return false;
        }

    } catch (BadQuery er) { // handle any connection or
        // query errors that may come up
        cerr << "Error: " << er.what() << endl;
        return false;
    } catch (const BadConversion& er) {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return false;
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return false;
    }
}

int checkUserNameAndPassword(string user_name, string user_password) {
    if (user_name.empty()) {
        return ERR_CODE_USER_NAME_EMPTY;
    }

    if (user_password.empty()) {
        return ERR_CODE_USER_PSW_EMPTY;
    }

    if (!isUserExistInDB(user_name)) {
        return ERR_CODE_USER_PSW_OR_USER_NAME_ERROR;
    }

    if (isUserAndPasswordExistInDB(user_name, user_password)) {
        return ERR_CODE_OK;
    }

    return ERR_CODE_USER_PSW_OR_USER_NAME_ERROR;
}

int saveUserInfoToDB(string user_name, string user_password) {

//    BCrypt bcrypt;
//    std::string hash = bcrypt.generateHash(user_password);
//    std::cout<<user_password << " " << hash <<std::endl;
//
//    std::cout<<bcrypt.validatePassword(user_password,hash)<<std::endl;
//    std::cout<<bcrypt.validatePassword("test1", hash)<<std::endl;

    if (isUserExistInDB(user_name)) {
        cout << "Name: " << user_name << " is in db, return exist code!" << endl;
        return ERR_CODE_USER_EXIST;
    } else {
        cout << "Name: " << user_name << " not in db, try insert" << endl;
    }

    cout << "About to insert name = " << user_name << ", pwd = " << user_password << endl;

    BCrypt bcrypt;
    user_password = bcrypt.generateHash(user_password);

    cout << "About to insert name = " << user_name << ", pwd crypt = " << user_password << endl;

    try {
        Connection conn(false);
        conn.connect("single_login", "localhost", "root", "12345678");
        Query query = conn.query();

        //query << "SELECT * FROM t_user LIMIT 10";

        query << "insert into t_user values('" + user_name + "', '" + user_password + "')";

        //query << "select * from t_user where user_name='" + user_name + "'";

        query.store();

        if (isCharPointerEmpty(query.error())) {
            cout << "Name: " << user_name << " - password = " << user_password << " insert ok " << endl;
            return ERR_CODE_OK;
        } else {
            cout << "Name: " << user_name << " - password = " << user_password << " insert fail " << query.error() << endl;
            return ERR_CODE_SYSTEM_ERROR;
        }

//        cout << "Name: " << user_name << " - row count = " << ares.num_rows() << " query error " << query.error() << endl;
//
//        if (ares.num_rows() == 1) {
//            cout << "Name: " << user_name << " - exist" << endl;
//            return -2;
//        } else {
//            cout << "Name: " << user_name << " - not exist" << endl;
//        }

//        for (size_t i = 0; i < ares.num_rows(); i++)
//            cout << "Name: " << ares[i]["user_name"] << " - psw: " << ares[i]["user_password"] << endl;

        return 0;
    } catch (BadQuery er) { // handle any connection or
        // query errors that may come up
        cerr << "Error: " << er.what() << endl;
        return ERR_CODE_SYSTEM_ERROR;
    } catch (const BadConversion& er) {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return ERR_CODE_SYSTEM_ERROR;
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return ERR_CODE_SYSTEM_ERROR;
    }
}


// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
    Status SayHello(ServerContext *context, const HelloRequest *request, HelloReply *reply) override {
        std::string prefix("HelloMMM ");
        reply->set_message(prefix + request->name());
        return Status::OK;
    }

    Status TryRegister(ServerContext *context, const RegisterRequest *request, RegisterReply *reply) override {
        std::string prefix("Hello Register");

        std::string user_name = request->user_name();
        std::string user_password = request->user_password();

        std::cout << "Register user name =  " << user_name << std::endl;
        std::cout << "Register user password =  " << user_password << std::endl;

        std::string dpt_pwd = getDecryptedPwd(user_password);
        std::cout << "Register user password decyrpted =  " << dpt_pwd << std::endl;

        int ret = saveUserInfoToDB(request->user_name(), dpt_pwd);

        reply->set_message(std::to_string(ret));
        return Status::OK;
    }

    Status TryLogin(ServerContext *context, const LoginRequest *request, LoginReply *reply) override {
        std::string prefix("Hello Login");

        string user_name = request->user_name();
        string user_password = request->user_password();

        string user_real_password = getDecryptedPwd(user_password);

        std::cout << "Login user name =  " << request->user_name() << std::endl;
        std::cout << "Login user password =  " << user_real_password << std::endl;

        int ret = checkUserNameAndPassword(request->user_name(), user_real_password);

        if (ret != ERR_CODE_OK) {
            reply->set_message(std::to_string(ret));
            reply->set_token("");
            return Status::OK;
        }

        milliseconds ms = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );

        string current_time = std::to_string(ms.count());

        std::cout << "Current time milli =  " << current_time << std::endl;

        class MD5 md5Object2(request->user_name() + current_time);

        std::cout << "Token origin =  " << request->user_name() + current_time << std::endl;

        string token = md5Object2.toStr();

        std::cout << "Token = " << token << std::endl;

        saveUserTokenToDB(user_name, token);

        reply->set_message(std::to_string(ret));
        reply->set_token(token);
        return Status::OK;
    }

    Status CheckKickOut(ServerContext *context, const CheckKickOutRequest *request, CheckKickOutReply *reply) override {
        std::string prefix("Start check kick out!");

        string user_name = request->user_name();
        string user_token = request->user_token();

        std::cout << "Check kick out user name =  " << user_name << std::endl;
        std::cout << "Check kick out user token =  " << user_token << std::endl;

        if (isUserTokenExistInDB(user_name, user_token)) {
            reply->set_message(std::to_string(ERR_CODE_OK));
        } else {
            reply->set_message(std::to_string(ERR_CODE_TOKEN_OUTDATED));
        }

        return Status::OK;
    }
};

void RunServer() {
    grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp;
    pkcp.private_key = serverkey;
    pkcp.cert_chain = servercert;

    grpc::SslServerCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs="";
    ssl_opts.pem_key_cert_pairs.push_back(pkcp);

    std::shared_ptr<grpc::ServerCredentials> creds;
    creds = grpc::SslServerCredentials(ssl_opts);

    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, creds);

    // 10秒一个ping包
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 10000);
    // 20秒的ping包超时时间
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 20000);
    // 没有请求的情况下，也允许无限发送ping包
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0);
    // 没有请求也允许发送ping包
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, true);
    // 允许无限多的bad ping
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MAX_PING_STRIKES, 0);
    // 服务端允许的最短ping间隔
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS, 5000);
    // 最短ping包间隔
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MIN_SENT_PING_INTERVAL_WITHOUT_DATA_MS, 5000);

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr <Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char **argv) {

    RunServer();

    return 0;
}
