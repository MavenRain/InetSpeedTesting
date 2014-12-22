#include "pch.h"
#include "Datum.h"
InetSpeedTesting::Datum::Datum(uint16 num, float32 rtt, float32 download) :TestNum(num), RTT(rtt), DownloadBW(download){}