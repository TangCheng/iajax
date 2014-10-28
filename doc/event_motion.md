# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/event_motion.json

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
GET|items|是|array: ['region1', 'region2']|需要查询的移动侦测区域的名称
PUT|items|是|object: |要设置的移动侦测区域参数的名值对

### 注意事项
无

### 调用样例
GET http://x.x.x.x/api/1.0/event_motion.json?items[]=region1  
PUT http://x.x.x.x/api/1.0/event_motion.json json_data

### 返回结果
JSON示例  
GET

	{
		"items" : {
			"region1" : {
				"enable" : false,
				"sensitivity": 50,
				"rect" : {
					"left": 0,
					"top": 0,
					"width": 0,
					"height": 0
				}
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
sensitivity|int|灵敏度 1% - 100%
rect.left|int|区域左边距，千分比
rect.top|int|区域上边距，千分比
rect.width|int|区域宽度，千分比
rect.height|int|区域高度，千分比
schedules.mon|string|周一布防时间
schedules.tue|string|周二布防时间
schedules.wed|string|周三布防时间
schedules.thu|string|周四布防时间
schedules.fri|string|周五布防时间
schedules.sat|string|周六布防时间
schedules.sun|string|周日布防时间

