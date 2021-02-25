# QuickBackupX 更新日志
### Language: zh-cn 简体中文
### Author: JasonZYT
### ©2020-2021 Jasonzyt All rights reserved.
### [QQ Group](https://jq.qq.com/?wv=1027&k=XQ95YehZ)  [Sponsor](http://pay.sa2y.net/paypage/?merchant=97a6ueUjyemLZeyQZK3TaCKluhQu5FTZM2LvKrX%2Btlpm)  GitHub(https://github.com/Jasonzyt/QuickBackupX)

!* The support of QuickBackup-BDS has stopped now.    
!* You can also use QuickBackupX instead of it.

## v4.0.1 Debug - 4.0.4 Alpha
支持的注入器: BDX,BDXCore,LiteLoader    
依赖: Visual C++ Runtime(x86_64)    
- 未公开版本

## v4.1.0 Beta
支持的注入器: BDX,BDXCore,LiteLoader    
依赖: Visual C++ Runtime(x86_64)    

- 创建备份与列出所有备份
- Make命令支持命令方块调用
- 优化Backup::CheckPermission系列函数()
- 加入新权限等级 Admin_Player
- 删除备份(玩家只能删除自己创建的备份,Admin可以删除任何人)
- 备份执行者记录改版(支持CB且内容更加详细)
- 优化Log内容(将错误和警告的源码详细位置记录清楚)(需开启Debug模式)
- 优化BRecJson类(减少BRecJson::ListRecord函数调用次数)
- 若备份失败则不获取相关信息
- 优化删除备份
- 若备份所在磁盘剩余空间不足,则取消执行备份(空间<2GB)
- 备份文件重名自动修改
- 若备份被手动删除,在启动时就删除该备份的记录
- 完成插件的EULA
- 插件文档
- 若配置缺失或不符合类型要求,则自动补齐(修改)相关配置

## v4.1.1 Beta [Coming soon]
