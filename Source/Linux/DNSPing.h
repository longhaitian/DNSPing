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


// Base Header
#include <cstring>                 //C-Style strings

//C++ Standard Template Library/STL header
#include <memory>                  //Threads
#include <string>                  //Strings

//Portable Operating System Interface/POSIX and Unix system header
#include <limits.h>                //Limits
#include <pthread.h>               //Threads
#include <signal.h>                //Signals
#include <unistd.h>                //Standard library API
#include <netdb.h>                 //Network database operations
#include <arpa/inet.h>             //Internet operations
#include <sys/time.h>              //Date and time

// Base defines
#pragma pack(1)                    //Memory alignment: 1 bytes/8 bits

// Protocol Header structures
//Protocol defines
#define IPPROTO_GGP                 3U
#define IPPROTO_ST                  5U
#define IPPROTO_CBT                 7U
#define IPPROTO_IGP                 9U
#define IPPROTO_ND                  77U
#define IPPROTO_ICLFXBM             78U
#define IPPROTO_PGM                 113U
#define IPPROTO_L2TP                115U

//Port defines(1 - 1024)
#define IPPORT_TCPMUX               1U
#define IPPORT_QOTD                 17U
#define IPPORT_MSP                  18U
#define IPPORT_CHARGEN              19U
#define IPPORT_FTP_DATA             20U
#define IPPORT_SSH                  22U
#define IPPORT_RAP                  38U
#define IPPORT_RLP                  39U
#define IPPORT_TACACS               49U
#define IPPORT_XNSAUTH              56U
#define IPPORT_BOOTPS               67U
#define IPPORT_BOOTPC               68U
#define IPPORT_HTTP                 80U
#define IPPORT_HTTPBACKUP           81U
#define IPPORT_POP3                 110U
#define IPPORT_SUNRPC               111U
#define IPPORT_SQL                  118U
#define IPPORT_NTP                  123U
#define IPPORT_EPMAP                135U
#define IPPORT_NETBIOS_NS           137U
#define IPPORT_NETBIOS_DGM          138U
#define IPPORT_NETBIOS_SSN          139U
#define IPPORT_IMAP                 143U
#define IPPORT_BFTP                 152U
#define IPPORT_SGMP                 153U
#define IPPORT_SQLSRV               156U
#define IPPORT_DMSP                 158U
#define IPPORT_SNMP                 161U
#define IPPORT_SNMP_TRAP            162U
#define IPPORT_ATRTMP               201U
#define IPPORT_ATHBP                202U
#define IPPORT_QMTP                 209U
#define IPPORT_IPX                  213U
#define IPPORT_IMAP3                220U
#define IPPORT_BGMP                 264U
#define IPPORT_TSP                  318U
#define IPPORT_IMMP                 323U
#define IPPORT_ODMR                 366U
#define IPPORT_RPC2PORTMAP          369U
#define IPPORT_CLEARCASE            371U
#define IPPORT_HPALARMMGR           383U
#define IPPORT_ARNS                 384U
#define IPPORT_AURP                 387U
#define IPPORT_LDAP                 389U
#define IPPORT_UPS                  401U
#define IPPORT_SLP                  427U
#define IPPORT_HTTPS                443U
#define IPPORT_SNPP                 444U
#define IPPORT_MICROSOFT_DS         445U
#define IPPORT_KPASSWD              464U
#define IPPORT_TCPNETHASPSRV        475U
#define IPPORT_RETROSPECT           497U
#define IPPORT_ISAKMP               500U
#define IPPORT_SYSLOG               514U
#define IPPORT_NCP                  524U
#define IPPORT_COURIER              530U
#define IPPORT_COMMERCE             542U
#define IPPORT_RTSP                 554U
#define IPPORT_NNTP                 563U
#define IPPORT_HTTPRPCEPMAP         593U
#define IPPORT_IPP                  631U
#define IPPORT_LDAPS                636U
#define IPPORT_MSDP                 639U
#define IPPORT_AODV                 654U
#define IPPORT_FTPSDATA             989U
#define IPPORT_FTPS                 990U
#define IPPORT_NAS                  991U
#define IPPORT_TELNETS              992U

