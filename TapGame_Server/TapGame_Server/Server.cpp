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
	//���� ������ ������
	auto shared(shared_from_this());

	_socket.async_handshake(boost::asio::ssl::stream_base::server,
		[this, shared](const boost::system::error_code& error)
		{
			//�ڵ彦��ũ ���� ��
			if (!error)
			{
				Start_Read();
			}
		}
	);
}

void Session::Start_Read()
{
	//���� ������ ������
	auto shared(shared_from_this());

	//1. ���� �� �б� (4����Ʈ)
	_socket.async_read_some(boost::asio::buffer(_buf),
		[this, shared](const boost::system::error_code& error, std::size_t len1)
		{
			if (!error)
			{
				//2. ���� ũ�⸦ ������ �Ľ�
				int size = std::stoi(_buf.data());
				//3. ���� �����Ͱ� ����Ǵ� ����
				auto dataBuf = std::make_shared<std::vector<char>>(size);

				//4. ���� ���� ���� ������ �б�
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
	//���� ������ ������
	auto shared(shared_from_this());

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

Server::Server(boost::asio::io_context& io, unsigned short port) :
	acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	context_(boost::asio::ssl::context::sslv23),
	_strand(boost::asio::make_strand(io))
{
	context_.set_options(
		boost::asio::ssl::context::default_workarounds |				//��ȸ �۾� ���
		boost::asio::ssl::context::single_dh_use |						//DH �Ķ���� ���� ���ΰ�ħ
		boost::asio::ssl::context::no_sslv2 |							//sslv2 ����
		boost::asio::ssl::context::no_sslv3 |							//sslv3	����
		boost::asio::ssl::context::no_tlsv1 |							//tlsv1.0 ����
		boost::asio::ssl::context::no_tlsv1_1							//tlsv1.1 ����
	);

	//�ڵ彦��ũ�� ���� �������� Ű�� �ε�
	context_.use_certificate_file("Server.crt", boost::asio::ssl::context::pem);
	context_.use_private_key_file("Server.key", boost::asio::ssl::context::pem);

	//dh Ű��ȯ ������� ����
	context_.use_tmp_dh_file("dh2048.pem");

	//�񵿱�� Accept ����
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

		//��й�ȣ ����
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