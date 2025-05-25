#include <vector>
#include <thread>
#include "Server.h"
#include "Database.h"


int main()
{
	//데이터베이스 싱글톤 초기화
	Database::GetInstance().Init();

	std::vector<std::thread> threads;

	boost::asio::io_context io;
	Server server(io, 6799);

	//CPU 개수만큼 스레드 생성 후 내부에서 run 함수 호출
	for (int i = 0; i < std::thread::hardware_concurrency(); i++)
		threads.emplace_back([&io]() {io.run(); });

	//모든 스레드 종료될 때까지 대기
	for (auto& t : threads)	
		t.join();

	return 0;
}