//Domain Name System/DNS Part
/* About RFC standards
RFC 920(https://tools.ietf.org/html/rfc920), Domain Requirements ¨C Specified original top-level domains
RFC 1032(https://tools.ietf.org/html/rfc1032), Domain Administrators Guide
RFC 1033(https://tools.ietf.org/html/rfc1033), Domain Administrators Operations Guide
RFC 1034(https://tools.ietf.org/html/rfc1034), Domain Names - Concepts and Facilities
RFC 1035(https://tools.ietf.org/html/rfc1035), Domain Names - Implementation and Specification
RFC 1101(https://tools.ietf.org/html/rfc1101), DNS Encodings of Network Names and Other Types
RFC 1123(https://tools.ietf.org/html/rfc1123), Requirements for Internet Hosts¡ªApplication and Support
RFC 1178(https://tools.ietf.org/html/rfc1178), Choosing a Name for Your Computer (FYI 5)
RFC 1183(https://tools.ietf.org/html/rfc1183), New DNS RR Definitions
RFC 1591(https://tools.ietf.org/html/rfc1591), Domain Name System Structure and Delegation (Informational)
RFC 1912(https://tools.ietf.org/html/rfc1912), Common DNS Operational and Configuration Errors
RFC 1995(https://tools.ietf.org/html/rfc1995), Incremental Zone Transfer in DNS
RFC 1996(https://tools.ietf.org/html/rfc1996), A Mechanism for Prompt Notification of Zone Changes (DNS NOTIFY)
RFC 2100(https://tools.ietf.org/html/rfc2100), The Naming of Hosts (Informational)
RFC 2136(https://tools.ietf.org/html/rfc2136), Dynamic Updates in the domain name system (DNS UPDATE)
RFC 2181(https://tools.ietf.org/html/rfc2181), Clarifications to the DNS Specification
RFC 2182(https://tools.ietf.org/html/rfc2182), Selection and Operation of Secondary DNS Servers
RFC 2308(https://tools.ietf.org/html/rfc2308), Negative Caching of DNS Queries (DNS NCACHE)
RFC 2317(https://tools.ietf.org/html/rfc2317), Classless IN-ADDR.ARPA delegation (BCP 20)
RFC 2671(https://tools.ietf.org/html/rfc2671), Extension Mechanisms for DNS (EDNS0)
RFC 2672(https://tools.ietf.org/html/rfc2672), Non-Terminal DNS Name Redirection
RFC 2845(https://tools.ietf.org/html/rfc2845), Secret Key Transaction Authentication for DNS (TSIG)
RFC 3225(https://tools.ietf.org/html/rfc3225), Indicating Resolver Support of DNSSEC
RFC 3226(https://tools.ietf.org/html/rfc3226), DNSSEC and IPv6 A6 aware server/resolver message size requirements
RFC 3597(https://tools.ietf.org/html/rfc3597), Handling of Unknown DNS Resource Record (RR) Types
RFC 3696(https://tools.ietf.org/html/rfc3696), Application Techniques for Checking and Transformation of Names (Informational)
RFC 4343(https://tools.ietf.org/html/rfc4343), Domain Name System (DNS) Case Insensitivity Clarification
RFC 4592(https://tools.ietf.org/html/rfc4592), The Role of Wildcards in the Domain Name System
RFC 4635(https://tools.ietf.org/html/rfc4635), HMAC SHA TSIG Algorithm Identifiers
RFC 4892(https://tools.ietf.org/html/rfc4892), Requirements for a Mechanism Identifying a Name Server Instance (Informational)
RFC 5001(https://tools.ietf.org/html/rfc5001), DNS Name Server Identifier (NSID) Option
RFC 5452(https://tools.ietf.org/html/rfc5452), Measures for Making DNS More Resilient against Forged Answers
RFC 5625(https://tools.ietf.org/html/rfc5625), DNS Proxy Implementation Guidelines (BCP 152)
RFC 5890(https://tools.ietf.org/html/rfc5890), Internationalized Domain Names for Applications (IDNA):Definitions and Document Framework
RFC 5891(https://tools.ietf.org/html/rfc5891), Internationalized Domain Names in Applications (IDNA): Protocol
RFC 5892(https://tools.ietf.org/html/rfc5892), The Unicode Code Points and Internationalized Domain Names for Applications (IDNA)
RFC 5893(https://tools.ietf.org/html/rfc5893), Right-to-Left Scripts for Internationalized Domain Names for Applications (IDNA)
RFC 5894(https://tools.ietf.org/html/rfc5894), Internationalized Domain Names for Applications (IDNA):Background, Explanation, and Rationale (Informational)
RFC 5895(https://tools.ietf.org/html/rfc5895), Mapping Characters for Internationalized Domain Names in Applications (IDNA) 2008 (Informational)
RFC 5966(https://tools.ietf.org/html/rfc5966), DNS Transport over TCP - Implementation Requirements
RFC 6195(https://tools.ietf.org/html/rfc6195), Domain Name System (DNS) IANA Considerations (BCP 42)
*/

