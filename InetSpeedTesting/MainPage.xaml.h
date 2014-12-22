//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "pplpp.h"
#include "Datum.h"
#include <time.h>

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace pplpp;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;

namespace InetSpeedTesting
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		double currentSpeed;
		double currentDownloadBandwidth;
		double payloadLength;
		int retries;
		Vector<Datum^>^ resultsVector;
		String^ aggregate;
		StreamSocket^ clientSocket[4];
		DataWriter^ writer[4];
		//tasks must complete in a fixed amount of time, cancel otherwise..
		timed_cancellation_token_source tcs[4];
		long long task_timeout_ms;
		std::chrono::milliseconds timeout;
		String^ _socketTcpWellKnownHostNames[4];
	};
}

using namespace InetSpeedTesting;
