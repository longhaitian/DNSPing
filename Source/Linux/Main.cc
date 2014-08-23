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

std::string TestDomain, TargetString, TargetDomainString;
long double TotalTime = 0, MaxTime = 0, MinTime = 0;
size_t SendNum = DEFAULT_SEND_TIMES, RealSendNum = 0, RecvNum = 0, TransmissionInterval = 0, BufferSize = PACKET_MAXSIZE, RawDataLen = 0, EDNS0PayloadSize = 0;
sockaddr_storage SockAddr = {0};
uint16_t Port = 0;
std::shared_ptr<char> RawData;
int IP_HopLimits = 0;
timeval SocketTimeout = {DEFAULT_TIME_OUT, 0};
auto /* IPv4_DF = false, */ EDNS0 = false;
dns_hdr HeaderParameter = {0};
dns_qry QueryParameter = {0};
dns_edns0_label EDNS0Parameter = {0};

//Main function of program
int main(int argc, char *argv[])
{
//Handle the system signal.
	if (signal(SIGHUP, SIG_Handler) == SIG_ERR || signal(SIGINT, SIG_Handler) == SIG_ERR || signal(SIGQUIT, SIG_Handler) == SIG_ERR || signal(SIGTERM, SIG_Handler) == SIG_ERR)
	{
		wprintf(L"Handle the system signal error, error code is %d.\n", errno);
		return EXIT_FAILURE;
	}

//Main
	if (argc > 2)
	{
		std::string Parameter;
		ssize_t Result = 0;

	//Read parameter
		auto ReverseLookup = false, DNSSEC = false;
		for (size_t Index = 1U;Index < (size_t)argc;Index++)
		{
			Parameter = argv[Index];
			Result = 0;

		//Description(Usage)
			if (Parameter.find("?") != std::string::npos || Parameter == ("-H") || Parameter == ("-h"))
			{
				PrintDescription();
			}
		//Pings the specified host until stopped. To see statistics and continue type Control-Break. To stop type Control-C.
			else if (Parameter == ("-t"))
			{
				SendNum = 0;
			}
		//Resolve addresses to host names.
			else if (Parameter == ("-a"))
			{
				ReverseLookup = true;
			}
		//Set number of echo requests to send.
			else if (Parameter == ("-n"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U16_MAXNUM)
					{
						SendNum = Result;
					}
					else {
						wprintf(L"\nParameter[-n Count] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
/*
		//Set the "Don't Fragment" flag in outgoing packets.
		//All Non-SOCK_STREAM will set "Don't Fragment" flag(Linux).
			else if (Parameter == ("-f"))
			{
				IPv4_DF = true;
			}
*/
		//Specifie a Time To Live for outgoing packets.
			else if (Parameter == ("-i"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U8_MAXNUM)
					{
						IP_HopLimits = (int)Result;
					}
					else {
						wprintf(L"\nParameter[-i HopLimit/TTL] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Set a long wait periods (in milliseconds) for a response.
			else if (Parameter == ("-w"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result >= 500 && Result < U16_MAXNUM)
					{
						SocketTimeout.tv_sec = (time_t)(Result / 1000);
						SocketTimeout.tv_usec = (suseconds_t)(Result % 1000 * 1000);
					}
					else {
						wprintf(L"\nParameter[-w Timeout] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie DNS header ID.
			else if (Parameter == ("-ID") || Parameter == ("-id"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U16_MAXNUM)
					{
						HeaderParameter.ID = htons((uint16_t)Result);
					}
					else {
						wprintf(L"\nParameter[-id DNS_ID] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Set DNS header flag: QR
			else if (Parameter == ("-QR") || Parameter == ("-qr"))
			{
				HeaderParameter.FlagsBits.QR = ~HeaderParameter.FlagsBits.QR;
			}
		//Specifie DNS header OPCode.
			else if (Parameter == ("-OPCode") || Parameter == ("-opcode"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U4_MAXNUM)
					{
					#if BYTE_ORDER == LITTLE_ENDIAN
						uint16_t TempFlags = (uint16_t)Result;
						TempFlags = htons(TempFlags << 11U);
						HeaderParameter.Flags = HeaderParameter.Flags | TempFlags;
					#else //BIG_ENDIAN
						uint8_t TempFlags = (uint8_t)Result;
						TempFlags = TempFlags & 15; //0x00001111
						HeaderParameter.FlagsBits.OPCode = TempFlags;
					#endif
					}
					else {
						wprintf(L"\nParameter[-opcode OPCode] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Set DNS header flag: AA
			else if (Parameter == ("-AA") || Parameter == ("-aa"))
			{
				HeaderParameter.FlagsBits.AA = ~HeaderParameter.FlagsBits.AA;
			}
		//Set DNS header flag: TC
			else if (Parameter == ("-TC") || Parameter == ("-tc"))
			{
				HeaderParameter.FlagsBits.TC = ~HeaderParameter.FlagsBits.TC;
			}
		//Set DNS header flag: RD
			else if (Parameter == ("-RD") || Parameter == ("-rd"))
			{
				HeaderParameter.FlagsBits.RD = ~HeaderParameter.FlagsBits.RD;
			}
		//Set DNS header flag: RA
			else if (Parameter == ("-RA") || Parameter == ("-ra"))
			{
				HeaderParameter.FlagsBits.RA = ~HeaderParameter.FlagsBits.RA;
			}
		//Set DNS header flag: AD
			else if (Parameter == ("-AD") || Parameter == ("-ad"))
			{
				HeaderParameter.FlagsBits.AD = ~HeaderParameter.FlagsBits.AD;
			}
		//Set DNS header flag: CD
			else if (Parameter == ("-CD") || Parameter == ("-cd"))
			{
				HeaderParameter.FlagsBits.CD = ~HeaderParameter.FlagsBits.CD;
			}
		//Specifie DNS header RCode.
			else if (Parameter == ("-RCode") || Parameter == ("-rcode"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U4_MAXNUM)
					{
					#if BYTE_ORDER == LITTLE_ENDIAN
						uint16_t TempFlags = (uint16_t)Result;
						TempFlags = htons(TempFlags);
						HeaderParameter.Flags = HeaderParameter.Flags | TempFlags;
					#else //BIG_ENDIAN
						uint8_t TempFlags = (uint8_t)Result;
						TempFlags = TempFlags & 15; //0x00001111
						HeaderParameter.FlagsBits.OPCode = TempFlags;
					#endif
					}
					else {
						wprintf(L"\nParameter[-rcode RCode] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie DNS header question count.
			else if (Parameter == ("-QN") || Parameter == ("-qn"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U16_MAXNUM)
					{
						HeaderParameter.Questions = htons((uint16_t)Result);
					}
					else {
						wprintf(L"\nParameter[-qn Count] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie DNS header Answer count.
			else if (Parameter == ("-ANN") || Parameter == ("-ann"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U16_MAXNUM)
					{
						HeaderParameter.Answer = htons((uint16_t)Result);
					}
					else {
						wprintf(L"\nParameter[-ann Count] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie DNS header Authority count.
			else if (Parameter == ("-AUN") || Parameter == ("-aun"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U16_MAXNUM)
					{
						HeaderParameter.Authority = htons((uint16_t)Result);
					}
					else {
						wprintf(L"\nParameter[-aun Count] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie DNS header Additional count.
			else if (Parameter == ("-ADN") || Parameter == ("-adn"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U16_MAXNUM)
					{
						HeaderParameter.Additional = htons((uint16_t)Result);
					}
					else {
						wprintf(L"\nParameter[-adn Count] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie transmission interval time(in milliseconds).
			else if (Parameter == ("-Ti") || Parameter == ("-ti"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result >= 0)
					{
						TransmissionInterval = Result * 1000;
					}
					else {
						wprintf(L"\nParameter[-ti Time] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Send with EDNS0 Label.
			else if (Parameter == ("-EDNS0") || Parameter == ("-edns0"))
			{
				EDNS0 = true;
			}
		//Specifie EDNS0 Label UDP Payload length.
			else if (Parameter == ("-Payload") || Parameter == ("-payload"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > OLD_DNS_MAXSIZE && Result <= U16_MAXNUM)
					{
						EDNS0PayloadSize = Result;
					}
					else {
						wprintf(L"\nParameter[-payload Length] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}

				EDNS0 = true;
			}
		//Send with DNSSEC requesting.
			else if (Parameter == ("-DNSSEC") || Parameter == ("-dnssec"))
			{
				EDNS0 = true;
				DNSSEC = true;
			}
		//Specifie Query Type.
			else if (Parameter == ("-QT") || Parameter == ("-qt"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

				//Name types
					if (Parameter == ("A") || Parameter == ("a"))
					{
						QueryParameter.Type = htons(DNS_A_RECORDS);
					}
					else if (Parameter == ("NS") || Parameter == ("ns"))
					{
						QueryParameter.Type = htons(DNS_NS_RECORDS);
					}
					else if (Parameter == ("CNAME") || Parameter == ("cname"))
					{
						QueryParameter.Type = htons(DNS_CNAME_RECORDS);
					}
					else if (Parameter == ("SOA") || Parameter == ("soa"))
					{
						QueryParameter.Type = htons(DNS_SOA_RECORDS);
					}
					else if (Parameter == ("PTR") || Parameter == ("ptr"))
					{
						QueryParameter.Type = htons(DNS_PTR_RECORDS);
					}
					else if (Parameter == ("MX") || Parameter == ("mx"))
					{
						QueryParameter.Type = htons(DNS_MX_RECORDS);
					}
					else if (Parameter == ("TXT") || Parameter == ("txt"))
					{
						QueryParameter.Type = htons(DNS_TXT_RECORDS);
					}
					else if (Parameter == ("RP") || Parameter == ("rp"))
					{
						QueryParameter.Type = htons(DNS_RP_RECORDS);
					}
					else if (Parameter == ("SIG") || Parameter == ("sig"))
					{
						QueryParameter.Type = htons(DNS_SIG_RECORDS);
					}
					else if (Parameter == ("KEY") || Parameter == ("key"))
					{
						QueryParameter.Type = htons(DNS_KEY_RECORDS);
					}
					else if (Parameter == ("AAAA") || Parameter == ("aaaa"))
					{
						QueryParameter.Type = htons(DNS_AAAA_RECORDS);
					}
					else if (Parameter == ("LOC") || Parameter == ("loc"))
					{
						QueryParameter.Type = htons(DNS_LOC_RECORDS);
					}
					else if (Parameter == ("SRV") || Parameter == ("srv"))
					{
						QueryParameter.Type = htons(DNS_SRV_RECORDS);
					}
					else if (Parameter == ("NAPTR") || Parameter == ("naptr"))
					{
						QueryParameter.Type = htons(DNS_NAPTR_RECORDS);
					}
					else if (Parameter == ("KX") || Parameter == ("kx"))
					{
						QueryParameter.Type = htons(DNS_KX_RECORDS);
					}
					else if (Parameter == ("CERT") || Parameter == ("cert"))
					{
						QueryParameter.Type = htons(DNS_CERT_RECORDS);
					}
					else if (Parameter == ("DNAME") || Parameter == ("dname"))
					{
						QueryParameter.Type = htons(DNS_DNAME_RECORDS);
					}
					else if (Parameter == ("EDNS0") || Parameter == ("edns0"))
					{
						QueryParameter.Type = htons(DNS_EDNS0_RECORDS);
					}
					else if (Parameter == ("APL") || Parameter == ("apl"))
					{
						QueryParameter.Type = htons(DNS_APL_RECORDS);
					}
					else if (Parameter == ("DS") || Parameter == ("ds"))
					{
						QueryParameter.Type = htons(DNS_DS_RECORDS);
					}
					else if (Parameter == ("SSHFP") || Parameter == ("sshfp"))
					{
						QueryParameter.Type = htons(DNS_SSHFP_RECORDS);
					}
					else if (Parameter == ("IPSECKEY") || Parameter == ("ipseckey"))
					{
						QueryParameter.Type = htons(DNS_IPSECKEY_RECORDS);
					}
					else if (Parameter == ("RRSIG") || Parameter == ("rrsig"))
					{
						QueryParameter.Type = htons(DNS_RRSIG_RECORDS);
					}
					else if (Parameter == ("NSEC") || Parameter == ("nsec"))
					{
						QueryParameter.Type = htons(DNS_NSEC_RECORDS);
					}
					else if (Parameter == ("DNSKEY") || Parameter == ("dnskey"))
					{
						QueryParameter.Type = htons(DNS_DNSKEY_RECORDS);
					}
					else if (Parameter == ("DHCID") || Parameter == ("dhcid"))
					{
						QueryParameter.Type = htons(DNS_DHCID_RECORDS);
					}
					else if (Parameter == ("NSEC3") || Parameter == ("nsec3"))
					{
						QueryParameter.Type = htons(DNS_NSEC3_RECORDS);
					}
					else if (Parameter == ("NSEC3PARAM") || Parameter == ("nsec3param"))
					{
						QueryParameter.Type = htons(DNS_NSEC3PARAM_RECORDS);
					}
					else if (Parameter == ("HIP") || Parameter == ("hip"))
					{
						QueryParameter.Type = htons(DNS_HIP_RECORDS);
					}
					else if (Parameter == ("SPF") || Parameter == ("spf"))
					{
						QueryParameter.Type = htons(DNS_SPF_RECORDS);
					}
					else if (Parameter == ("TKEY") || Parameter == ("tkey"))
					{
						QueryParameter.Type = htons(DNS_TKEY_RECORDS);
					}
					else if (Parameter == ("TSIG") || Parameter == ("tsig"))
					{
						QueryParameter.Type = htons(DNS_TSIG_RECORDS);
					}
					else if (Parameter == ("IXFR") || Parameter == ("ixfr"))
					{
						QueryParameter.Type = htons(DNS_IXFR_RECORDS);
					}
					else if (Parameter == ("AXFR") || Parameter == ("axfr"))
					{
						QueryParameter.Type = htons(DNS_AXFR_RECORDS);
					}
					else if (Parameter == ("ANY") || Parameter == ("any"))
					{
						QueryParameter.Type = htons(DNS_ANY_RECORDS);
					}
					else if (Parameter == ("TA") || Parameter == ("ta"))
					{
						QueryParameter.Type = htons(DNS_TA_RECORDS);
					}
					else if (Parameter == ("DLV") || Parameter == ("dlv"))
					{
						QueryParameter.Type = htons(DNS_DLV_RECORDS);
					}
					else {
					//Number types
						Result = atol(Parameter.c_str());
						if (Result > 0 && Result <= U16_MAXNUM)
						{
							QueryParameter.Type = htons((uint16_t)Result);
						}
						else {
							wprintf(L"\nParameter[-qt Type] error.\n");
							return EXIT_FAILURE;
						}
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie Query Classes.
			else if (Parameter == ("-QC") || Parameter == ("-qc"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

				//Name types
					if (Parameter == ("INTERNET") || Parameter == ("internet") || Parameter == ("IN") || Parameter == ("in"))
					{
						QueryParameter.Classes = htons(DNS_CLASS_IN);
					}
					else if (Parameter == ("CSNET") || Parameter == ("csnet"))
					{
						QueryParameter.Classes = htons(DNS_CLASS_CSNET);
					}
					else if (Parameter == ("CHAOS") || Parameter == ("chaos"))
					{
						QueryParameter.Classes = htons(DNS_CLASS_CHAOS);
					}
					else if (Parameter == ("HESIOD") || Parameter == ("hesiod"))
					{
						QueryParameter.Classes = htons(DNS_CLASS_HESIOD);
					}
					else if (Parameter == ("NONE") || Parameter == ("none"))
					{
						QueryParameter.Classes = htons(DNS_CLASS_NONE);
					}
					else if (Parameter == ("ALL") || Parameter == ("ANY") || Parameter == ("all") || Parameter == ("any"))
					{
						QueryParameter.Classes = htons(DNS_CLASS_ANY);
					}
					else {
					//Number types
						Result = atol(Parameter.c_str());
						if (Result > 0 && Result <= U16_MAXNUM)
						{
							QueryParameter.Classes = htons((uint16_t)Result);
						}
						else {
							wprintf(L"\nParameter[-qc Classes] error.\n");
							return EXIT_FAILURE;
						}
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie requesting port.
			else if (Parameter == ("-p"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result > 0 && Result <= U16_MAXNUM)
					{
						Port = htons((uint16_t)Result);
					}
					else {
						wprintf(L"\nParameter[-p Port] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie Raw data to send.
			else if (Parameter == ("-RAW") || Parameter == ("-raw"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;

				//Initialization
					std::string RawDataString = argv[Index];
					if (RawDataString.length() < PACKET_MINSIZE && RawDataString.length() > PACKET_MAXSIZE)
					{
						wprintf(L"\nParameter[-raw RAW_Data] error.\n");
						return EXIT_FAILURE;
					}
					std::shared_ptr<char> TempRawData(new char[PACKET_MAXSIZE]());
					RawData.swap(TempRawData);
					TempRawData.reset();
					std::shared_ptr<char> Temp(new char[4U]());
					Temp.get()[0] = 48; //"0"
					Temp.get()[1U] = 120; //"x"

				//Read raw data.
					for (size_t InnerIndex = 0;InnerIndex < RawDataString.length();InnerIndex++)
					{
						Temp.get()[2U] = RawDataString[InnerIndex];
						Result = strtol(Temp.get(), NULL, 16);
						if (Result > 0 && Result <= U4_MAXNUM)
						{
							RawData.get()[RawDataLen] = (char)Result;
							RawDataLen++;
						}
						else {
							wprintf(L"\nParameter[-raw RAW_Data] error.\n");
							return EXIT_FAILURE;
						}
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Specifie buffer size.
			else if (Parameter == ("-Buf") || Parameter == ("-buf"))
			{
				if (Index + 1U < (size_t)argc)
				{
					Index++;
					Parameter = argv[Index];

					Result = atol(Parameter.c_str());
					if (Result >= OLD_DNS_MAXSIZE && Result <= LARGE_PACKET_MAXSIZE)
					{
						BufferSize = Result;
					}
					else {
						wprintf(L"\nParameter[-buf Size] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					wprintf(L"\nNot enough parameters error.\n");
					return EXIT_FAILURE;
				}
			}
		//Using IPv6.
			else if (Parameter == ("-6"))
			{
				SockAddr.ss_family = AF_INET6;
			}
		//Using IPv4.
			else if (Parameter == ("-4"))
			{
				SockAddr.ss_family = AF_INET;
			}
		//Specifie Query Domain.
			else if (!RawData && TestDomain.empty() && Index == argc - 2 && Parameter.length() > 2U)
			{
				TestDomain = Parameter;
			}
		//Specifie target.
			else if (Index == argc - 1 && Parameter.length() > 2U)
			{
			//IPv6 address
				if (Parameter.find(58) != std::string::npos)
				{
					if (SockAddr.ss_family == AF_INET)
					{
						wprintf(L"\nTarget protocol error.\n");
						return EXIT_FAILURE;
					}

					SockAddr.ss_family = AF_INET6;
					if (AddressStringToBinary((char *)Parameter.c_str(), &((sockaddr_in6 *)&SockAddr)->sin6_addr, AF_INET6, Result) == EXIT_FAILURE)
					{
						wprintf(L"\nTarget format error, error code is %d.\n", (int)Result);
						return EXIT_FAILURE;
					}

					TargetString.append("[");
					TargetString.append(Parameter);
					TargetString.append("]");
				}
				else {
					for (auto StringIter = Parameter.begin();StringIter != Parameter.end();StringIter++)
					{
					//Domain
						if (*StringIter < 46 || *StringIter == 47 || *StringIter > 57)
						{
							addrinfo AddrInfoHints = {0}, *AddrInfo = nullptr;
							if (SockAddr.ss_family == AF_INET6) //IPv6
							{
								AddrInfoHints.ai_family = AF_INET6;
							}
							else { //IPv4
								SockAddr.ss_family = AF_INET;
								AddrInfoHints.ai_family = AF_INET;
							}

						//Get address.
							Result = getaddrinfo(Parameter.c_str(), NULL, &AddrInfoHints, &AddrInfo);
							if (Result != 0)
							{
								wprintf(L"\nResolve domain name error, error code is %d.\n", (int)Result);
								return EXIT_FAILURE;
							}
							else {
							//Get address form PTR.
								for (auto PTR = AddrInfo;PTR != nullptr;PTR = PTR->ai_next)
								{
								//IPv6
									if (PTR->ai_family == AF_INET6 && SockAddr.ss_family == AF_INET6 && 
										!IN6_IS_ADDR_LINKLOCAL((in6_addr *)(PTR->ai_addr)) &&
										!(((sockaddr_in6 *)(PTR->ai_addr))->sin6_scope_id == 0)) //Get port from first(Main) IPv6 device
									{
										((sockaddr_in6 *)&SockAddr)->sin6_addr = ((sockaddr_in6 *)(PTR->ai_addr))->sin6_addr;

									//Get string of address.
										TargetDomainString = Parameter;
										std::shared_ptr<char> Buffer(new char[ADDR_STRING_MAXSIZE]());
										inet_ntop(AF_INET6, &((sockaddr_in6 *)&SockAddr)->sin6_addr, Buffer.get(), ADDR_STRING_MAXSIZE);
										TargetString.append("[");
										TargetString.append(Buffer.get());
										TargetString.append("]");
										break;
									}
								//IPv4
									else if (PTR->ai_family == AF_INET && SockAddr.ss_family == AF_INET && 
										((sockaddr_in *)(PTR->ai_addr))->sin_addr.s_addr != INADDR_LOOPBACK && 
										((sockaddr_in *)(PTR->ai_addr))->sin_addr.s_addr != INADDR_BROADCAST)
									{
										((sockaddr_in *)&SockAddr)->sin_addr = ((sockaddr_in *)(PTR->ai_addr))->sin_addr;

									//Get string of address.
										TargetDomainString = Parameter;
										std::shared_ptr<char> Buffer(new char[ADDR_STRING_MAXSIZE]());
										inet_ntop(AF_INET, &((sockaddr_in *)&SockAddr)->sin_addr, Buffer.get(), ADDR_STRING_MAXSIZE);
										TargetString = Buffer.get();
										break;
									}
								}

								freeaddrinfo(AddrInfo);
							}

							break;
						}

					//IPv4
						if (StringIter == Parameter.end() - 1U)
						{
							if (SockAddr.ss_family == AF_INET6)
							{
								wprintf(L"\nTarget protocol error.\n");
								return EXIT_FAILURE;
							}

							SockAddr.ss_family = AF_INET;
							if (AddressStringToBinary(Parameter.c_str(), &((sockaddr_in *)&SockAddr)->sin_addr, AF_INET, Result) == EXIT_FAILURE)
							{
								wprintf(L"\nTarget format error, error code is %d.\n", (int)Result);
								return EXIT_FAILURE;
							}

							TargetString = Parameter;
						}
					}
				}
			}
		}

	//Check parameter reading.
		if (SockAddr.ss_family == AF_INET6)
		{
			if (CheckEmptyBuffer(&((sockaddr_in6 *)&SockAddr)->sin6_addr, sizeof(in6_addr)))
			{
				wprintf(L"\nTarget is empty.\n");
				return EXIT_FAILURE;
			}
			else {
			//Mark port.
				if (Port == 0)
				{
					Port = htons(DNS_PORT);
					((sockaddr_in6 *)&SockAddr)->sin6_port = htons(DNS_PORT);
				}
				else {
					((sockaddr_in6 *)&SockAddr)->sin6_port = Port;
				}
			}
		}
		else { //IPv4
			if (((sockaddr_in *)&SockAddr)->sin_addr.s_addr == 0)
			{
				wprintf(L"\nTarget is empty.\n");
				return EXIT_FAILURE;
			}
			else {
			//Mark port.
				if (Port == 0)
				{
					Port = htons(DNS_PORT);
					((sockaddr_in *)&SockAddr)->sin_port = htons(DNS_PORT);
				}
				else {
					((sockaddr_in *)&SockAddr)->sin_port = Port;
				}
			}
		}

	//Check parameter.
		MinTime = SocketTimeout.tv_sec * 1000 + SocketTimeout.tv_usec / 1000;

	//Check DNS header.
		if (HeaderParameter.Flags == 0)
			HeaderParameter.Flags = htons(DNS_STANDARD);
		if (HeaderParameter.Questions == 0)
			HeaderParameter.Questions = htons(0x0001);

	//Check DNS query.
		if (QueryParameter.Classes == 0)
			QueryParameter.Classes = htons(DNS_CLASS_IN);
		if (QueryParameter.Type == 0)
		{
			if (SockAddr.ss_family == AF_INET6) //IPv6
				QueryParameter.Type = htons(DNS_AAAA_RECORDS);
			else //IPv4
				QueryParameter.Type = htons(DNS_A_RECORDS);
		}

	//Check EDNS0 Label.
		if (DNSSEC)
			EDNS0 = true;
		if (EDNS0)
		{
			HeaderParameter.Additional = htons(0x0001);
			EDNS0Parameter.Type = htons(DNS_EDNS0_RECORDS);
			if (EDNS0PayloadSize == 0)
				EDNS0Parameter.UDPPayloadSize = htons(EDNS0_MINSIZE);
			else 
				EDNS0Parameter.UDPPayloadSize = htons((uint16_t)EDNS0PayloadSize);
			if (DNSSEC)
			{
				HeaderParameter.FlagsBits.AD = ~HeaderParameter.FlagsBits.AD; //Local DNSSEC Server validate
				HeaderParameter.FlagsBits.CD = ~HeaderParameter.FlagsBits.CD; //Client validate
				EDNS0Parameter.Z_Bits.DO = ~EDNS0Parameter.Z_Bits.DO; //Accepts DNSSEC security RRs
			}
		}

	//Print to screen before sending.
		wprintf(L"\n");
		if (ReverseLookup)
		{
			if (TargetDomainString.empty())
			{
				std::shared_ptr<char> FQDN(new char[NI_MAXHOST]());
				if (getnameinfo((sockaddr *)&SockAddr, sizeof(sockaddr_in), FQDN.get(), NI_MAXHOST, nullptr, 0, NI_NUMERICSERV) != 0)
				{
					wprintf(L"Resolve addresses to host names error, error code is %d.\n", errno);
					return EXIT_FAILURE;
				}
				else {
					if (TargetString == FQDN.get())
					{
						wprintf(L"DNSPing %s:%u with %s:\n", TargetString.c_str(), ntohs(Port), TestDomain.c_str());
					}
					else {
						wprintf(L"DNSPing %s:%u [%s] with %s:\n", FQDN.get(), ntohs(Port), TargetString.c_str(), TestDomain.c_str());
					}
				}
			}
			else {
				wprintf(L"DNSPing %s:%u [%s] with %s:\n", TargetDomainString.c_str(), ntohs(Port), TargetString.c_str(), TestDomain.c_str());
			}
		}
		else {
			if (!TargetDomainString.empty())
				wprintf(L"DNSPing %s:%u [%s] with %s:\n", TargetDomainString.c_str(), ntohs(Port), TargetString.c_str(), TestDomain.c_str());
			else 
				wprintf(L"DNSPing %s:%u with %s:\n", TargetString.c_str(), ntohs(Port), TestDomain.c_str());
		}

	//Send.
		if (SendNum == 0)
		{
			while (true)
			{
				if (RealSendNum <= U16_MAXNUM)
				{
					RealSendNum++;
					if (SendProcess(AF_INET, SockAddr) == EXIT_FAILURE)
						return EXIT_FAILURE;
				}
				else {
					wprintf(L"\nStatistics is full.\n");
					PrintProcess(true, true);

					return EXIT_SUCCESS;
				}
			}
		}
		else {
			for (size_t Index = 0;Index < SendNum;Index++)
			{
				RealSendNum++;
				if (SendProcess(AF_INET, SockAddr) == EXIT_FAILURE)
					return EXIT_FAILURE;
			}
		}

	//Print to screen before finished.
		PrintProcess(true, true);
	}
	else {
		PrintDescription();
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}
