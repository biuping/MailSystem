#include "CppUnitTest.h"
#include "HttpRequest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HttpTest
{
	TEST_CLASS(HttpTest)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			//HttpRequest* request = new HttpRequest();
			const char* post_request_text = "POST / HTTP/1.1\r\nContent-Type: application/json\r\nHost: 127.0.0.1:8006\r\nConnection: keep-alive\r\nContent-Length: 31\r\n\r\n{\n\t\"name\":\"13\",\n\t\"type\":\"pop\"\n}";
			//request->load_packet(post_request_text, strlen(post_request_text));
			std::cout << post_request_text;
		}
	};
}