//Default Port and Record Types defines
#define DNS_PORT                53U      //Port(TCP and UDP)
#define DNS_STANDARD            0x0100   //System Standard query
#define DNS_SQRNE               0x8180   //Standard query response and no error.
#define DNS_SQRNE_TC            0x8380   //Standard query response and no error, but Truncated.
#define DNS_SQRNE_FE            0x8181   //Standard query response, Format Error
#define DNS_SQRNE_SF            0x8182   //Standard query response, Server failure
#define DNS_NO_SUCH_NAME        0x8183   //Standard query response, but no such name.
#define DNS_RCODE_NO_SUCH_NAME  0x0003   //RCode is 0x0003(No Such Name).
#define DNS_CLASS_IN            0x0001   //DNS Class IN, its ID is 1.
#define DNS_A_RECORDS           0x0001   //DNS A records, its ID is 1.
#define DNS_NS_RECORDS          0x0002   //DNS NS records, its ID is 2.
#define DNS_CNAME_RECORDS       0x0005   //DNS CNAME records, its ID is 5.
#define DNS_SOA_RECORDS         0x0006   //DNS SOA records, its ID is 6.
#define DNS_PTR_RECORDS         0x000C   //DNS PTR records, its ID is 12.
#define DNS_MX_RECORDS          0x000F   //DNS MX records, its ID is 15.
#define DNS_TXT_RECORDS         0x0010   //DNS TXT records, its ID is 16.
#define DNS_RP_RECORDS          0x0011   //DNS RP records, its ID is 17.
#define DNS_SIG_RECORDS         0x0018   //DNS SIG records, its ID is 24.
#define DNS_KEY_RECORDS         0x0019   //DNS KEY records, its ID is 25.
#define DNS_AAAA_RECORDS        0x001C   //DNS AAAA records, its ID is 28.
#define DNS_LOC_RECORDS         0x001D   //DNS LOC records, its ID is 29.
#define DNS_SRV_RECORDS         0x0021   //DNS SRV records, its ID is 33.
#define DNS_NAPTR_RECORDS       0x0023   //DNS NAPTR records, its ID is 35.
#define DNS_KX_RECORDS          0x0024   //DNS KX records, its ID is 36.
#define DNS_CERT_RECORDS        0x0025   //DNS CERT records, its ID is 37.
#define DNS_DNAME_RECORDS       0x0027   //DNS DNAME records, its ID is 39.
#define DNS_EDNS0_RECORDS       0x0029   //DNS EDNS0 Label/OPT records, its ID is 41.
#define DNS_APL_RECORDS         0x002A   //DNS APL records, its ID is 42.
#define DNS_DS_RECORDS          0x002B   //DNS DS records, its ID is 43.
#define DNS_SSHFP_RECORDS       0x002C   //DNS SSHFP records, its ID is 44.
#define DNS_IPSECKEY_RECORDS    0x002D   //DNS IPSECKEY records, its ID is 45.
#define DNS_RRSIG_RECORDS       0x002E   //DNS RRSIG records, its ID is 46.
#define DNS_NSEC_RECORDS        0x002F   //DNS NSEC records, its ID is 47.
#define DNS_DNSKEY_RECORDS      0x0030   //DNS DNSKEY records, its ID is 48.
#define DNS_DHCID_RECORDS       0x0031   //DNS DHCID records, its ID is 49.
#define DNS_NSEC3_RECORDS       0x0032   //DNS NSEC3 records, its ID is 50.
#define DNS_NSEC3PARAM_RECORDS  0x0033   //DNS NSEC3PARAM records, its ID is 51.
#define DNS_HIP_RECORDS         0x0037   //DNS HIP records, its ID is 55.
#define DNS_SPF_RECORDS         0x0063   //DNS SPF records, its ID is 99.
#define DNS_TKEY_RECORDS        0x00F9   //DNS TKEY records, its ID is 249.
#define DNS_TSIG_RECORDS        0x00FA   //DNS TSIG records, its ID is 250.
#define DNS_IXFR_RECORDS        0x00FB   //DNS IXFR records, its ID is 251.
#define DNS_AXFR_RECORDS        0x00FC   //DNS AXFR records, its ID is 252.
#define DNS_ANY_RECORDS         0x00FF   //DNS ANY records, its ID is 255.
#define DNS_TA_RECORDS          0x8000   //DNS TA records, its ID is 32768.
#define DNS_DLV_RECORDS         0x8001   //DNS DLV records, its ID is 32769.
#define DNS_CLASS_IN            0x0001   //DNS Class INTERNET, its ID is 1.
#define DNS_CLASS_CSNET         0x0002   //DNS Class CSNET, its ID is 2.
#define DNS_CLASS_CHAOS         0x0003   //DNS Class CHAOS, its ID is 3.
#define DNS_CLASS_HESIOD        0x0004   //DNS Class HESIOD, its ID is 4.
#define DNS_CLASS_NONE          0x00FE   //DNS Class NONE, its ID is 254.
#define DNS_CLASS_ALL           0x00FF   //DNS Class ALL, its ID is 255.
#define DNS_CLASS_ANY           0x00FF   //DNS Class ANY, its ID is 255.
#define DNS_QUERY_PTR           0xC00C   //Pointer of first query

