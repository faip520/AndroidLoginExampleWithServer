// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: piggy.proto

#include "piggy.pb.h"
#include "piggy.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace piggy {

static const char* Greeter_method_names[] = {
  "/piggy.Greeter/SayHello",
  "/piggy.Greeter/TryRegister",
  "/piggy.Greeter/TryLogin",
  "/piggy.Greeter/CheckKickOut",
};

std::unique_ptr< Greeter::Stub> Greeter::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Greeter::Stub> stub(new Greeter::Stub(channel));
  return stub;
}

Greeter::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_SayHello_(Greeter_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_TryRegister_(Greeter_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_TryLogin_(Greeter_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_CheckKickOut_(Greeter_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Greeter::Stub::SayHello(::grpc::ClientContext* context, const ::piggy::HelloRequest& request, ::piggy::HelloReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SayHello_, context, request, response);
}

void Greeter::Stub::experimental_async::SayHello(::grpc::ClientContext* context, const ::piggy::HelloRequest* request, ::piggy::HelloReply* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SayHello_, context, request, response, std::move(f));
}

void Greeter::Stub::experimental_async::SayHello(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::piggy::HelloReply* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SayHello_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::piggy::HelloReply>* Greeter::Stub::AsyncSayHelloRaw(::grpc::ClientContext* context, const ::piggy::HelloRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::piggy::HelloReply>::Create(channel_.get(), cq, rpcmethod_SayHello_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::piggy::HelloReply>* Greeter::Stub::PrepareAsyncSayHelloRaw(::grpc::ClientContext* context, const ::piggy::HelloRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::piggy::HelloReply>::Create(channel_.get(), cq, rpcmethod_SayHello_, context, request, false);
}

::grpc::Status Greeter::Stub::TryRegister(::grpc::ClientContext* context, const ::piggy::RegisterRequest& request, ::piggy::RegisterReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_TryRegister_, context, request, response);
}

void Greeter::Stub::experimental_async::TryRegister(::grpc::ClientContext* context, const ::piggy::RegisterRequest* request, ::piggy::RegisterReply* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_TryRegister_, context, request, response, std::move(f));
}

void Greeter::Stub::experimental_async::TryRegister(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::piggy::RegisterReply* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_TryRegister_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::piggy::RegisterReply>* Greeter::Stub::AsyncTryRegisterRaw(::grpc::ClientContext* context, const ::piggy::RegisterRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::piggy::RegisterReply>::Create(channel_.get(), cq, rpcmethod_TryRegister_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::piggy::RegisterReply>* Greeter::Stub::PrepareAsyncTryRegisterRaw(::grpc::ClientContext* context, const ::piggy::RegisterRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::piggy::RegisterReply>::Create(channel_.get(), cq, rpcmethod_TryRegister_, context, request, false);
}

::grpc::Status Greeter::Stub::TryLogin(::grpc::ClientContext* context, const ::piggy::LoginRequest& request, ::piggy::LoginReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_TryLogin_, context, request, response);
}

void Greeter::Stub::experimental_async::TryLogin(::grpc::ClientContext* context, const ::piggy::LoginRequest* request, ::piggy::LoginReply* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_TryLogin_, context, request, response, std::move(f));
}

void Greeter::Stub::experimental_async::TryLogin(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::piggy::LoginReply* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_TryLogin_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::piggy::LoginReply>* Greeter::Stub::AsyncTryLoginRaw(::grpc::ClientContext* context, const ::piggy::LoginRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::piggy::LoginReply>::Create(channel_.get(), cq, rpcmethod_TryLogin_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::piggy::LoginReply>* Greeter::Stub::PrepareAsyncTryLoginRaw(::grpc::ClientContext* context, const ::piggy::LoginRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::piggy::LoginReply>::Create(channel_.get(), cq, rpcmethod_TryLogin_, context, request, false);
}

::grpc::Status Greeter::Stub::CheckKickOut(::grpc::ClientContext* context, const ::piggy::CheckKickOutRequest& request, ::piggy::CheckKickOutReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_CheckKickOut_, context, request, response);
}

void Greeter::Stub::experimental_async::CheckKickOut(::grpc::ClientContext* context, const ::piggy::CheckKickOutRequest* request, ::piggy::CheckKickOutReply* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_CheckKickOut_, context, request, response, std::move(f));
}

void Greeter::Stub::experimental_async::CheckKickOut(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::piggy::CheckKickOutReply* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_CheckKickOut_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::piggy::CheckKickOutReply>* Greeter::Stub::AsyncCheckKickOutRaw(::grpc::ClientContext* context, const ::piggy::CheckKickOutRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::piggy::CheckKickOutReply>::Create(channel_.get(), cq, rpcmethod_CheckKickOut_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::piggy::CheckKickOutReply>* Greeter::Stub::PrepareAsyncCheckKickOutRaw(::grpc::ClientContext* context, const ::piggy::CheckKickOutRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::piggy::CheckKickOutReply>::Create(channel_.get(), cq, rpcmethod_CheckKickOut_, context, request, false);
}

Greeter::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Greeter_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Greeter::Service, ::piggy::HelloRequest, ::piggy::HelloReply>(
          std::mem_fn(&Greeter::Service::SayHello), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Greeter_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Greeter::Service, ::piggy::RegisterRequest, ::piggy::RegisterReply>(
          std::mem_fn(&Greeter::Service::TryRegister), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Greeter_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Greeter::Service, ::piggy::LoginRequest, ::piggy::LoginReply>(
          std::mem_fn(&Greeter::Service::TryLogin), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Greeter_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Greeter::Service, ::piggy::CheckKickOutRequest, ::piggy::CheckKickOutReply>(
          std::mem_fn(&Greeter::Service::CheckKickOut), this)));
}

Greeter::Service::~Service() {
}

::grpc::Status Greeter::Service::SayHello(::grpc::ServerContext* context, const ::piggy::HelloRequest* request, ::piggy::HelloReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Greeter::Service::TryRegister(::grpc::ServerContext* context, const ::piggy::RegisterRequest* request, ::piggy::RegisterReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Greeter::Service::TryLogin(::grpc::ServerContext* context, const ::piggy::LoginRequest* request, ::piggy::LoginReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Greeter::Service::CheckKickOut(::grpc::ServerContext* context, const ::piggy::CheckKickOutRequest* request, ::piggy::CheckKickOutReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace piggy
