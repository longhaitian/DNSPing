// This code is part of DNSPing(Linux)
// DNSPing, Ping with DNS requesting.
// Copyright (C) 2014 Chengr28
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "DNSPing.h"

extern std::string TestDomain, TargetString;
extern long double TotalTime, MaxTime, MinTime;
extern size_t SendNum, RealSendNum, RecvNum, TransmissionInterval, BufferSize, RawDataLen;
extern sockaddr_storage SockAddr;
extern uint16_t Port;
extern std::shared_ptr<char> RawData;
extern int IP_HopLimits;
extern timeval SocketTimeout;
extern bool /* IPv4_DF, */ EDNS0;
extern dns_hdr HeaderParameter;
extern dns_qry QueryParameter;
extern dns_edns0_label EDNS0Parameter;

//Send DNS requesting process
size_t SendProcess(const uint16_t Protocol, const sockaddr_storage Target)
{
//Initialization
	std::shared_ptr<char> Buffer(new char[BufferSize]()), RecvBuffer(new char[BufferSize]());
	ssize_t DataLength = 0;
	timeval BeforeTime = {0}, AfterTime = {0};
	int Socket = 0;
	socklen_t AddrLen = 0;

//IPv6
	if (Protocol == AF_INET6)
	{
	//Socket initialization
		AddrLen = sizeof(sockaddr_in6);
		Socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		if (Socket == RETURN_ERROR)
		{
			wprintf(L"Socket initialization error, error code is %d.\n", errno);
			return EXIT_FAILURE;
		}
	}
//IPv4
	else {
	//Socket initialization
		AddrLen = sizeof(sockaddr_in);
		Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (Socket == RETURN_ERROR)
		{
			wprintf(L"Socket initialization error, error code is %d.\n", errno);
			return EXIT_FAILURE;
		}
	}

//Set socket timeout.
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, &SocketTimeout, sizeof(timeval)) == RETURN_ERROR || 
		setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, &SocketTimeout, sizeof(timeval)) == RETURN_ERROR)
	{
		wprintf(L"Set UDP socket timeout error, error code is %d.\n", errno);
		return EXIT_FAILURE;
	}

//Set IP options.
	if (Protocol == AF_INET6) //IPv6
	{
		if (IP_HopLimits != 0 && setsockopt(Socket, IPPROTO_IP, IPV6_UNICAST_HOPS, &IP_HopLimits, sizeof(int)) == RETURN_ERROR)
		{
			wprintf(L"Set HopLimit/TTL flag error, error code is %d.\n", errno);
			return EXIT_FAILURE;
		}
	}
	else { //IPv4
		if (IP_HopLimits != 0 && setsockopt(Socket, IPPROTO_IP, IP_TTL, &IP_HopLimits, sizeof(int)) == RETURN_ERROR)
		{
			wprintf(L"Set HopLimit/TTL flag error, error code is %d.\n", errno);
			return EXIT_FAILURE;
		}
/*
	//Set "Don't Fragment" flag.
	//All Non-SOCK_STREAM will set "Don't Fragment" flag(Linux).
		int iIPv4_DF = IP_PMTUDISC_DO;
		if (IPv4_DF && setsockopt(Socket, IPPROTO_IP, IP_MTU_DISCOVER, &iIPv4_DF, sizeof(int)) == RETURN_ERROR)
		{
			wprintf(L"Set Don't Fragment flag error, error code is %d.\n", errno);
			return EXIT_FAILURE;
		}
*/
	}

//Make packet.
	if (!RawData)
	{
		memcpy(Buffer.get(), &HeaderParameter, sizeof(dns_hdr));
		if (HeaderParameter.ID == 0)
		{
			auto pdns_hdr = (dns_hdr *)Buffer.get();
			pdns_hdr->ID = htons(pthread_self());
		}
		DataLength += sizeof(dns_hdr);
		DataLength += CharToDNSQuery((char *)TestDomain.c_str(), Buffer.get() + sizeof(dns_hdr));
		memcpy(Buffer.get() + DataLength, &QueryParameter, sizeof(dns_qry));
		DataLength += sizeof(dns_qry);
		if (EDNS0)
		{
			memcpy(Buffer.get() + DataLength, &EDNS0Parameter, sizeof(dns_edns0_label));
			DataLength += sizeof(dns_edns0_label);
		}
	}
	else {
		if (BufferSize >= RawDataLen)
		{
			memcpy(Buffer.get(), RawData.get(), RawDataLen);
			DataLength = RawDataLen;
		}
		else {
			memcpy(Buffer.get(), RawData.get(), BufferSize);
			DataLength = BufferSize;
		}
	}

