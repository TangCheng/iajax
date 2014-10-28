# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/osd.json

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
GET|items|是|object: { master: ['datetime', 'device_name', 'comment', 'frame_rate', 'bit_rate'], slave: []}|需要查询的OSD显示项的名称
PUT|items|是|object: |要设置的OSD显示项的名称，是否显示，大小，X/Y坐标，和颜色值

### 注意事项
无

### 调用样例
GET http://127.0.0.1/api/1.0/osd.json?items[master][]=comment&items[master][]=bit_rate&items[slave][]=device_name&items[slave][]=frame_rate  
PUT http://x.x.x.x/api/1.0/osd.json json_data

### 返回结果
JSON示例  
GET

	{
		"items" : {
			"master" : {
				"comment" : {
					"isshow" : true,
          			"size" : 20,
          			"left" : 800,
          			"top" : 10,
          			"color" : {
            			"red" : 0,
            			"green" : 0,
            			"blue" : 0,
            			"alpha" : 0
          			}
        		},
				"bit_rate" : {
          			"isshow" : true,
          			"size" : 20,
          			"left" : 10,
          			"top" : 970,
          			"color" : {
            			"red" : 0,
            			"green" : 0,
            			"blue" : 0,
            			"alpha" : 0
          			}
        		}
      		},
    		"slave" : {
				"device_name" : {
          			"isshow" : true,
          			"size" : 20,
          			"left" : 10,
          			"top" : 10,
          			"color" : {
            			"red" : 0,
            			"green" : 0,
            			"blue" : 0,
            			"alpha" : 0
          			}
        		},
				"frame_rate" : {
          			"isshow" : true,
          			"size" : 20,
          			"left" : 10,
          			"top" : 945,
          			"color" : {
            			"red" : 0,
            			"green" : 0,
            			"blue" : 0,
            			"alpha" : 0
          			}
        		}
      		}
  		}
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
isshow|boolean|是否在屏幕显示
size|int|字体大小
left|int|左边距 千分比
top|int|上边距 千分比
red|int|红色分量 (0-255)
green|int|绿色分量 (0-255)
blue|int|蓝色分量 (0-255)
alpha|int|透明度 (0-255)

