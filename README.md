# 驭介智能科教外骨骼嵌入式开发资料

本项目面向外骨骼教学、科研与二次开发，汇集用户手册、示例代码、开发工具、硬件原理图及芯片技术手册。示例基于 STM32 平台，涵盖 LED 控制、串口通信、语音模块控制、IMU 与气压计数据采集、电池状态读取及电机控制等内容。

## 机器适用版本

主分支 `main` 存储适用于 **2026 年 10 月 1 日之后新机器**的嵌入式开发资料，日期分支 `new-machines-2026-10-01` 保留本次资料归档。日期表示机器适用版本，不是上传日期。使用例程前，请核对实际主板版本并选择对应目录。

## 目录导航

| 目录 | 内容 |
| --- | --- |
| [0-驭介智能](0-驭介智能/) | 驭介智能相关资料 |
| [1.用户手册](1.用户手册/) | 外骨骼用户手册 |
| [2.例程代码](2.例程代码/) | 新主板与旧主板 STM32 示例工程 |
| [3.嵌入式开发软件](3.嵌入式开发软件/) | 开发环境、安装程序及芯片支持包 |
| [4.原理图](4.原理图/) | 硬件原理图 |
| [5.芯片手册](5.芯片手册/) | 芯片参考手册与规格书 |
| [6.更新日志](6.更新日志/) | 压缩包原始版本记录 |
| [7.其他工具](7.其他工具/) | 辅助开发工具 |
| [8.嵌入式AI_AGENTS](8.嵌入式AI_AGENTS/) | 嵌入式 AI 开发相关资料与约定 |

## 下载完整资料

仓库使用 Git LFS 存储大型安装程序、芯片支持包和参考手册。请先安装 Git 与 [Git LFS](https://git-lfs.com/)，再执行：

```sh
git lfs install
git -c core.longpaths=true -c core.autocrlf=false clone --branch main --single-branch https://github.com/DRVEXO-Tech/exoskeleton-embedded-resources.git
cd exoskeleton-embedded-resources
git config core.longpaths true
git config core.autocrlf false
git lfs pull
git lfs fsck
```

Windows 建议克隆至较短路径，避免示例工程的多层目录触发路径长度限制。为获得完整的大文件内容，请按上述方式克隆并拉取 LFS 对象；仅下载 GitHub 页面上的 ZIP 不能保证包含实际 LFS 文件。