//Send.
	if (gettimeofday(&BeforeTime, NULL) != 0)
	{
		wprintf(L"Get current time error, error code is %d.\n", errno);
		return EXIT_FAILURE;
	}
	sendto(Socket, Buffer.get(), DataLength, MSG_NOSIGNAL, (sockaddr *)&Target, AddrLen);
	DataLength = recvfrom(Socket, RecvBuffer.get(), BufferSize, MSG_NOSIGNAL, (sockaddr *)&Target, &AddrLen);
	if (gettimeofday(&AfterTime, NULL) != 0)
	{
		wprintf(L"Get current time error, error code is %d.\n", errno);
		return EXIT_FAILURE;
	}

//Get time.
	long double Result = (long double)(AfterTime.tv_sec - BeforeTime.tv_sec) * (long double)1000;
	if (AfterTime.tv_sec >= BeforeTime.tv_sec)
		Result += (long double)(AfterTime.tv_usec - BeforeTime.tv_usec) / (long double)1000;
	else 
		Result += (long double)(AfterTime.tv_usec + 1000000 - BeforeTime.tv_usec) / (long double)1000;


//Print to screen.
	if (DataLength > 0)
	{
		wprintf(L"Receive from %s:%u -> %d bytes, waiting %Lf ms.\n", TargetString.c_str(), ntohs(Port), (int)DataLength, Result);
		TotalTime += Result;
		RecvNum++;

	//Mark time.
		if (MaxTime == 0)
		{
			MinTime = Result;
			MaxTime = Result;
		}
		else if (Result < MinTime)
		{
			MinTime = Result;
		}
		else if (Result > MaxTime)
		{
			MaxTime = Result;
		}

	//Transmission interval
		if (TransmissionInterval != 0 && TransmissionInterval > Result)
			usleep(TransmissionInterval - Result);
		else 
			usleep(TIME_OUT);
	}
	else { //RETURN_ERROR
		wprintf(L"Receive error: %d(%d), waiting %Lf ms.\n", (int)DataLength, errno, Result);

	//Transmission interval
		if (TransmissionInterval != 0 && TransmissionInterval > Result)
			usleep(TransmissionInterval - Result);
		else if (Result <= TIME_OUT)
			usleep(TIME_OUT);
	}

	return EXIT_SUCCESS;
}

//Print statistics to screen
size_t PrintProcess(const bool PacketStatistics, const bool TimeStatistics)
{
//Packet Statistics
	if (PacketStatistics)
	{
		wprintf(L"\nPacket statistics for pinging %s:\n", TargetString.c_str());
		wprintf(L"   Send: %lu\n", (unsigned long)RealSendNum);
		wprintf(L"   Receive: %lu\n", (unsigned long)RecvNum);
		if ((ssize_t)RealSendNum - (ssize_t)RecvNum >= 0)
		{
			wprintf(L"   Lost: %lu", (unsigned long)(RealSendNum - RecvNum));
			if (RealSendNum > 0)
				wprintf(L" (%lu%%)\n", (unsigned long)((RealSendNum - RecvNum) * 100 / RealSendNum));
			else  //Not any packets.
				wprintf(L"\n");
		}
		else {
			wprintf(L"   Lost: 0 (0%%)\n");
		}
	}

//Time Statistics
	if (TimeStatistics && 
		RecvNum > 0 && MaxTime > 0 && MinTime > 0)
	{
		wprintf(L"\nTime statistics for pinging %s:\n", TargetString.c_str());
		wprintf(L"   Minimum time: %Lf ms.\n", MinTime);
		wprintf(L"   Maximum time: %Lf ms.\n", MaxTime);
		wprintf(L"   Average time: %Lf ms.\n", TotalTime / (long double)RecvNum);
	}

	wprintf(L"\n");
	return EXIT_SUCCESS;
}

