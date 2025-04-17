# 网络连接问题分析与解决方案

## 问题描述

客户端在尝试获取考场信息时，出现以下错误：
- "Error transferring http://localhost:8080/teacher/examrooms - server replied: Not Found"
- "Host requires authentication"

## 问题分析

通过检查服务器和客户端代码，我们发现了以下问题：

1. **登录流程不完整**：
   - 客户端登录功能没有真正实现，只是做了前端验证
   - 没有向服务器发送登录请求获取认证令牌

2. **认证令牌缺失**：
   - 对需要认证的API请求没有附加认证令牌
   - 服务器的认证中间件要求所有API请求(除登录外)必须包含有效的JWT令牌

3. **网络请求实现问题**：
   - `NetworkManager::sendRequest`方法没有正确处理GET和POST请求的区别
   - 缺少必要的错误处理和调试输出

## 解决方案

1. **重新实现登录功能**：
   - 完善`LoginDialog::onLoginButtonClicked`方法，在用户点击登录按钮时调用NetworkManager的login方法
   - 在登录成功后保存服务器返回的认证令牌

2. **修改NetworkManager**：
   - 重写login方法，实现真正的服务器登录请求，并保存返回的JWT令牌
   - 更新其他网络请求方法(getExamRooms, getStudentsForExam等)，确保附加认证令牌
   - 增加更详细的错误处理和调试输出

3. **请求流程优化**：
   - 使用单例模式获取NetworkManager实例
   - 每个网络请求前检查认证令牌是否存在，避免未认证请求
   - 统一处理服务器响应，解析status字段判断请求是否成功

## 测试步骤

1. 启动服务器：`node server.js`
2. 使用测试脚本验证API可用性：`node test-api.js`
3. 修改客户端代码后重新编译
4. 登录测试：
   - 服务器地址：http://localhost:8080
   - 用户名：operator
   - 密码：operator123
5. 测试刷新考场功能，确认能正确获取考场信息

## 注意事项

- 确保服务器在客户端尝试连接前已启动
- 登录成功后，所有需要认证的API请求都必须附加正确格式的Bearer令牌
- 如果仍有问题，查看控制台的调试输出，特别是网络请求和响应的详细信息

## 解决方案文件

- [fix-client.md](./fix-client.md) - 修复指南
- [client-patch-fix.cpp](./client-patch-fix.cpp) - 包含所有需要修改的代码
- [test-api.js](./test-api.js) - 用于测试服务器API的脚本 