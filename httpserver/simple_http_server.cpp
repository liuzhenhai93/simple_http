#include <utility>
#include <string>


#include "httpserver/simple_http_server.h"

void SimpleHttpServer::Init(const std::string &port)
{
	m_port_ = port;
	s_server_option.enable_directory_listing = "yes";
	s_server_option.document_root = s_web_dir.c_str();
}

bool SimpleHttpServer::Start()
{
	mg_mgr_init(&m_mgr_, NULL);
	mg_connection *connection = mg_bind(&m_mgr_, m_port_.c_str(), SimpleHttpServer::OnHttpEvent);
	if (connection == NULL)
		return false;
	mg_set_protocol_http_websocket(connection);
	printf("starting http server at port: %s\n", m_port_.c_str());
	loop_thread_.reset(new std::thread(&SimpleHttpServer::Loop,this));
	return true;
}

void SimpleHttpServer::Stop(){
  if(terminate_){
    return;
  }
  terminate_ = true;
  loop_thread_->join();
  loop_thread_ = nullptr;
  Close();
}

void SimpleHttpServer::OnHttpEvent(mg_connection *connection, int event_type, void *event_data)
{
	switch(event_type){
	  case MG_EV_HTTP_REQUEST:
    {
      http_message *http_req = (http_message *)event_data;
      HandleHttpEvent(connection, http_req);
      break;
    }
    default:
      break;
	}
}

void SimpleHttpServer::AddHandler(const std::string &url, ReqHandler req_handler)
{
	if (s_handler_map.find(url) != s_handler_map.end())
		return;

	s_handler_map.insert(std::make_pair(url, req_handler));
}

void SimpleHttpServer::RemoveHandler(const std::string &url)
{
	auto it = s_handler_map.find(url);
	if (it != s_handler_map.end())
		s_handler_map.erase(it);
}

void SimpleHttpServer::SendHttpRsp(mg_connection *connection, const std::string& rsp)
{
	mg_printf(connection, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
	mg_printf_http_chunk(connection, "{ \"result\": %s }", rsp.c_str());
	mg_send_http_chunk(connection, "", 0);
}

void SimpleHttpServer::HandleHttpEvent(mg_connection *connection, http_message *http_req)
{
	std::string req_str = std::string(http_req->message.p, http_req->message.len);
	printf("got request: %s\n", req_str.c_str());
	std::string url = std::string(http_req->uri.p, http_req->uri.len);
	std::string body = std::string(http_req->body.p, http_req->body.len);
	auto it = s_handler_map.find(url);
	if (it != s_handler_map.end())
	{
		ReqHandler handle_func = it->second;
		handle_func(url, body, connection, &SimpleHttpServer::SendHttpRsp);
	} else if(url == "/"){
    mg_serve_http(connection, http_req, s_server_option);
	} else {
    SendError(connection, url,  501, " Not Implemented");
	}
}

void SimpleHttpServer::SendError(mg_connection *connection, const std::string& uri, int err_code, const std::string& reply){
  mg_printf(
      connection,
      "HTTP/1.1 %d %s\r\n"
      "Content-Length: 0\r\n\r\n",
      err_code, reply.c_str());
}

void SimpleHttpServer::Loop(){
  while(!terminate_){
      mg_mgr_poll(&m_mgr_, 300);
  }
}

bool SimpleHttpServer::Close()
{
	mg_mgr_free(&m_mgr_);
	return true;
}