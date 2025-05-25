#include "Client.h"
#include <nlohmann/json.hpp>

Client::Client(boost::asio::io_context& io) try :
	_ssl_context(boost::asio::ssl::context::tlsv12_client),
	_socket(io, _ssl_context),
	_strand(boost::asio::make_strand(io))
{
	//������ ���� ��� (���� �������� �߱޹����� verify_peer�� ����)
	_ssl_context.set_verify_mode(boost::asio::ssl::verify_none);

	//���� ���� �� ��������Ʈ ����
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket(io, _ssl_context);
	boost::asio::ip::tcp::resolver resolver(io);
	_endpoints = resolver.resolve("127.0.0.1", "6799");

	//�񵿱�� Conenct
	Start_Connect();
}
catch (const std::exception& e)
{
	//std::cout << "Client Error : " << e.what() << std::endl;
}


void Client::Start_Connect()
{
	boost::asio::async_connect(
		_socket.lowest_layer(),
		_endpoints,
		[this](const boost::system::error_code& error, const boost::asio::ip::tcp::endpoint& endppoint) 
		{		
			//Connect ���� �� �񵿱�� �ڵ彦��ũ ����
			if (!error)
			{
				Start_Handshake();
			}
		}
	);
}

void Client::Start_Handshake()
{
	_socket.async_handshake(
	boost::asio::ssl::stream_base::client,
		[this](const boost::system::error_code& error)
		{
			if (!error)
			{
				Start_Read();
			}
		}	
	);
}

void Client::Start_Read()
{
	//1. ���� �� �б� (4����Ʈ)
	_socket.async_read_some(boost::asio::buffer(_buf),
		[this](const boost::system::error_code& error, std::size_t len1)
		{
			if (!error)
			{
				//2. ���� ũ�⸦ ������ �Ľ�
				int size = std::stoi(_buf.data());
				//3. ���� �����Ͱ� ����Ǵ� ����
				auto dataBuf = std::make_shared<std::vector<char>>(size);

				//4. ���� ���� ���� ������ �б�
				_socket.async_read_some(boost::asio::buffer(*dataBuf),
					[this, dataBuf](const boost::system::error_code& ec, std::size_t len2)
					{
						if (!ec)
						{
							Start_Read();
							Start_Dispatch(dataBuf->data());
						}
					});
			}
		}
	);
}

void Client::Start_Write(std::string str)
{
	//�񵿱� �۾��� �Ϸ�� ������ ���۰� ����־���ϹǷ� ����Ʈ �����ͷ� �ʱ�ȭ
	auto buffer = std::make_shared<std::string>(str);

	//strand�� ��� �ߺ� ���� ����
	_socket.async_write_some(boost::asio::buffer(*buffer),
		boost::asio::bind_executor(_strand,
		[this, buffer](const boost::system::error_code& error, std::size_t len)
		{
				if (!error)
				{
					/*
					
					Write �Ϸ�
					
					*/
				}
		}
	));
}

void Client::Start_Dispatch(std::string str)
{
	nlohmann::json j;
	j = nlohmann::json::parse(str);

	if (j["type"] == "Login_Success")
	{
		std::cout << "";
	}
	else if (j["type"] == "Login_Failed")
	{

		std::cout << "";
	}
}