# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/image.json

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
GET|items|是|array: ['watermark','3ddnr','brightness','chrominance','contrast','saturation','scenario']|需要查询的场景设置项的名称
PUT|items|是|object:|要设置的场景设置项的名称，值

### 注意事项
无

### 调用样例
GET http://x.x.x.x/api/1.0/image.json?items[]=brightness  
PUT http://x.x.x.x/api/1.0/image.json json_data

### 返回结果
JSON示例  
GET

	{
		'items': {
			'brightness': 128,
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
wartermark|boolean|数字水印开关
3ddnr|boolean|3D降噪开关
brightness|int|亮度
chrominance|int|色度
contrast|int|对比度
saturation|int|饱和度
scenario|string|场景设置（50Hz/60Hz/混合模式）

