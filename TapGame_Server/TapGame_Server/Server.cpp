#include "Server.h"
#include "Utility.h"
#include "Database.h"
#include <nlohmann/json.hpp>


Session::Session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& context, boost::asio::strand<boost::asio::io_context::executor_type>& strand) :
	_socket(std::move(socket), context),
	_strand(strand)
{

}

void Session::Start()
{
	Start_Handshake();
}

void Session::Start_Handshake()
{
	//세션 유지용 포인터
	auto shared(shared_from_this());

	_socket.async_handshake(boost::asio::ssl::stream_base::server,
		[this, shared](const boost::system::error_code& error)
		{
			//핸드쉐이크 성공 시
			if (!error)
			{
				Start_Read();
			}
		}
	);
}

void Session::Start_Read()
{
	//세션 유지용 포인터
	auto shared(shared_from_this());

	//1. 글자 수 읽기 (4바이트)
	_socket.async_read_some(boost::asio::buffer(_buf),
		[this, shared](const boost::system::error_code& error, std::size_t len1)
		{
			if (!error)
			{
				//2. 구한 크기를 정수로 파싱
				int size = std::stoi(_buf.data());
				//3. 실제 데이터가 저장되는 버퍼
				auto dataBuf = std::make_shared<std::vector<char>>(size);

				//4. 글자 수를 토대로 데이터 읽기
				_socket.async_read_some(boost::asio::buffer(*dataBuf),
					[this, shared, dataBuf](const boost::system::error_code& ec, std::size_t len2)
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

void Session::Start_Write(std::string str)
{
	//세션 유지용 포인터
	auto shared(shared_from_this());

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

Server::Server(boost::asio::io_context& io, unsigned short port) :
	acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	context_(boost::asio::ssl::context::sslv23),
	_strand(boost::asio::make_strand(io))
{
	context_.set_options(
		boost::asio::ssl::context::default_workarounds |				//우회 작업 허용
		boost::asio::ssl::context::single_dh_use |						//DH 파라미터 강제 새로고침
		boost::asio::ssl::context::no_sslv2 |							//sslv2 금지
		boost::asio::ssl::context::no_sslv3 |							//sslv3	금지
		boost::asio::ssl::context::no_tlsv1 |							//tlsv1.0 금지
		boost::asio::ssl::context::no_tlsv1_1							//tlsv1.1 금지
	);

	//핸드쉐이크를 위해 인증서와 키를 로드
	context_.use_certificate_file("Server.crt", boost::asio::ssl::context::pem);
	context_.use_private_key_file("Server.key", boost::asio::ssl::context::pem);

	//dh 키교환 방식으로 변경
	context_.use_tmp_dh_file("dh2048.pem");

	//비동기로 Accept 시작
	Start_Accept();
}

void Server::Start_Accept()
{
	acceptor_.async_accept(
		[this](const boost::system::error_code& error, boost::asio::ip::tcp::socket socket)
		{
			if (!error)
				std::make_shared<Session>(std::move(socket), context_, _strand)->Start();

			Start_Accept();
		}
	);
}

void Session::Start_Dispatch(std::string str)
{
	nlohmann::json j;
	j = nlohmann::json::parse(str);

	if(j["type"] == "Login")
	{
		nlohmann::json newJ;

		//비밀번호 대조
		if (Utility::verifyPassword(j["password"], Database::GetInstance().GetPassword(j["id"])))
			newJ["type"] = "Login_Success";
		else		
			newJ["type"] = "Login_Failed";
		

		std::string newStr = newJ.dump();
		newStr = Utility::fillZero(std::to_string(newStr.size()), 4) + newStr;
		Start_Write(newStr);
	}
	else if (j["type"] == "Register")
	{


	}
}