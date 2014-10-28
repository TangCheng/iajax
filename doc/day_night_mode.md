# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/day_night_mode.json

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
GET|items|是|array: ['force_night_mode', 'night_mode_threshold', 'ir_intensity']|需要查询的信息的名称
PUT|items|是|object:|要设置的信息的名值对

### 注意事项
无

### 调用样例
GET http://x.x.x.x/api/1.0/day_night_mode.json?items[]=fore_night_mode&items[]=night_mode_threshold&items[]=ir_intensity  
PUT http://x.x.x.x/api/1.0/day_night_mode.json json_data

### 返回结果
JSON示例  
GET

	{
		'items': {
			“force_night_mode”: false,
			"night_mode_threshold": 50,
			"ir_intensity": 80
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
force_night_mode|boolean|强制夜间模式是否开启
night_mode_threshold|int|日夜转换阈值，百分比
ir_intensity|int|夜间模式，IR发光强度，百分比


