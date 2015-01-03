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
		unsigned int _trials = 100;
		Array<StreamSocket^>^ streamSocket = ref new Array<StreamSocket^>(_trials);
		for (unsigned int i = 0; i < streamSocket->Length; i++) streamSocket[i] = ref new StreamSocket();
		Array<String^>^ hostNames = ref new Array<String^>{ "google.com","abc.com","bing.com","msn.com"};
		Vector<Datum^>^ resultsVector = ref new Vector<Datum^>();
		_testList->ItemsSource = resultsVector;
		auto procession = [streamSocket, resultsVector, hostNames, _trials]() mutable
		{
			return create_task(streamSocket[0]->ConnectAsync(ref new HostName(hostNames[0]), ref new String(L"80")))
				.then([streamSocket, resultsVector, hostNames, _trials]() mutable
			{
				for (unsigned int i = 1; i < streamSocket->Length; i++) streamSocket[i]->ConnectAsync(ref new HostName(hostNames[(i+1) % hostNames->Length]), ref new String(L"80"));
				String^ possible("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
				String^ aggregate = "";
				unsigned int payloadSize = 4800;
				for (unsigned int i = 0; i < payloadSize; i++, aggregate += possible->Begin()[rand() % possible->Length()]);
				Array<DataWriter^>^ writer = ref new Array<DataWriter^>(_trials);
				for (unsigned int i = 0; i < writer->Length; i++) writer[i] = ref new DataWriter(streamSocket[i]->OutputStream);
				for (unsigned int i = 0; i < writer->Length; i++) writer[i]->WriteString(aggregate);
				auto beginTime = clock();
				create_task(writer[0]->StoreAsync()).then([beginTime, streamSocket, writer, resultsVector, payloadSize](unsigned int previousTask) mutable
				{
					auto endTime = clock();
					//for (unsigned int i = 1; i < writer->Length; i++) writer[i]->StoreAsync();
					auto uploadBandwidth = payloadSize / (float32)(endTime - beginTime) / 10 * CLOCKS_PER_SEC / 1024;
					if (streamSocket[0]->Information->BandwidthStatistics.OutboundBandwidthPeaked == true) resultsVector->Append(ref new Datum(0, streamSocket[0]->Information->RoundTripTimeStatistics.Min / 1000000.0F, streamSocket[0]->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024.0F));
					else resultsVector->Append(ref new Datum(0, streamSocket[0]->Information->RoundTripTimeStatistics.Min / 1000000.0F, uploadBandwidth));
					for (unsigned int i = 1; i < streamSocket->Length; i++) resultsVector->Append(ref new Datum(i, streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F,streamSocket[i]->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024.0F));
				});
			});
		};
		procession();
	};

	// Main execution flow
	//--------------------
	flow();
}


