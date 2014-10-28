# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/datetime.json

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
GET|items|是|array: ['timezone', 'use_ntp', 'ntp_server', 'datetime']|需要查询的Datetime设置项的名称
PUT|items|是|object: {'timezone': 'Beijing', 'use_ntp': false, 'ntp_server': 'pool.ntp.org', 'datetime': '2014-06-17:13:00:00'}|要设置的Datetime设置项的名值对

### 注意事项
无

### 调用样例
GET http://x.x.x.x/api/1.0/datetime.json?items[]=timezone&items[]=use_ntp&items[]=ntp_server&items[]=datetime  
PUT http://x.x.x.x/api/1.0/datetime.json json_data

### 返回结果
JSON示例  
GET

	{
		'items': {
			'timezone': 'beijing',
			'use_ntp': false,
			'ntp_server': 'pool.ntp.org',
			'datetime': '2014-06-17 13:00:00'
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
timezone|string|时区
use_ntp|boolean|是否开启网络时间同步
ntp_server|string|NTP服务器域名或IP
datetime|string|设备当前日期时间
