#include "Client.h"
#include <nlohmann/json.hpp>

Client::Client(boost::asio::io_context& io) try :
	_ssl_context(boost::asio::ssl::context::tlsv12_client),
	_socket(io, _ssl_context),
	_strand(boost::asio::make_strand(io))
{
	//인증서 검증 방식 (실제 인증서를 발급받으면 verify_peer로 변경)
	_ssl_context.set_verify_mode(boost::asio::ssl::verify_none);

	//소켓 생성 및 엔드포인트 설정
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket(io, _ssl_context);
	boost::asio::ip::tcp::resolver resolver(io);
	_endpoints = resolver.resolve("127.0.0.1", "6799");

	//비동기로 Conenct
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
			//Connect 성공 시 비동기로 핸드쉐이크 시작
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
	//1. 글자 수 읽기 (4바이트)
	_socket.async_read_some(boost::asio::buffer(_buf),
		[this](const boost::system::error_code& error, std::size_t len1)
		{
			if (!error)
			{
				//2. 구한 크기를 정수로 파싱
				int size = std::stoi(_buf.data());
				//3. 실제 데이터가 저장되는 버퍼
				auto dataBuf = std::make_shared<std::vector<char>>(size);

				//4. 글자 수를 토대로 데이터 읽기
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
	//비동기 작업이 완료될 때까지 버퍼가 살아있어야하므로 스마트 포인터로 초기화
	auto buffer = std::make_shared<std::string>(str);

	//strand로 묶어서 중복 실행 방지
	_socket.async_write_some(boost::asio::buffer(*buffer),
		boost::asio::bind_executor(_strand,
		[this, buffer](const boost::system::error_code& error, std::size_t len)
		{
				if (!error)
				{
					/*
					
					Write 완료
					
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