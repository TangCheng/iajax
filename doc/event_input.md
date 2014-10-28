# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/event_input.json

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
GET|items|是|array: ['input1', 'input2']|需要查询的输入的名称
PUT|items|是|object: |要设置的输入参数的名值对

### 注意事项
根据设备硬件提供的输入口数决定参数个数。

### 调用样例
GET http://x.x.x.x/api/1.0/event_input.json?items[]=input1  
PUT http://x.x.x.x/api/1.0/event_input.json json_data

### 返回结果
JSON示例  
GET

	{
		"items" : {
			"input1" : {
				"enable" : false,
				"schedules" : {
					"mon" : "",
					"tue" : "1,2,3",
					"wed" : "",
					"thu" : "",
					"fri" : "22,23",
					"sat" : "",
					"sun" : ""
				}
			}
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
enable|boolean|是否启用
schedules.mon|string|周一布防时间
schedules.tue|string|周二布防时间
schedules.wed|string|周三布防时间
schedules.thu|string|周四布防时间
schedules.fri|string|周五布防时间
schedules.sat|string|周六布防时间
schedules.sun|string|周日布防时间

