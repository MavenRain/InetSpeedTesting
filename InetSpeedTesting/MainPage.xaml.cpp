//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();
}


void InetSpeedTesting::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	
	//Lambda flow
	//-------------------------------------------------------------------------------------

	auto currentSpeed = [] 
	{
		HostName^ hostNames[] = {ref new HostName(ref new String(L"google.com")),ref new HostName(ref new String(L"bing.com")), ref new HostName(ref new String(L"facebook.com")),ref new HostName(ref new String(L"twitter.com"))};
		auto retries = 4;
		float32 _currentSpeed = 0.0;
		for (auto i = 0; i < retries; i++) 
		{
			StreamSocket^ streamSocket = ref new StreamSocket();
			streamSocket->Control->NoDelay = true;
			streamSocket->Control->QualityOfService = SocketQualityOfService::LowLatency;
			streamSocket->Control->KeepAlive = false;
			create_task(streamSocket->ConnectAsync(hostNames[i], "80", SocketProtectionLevel::PlainSocket)).then([&](void)
			{
				_currentSpeed += streamSocket->Information->RoundTripTimeStatistics.Min / 1000000;
			});
			
			delete streamSocket;
		};
		return _currentSpeed/retries;
	};

	auto downloadBandwidth = [] 
	{
		Uri^ uri[] = { ref new Uri(ref new String(L"http://www.google.com")),ref new Uri(ref new String(L"http://www.bing.com")),ref new Uri(ref new String(L"http://www.facebook.com")),ref new Uri(ref new String(L"http://www.twitter.com")) };
		auto retries = 4;
		float32 _downloadBandwidth = 0.0;
		float32 timeElapsed = 0.0;
		HttpClient^ httpClient = ref new HttpClient();
		for (auto i = 0; i < retries; i++)
		{
			auto beginTime = clock();
			clock_t endTime = 0;
			create_task(httpClient->GetAsync(uri[i])).then([&beginTime, &endTime, &timeElapsed](HttpResponseMessage^ response)
			{
				endTime = clock();
				timeElapsed = (endTime - beginTime) * 10 / CLOCKS_PER_SEC;
				return response->Content->ReadAsInputStreamAsync();
			}).then([](IInputStream^ inputStream)
			{
				IBuffer^ buffer = ref new Buffer(10000);
				return inputStream->ReadAsync(buffer, buffer->Capacity, InputStreamOptions::Partial);
			}).then([&_downloadBandwidth, &timeElapsed](IBuffer^ readBuffer)
			{
				if (timeElapsed <= 0) return;
				_downloadBandwidth += readBuffer->Length / 1000 / timeElapsed;
			});
		};
		return _downloadBandwidth / retries;
	};

	//---------------------------------------------------------------------------------------------------------------------------------------------
	//End lambda flow

	//Main execution flow
	//----------------------------------------------------------------------------
	Vector<Datum^>^ resultsVector = ref new Vector<Datum^>();
	_testList->ItemsSource = resultsVector;
	for (auto j = 0; j < 4; j++)
	{
		resultsVector->Append(ref new Datum(j, currentSpeed(), downloadBandwidth()));
	};
	//----------------------------------------------------------------------------
	//End main execution flow
}
