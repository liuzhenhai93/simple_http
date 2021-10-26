#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include "httpserver/simple_http_server.h"
#include "utils/base_string.h"

mg_serve_http_opts SimpleHttpServer::s_server_option;
std::string SimpleHttpServer::s_web_dir = "./web";
std::unordered_map<std::string, ReqHandler> SimpleHttpServer::s_handler_map;

bool handle_fun1(const std::string& url, const std::string& body, mg_connection *c, OnRspCallback rsp_callback)
{

	std::cout << "handle fun1" << std::endl;
	std::cout << "url: " << url << std::endl;
	std::cout << "body: " << body << std::endl;
	rsp_callback(c, "rsp1");
	return true;
}

bool hello(const std::string& url, const std::string& body, mg_connection *c, OnRspCallback rsp_callback)
{

  std::cout << "handle fun1" << std::endl;
  std::cout << "url: " << url << std::endl;
  std::cout << "body: " << body << std::endl;
  rsp_callback(c, "welcome to httpserver");
  return true;
}

bool sum(const std::string& url, const std::string& body, mg_connection *c, OnRspCallback rsp_callback)
{

  std::cout << "handle fun1" << std::endl;
  std::cout << "url: " << url << std::endl;
  std::cout << "body: " << body << std::endl;
  std::string raw_body = BaseString::trim(body);
  std::vector<std::string> items;
  BaseString::Split(raw_body, items, "&");
  std::map<std::string, std::string> params;
  for(const auto& e: items){
    std::vector<std::string> key_val;
    BaseString::Split(e,key_val, "=");
    std::string key = key_val[0];
    std::string val = "0";
    if(key_val.size() == 2){
      val = key_val[1];
    }
    params[key] = val;
  }
  double n1 = std::stod(params.at("n1"));
  double n2 = std::stod(params.at("n2"));
  double result = n1 + n2;
  rsp_callback(c, std::to_string(result));
  return true;
}

bool handle_fun2(const std::string& url, const std::string& body, mg_connection *c, OnRspCallback rsp_callback)
{
	std::cout << "handle fun2" << std::endl;
	std::cout << "url: " << url << std::endl;
	std::cout << "body: " << body << std::endl;
	rsp_callback(c, "rsp2");
	return true;
}

int main(int argc, char *argv[]) 
{
	std::string port = "7999";
	auto http_server = std::shared_ptr<SimpleHttpServer>(new SimpleHttpServer);
	http_server->Init(port);
	http_server->AddHandler("/api/hello", hello);
  http_server->AddHandler("/api/sum", sum);
  http_server->AddHandler("/api/fun1", handle_fun1);
	http_server->AddHandler("/api/fun2", handle_fun2);
	http_server->Start();
  std::cout << "server started, exit";
	sleep(60);
  http_server->Stop();
  std::cout << "server closed, exit";
	return 0;
}