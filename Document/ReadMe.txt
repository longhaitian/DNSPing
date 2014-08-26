DNSPing v0.1 Beta(Windows)
DNSPing, Ping with DNS requesting.
Copyright (C) 2014 Chengr28


-------------------------------------------------------------------------------


Project on GitHub:
https://github.com/chengr28/DNSPing

TCPing(http://www.elifulkerson.com/projects/tcping.php):
* DNSPing server which using UDP protocol.
* TCPing 53 port of server if you want to using TCP protocol.

TraceTCP(https://simulatedsimian.github.io/tracetcp.html):
* TCP traceroute 53 port of server if you want to using TCP protocol.


-------------------------------------------------------------------------------


Usage: DNSPing [-h] [-t] [-a] [-n Count] [-f] [-i HopLimit/TTL] [-w Timeout]
               [-id DNS_ID] [-qr] [-opcode OPCode] [-aa] [-tc]
               [-rd] [-ra] [-ad] [-cd] [-rcode RCode] [-qn Count]
               [-ann Count] [-aun Count] [-adn Count] [-ti Time] [-edns0]
               [-payload Length] [-dnssec] [-qt Type] [-qc Classes]
               [-p ServiceName] [-rawdata RAW_Data] [-raw ServiceName]
               [-buf Size] [-of FileName] Test_DomainName Target


-------------------------------------------------------------------------------


Options:
   N/A               Description.
   ?                 Description.
   -h                Description.
   -t                Pings the specified host until stopped.
                     To see statistics and continue type Control-Break.
                     To stop type Control-C.
   -a                Resolve addresses to host names.
   -n Count          Set number of echo requests to send.
                     Count must between 1 - 0xFFFF/65535.
   -f                Set the "Don't Fragment" flag in outgoing packets(IPv4).
   -i HopLimit/TTL   Specifie a Time To Live for outgoing packets.
                     HopLimit/TTL must between 1 - 255.
   -w Timeout        Set a long wait periods (in milliseconds) for a response.
                     Timeout must between 500 - 0xFFFF/65535.
   -id DNS_ID        Specifie DNS header ID.
                     DNS ID must between 0x0001 - 0xFFFF/65535.
   -qr               Set DNS header QR flag.
   -opcode OPCode    Specifie DNS header OPCode.
                     OPCode must between 0x0000 - 0x00FF/255.
   -aa               Set DNS header AA flag.
   -tc               Set DNS header TC flag.
   -rd               Set DNS header RD flag.
   -ra               Set DNS header RA flag.
   -ad               Set DNS header AD flag.
   -cd               Set DNS header CD flag.
   -rcode RCode      Specifie DNS header RCode.
                     RCode must between 0x0000 - 0x00FF/255.
   -qn Count         Specifie DNS header Question count.
                     Question count must between 0x0001 - 0xFFFF/65535.
   -ann Count        Specifie DNS header Answer count.
                     Answer count must between 0x0001 - 0xFFFF/65535.
   -aun Count        Specifie DNS header Authority count.
                     Authority count must between 0x0001 - 0xFFFF/65535.
   -adn Count        Specifie DNS header Additional count.
                     Additional count must between 0x0001 - 0xFFFF/65535.
   -ti IntervalTime  Specifie transmission interval time(in milliseconds).
   -edns0            Send with EDNS0 Label.
   -payload Length   Specifie EDNS0 Label UDP Payload length.
                     Payload length must between 512 - 0xFFFF/65535.
   -dnssec           Send with DNSSEC requesting.
                     EDNS0 Label will enable when DNSSEC is enable.
   -qt Type          Specifie Query type.
                     Query type must between 0x0001 - 0xFFFF/65535.
                     Type: A|NS|CNAME|SOA|PTR|MX|TXT|RP|SIG|KEY|AAAA|LOC|SRV|
                           NAPTR|KX|CERT|DNAME|EDNS0|APL|DS|SSHFP|IPSECKEY|
                           RRSIG|NSEC|DNSKEY|DHCID|NSEC3|NSEC3PARAM|HIP|SPF|
                           TKEY|TSIG|IXFR|AXFR|ANY|TA|DLV
   -qc Classes       Specifie Query classes.
                     Query classes must between 0x0001 - 0xFFFF/65535.
                     Classes: IN|CSNET|CHAOS|HESIOD|NONE|ALL|ANY
   -p ServiceName    Specifie UDP port/protocol(Sevice names).
                     UDP port must between 0x0001 - 0xFFFF/65535.
                     Protocol: TCPMUX|ECHO|DISCARD|SYSTAT|DAYTIME|NETSTAT|
                               QOTD|MSP|CHARGEN|FTP|SSH|TELNET|SMTP|
                               TIME|RAP|RLP|NAME|WHOIS|TACACS|XNSAUTH|MTP|
                               BOOTPS|BOOTPC|TFTP|RJE|FINGER|TTYLINK|SUPDUP|
                               SUNRPC|SQL|NTP|EPMAP|NETBIOSNS|NETBIOSDGM|
                               NETBIOSSSN|IMAP|BFTP|SGMP|SQLSRV|DMSP|SNMP|
                               SNMPTRAP|ATRTMP|ATHBP|QMTP|IPX|IMAP|IMAP3|
                               BGMP|TSP|IMMP|ODMR|RPC2PORTMAP|CLEARCASE|
                               HPALARMMGR|ARNS|AURP|LDAP|UPS|SLP|SNPP|
                               MICROSOFTDS|KPASSWD|TCPNETHASPSRV|RETROSPECT|
                               ISAKMP|BIFFUDP|WHOSERVER|SYSLOG|ROUTERSERVER|
                               NCP|COURIER|COMMERCE|RTSP|NNTP|HTTPRPCEPMAP|
                               IPP|LDAPS|MSDP|AODV|FTPSDATA|FTPS|NAS|TELNETS
   -rawdata RAW_Data Specifie Raw data to send.
                     RAW_Data is hex, but do not add "0x" before hex.
                     Length of RAW_Data must between 64 - 1512 bytes.
   -raw ServiceName  Specifie Raw socket type.
                     Service Name: HOPOPTS|ICMP|IGMP|GGP|IPV4|ST|TCP|CBT|EGP|
                                   IGP|PUP|IDP|IPV6|ROUTING|ESP|FRAGMENT|AH|
                                   ICMPV6|NONE|DSTOPTS|ND|ICLFXBM|PIM|PGM|L2TP|
                                   SCTP|RAW
   -buf Size         Specifie receive buffer size.
                     Buffer size must between 512 - 4096 bytes.
   -of FileName      Output result to file.
                     FileName must less than 260 bytes.
   -6                Using IPv6.
   -4                Using IPv4.
   Test_DomainName   A domain name which will make requesting to send
                     to DNS server.
   Target            Target of DNSPing, support IPv4/IPv6 address and domain.