//Print description to screen
void PrintDescription(void)
{
	wprintf(L"\n");

//Description
	wprintf(L"--------------------------------------------------\n");
	wprintf(L"DNSPing v0.1 Beta(Linux)\n");
	wprintf(L"Copyright (C) 2014 Chengr28\n");
	wprintf(L"--------------------------------------------------\n");

//Usage
	//All Non-SOCK_STREAM will set "Don't Fragment" flag(Linux).
//	wprintf(L"\nUsage: DNSPing [-h] [-t] [-a] [-n Count] [-f] [-i HopLimit/TTL] [-w Timeout]\n");
	wprintf(L"\nUsage: DNSPing [-h] [-t] [-a] [-n Count] [-i HopLimit/TTL] [-w Timeout]\n");
	wprintf(L"               [-id DNS_ID] [-qr] [-opcode OPCode] [-aa] [-tc]\n");
	wprintf(L"               [-rd] [-ra] [-ad] [-cd] [-rcode RCode] [-qn Count]\n");
	wprintf(L"               [-ann Count] [-aun Count] [-adn Count] [-ti Time] [-edns0]\n");
	wprintf(L"               [-payload Length] [-dnssec] [-qt Type] [-qc Classes] [-p Port]\n");
	wprintf(L"               [-raw RAW_Data] [-buf Size] Test_DomainName Target\n");

//Options
	wprintf(L"\nOptions:\n");
	wprintf(L"   N/A               Description.\n");
	wprintf(L"   ?                 Description.\n");
	wprintf(L"   -h                Description.\n");
	wprintf(L"   -t                Pings the specified host until stopped.\n                     To see statistics and continue type Control-Break.\n                     To stop type Control-C.\n");
	wprintf(L"   -a                Resolve addresses to host names.\n");
	wprintf(L"   -n Count          Set number of echo requests to send.\n                     Count must between 1 - 0xFFFF/65535.\n");
//All Non-SOCK_STREAM will set "Don't Fragment" flag(Linux).
//	wprintf(L"   -f                Set the \"Don't Fragment\" flag in outgoing packets.\n");
	wprintf(L"   -i HopLimit/TTL   Specifie a Time To Live for outgoing packets.\n                     HopLimit/TTL must between 1 - 255.\n");
	wprintf(L"   -w Timeout        Set a long wait periods (in milliseconds) for a response.\n                     Timeout must between 500 - 0xFFFF/65535.\n");
	wprintf(L"   -id DNS_ID        Specifie DNS header ID.\n                     DNS ID must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -qr               Set DNS header QR flag.\n");
	wprintf(L"   -opcode OPCode    Specifie DNS header OPCode.\n                     OPCode must between 0x0000 - 0x00FF/255.\n");
	wprintf(L"   -aa               Set DNS header AA flag.\n");
	wprintf(L"   -tc               Set DNS header TC flag.\n");
	wprintf(L"   -rd               Set DNS header RD flag.\n");
	wprintf(L"   -ra               Set DNS header RA flag.\n");
	wprintf(L"   -ad               Set DNS header AD flag.\n");
	wprintf(L"   -cd               Set DNS header CD flag.\n");
	wprintf(L"   -rcode RCode      Specifie DNS header RCode.\n                     OPCode must between 0x0000 - 0x00FF/255\n");
	wprintf(L"   -qn Count         Specifie DNS header Question count.\n                     Question count must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -ann Count        Specifie DNS header Answer count.\n                     Answer count must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -aun Count        Specifie DNS header Authority count.\n                     Authority count must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -adn Count        Specifie DNS header Additional count.\n                     Additional count must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -ti Time          Specifie transmission interval time(in milliseconds).\n");
	wprintf(L"   -edns0            Send with EDNS0 Label.\n");
	wprintf(L"   -payload Length   Specifie EDNS0 Label UDP Payload length.\n                     Payload length must between 512 - 0xFFFF/65535.\n");
	wprintf(L"   -dnssec           Send with DNSSEC requesting.\n                     EDNS0 Label will enable when DNSSEC is enable\n");
	wprintf(L"   -qt Type          Specifie query type.\n");
	wprintf(L"                     Type: A|NS|CNAME|SOA|PTR|MX|TXT|RP|SIG|KEY|AAAA|LOC|SRV|\n");
	wprintf(L"                           NAPTR|KX|CERT|DNAME|EDNS0|APL|DS|SSHFP|IPSECKEY|\n");
	wprintf(L"                           RRSIG|NSEC|DNSKEY|DHCID|NSEC3|NSEC3PARAM|HIP|SPF|\n");
	wprintf(L"                           TKEY|TSIG|IXFR|AXFR|ANY|TA|DLV\n");
	wprintf(L"   -qc Classes       Specifie query classes.\n");
	wprintf(L"                     Classes: IN|CSNET|CHAOS|HESIOD|NONE|ALL|ANY\n");
	wprintf(L"   -p Port           Specifie UDP port.\n                     UDP port must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -raw RAW_Data     Specifie Raw data to send.\n");
	wprintf(L"                     RAW_Data is hex, but do not add \"0x\" before hex.\n");
	wprintf(L"                     Length of RAW_Data must between 64 - 1512 bytes.\n");
	wprintf(L"   -buf Size         Specifie receive buffer size.\n                     Buffer size must between 512 - 4096(bytes).\n");
	wprintf(L"   -6                Using IPv6.\n");
	wprintf(L"   -4                Using IPv4.\n");
	wprintf(L"   Test_DomainName   A domain name which will make requesting to send \n");
	wprintf(L"                     to DNS server.\n");
	wprintf(L"   Target            Target of DNSPing, support IPv4/IPv6 address and domain.\n");

	wprintf(L"\n");
	return;
}
