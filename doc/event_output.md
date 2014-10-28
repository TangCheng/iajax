# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/event_output.json

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
GET|items|是|array: ['output1', 'output2']|需要查询的输出的名称
PUT|items|是|object: |要设置的输出参数的名值对

### 注意事项
根据设备硬件提供的输出口数决定参数个数。

### 调用样例
GET http://x.x.x.x/api/1.0/event_output.json?items[]=output1  
PUT http://x.x.x.x/api/1.0/event_output.json json_data

### 返回结果
JSON示例  
GET

	{
		"items" : {
			"output1" : {
				"normal" : "open",
				"period" : 0
			}
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
normal|string|常态[open, close]
period|int|触发态保持时间,单位为秒

