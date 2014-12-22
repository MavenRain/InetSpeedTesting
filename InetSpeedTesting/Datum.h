namespace InetSpeedTesting
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Datum sealed
	{
		uint32 TestNum;
		float32 RTT;
		float32 DownloadBW;
	public:
		Datum(uint32, float32, float32);
	};
}

using namespace InetSpeedTesting;