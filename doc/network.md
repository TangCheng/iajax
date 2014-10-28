# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/network.json

### 支持格式
JSON

### HTTP请求方式
GET  
PUT

### 是否需要登录
是

### 请求参数
请求|参数|必选|类型及范围|说明
---|---|:-:|---|---|---
GET|items|是|array: ['method', 'address', 'pppoe', 'port']|需要查询的网络配置项的名称
PUT|items|是|object: {'method': 'static', 'address': {'ipaddr': '192.168.0.15', 'netmask': '255.255.255.0', 'gateway': '192.168.0.1', 'dns1':'192.168.0.1', 'dns2':''}, 'pppoe': {'username': 'admin', 'password': '123456'}, 'port': {'http': 80, 'ftp': 21, 'rtsp': 554}}|要设置的网络设置项的名称，值

### 注意事项
无

### 调用样例
GET http://x.x.x.x/api/1.0/network.json?items[]=method&items[]=address&items[]=pppoe&items[]=port  
PUT http://x.x.x.x/api/1.0/network.json json_data

### 返回结果
JSON示例  
GET

	{
		'items': {
			'method: 'static',
            'address': {
                'ipaddr': '192.168.0.15',
                'netmask': '255.255.255.0',
                'gateway': '192.168.0.1',
                'dns1': '192.168.0.1',
                'dns2': ''
            },
            'pppoe': {
                'username': 'admin',
                'password': '123456'
            },
            'port': {
                'http': 80,
				'ftp': 21,
                'rtsp': 554
            }
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
method|string|IP地址获取方法（static/dhcp/pppoe）
address.ipaddr|string|IP地址
address.netmask|string|网络掩码
address.gateway|string|网关IP地址
address.dns1|string|主DNS
address.dns2|string|备用DNS
pppoe.username|string|PPPoE账户名
pppoe.password|string|PPPoE账户密码
port.http|int|HTTP服务端口
port.ftp|int|FTP服务端口
port.rtsp|int|RTSP服务端口

