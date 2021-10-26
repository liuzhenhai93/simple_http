#pragma once

#include <string.h>
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "mongoose/mongoose.h"

typedef void OnRspCallback(mg_connection *c, const std::string &rsp);
using ReqHandler = std::function<bool(const std::string &, const std::string &,
                                      mg_connection *c, OnRspCallback)>;

class SimpleHttpServer {
 public:
  SimpleHttpServer() : terminate_(false) {}
  ~SimpleHttpServer() {}
  void Init(const std::string &port);
  bool Start();
  void Stop();
  void AddHandler(const std::string &url, ReqHandler req_handler);
  void RemoveHandler(const std::string &url);
  static std::string s_web_dir;
  static mg_serve_http_opts s_server_option;
  static std::unordered_map<std::string, ReqHandler> s_handler_map;

 private:
  void Loop();
  bool Close();
  static void OnHttpEvent(mg_connection *connection, int event_type,
                          void *event_data);

  static void HandleHttpEvent(mg_connection *connection,
                              http_message *http_req);
  static void SendHttpRsp(mg_connection *connection, const std::string &rsp);
  static void SendError(mg_connection *connection, const std::string &uri,
                        int err_code, const std::string &reply);

  std::string m_port_;
  mg_mgr m_mgr_;
  std::atomic<bool> terminate_;
  std::unique_ptr<std::thread> loop_thread_;
};
