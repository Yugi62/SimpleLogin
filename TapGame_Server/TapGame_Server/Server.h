#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <memory>
#include <array>

class Session : public std::enable_shared_from_this<Session>
{
private:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _socket;
	boost::asio::strand<boost::asio::io_context::executor_type>& _strand;
	std::array<char, 4> _buf;

	void Start_Handshake();
	void Start_Read();
	void Start_Write(std::string str);
	void Start_Dispatch(std::string str);

public:
	Session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& context, boost::asio::strand<boost::asio::io_context::executor_type>& strand);
	void Start();
};

class Server
{
private:
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ssl::context context_;
	boost::asio::strand<boost::asio::io_context::executor_type> _strand;

	//비동기로 Accpet 시작
	void Start_Accept();

public:
	Server(boost::asio::io_context& io, unsigned short port);	
};
