# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/event_proc.json

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
GET|items|是|array: ['input1', 'motion', 'cover']|需要查询的事件的名称
PUT|items|是|object: |要设置的事件联动参数名值对

### 注意事项
无

### 调用样例
GET http://x.x.x.x/api/1.0/event_proc.json?items[]=input1  
PUT http://x.x.x.x/api/1.0/event_proc.json json_data

### 返回结果
JSON示例  
GET

	{
		"items" : {
			"input1" : {
      			"record" : true,
      			"sound" : true,
      			"output1" : true
    		}
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
record|boolean|是否触发录像
sound|boolean|是否触发报警声
output1|boolean|是否触发输出

