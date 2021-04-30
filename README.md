# QuickBackupX
The OOP Version of QuickBackup-BDS    
©2020-2021 Jasonzyt All rights reserved.    
[QQGroup](https://jq.qq.com/?wv=1027&k=XQ95YehZ)  [Sponsor](http://pay.sa2y.net/paypage/?merchant=97a6ueUjyemLZeyQZK3TaCKluhQu5FTZM2LvKrX%2Btlpm)    
English is not supported for the time being. If there is a demand, you can submit an issue or a pull request   

## [Wiki](https://github.com/Jasonzyt/QuickBackupX/wiki)
- 在提问之前,请先查看Wiki有没有对您的问题作出解释或回答

## 说明
本插件基于GPLv3.0协议开源    
部分资料因某些原因不能公开 见谅

## 为什么使用QuickBackupX?
- 轻松创建备份(make)
- 简单管理备份(list del view)
- 全自动备份(auto)
- 快速从备份回档(back) //未实现的功能
- 轻松控制自动备份
- 完整的配置文件,几乎可以操作任何一个有用的变量w2
- 无需第三方软件进行压缩,可自控压缩等级
- 完整的权限系统
- 支持命令方块执行备份
- 对接游戏save hold/query,减少崩服概率 // 未实现的功能
- 多线程压缩文件,更高效
- 开源项目,不存在任何病毒
- 完整JSON记录&配置,配置更新无需手动增补
- 自动备份时间点(精确到时分) 秒钟为自动备份线程开始的一刻
- 面向对象编程,开发者使用API上手容易

## 注意!
- 本插件所需要的目录都不要包含中文/特殊字符,建议全部使用ASCII字符

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
- `qb del all` 删除所有备份    
  [√] 控制台 [X] 玩家 [X] 命令方块
- `qb back [备份序号: int] [*是否删除源备份: bool] [*是否删除存档: bool]` ???
  [√] 控制台 [√] 玩家 [X] 命令方块
- `qb auto` 查询当前自动备份状态    
  [√] 控制台 [√] 玩家 [X] 命令方块
- `qb auto [开关on/off: on_off]` 关闭或打开自动备份    
  [√] 控制台 [√] 玩家 [X] 命令方块
- `qb help` ???    
  [√] 控制台 [√] 玩家 [X] 命令方块。  
[!] 标有* 的参数表示可省略

## 权限说明
[!] 高级权限玩家可以执行所有低级权限
### Console Level[3]
- 所有功能
### Admin Level[2]
- 可以执行Backup Back Auto Del
- 接收QB所有玩家相关的信息(方便管理)
- 可以删除任何备份(包括控制台 命令方块执行的)
### Backup Level[1]
- 仅可以执行Backup
### Back Level[1]
- 仅可以执行Back
### CmdBlk Level[0]
- 如果配置运行,则可以执行Make
### Player Level[0]
- 可以删除自己执行的备份

## 文件说明
### 配置文件
`[BDS路径]/QuickBackupX/config.json`    
- Allow_CB: 布尔值 是否允许命令方块执行QB命令(出于安全和实用考虑,目前仅支持Make)
- BackupOutoutPath: 字符串 备份输出目录及[文件名格式](#文件名格式)
- ListOutputSize: 整型 List备份列表每页输出备份的数量(若填写值小于等于0,则不分页)
- ListOutputCont: 字符串 ???
- ViewOutputCont: 字符串 ???
- Debug: 布尔值 是否输出调试信息
- EULA: 布尔值 是否同意[EULA](EULA.txt)(不同意将无法启动服务端)
- Auto_On_AB: 布尔值 在服务端启动时是否自动打开自动备份
- Admin_Player: 数组 拥有Admin权限的玩家
- Backup_Player: 数组 可以执行备份的玩家
- Back_Player: 数组 可以执行回档的玩家
- AutoBackup_Time: 数组 执行自动备份的时间: 格式:`Hour:Minute 例 04:00`
- Compression: 整型 压缩等级,0表示不压缩,仅打包;100表示最大限度压缩(支持0-100,小于等于0则为0,大于等于100则为100)
- AB_Max: 整型 自动备份的最大数量,小于等于0表示不限制(默认删除最远久的备份)
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

## [更新日志](Update.md)

## [待办事项](TODO.md)

## 附录
### 文件名格式
```
正确的例子:
"./BACKUPS/%Y-%m-%d %H.zip""
"C:/QB/%H.zip"
".\\QBX\\%H.zip"
"../MC_Backups/%Y/%m/%d/%H/%M.zip" (格式化目录名 理论上没有问题 但是写的时候没有考虑这些情况)
错误的例子:
"./MCBAK/备份.zip"
"C:/这就是QB吗.zip"

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
// 注入插件后会自动生成
```

### 输出格式说明
```
// 此内容目前还未实现!!!
%onum%  备份序号
%date%  备份日期
%time%  备份时间
%sizeb% 备份存档大小(单位:Byte)
%size%  备份存档大小
%path%  备份存档路径
%exer%  备份执行者
%exert% 备份执行者类型
%rmd5%  备份时的文件MD5值
%nmd5%  当前的文件MD5值
%md5ok% %rmd5和nmd5%是否匹配
```
