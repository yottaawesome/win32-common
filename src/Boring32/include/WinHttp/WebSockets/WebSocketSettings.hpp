#pragma once
#include <string>
#include "../Session.hpp"
#include "../../Crypto/Certificate.hpp"

namespace Boring32::WinHttp::WebSockets
{
	struct WebSocketSettings
	{
		std::wstring UserAgent;
		std::wstring Server;
		unsigned int Port = 0;
		bool IgnoreSslErrors = false;
		std::wstring ConnectionHeaders;
		Session WinHttpSession;
		Crypto::Certificate ClientCert;
	};
}