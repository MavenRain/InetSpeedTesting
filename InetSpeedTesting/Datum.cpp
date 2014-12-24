#include "pch.h"
#include "Datum.h"
InetSpeedTesting::Datum::Datum(uint32 num, float32 rtt, float32 download) : _testNum(num), _rtt(rtt), _downloadBW(download) {}

uint32 InetSpeedTesting::Datum::TestNum::get()
{
	return _testNum;
}

void InetSpeedTesting::Datum::TestNum::set(uint32 value)
{
	_testNum = value;
}

float32 InetSpeedTesting::Datum::RTT::get()
{
	return _rtt;
}

void InetSpeedTesting::Datum::RTT::set(float32 value)
{
	_rtt = value;
}

float32 InetSpeedTesting::Datum::DownloadBW::get()
{
	return _downloadBW;
}

void InetSpeedTesting::Datum::DownloadBW::set(float32 value)
{
	_downloadBW = value;
}





