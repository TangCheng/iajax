# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/base_info.json

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
GET|items|是|array: ['device_name', 'location', 'comment', 'manufacturer', 'model', 'serial', 'firmware', 'hardware', 'hwaddr']|需要查询的基本信息的名称
PUT|items|是|object: {'device_name':'ipcam','location':'China','comment':'my content'}|要设置的基本信息的名值对

### 注意事项
仅device_name，location，comment三项可写，其余项为只读。

### 调用样例
GET http://x.x.x.x/api/1.0/base_info.json?items[]=device_name&items[]=comment  
PUT http://x.x.x.x/api/1.0/base_info.json json_data

### 返回结果
JSON示例  
GET

	{
		'items': {
			“device_name”: "ipcam",
			"location": "China",
			"comment": "my content",
			"manufacturer": "IPNC",
			"model": "IPNC-100",
			"serial": "12345678",
			"firmware": "V1.0.0",
			"hardware": "Rev1",
			"hwaddr": "xx:xx:xx:xx:xx:xx"
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
device_name|string|设备名称
comment|string|用户自定义说明
location|string|设备位置
manufacturer|string|制造商
model|string|设备型号
serial|string|设备序列号
firmware|string|设备软件版本
hardware|string|设备硬件版本
hwaddr|string|设备MAC地址
