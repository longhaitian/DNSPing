DNSPing v0.1 Beta(Linux)
DNSPing, Ping with DNS requesting.
Copyright (C) 2014 Chengr28


-------------------------------------------------------------------------------


GitHub 项目页面：
https://github.com/chengr28/DNSPing

* DNSPing server with UDP protocol.
* TCPTraceroute/tcptraceroute 53 port of server if you want to using TCP protocol in Linux.


-------------------------------------------------------------------------------


用法: DNSPing [-h] [-t] [-a] [-n Count] [-i HopLimit/TTL] [-w Timeout]
               [-id DNS_ID] [-qr] [-opcode OPCode] [-aa] [-tc]
               [-rd] [-ra] [-ad] [-cd] [-rcode RCode] [-qn Count]
               [-ann Count] [-aun Count] [-adn Count] [-ti Time] [-edns0]
               [-payload Length] [-dnssec] [-qt Type] [-qc Classes] [-p Port]
               [-raw RAW_Data] [-buf Size] Test_DomainName Target


-------------------------------------------------------------------------------


选项：
   N/A               打印说明
   ?                 打印说明
   -h                打印说明
   -t                直到按下 Control-Break 或 Control-C 才停止 Ping
                     想看从 Ping 开始到按下时的统计信息但不想停止请按 Control-Break
                     想停止 Ping 并查看统计信息请按 Control-C
   -a                反向解析地址的域名
   -n Count          发送 Ping 的数量
                     Count 必须介乎于 1 - 0xFFFF/65535
   -i HopLimit/TTL   设定 Ping 数据包的生存时间/跳数限制
                     HopLimit/TTL 必须介乎于 1 - 255
   -w Timeout        设定超时时间（单位：毫秒）
                     Timeout 必须介乎于 500 - 0xFFFF/65535
   -id DNS_ID        设定 DNS 请求包头的 ID
                     DNS ID 必须介乎于 0x0001 - 0xFFFF/65535
   -qr               设定 DNS 请求包头的 QR 标志
   -opcode OPCode    设定 DNS 请求包头的 OPCode
                     OPCode 必须介乎于 0x0000 - 0x00FF/255
   -aa               设定 DNS 请求包头的 AA 标志
   -tc               设定 DNS 请求包头的 TC 标志
   -rd               设定 DNS 请求包头的 RD 标志
   -ra               设定 DNS 请求包头的 RA 标志
   -ad               设定 DNS 请求包头的 AD 标志
   -cd               设定 DNS 请求包头的 CD 标志
   -rcode RCode      设定 DNS 请求包头的 RCode
                     RCode 必须介乎于 0x0000 - 0x00FF/255
   -qn Count         设定 DNS 请求包头的 Question count
                     Question count 必须介乎于 0x0001 - 0xFFFF/65535
   -ann Count        设定 DNS 请求包头的 Answer count
                     Answer count 必须介乎于 0x0001 - 0xFFFF/65535
   -aun Count        设定 DNS 请求包头的 Authority count
                     Authority count 必须介乎于 0x0001 - 0xFFFF/65535
   -adn Count        设定 DNS 请求包头的 Additional count
                     Additional count 必须介乎于 0x0001 - 0xFFFF/65535
   -ti Time          设定每次请求之间的时间间隔（单位：毫秒）
   -edns0            发送时添加 EDNS0 标签
   -payload Length   设定 EDNS0 标签的 UDP Payload length
                     Payload length 必须介乎于 512 - 0xFFFF/65535
   -dnssec           发送时添加可以接受 DNSSEC 的请求
                     启用添加可以接受 DNSSEC 时发送时添加 EDNS0 标签也会被启用
   -qt Type          设定请求类型 Query Type
                     Type: A|NS|CNAME|SOA|PTR|MX|TXT|RP|SIG|KEY|AAAA|LOC|SRV|
                           NAPTR|KX|CERT|DNAME|EDNS0|APL|DS|SSHFP|IPSECKEY|
                           RRSIG|NSEC|DNSKEY|DHCID|NSEC3|NSEC3PARAM|HIP|SPF|
                           TKEY|TSIG|IXFR|AXFR|ANY|TA|DLV
   -qc Classes       设定请求类型 Query Classes
                     Classes: IN|CSNET|CHAOS|HESIOD|NONE|ALL|ANY
   -p Port           设定UDP端口
                     UDP端口必须介乎于 0x0001 - 0xFFFF/65535
   -raw RAW_Data     设定发送原始数据
                     原始数据为不需要添加 0x 开头的16进制数
                     原始数据长度必须介乎于 64 - 1512 字节
   -buf Size         设定接收缓冲区长度
                     接收缓冲区长度必须介乎于 512 - 4096 字节
   -6                强制使用IPv6
   -4                强制使用IPv4
   Test_DomainName   设定发送 DNS 请求用的查询的域名
   Target            Ping 的目标，支持 IPv4/IPv6 地址和域名
