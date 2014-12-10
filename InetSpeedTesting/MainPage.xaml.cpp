//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
using namespace InetSpeedTesting;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Array<String^>^ _socketTcpWellKnownHostNames = ref new Array<String^>(4){ "google.com", "bing.com", "facebook.com", "yahoo.com" };

MainPage::MainPage()
{
	InitializeComponent();
	_payloadChoices = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

ListView^ MainPage::TestList::get() { return _testList; }
void MainPage::TestList::set(ListView^ view) { _testList = view; }


void InetSpeedTesting::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([&]()
	{
	double currentSpeed = 0.0;
	double currentUploadBandwidth = 0.0;
	double currentDownloadBandwidth = 0.0;
	double payloadLength = 1024;
	uint8 trials = 10;

	
	String^ aggregate = "";
	auto it = _payloadChoices->Begin();
	for (int i = 0; i < payloadLength; i++, aggregate += it[rand() % _payloadChoices->Length()]);

	StreamSocket^ _clientSocket = ref new StreamSocket();
	_clientSocket->Control->NoDelay = true;
	_clientSocket->Control->QualityOfService = SocketQualityOfService::LowLatency;
	_clientSocket->Control->KeepAlive = false;
	

	DataWriter^ writer = ref new DataWriter(_clientSocket->OutputStream);

	//tasks must complete in a fixed amount of time, cancel otherwise..
	timed_cancellation_token_source tcs;
	std::chrono::milliseconds timeout(task_timeout_ms);

	auto handleExceptions = [this,&currentSpeed](task<void> previousTask)
	{
		try { previousTask.get(); }
		catch (Platform::COMException^ e)
		{
			currentSpeed = 0.0;
			retries--;
		}
		catch (task_canceled&)
		{
			currentSpeed = 0.0;
			retries--;
		}
	};

	//run network testing and record results
	for (uint8 j = 0; j < trials; j++)
	{
		for (int i = 0; i < retries; ++i)
		{
			HostName^ _serverHost = ref new HostName(_socketTcpWellKnownHostNames[i]);
			

			create_task([&]
			{
				

				tcs.cancel(timeout);
				
					_clientSocket->ConnectAsync(_serverHost, "80");
				
				
				
			}, tcs.get_token()).then([&]
			{
				writer->WriteString(aggregate);
				writer->StoreAsync();
				currentSpeed += _clientSocket->Information->RoundTripTimeStatistics.Min / 1000000.0;
				currentUploadBandwidth += _clientSocket->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024; //Kbps
				currentDownloadBandwidth += _clientSocket->Information->BandwidthStatistics.InboundBitsPerSecond / 1024; //Kbps
			}).then(handleExceptions);

			delete writer;
			delete _clientSocket;
		}
		_resultsVector->Append(ref new Datum(j, currentSpeed / retries, currentUploadBandwidth / retries, currentDownloadBandwidth / retries));
	}

	//Marshal results to UI
	this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
	{
		TestList->ItemsSource = _resultsVector;
	}));
	}));
	
}
