#pragma once

#include <iostream>
#include <memory>
#include <array>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class Client
{
private:
	boost::asio::ssl::context _ssl_context;
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _socket;
	boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> _endpoints;
	boost::asio::strand<boost::asio::io_context::executor_type> _strand;

	std::array<char, 4> _buf;

	void Start_Connect();
	void Start_Handshake();
	void Start_Read();
	void Start_Dispatch(std::string str);

public:
	Client(boost::asio::io_context& io);
	void Start_Write(std::string str);
};