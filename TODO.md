# QuickBackupX 待办事项 
### Language: zh-cn 简体中文
### Author: JasonZYT
### ©2020-2021 Jasonzyt All rights reserved.
### [QQ Group](https://jq.qq.com/?wv=1027&k=XQ95YehZ)  [Sponsor](http://pay.sa2y.net/paypage/?merchant=97a6ueUjyemLZeyQZK3TaCKluhQu5FTZM2LvKrX%2Btlpm)  GitHub(https://github.com/Jasonzyt/QuickBackupX)

## TODO:
- [x] 创建备份与列出所有备份
- [x] 命令解析器
- [ ] 自定义View List命令输出
- [x] Make命令支持命令方块调用
- [x] 优化Backup::CheckPermission系列函数()
- [x] 加入新权限等级 Admin_Player
- [x] 删除备份(玩家只能删除自己创建的备份,Admin可以删除任何人)
- [x] 备份执行者记录改版(支持CB且内容更加详细)
- [x] 优化Log内容(将错误和警告的源码详细位置记录清楚)(需开启Debug模式)
- [x] 优化BRecJson类(减少BRecJson::ListRecord函数调用次数)
- [x] 若备份失败则不获取相关信息
- [x] 优化删除备份
- [ ] 回档功能(关服回档)
- [x] 若备份所在磁盘剩余空间不足,则取消执行备份(空间<2GB)
- [x] 备份文件重名自动修改
- [ ] 加入范围删除功能(控制台)
- [x] 若备份被手动删除,在启动时就删除该备份的记录
- [ ] 本插件用于外部启动BDS的可执行文件
- [x] 完成插件的EULA
- [x] 插件文档
- [x] 若配置缺失或不符合类型要求,则自动补齐(修改)相关配置
- [ ] 命令注册
- [x] 优化命令解析器
- [ ] 玩家命令解析
- [x] 在服务器没玩家的时候不执行tellraw命令
- [x] 修复控制台命令输出一大堆的bug
- [x] 在备份执行前执行save hold
- [x] 自定义List输出每页大小
- [x] 加入新执行者类型:AutoBackup
- [x] 自动备份
- [ ] 自动备份命令开关