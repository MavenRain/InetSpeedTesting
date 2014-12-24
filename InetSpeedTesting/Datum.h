namespace InetSpeedTesting
{
	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Datum sealed
	{
		uint32 _testNum;
		float32 _rtt;
		float32 _downloadBW;
	public:
		Datum(uint32, float32, float32);
		property uint32 TestNum {uint32 get(void); void set(uint32);  }
		property float32 RTT {float32 get(void); void set(float32); }
		property float32 DownloadBW {float32 get(void); void set(float32); }
	};
}

using namespace InetSpeedTesting;