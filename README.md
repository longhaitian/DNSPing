DNSPing
=======
Ping with DNS requesting.

### Version
* **Windows: v0.1 Beta(2014-08-26)**
* **Linux: v0.1 Beta(2014-08-26)**

### ReadMe
* [Windows(English)](https://github.com/chengr28/DNSPing/wiki/ReadMe)
* [Windows(Chinese-Simplified)](https://github.com/chengr28/DNSPing/wiki/ReadMe#%E7%94%A8%E6%B3%95)
* [Linux(English)](https://github.com/chengr28/DNSPing/wiki/ReadMe_Linux)
* [Linux(Chinese-Simplified)](https://github.com/chengr28/DNSPing/wiki/ReadMe_Linux#%E7%94%A8%E6%B3%95)

### Usage
`DNSPing [-h] [-t] [-a] [-n Count] [-f] [-i HopLimit/TTL] [-w Timeout] [-id DNS_ID] [-qr] [-opcode OPCode] [-aa] [-tc] [-rd] [-ra] [-ad] [-cd] [-rcode RCode] [-qn Count] [-ann Count] [-aun Count] [-adn Count] [-ti Time] [-edns0] [-payload Length] [-dnssec] [-qt Type] [-qc Classes] [-p ServiceName] [-rawdata RAW_Data] [-raw ServiceName] [-buf Size] [-of FileName] Test_DomainName Target`<br />

* `[-f]` is only available in Windows.<br />

### Options
* N/A
    * Description.<br />
* `?`
    * Description.<br />
* `-h`
    * Description.<br />
* `-t`
    * Pings the specified host until stopped.<br />
    * To see statistics and continue type `Control-Break`.<br />
    * To stop type `Control-C`.<br />
* `-a`
    * Resolve addresses to host names.<br />
* `-n Count`
    * Set number of echo requests to send.<br />
    * `Count` must between 1 - 0xFFFF/65535.<br />
* `-f`(Windows)
    * Set the `Don't Fragment` flag in outgoing packets(IPv4).<br />
* `-i HopLimit/TTL`
    * Specifie a `Time To Live` for outgoing packets.<br />
    * `HopLimit/TTL` must between 1 - 255.<br />
* `-w Timeout`
    * Set a long wait periods (in milliseconds) for a response.<br />
    * `Timeout` must between 500 - 0xFFFF/65535.<br />
* `-id DNS_ID`
    * Specifie DNS header ID.<br />
    * `DNS ID` must between 0x0001 - 0xFFFF/65535.<br />
* `-qr`
    * Set DNS header `QR` flag.<br />
* `-opcode OPCode`
    * Specifie DNS header `OPCode`.<br />
    * `OPCode` must between 0x0000 - 0x00FF/255.<br />
* `-aa`
    * Set DNS header `AA` flag.<br />
* `-tc`
    * Set DNS header `TC` flag.<br />
* `-rd`
    * Set DNS header `RD` flag.<br />
* `-ra`
    * Set DNS header `RA` flag.<br />
* `-ad`
    * Set DNS header `AD` flag.<br />
* `-cd`
    * Set DNS header `CD` flag.<br />
* `-rcode RCode`
    * Specifie DNS header `RCode`.<br />
    * `RCode` must between 0x0000 - 0x00FF/255<br />
* `-qn Count`
    * Specifie DNS header `Question count`.<br />
    * `Question count` must between 0x0001 - 0xFFFF/65535.<br />
* `-ann Count`
    * Specifie DNS header `Answer count`.<br />
    * `Answer count` must between 0x0001 - 0xFFFF/65535.<br />
* `-aun Count`
    * Specifie DNS header `Authority count`.<br />
    * `Authority count` must between 0x0001 - 0xFFFF/65535.<br />
* `-adn Count`
    * Specifie DNS header `Additional count`.<br />
    * `Additional count` must between 0x0001 - 0xFFFF/65535.<br />
* `-ti IntervalTime`
    * Specifie transmission interval time(in milliseconds).<br />
* `-edns0`
    * Send with `EDNS0 Label`.<br />
* `-payload Length`
    * Specifie EDNS0 Label `UDP Payload length`.<br />
    * `Payload length` must between 512 - 0xFFFF/65535.<br />
* `-dnssec`
    * Send with `DNSSEC` requesting.<br />
    * `EDNS0 Label` will enable when `DNSSEC` is enable<br />
* `-qt Type`
    * Specifie `Query type`.<br />
    * `Type: A|NS|CNAME|SOA|PTR|MX|TXT|RP|SIG|KEY|AAAA|LOC|SRV|`<br />
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`NAPTR|KX|CERT|DNAME|EDNS0|APL|DS|SSHFP|IPSECKEY|`<br />
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`RRSIG|NSEC|DNSKEY|DHCID|NSEC3|NSEC3PARAM|HIP|SPF|`<br />
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`TKEY|TSIG|IXFR|AXFR|ANY|TA|DLV`<br />
* `-qc Classes`
    * Specifie `Query classes`.<br />
    * `Classes: IN|CSNET|CHAOS|HESIOD|NONE|ALL|ANY`<br />
* `-p ServiceName`
    * Specifie UDP port/protocol(Sevice names).<br />
    * UDP port must between 0x0001 - 0xFFFF/65535.<br />
    * `Protocol: TCPMUX|ECHO|DISCARD|SYSTAT|DAYTIME|NETSTAT|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`QOTD|MSP|CHARGEN|FTP|SSH|TELNET|SMTP|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`TIME|RAP|RLP|NAME|WHOIS|TACACS|XNSAUTH|MTP|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`BOOTPS|BOOTPC|TFTP|RJE|FINGER|TTYLINK|SUPDUP|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`SUNRPC|SQL|NTP|EPMAP|NETBIOSNS|NETBIOSDGM|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`NETBIOSSSN|IMAP|BFTP|SGMP|SQLSRV|DMSP|SNMP|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`SNMPTRAP|ATRTMP|ATHBP|QMTP|IPX|IMAP|IMAP3|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`BGMP|TSP|IMMP|ODMR|RPC2PORTMAP|CLEARCASE|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`HPALARMMGR|ARNS|AURP|LDAP|UPS|SLP|SNPP|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`MICROSOFTDS|KPASSWD|TCPNETHASPSRV|RETROSPECT|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`ISAKMP|BIFFUDP|WHOSERVER|SYSLOG|ROUTERSERVER|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`NCP|COURIER|COMMERCE|RTSP|NNTP|HTTPRPCEPMAP|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`IPP|LDAPS|MSDP|AODV|FTPSDATA|FTPS|NAS|TELNETS`<br />
* `-rawdata RAW_Data`
    * Specifie Raw data to send.<br />
    * `RAW_Data` is hex, but do not add `0x` before hex.<br />
    * Length of `RAW_Data` must between 64 - 1512 bytes.<br />
* `-raw ServiceName`
    * Specifie Raw socket type.<br />
    * `Service Name: HOPOPTS|ICMP|IGMP|GGP|IPV4|ST|TCP|CBT|EGP|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`IGP|PUP|IDP|IPV6|ROUTING|ESP|FRAGMENT|AH|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`ICMPV6|NONE|DSTOPTS|ND|ICLFXBM|PIM|PGM|L2TP|`<br />
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`SCTP|RAW`<br />
* `-buf Size`
    * Specifie receive buffer size.<br />
    * `Buffer size` must between 512 - 4096 bytes.<br />
* `-of FileName`
    * Output result to file.<br />
    * FileName must less than 260 bytes.<br />
* `-6`
    * Using IPv6.<br />
* `-4`
    * Using IPv4.<br />
* `Test_DomainName`
    * A domain name which will make requesting to send to DNS server.<br />
* `Target`
    * Target of DNSPing, support IPv4/IPv6 address and domain.<br />

### License
GNU General Public License/GNU GPL v2

### Thanks
* [tcping.exe - Ping over a tcp connection.](http://www.elifulkerson.com/projects/tcping.php)
* [tracetcp.exe - Traceroute utility that uses tcp syn packets to trace network routes.](https://simulatedsimian.github.io/tracetcp.html)
* [tcpping - Ping look-alike that uses TCP SYN packets to get around firewalls and ICMP blackholes](https://github.com/jwyllie83/tcpping)