/*
// With User Datagram Protocol/UDP

                    1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        Identification         |Q|OPCode |A|T|R|R|Z|A|C| RCode |  QR/Query and Response, AA/Authoritative Answer, TC/Truncated, RD/Recursion Desired, RA/Recursion Available
|                               |R|       |A|C|D|A| |D|D|       |  Z/Zero, AD/Authenticated Data, CD/Checking Disabled, RCode/Return Code
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        Total Questions        |       Total Answer RRs        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      Total Authority RRs      |     Total Additional RRs      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/
#define OLD_DNS_MAXSIZE 512U
typedef struct _dns_hdr
{
	uint16_t              ID;
	union {
		uint16_t          Flags;
		struct {
		#if __BYTE_ORDER == __LITTLE_ENDIAN
			uint8_t       RD:1;
			uint8_t       TC:1;
			uint8_t       AA:1;
			uint8_t       OPCode_Second:1;
			uint8_t       OPCode_First:3;
			uint8_t       QR:1;
			uint8_t       RCode:4;
			uint8_t       CD:1;
			uint8_t       AD:1;
			uint8_t       Zero:1;
			uint8_t       RA:1;
		#else //BIG_ENDIAN
			uint8_t       QR:1;
			uint8_t       OPCode:4;
			uint8_t       AA:1;
			uint8_t       TC:1;
			uint8_t       RD:1;
			uint8_t       RA:1;
			uint8_t       Zero:1;
			uint8_t       AD:1;
			uint8_t       CD:1;
			uint8_t       RCode:4;
		#endif
		}FlagsBits;
	};
	uint16_t              Questions;
	uint16_t              Answer;
	uint16_t              Authority;
	uint16_t              Additional;
}dns_hdr;

/* Domain Name System/DNS Query

                    1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/                                                               /
/                            Domain                             /
/                                                               /
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|             Type              |            Classes            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/
typedef struct _dns_qry
{
//	PUCHAR                Name;
	uint16_t              Type;
	uint16_t              Classes;
}dns_qry;

/* Extension Mechanisms for Domain Name System/DNS, EDNS0 Label/OPT Resource

                    1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/                                                               /
/                            Domain                             /
/                                                               /
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|             Type              |       UDP Payload Size        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Extended RCode | EDNS0 Version |D|          Reserved           |  Extended RCode/Higher bits in extended Return Code, D/DO bit
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            Length             |\---------- Z Field -----------/
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/
#define EDNS0_MINSIZE 1220U
typedef struct _dns_edns0_
{
	uint8_t               Name;
	uint16_t              Type;              //Additional RRs Type
	uint16_t              UDPPayloadSize;
	uint8_t               Extended_RCode;
	uint8_t               Version;           //EDNS0 Version
	union {
		uint16_t          Z_Field;
		struct {
		#if __BYTE_ORDER == __LITTLE_ENDIAN
			uint8_t       Reserved_First:7;
			uint8_t       DO:1;              //DO bit
		#else //BIG_ENDIAN
			uint8_t       DO:1;              //DO bit
			uint8_t       Reserved_First:7;
		#endif
			uint8_t       Reserved_Second;
		}Z_Bits;
	};
	uint16_t              DataLength;
}dns_edns0_label;

//Base defines
#define RETURN_ERROR         -1
#define PACKET_MINSIZE       64U                  //Minimum size of packets in Ethernet network.
#define PACKET_MAXSIZE       1512U                //Maximum size of packets(1500 bytes maximum payload length + 8 bytes Ethernet header + 4 bytes FCS), Standard MTU of Ethernet network
#define LARGE_PACKET_MAXSIZE 4096U                //Maximum size of packets(4KB/4096 bytes) of TCP protocol
#define ADDR_STRING_MAXSIZE  64U                  //Maximum size of addresses(IPv4/IPv6) words
#define U4_MAXNUM            0x000F               //Maximum value of half of uint8_t/4 bits
#define U8_MAXNUM            0x00FF               //Maximum value of uint8_t/8 bits
#define U16_MAXNUM           0xFFFF               //Maximum value of uint16_t/16 bits
#define TIME_OUT             1000000U             //Timeout(1000000 us or 1000 ms or 1 second)
#define TIME_OUT_MIN         500U                 //Minimum timeout(500 ms)
#define DEFAULT_TIME_OUT     2U                   //Default timeout(2 seconds)
#define DEFAULT_SEND_TIMES   4U                   //Default send times

//Protocol.cc
bool CheckEmptyBuffer(const void *Buffer, const size_t Length);
size_t AddressStringToBinary(const char *AddrString, void *pAddr, const uint16_t Protocol, ssize_t &ErrorCode);
size_t CharToDNSQuery(const char *FName, char *TName);

//Process.cc
size_t SendProcess(const sockaddr_storage Target);
size_t PrintProcess(const bool PacketStatistics, const bool TimeStatistics);
void PrintDescription(void);

//Terminal.cc
void SIG_Handler(int Signal);
