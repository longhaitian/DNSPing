// This code is part of DNSPing(Windows)
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

extern std::string TestDomain;
extern std::wstring wTargetString;
extern long double TotalTime, MaxTime, MinTime;
extern size_t SendNum, RealSendNum, RecvNum, TransmissionInterval, BufferSize, RawDataLen;
extern uint16_t Protocol, ServiceName;
extern std::shared_ptr<char> RawData;
extern int SocketTimeout, IP_HopLimits;
extern bool RawSocket, IPv4_DF, EDNS0;
extern dns_hdr HeaderParameter;
extern dns_qry QueryParameter;
extern dns_edns0_label EDNS0Parameter;
extern FILE *OutputFile;

//Send DNS requesting process
size_t __fastcall SendProcess(const sockaddr_storage Target)
{
//Initialization
	std::shared_ptr<char> Buffer(new char[BufferSize]()), RecvBuffer(new char[BufferSize]());
	SSIZE_T DataLength = 0;
	LARGE_INTEGER CPUFrequency = {0}, BeforeTime = {0}, AfterTime = {0};
	SOCKET Socket = 0;
	int AddrLen = 0;

//IPv6
	if (Protocol == AF_INET6)
	{
	//Socket initialization
		AddrLen = sizeof(sockaddr_in6);
		if (RawSocket && RawData)
			Socket = socket(AF_INET6, SOCK_RAW, ServiceName);
		else 
			Socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		if (Socket == INVALID_SOCKET)
		{
			wprintf(L"Socket initialization error, error code is %d.\n", WSAGetLastError());

			WSACleanup();
			return EXIT_FAILURE;
		}
	}
//IPv4
	else {
	//Socket initialization
		AddrLen = sizeof(sockaddr_in);
		if (RawSocket && RawData)
			Socket = socket(AF_INET, SOCK_RAW, ServiceName);
		else 
			Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (Socket == INVALID_SOCKET)
		{
			wprintf(L"Socket initialization error, error code is %d.\n", WSAGetLastError());

			WSACleanup();
			return EXIT_FAILURE;
		}
	}

//Set socket timeout.
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (PSTR)&SocketTimeout, sizeof(int)) == SOCKET_ERROR || 
		setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (PSTR)&SocketTimeout, sizeof(int)) == SOCKET_ERROR)
	{
		wprintf(L"Set UDP socket timeout error, error code is %d.\n", WSAGetLastError());
		return EXIT_FAILURE;
	}

//Set IP options.
	if (Protocol == AF_INET6) //IPv6
	{
		if (IP_HopLimits != 0 && setsockopt(Socket, IPPROTO_IP, IPV6_UNICAST_HOPS, (PSTR)&IP_HopLimits, sizeof(int)) == SOCKET_ERROR)
		{
			wprintf(L"Set HopLimit/TTL flag error, error code is %d.\n", WSAGetLastError());
			return EXIT_FAILURE;
		}
	}
	else { //IPv4
		if (IP_HopLimits != 0 && setsockopt(Socket, IPPROTO_IP, IP_TTL, (PSTR)&IP_HopLimits, sizeof(int)) == SOCKET_ERROR)
		{
			wprintf(L"Set HopLimit/TTL flag error, error code is %d.\n", WSAGetLastError());
			return EXIT_FAILURE;
		}

	//Set "Don't Fragment" flag.
		int iIPv4_DF = 1;
		if (IPv4_DF && setsockopt(Socket, IPPROTO_IP, IP_DONTFRAGMENT, (PSTR)&iIPv4_DF, sizeof(int)) == SOCKET_ERROR)
		{
			wprintf(L"Set \"Don't Fragment\" flag error, error code is %d.\n", WSAGetLastError());
			return EXIT_FAILURE;
		}
	}

