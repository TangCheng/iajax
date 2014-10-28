# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/video.json

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
GET|items|是|array: ['profile', 'flip', 'mirror', 'main_profile', 'sub_profile']|需要查询的Video设置的profile
PUT|items|是|object-array: {'profile':0, 'flip':0, 'quanlity':0, 'frame_rate':25, 'bit_rate':0, 'bit_rate_value':300}|要设置的Video设置项的名称，值

### 注意事项
无

### 调用样例
GET http://x.x.x.x/api/1.0/video.json?items[]=flip&items[]=mirror&items[]=profile&items[]=main_profile&items[]=sub_profile  
PUT http://x.x.x.x/api/1.0/video.json json_data

### 返回结果
JSON示例 
GET

	{
		'body': {
			'items': {
				'profile': 'baseline',
				'flip': false,
				'mirror': false,
				'main_profile': {
					'frame_rate': 25,
					'bit_rate': 'CBR',
					'bit_rate_value': 4096,
					'resolution': 'UXGA',
					'stream_path': 'main_stream'
				},
				'sub_profile': {
					'frame_rate': 25,
					'bit_rate': 'VBR',
					'bit_rate_value': 1024,
					'resolution': 'D1',
					'stream_path': 'sub_stream'
				}
			}
		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
profile|string|视频编码级别['baseline','main','high']
flip|boolean|视频垂直翻转
mirror|boolean|视频左右镜像
frame_rate|int|视频帧率(1～30fps)
bit_rate|string|视频码率['CBR','VBR']
bit_rate_value|int|视频码率值
resolution|string|视频分辨率
stream_path|string|视频流路径

