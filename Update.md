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
### 新功能
- 命令解析器
- 创建备份与列出所有备份
- Make命令支持命令方块调用
- 加入新权限等级 Admin_Player
- 删除备份(玩家只能删除自己创建的备份,Admin可以删除任何人)
- 备份执行者记录改版(支持CB且内容更加详细)
- 备份文件重名自动修改
- 完成插件的EULA
### 优化
- 优化Backup::CheckPermission系列函数()
- 优化Log内容(将错误和警告的源码详细位置记录清楚)(需开启Debug模式)
- 优化BRecJson类(减少BRecJson::ListRecord函数调用次数)
- 优化删除备份
- 若备份被手动删除,在启动时就删除该备份的记录
- 若备份失败则不获取相关信息
- 若备份所在磁盘剩余空间不足,则取消执行备份(空间<2GB)
- 若配置缺失或不符合类型要求,则自动补齐(修改)相关配置

## v4.1.1 Beta
支持的注入器: BDX,BDXCore,LiteLoader    
依赖: Visual C++ Runtime(x86_64)
### 新功能
- qb命令记录
### 优化
- 优化命令解析器
- 在服务器没玩家的时候不执行tellraw命令
### Bug修复
- 修复控制台命令输出一大堆的bug

## v4.1.2 Beta 
支持的注入器: BDX,BDXCore,LiteLoader    
依赖: Visual C++ Runtime(x86_64)
### 新功能
- 现在可以自定义List输出的大小了
### 优化
- 在备份执行前执行save hold
- getBackupDir现在支持不含/或\的路径了
- 现在无需在dll_init时单独创建config.json了
- 现在将用于向上取整的ceil函数改为+0.99999999,节省一丢丢性能
- 优化命令错误输出
- 若未找到配置中填写的备份文件夹,则自动创建(感谢@ExquisiteCore的反馈)
### 修复
- 修复了插件文件夹创建太晚的bug
- 修复命令错误输出乱码的bug

## 4.1.3 Beta
支持的注入器: BDX,BDXCore,LiteLoader    
依赖: Visual C++ Runtime(x86_64)
### 新功能
- 加入新执行者类型:AutoBackup
- 现在支持自动备份了
- 现在如果不填写"Backup_Player"或"Back_Player",则表示所有玩家均可执行备份和回档
### 修复
- 修复了save hold命令执行时总比备份执行晚的bug(并不是最优解决方案 欢迎PR)

## 4.1.4 Alpha
支持的注入器: BDX,BDXCore,LiteLoader    
依赖: Visual C++ Runtime(x86_64)
### 注意
- 此版本为衔接版本 代码更新幅度相对更大
- 此版本为ZipUtils向Bundle转换的衔接版本 BUG可能会相对较多
### 新功能
- 现在支持玩家执行Make,Del,List
- 现在支持控制台执行"qb del all"来删除所有的备份了 安全起见,此功能仅控制台可用
- 现在filesystem错误会被catch并输出而不是直接抛错误了
- 新增自动备份的开关
- 新增自动备份的限量
- 新增配置: Auto_On_AB 在服务端启动时是否自动打开自动备份
- 新增配置: Compression 压缩等级
- 新增配置: AB_Max 自动备份的最大数量
### 修复
- 修复了几处可能会导致内存泄漏的地方
- 修复了获取存档文件夹大小不会遍历其子目录的bug
- 优化了配置模块
- 优化了AutoBackup时间配置存储: vector<int> => time_t
### 优化
- 增加了编译器命令行选项: /utf-8,以改善乱码
### 已知Bugs
- `qb auto list`命令会造成服务端崩溃

## v4.1.5 Beta [Coming soon]
