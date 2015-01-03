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
	
	auto flow = [this]
	{
		StreamSocket^ socket = ref new StreamSocket();
		Array<String^>^ hostNames = ref new Array<String^>{ "google.com","abc.com","bing.com","msn.com"};
		unsigned int hostNamesSize = 4;
		Vector<Datum^>^ resultsVector = ref new Vector<Datum^>();
		_testList->ItemsSource = resultsVector;
		auto procession = [socket, resultsVector, hostNames]() mutable
		{
			return create_task(socket->ConnectAsync(ref new HostName(hostNames[0]), ref new String(L"80")))
				.then([socket, resultsVector, hostNames]() mutable
			{
				Array<StreamSocket^>^ streamSocket = ref new Array<StreamSocket^>(3);
				for (auto i = 0; i < 3; i++) streamSocket[i] = ref new StreamSocket();
				for (auto i = 0; i < 3; i++) streamSocket[i]->ConnectAsync(ref new HostName(hostNames[i + 1]), ref new String(L"80"));
				float32 speed = socket->Information->RoundTripTimeStatistics.Min / 1000000.0F;
				String^ possible("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
				String^ aggregate = "";
				unsigned int payloadSize = 4800;
				for (unsigned int i = 0; i < payloadSize; i++, aggregate += possible->Begin()[rand() % possible->Length()]);
				DataWriter^ dataWriter = ref new DataWriter(socket->OutputStream);
				dataWriter->WriteString(aggregate);
				auto beginTime = clock();
				create_task(dataWriter->StoreAsync()).then([beginTime, speed, socket, streamSocket, resultsVector, payloadSize](unsigned int previousTask) mutable
				{
					auto endTime = clock();
					Array<DataWriter^>^ writer = ref new Array<DataWriter^>(3);
					for (auto i = 0; i < 3; i++) writer[i] = ref new DataWriter(streamSocket[i]->OutputStream);
					for (auto i = 0; i < 3; i++) writer[i]->StoreAsync();
					auto uploadBandwidth = payloadSize / (float32)(endTime - beginTime) / 10 * CLOCKS_PER_SEC / 1024;
					if (socket->Information->BandwidthStatistics.OutboundBandwidthPeaked == true) resultsVector->Append(ref new Datum(0, speed, socket->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024.0F));
					else resultsVector->Append(ref new Datum(0, speed, uploadBandwidth));
					for (auto i = 0; i < 3; i++) resultsVector->Append(ref new Datum(i+1, streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F,streamSocket[i]->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024.0F));
				});
			});
		};
		procession();
	};

	// Main execution flow
	//--------------------
	flow();
}


