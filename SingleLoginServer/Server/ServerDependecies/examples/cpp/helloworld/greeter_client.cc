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
#include <chrono>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else

#include "helloworld.grpc.pb.h"

#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

using namespace grpc;
using namespace helloworld;

class GreeterClient {
public:
    GreeterClient(std::shared_ptr <Channel> channel)
            : stub_(Greeter::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string &user) {
        // Data we are sending to the server.
        HelloRequest request;
        request.set_name(user);

        // Container for the data we expect from the server.
        HelloReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        std::chrono::system_clock::time_point deadline =
                std::chrono::system_clock::now() + std::chrono::seconds(5);
        context.set_deadline(deadline);

        // The actual RPC.
        Status status = stub_->SayHello(&context, request, &reply);

        std::cout << "Network code = " << status.error_code() << std::endl;
        std::cout << "Network err message = " << status.error_message() << std::endl;
        std::cout << "Network err detail = " << status.error_details() << std::endl;

        // Act upon its status.
        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string TryRegister(const std::string &user, const std::string &user_password) {
        // Data we are sending to the server.
        RegisterRequest request;
        request.set_user_name(user);
        request.set_user_password(user_password);

        // Container for the data we expect from the server.
        RegisterReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        std::chrono::system_clock::time_point deadline =
                std::chrono::system_clock::now() + std::chrono::seconds(5);
        context.set_deadline(deadline);

        // The actual RPC.
        Status status = stub_->TryRegister(&context, request, &reply);

        std::cout << "Network code = " << status.error_code() << std::endl;
        std::cout << "Network err message = " << status.error_message() << std::endl;
        std::cout << "Network err detail = " << status.error_details() << std::endl;

        // Act upon its status.
        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr <Greeter::Stub> stub_;
};

int main(int argc, char **argv) {

    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint (in this case,
    // localhost at port 50051). We indicate that the channel isn't authenticated
    // (use of InsecureChannelCredentials()).

    ChannelArguments channelArgs;

    channelArgs.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 10000);
    channelArgs.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 20000);
    channelArgs.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
    channelArgs.SetInt(GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0);
    channelArgs.SetInt(GRPC_ARG_HTTP2_MIN_SENT_PING_INTERVAL_WITHOUT_DATA_MS, 21000);

    GreeterClient greeter(grpc::CreateCustomChannel("localhost:50051", grpc::InsecureChannelCredentials(), channelArgs));
    std::string user_name("Faip");
    std::string user_password("password");

    std::cout << "Greeter client send request" << std::endl;
    std::string reply = greeter.TryRegister(user_name, user_password);

    std::cout << "Greeter received: " << reply << std::endl;

    return 0;
}