//Make packet.
	if (!RawData)
	{
	//DNS requesting
		memcpy(Buffer.get() + DataLength, &HeaderParameter, sizeof(dns_hdr));
		if (HeaderParameter.ID == 0)
		{
			auto pdns_hdr = (dns_hdr *)(Buffer.get() + DataLength);
			pdns_hdr->ID = htons((uint16_t)GetCurrentProcessId());
		}
		DataLength += sizeof(dns_hdr);
		DataLength += CharToDNSQuery((PSTR)TestDomain.c_str(), Buffer.get() + DataLength);
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
	if (QueryPerformanceFrequency(&CPUFrequency) == 0 || QueryPerformanceCounter(&BeforeTime) == 0)
	{
		wprintf(L"Get current time form High Precision Event Timer/HPET error, error code is %d.\n", (int)GetLastError());
		return EXIT_FAILURE;
	}
	sendto(Socket, Buffer.get(), (int)DataLength, NULL, (PSOCKADDR)&Target, AddrLen);

//Receive.
	DataLength = recvfrom(Socket, RecvBuffer.get(), (int)BufferSize, NULL, (PSOCKADDR)&Target, &AddrLen);
	if (QueryPerformanceCounter(&AfterTime) == 0)
	{
		wprintf(L"Get current time form High Precision Event Timer/HPET error, error code is %d.\n", (int)GetLastError());
		return EXIT_FAILURE;
	}

//Get waiting time.
	long double Result = (long double)((AfterTime.QuadPart - BeforeTime.QuadPart) * (long double)1000 / (long double)CPUFrequency.QuadPart);

//Print to screen.
	if (DataLength > 0)
	{
		wprintf(L"Receive from %ls:%u -> %d bytes, waiting %lf ms.\n", wTargetString.c_str(), ntohs(ServiceName), (int)DataLength, Result);
	
	//Output to file.
		if (OutputFile != nullptr)
			fwprintf_s(OutputFile, L"Receive from %ls:%u -> %d bytes, waiting %lf ms.\n", wTargetString.c_str(), ntohs(ServiceName), (int)DataLength, Result);

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
			Sleep((DWORD)(TransmissionInterval - Result));
		else 
			Sleep(TIME_OUT - (DWORD)Result);
	}
	else { //SOCKET_ERROR
		wprintf(L"Receive error: %d(%d), waiting %lf ms.\n", (int)DataLength, WSAGetLastError(), Result);

	//Output to file.
		if (OutputFile != nullptr)
			fwprintf_s(OutputFile, L"Receive error: %d(%d), waiting %lf ms.\n", (int)DataLength, WSAGetLastError(), Result);

	//Transmission interval
		if (TransmissionInterval != 0 && TransmissionInterval > Result)
			Sleep((DWORD)(TransmissionInterval - Result));
		else if (Result <= TIME_OUT)
			Sleep(TIME_OUT);
	}

	return EXIT_SUCCESS;
}

//Print statistics to screen(and/or output result to file)
size_t __fastcall PrintProcess(const bool PacketStatistics, const bool TimeStatistics)
{
//Packet Statistics
	if (PacketStatistics)
	{
		wprintf(L"\nPacket statistics for pinging %ls:\n", wTargetString.c_str());
		wprintf(L"   Send: %lu\n", (ULONG)RealSendNum);
		wprintf(L"   Receive: %lu\n", (ULONG)RecvNum);

	//Output to file.
		if (OutputFile != nullptr)
		{
			fwprintf_s(OutputFile, L"\nPacket statistics for pinging %ls:\n", wTargetString.c_str());
			fwprintf_s(OutputFile, L"   Send: %lu\n", (ULONG)RealSendNum);
			fwprintf_s(OutputFile, L"   Receive: %lu\n", (ULONG)RecvNum);
		}

		if ((SSIZE_T)RealSendNum - (SSIZE_T)RecvNum >= 0)
		{
			wprintf(L"   Lost: %lu", (ULONG)(RealSendNum - RecvNum));
			if (RealSendNum > 0)
				wprintf(L" (%lu%%)\n", (ULONG)((RealSendNum - RecvNum) * 100 / RealSendNum));
			else  //Not any packets.
				wprintf(L"\n");

		//Output to file.
			if (OutputFile != nullptr)
			{
				fwprintf_s(OutputFile, L"   Lost: %lu", (ULONG)(RealSendNum - RecvNum));
				if (RealSendNum > 0)
					fwprintf_s(OutputFile, L" (%lu%%)\n", (ULONG)((RealSendNum - RecvNum) * 100 / RealSendNum));
				else  //Not any packets.
					fwprintf_s(OutputFile, L"\n");
			}
		}
		else {
			wprintf(L"   Lost: 0 (0%%)\n");

		//Output to file.
			if (OutputFile != nullptr)
				fwprintf_s(OutputFile, L"   Lost: 0 (0%%)\n");
		}
	}

//Time Statistics
	if (TimeStatistics && 
		RecvNum > 0 && MaxTime > 0 && MinTime > 0)
	{
		wprintf(L"\nTime statistics for pinging %ls:\n", wTargetString.c_str());
		wprintf(L"   Minimum time: %lf ms.\n", MinTime);
		wprintf(L"   Maximum time: %lf ms.\n", MaxTime);
		wprintf(L"   Average time: %lf ms.\n", TotalTime / (long double)RecvNum);

	//Output to file.
		if (OutputFile != nullptr)
		{
			fwprintf_s(OutputFile, L"\nTime statistics for pinging %ls:\n", wTargetString.c_str());
			fwprintf_s(OutputFile, L"   Minimum time: %lf ms.\n", MinTime);
			fwprintf_s(OutputFile, L"   Maximum time: %lf ms.\n", MaxTime);
			fwprintf_s(OutputFile, L"   Average time: %lf ms.\n", TotalTime / (long double)RecvNum);
		}
	}

	wprintf(L"\n");
//Output to file.
	if (OutputFile != nullptr)
		fwprintf_s(OutputFile, L"\n");

	return EXIT_SUCCESS;
}

