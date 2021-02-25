# QuickBackupX
### The OOP Version of QuickBackup-BDS
### ©2020-2021 Jasonzyt All rights reserved.
### [QQ Group](https://jq.qq.com/?wv=1027&k=XQ95YehZ)  [Sponsor](http://pay.sa2y.net/paypage/?merchant=97a6ueUjyemLZeyQZK3TaCKluhQu5FTZM2LvKrX%2Btlpm)

## 说明
本插件基于GPLv3.0协议开源    
部分资料因某些原因不能公开 见谅

## 鸣谢
- [Mojang](https://github.com/Mojang)和[Microsoft](https://microsoft.com)开发的[Minecraft](https://minecraft.net/)游戏
- [TISUnion](https://www.github.com/TISUnion/QuickBackupM)给予的灵感
- [ZipUtils](https://www.codeproject.com/Articles/7530/Zip-Utils-Clean-Elegant-Simple-Cplusplus-Win)可以方便的压缩文件(尽管有一些Bug)
- [JsonCPP](https://github.com/open-source-parsers/jsoncpp)库让我操作JSON时方便了很多
- [OpenSSL](https://github.com/openssl/openssl)中的MD5相关操作十分好用
- [BDX SDK]()提供了近乎完美的API
- [Sysca11](https://github.com/Sysca11)在开发本插件的过程中,他帮了我许多忙

## 插件使用的第三方库
- [ZipUtils](https://www.codeproject.com/Articles/7530/Zip-Utils-Clean-Elegant-Simple-Cplusplus-Win)
- [JsonCPP](https://github.com/open-source-parsers/jsoncpp) MIT
- [OpenSSL](https://github.com/openssl/openssl) Apache-2.0
- [BDX SDK]()

## 安装方法
1. 从[Release](https://github.com/Jasonzyt/QuickBackupX/releases)下载插件文件(.dll)
2. 将插件放入注入器的插件文件夹 BDX`[BDS路径]/bdxmod/` BDXCore`[BDS路径]/bdxcore_mod/` LiteLoader`[BDS路径]/plugins/`
3. 启动BDS服务端(bedrock_server.exe)
4. 完成Step 3后,`[BDS路径]/QuickBackupX/`目录下会多出来一些文件,您可以根据需求更改[配置](#配置文件)
5. 若要了解更多 请看下文

## 游戏命令
`qb <cmd: QBCMDT>`
- `qb make` 创建一个备份    
  [√] 控制台 [√] 玩家 [√] 命令方块
- `qb list [*页码: int]` 列出第n页的备份    
  [√] 控制台 [√] 玩家 [X] 命令方块
- `qb view [备份序号: int]` ???    
  [√] 控制台 [√] 玩家 [X] 命令方块    
- `qb del [备份序号: int]` 删除备份[n]    
  [√] 控制台 [√] 玩家 [X] 命令方块
- `qb del all` ???    
  [√] 控制台 [X] 玩家 [X] 命令方块
- `qb back [备份序号: int] [*是否删除源备份: bool] [*是否删除存档: bool]` ???
  [√] 控制台 [√] 玩家 [X] 命令方块
- `qb help` ???    
  [√] 控制台 [√] 玩家 [X] 命令方块
¤ 标有* 的参数表示可省略

## 文件说明
### 配置文件
`[BDS路径]/QuickBackupX/config.json`    
- Allow_CB: 布尔值 是否允许命令方块执行QB命令(出于安全和实用考虑,目前仅支持Make)
- BackupOutoutPath: 字符串 备份输出目录及[文件名格式](#文件名格式)
- Debug: 布尔值 是否输出调试信息
- EULA: 布尔值 是否同意[EULA](EULA.txt)(不同意将无法启动服务端)
- Admin_Player: 数组 拥有Admin权限的玩家
- Backup_Player: 数组 可以执行备份的玩家
- Back_Player: 数组 可以执行回档的玩家
### 备份记录文件
`[BDS路径]/QuickBackupX/backups.json`   
- 此文件中的内容若受到破坏,插件将无法获取到备份的信息
### 用户协议
`[BDS路径]/QuickBackupX/EULA.txt`   
- 此文件中的内容理论上可能会比[仓库中的EULA.txt](EULA.txt)要新一些
### 运行记录文件
`[BDS路径]/QuickBackupX/QBX.log`   
- 此文件中包含了运行中作者能想到的几乎所有信息输出,此文件可以随时删除
- 在[发送Issue](https://github.com/Jasonzyt/QuickBackupX/issues/new)时,我们建议您附加此文件输出的内容以便我们更好的解决问题
### 附带可执行文件
`[BDS路径]/QuickBackupX/QBX.exe`   
- ???

## [更新日志](Update.txt)

## [待办事项](TODO.md)

## 附录
### 文件名格式
```
%a 星期几的缩写。
%A 星期几的全名。
%b 月份名称的缩写。
%B 月份名称的全名。
%c 本地端日期时间较佳表示字符串。
%d 用数字表示本月的第几天 (范围为 00 至 31)。日期
%H 用 24 小时制数字表示小时数 (范围为 00 至 23)。
%I 用 12 小时制数字表示小时数 (范围为 01 至 12)。
%j 以数字表示当年度的第几天 (范围为 001 至 366)。
%m 月份的数字 (范围由 1 至 12)。
%M 分钟。
%p 以 ''AM'' 或 ''PM'' 表示本地端时间。
%S 秒数。
%U 数字表示为本年度的第几周，第一个星期由第一个周日开始。
%W 数字表示为本年度的第几周，第一个星期由第一个周一开始。
%w 用数字表示本周的第几天 ( 0 为周日)。
%x 不含时间的日期表示法。
%X 不含日期的时间表示法。
%y 二位数字表示年份 (范围由 00 至 99)。
%Y 完整的年份数字表示，即四位数。
%Z 时区或名称缩写。
%z 同上。
%% % 字符。
```
### 配置文件示例
```json
// 此处配置无法保证紧跟最新版本
{
   "Admin_Player" : [],
   "Allow_CB" : false,
   "Back_Player" : [],
   "BackupOutputPath" : "./backup/%Y-%m-%d-%H-%M.zip",
   "Backup_Player" : [
      {
         "Name" : "Steve",
         "Xuid" : "1145141919810"
      },
      {
         "Name" : "Alex"
      }
   ],
   "Debug" : true,
   "EULA" : true
}
```