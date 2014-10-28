# API Reference (1.0)
---

### url
http://x.x.x.x/api/1.0/users.json

### 支持格式
JSON

### HTTP请求方式
GET  
PUT  
POST  
DELETE  

### 是否需要登录
是

### 请求参数
 请求|参数|必选|类型及范围|说明
---|---|:-:|---|---|---
GET|items|是|array: ['password', 'role]|是否需要返回用户密码和特权级
PUT|items|是|object array: [ {'username':'username', 'password':'password', 'role:'user'}]|修改用户的密码和角色信息，需要设置用户的用户名，密码，角色的数组，允许一次调用设置多个用户
POST|items|是|object array: [ {'username':'username', 'password':'password', 'role:'user'}]|新增用户
DELETE|items|是|object array: [ { 'username':'user1' }, { 'username':'user2' } ]|需要被删除的用户名，可以一次删除多个

### 注意事项
无

### 调用样例
GET http://x.x.x.x/api/1.0/users.json&items[]=password&items[]=role  
PUT http://x.x.x.x/api/1.0/users.json json_data  
POST http://x.x.x.x/api/1.0/users.json json_data  
DELETE http://x.x.x.x/api/1.0/users.json json_data  

### 返回结果
JSON示例
GET

	{
		'items': [
			{
				'username': 'admin',
				'password': 'passwd',
				'role': 'administrator'
			},
			{
				'username': 'guest',
				'password': 'passwd',
				'role': 'user'
			}
		]
	}
	
PUT

	{'success': 'true' | 'false'}
	
### 返回字段说明
返回值字段|字段类型|字段说明
---|---|---
username|string|用户名
password|string|密码
role|string|用户角色