//Print description to screen
void __fastcall PrintDescription(void)
{
	wprintf(L"\n");

//Description
	wprintf(L"--------------------------------------------------\n");
	wprintf(L"DNSPing v0.1 Beta(Windows)\n");
	wprintf(L"DNSPing, Ping with DNS requesting.\n");
	wprintf(L"Copyright (C) 2014 Chengr28\n");
	wprintf(L"--------------------------------------------------\n");

//Usage
	wprintf(L"\nUsage: DNSPing [-h] [-t] [-a] [-n Count] [-f] [-i HopLimit/TTL] [-w Timeout]\n");
	wprintf(L"               [-id DNS_ID] [-qr] [-opcode OPCode] [-aa] [-tc]\n");
	wprintf(L"               [-rd] [-ra] [-ad] [-cd] [-rcode RCode] [-qn Count]\n");
	wprintf(L"               [-ann Count] [-aun Count] [-adn Count] [-ti Time] [-edns0]\n");
	wprintf(L"               [-payload Length] [-dnssec] [-qt Type] [-qc Classes]\n");
	wprintf(L"               [-p ServiceName] [-rawdata RAW_Data] [-raw ServiceName]\n");
	wprintf(L"               [-buf Size] [-of FileName] Test_DomainName Target\n");

//Options
	wprintf(L"\nOptions:\n");
	wprintf(L"   N/A               Description.\n");
	wprintf(L"   ?                 Description.\n");
	wprintf(L"   -h                Description.\n");
	wprintf(L"   -t                Pings the specified host until stopped.\n                     To see statistics and continue type Control-Break.\n                     To stop type Control-C.\n");
	wprintf(L"   -a                Resolve addresses to host names.\n");
	wprintf(L"   -n Count          Set number of echo requests to send.\n                     Count must between 1 - 0xFFFF/65535.\n");
	wprintf(L"   -f                Set the \"Don't Fragment\" flag in outgoing packets(IPv4).\n");
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
	wprintf(L"   -rcode RCode      Specifie DNS header RCode.\n                     RCode must between 0x0000 - 0x00FF/255.\n");
	wprintf(L"   -qn Count         Specifie DNS header Question count.\n                     Question count must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -ann Count        Specifie DNS header Answer count.\n                     Answer count must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -aun Count        Specifie DNS header Authority count.\n                     Authority count must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -adn Count        Specifie DNS header Additional count.\n                     Additional count must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"   -ti IntervalTime  Specifie transmission interval time(in milliseconds).\n");
	wprintf(L"   -edns0            Send with EDNS0 Label.\n");
	wprintf(L"   -payload Length   Specifie EDNS0 Label UDP Payload length.\n                     Payload length must between 512 - 0xFFFF/65535.\n");
	wprintf(L"   -dnssec           Send with DNSSEC requesting.\n                     EDNS0 Label will enable when DNSSEC is enable.\n");
	wprintf(L"   -qt Type          Specifie Query type.\n                     Query type must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"                     Type: A|NS|CNAME|SOA|PTR|MX|TXT|RP|SIG|KEY|AAAA|LOC|SRV|\n");
	wprintf(L"                           NAPTR|KX|CERT|DNAME|EDNS0|APL|DS|SSHFP|IPSECKEY|\n");
	wprintf(L"                           RRSIG|NSEC|DNSKEY|DHCID|NSEC3|NSEC3PARAM|HIP|SPF|\n");
	wprintf(L"                           TKEY|TSIG|IXFR|AXFR|ANY|TA|DLV\n");
	wprintf(L"   -qc Classes       Specifie Query classes.\n                     Query classes must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"                     Classes: IN|CSNET|CHAOS|HESIOD|NONE|ALL|ANY\n");
	wprintf(L"   -p ServiceName    Specifie UDP port/protocol(Sevice names).\n                     UDP port must between 0x0001 - 0xFFFF/65535.\n");
	wprintf(L"                     Protocol: TCPMUX|ECHO|DISCARD|SYSTAT|DAYTIME|NETSTAT|\n");
	wprintf(L"                               QOTD|MSP|CHARGEN|FTP|SSH|TELNET|SMTP|\n");
	wprintf(L"                               TIME|RAP|RLP|NAME|WHOIS|TACACS|XNSAUTH|MTP|\n");
	wprintf(L"                               BOOTPS|BOOTPC|TFTP|RJE|FINGER|TTYLINK|SUPDUP|\n");
	wprintf(L"                               SUNRPC|SQL|NTP|EPMAP|NETBIOSNS|NETBIOSDGM|\n");
	wprintf(L"                               NETBIOSSSN|IMAP|BFTP|SGMP|SQLSRV|DMSP|SNMP|\n");
	wprintf(L"                               SNMPTRAP|ATRTMP|ATHBP|QMTP|IPX|IMAP|IMAP3|\n");
	wprintf(L"                               BGMP|TSP|IMMP|ODMR|RPC2PORTMAP|CLEARCASE|\n");
	wprintf(L"                               HPALARMMGR|ARNS|AURP|LDAP|UPS|SLP|SNPP|\n");
	wprintf(L"                               MICROSOFTDS|KPASSWD|TCPNETHASPSRV|RETROSPECT|\n");
	wprintf(L"                               ISAKMP|BIFFUDP|WHOSERVER|SYSLOG|ROUTERSERVER|\n");
	wprintf(L"                               NCP|COURIER|COMMERCE|RTSP|NNTP|HTTPRPCEPMAP|\n");
	wprintf(L"                               IPP|LDAPS|MSDP|AODV|FTPSDATA|FTPS|NAS|TELNETS\n");
	wprintf(L"   -rawdata RAW_Data Specifie Raw data to send.\n");
	wprintf(L"                     RAW_Data is hex, but do not add \"0x\" before hex.\n");
	wprintf(L"                     Length of RAW_Data must between 64 - 1512 bytes.\n");
	wprintf(L"   -raw ServiceName  Specifie Raw socket type.\n");
	wprintf(L"                     Service Name: HOPOPTS|ICMP|IGMP|GGP|IPV4|ST|TCP|CBT|EGP|\n");
	wprintf(L"                                   IGP|PUP|IDP|IPV6|ROUTING|ESP|FRAGMENT|AH|\n");
	wprintf(L"                                   ICMPV6|NONE|DSTOPTS|ND|ICLFXBM|PIM|PGM|L2TP|\n");
	wprintf(L"                                   SCTP|RAW\n");
	wprintf(L"   -buf Size         Specifie receive buffer size.\n                     Buffer size must between 512 - 4096 bytes.\n");
	wprintf(L"   -of FileName      Output result to file.\n                     FileName must less than 260 bytes.\n");
	wprintf(L"   -6                Using IPv6.\n");
	wprintf(L"   -4                Using IPv4.\n");
	wprintf(L"   Test_DomainName   A domain name which will make requesting to send \n");
	wprintf(L"                     to DNS server.\n");
	wprintf(L"   Target            Target of DNSPing, support IPv4/IPv6 address and domain.\n");

	return;
}
