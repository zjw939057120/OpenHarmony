# GD32F470 快速上手 OpenHarmony


---

## 目录

- [开源鸿蒙开发指南](#开源鸿蒙开发指南)
  - [开发环境配置](#开发环境配置)
  - [源码获取](#源码获取)
  - [编译构建](#编译构建)

---

## 开源鸿蒙开发指南

### 开发环境配置

#### 安装依赖库

> **注意**：以下命令适用于 Ubuntu 20.04/22.04 系统，其他发行版请根据实际情况调整。

在 Ubuntu 系统上执行以下命令安装编译所需依赖：

```bash
sudo apt-get update && sudo apt-get install \
  libstdc++-12-dev binutils binutils-dev git git-lfs gnupg flex bison gperf \
  build-essential zip curl zlib1g-dev gcc-multilib libc6-dev-i386 \
  x11proto-core-dev libx11-dev ccache libgl1-mesa-dev libxml2-utils \
  xsltproc unzip m4 bc gnutls-bin python3-pip ruby genext2fs \
  device-tree-compiler make libffi-dev e2fsprogs pkg-config perl \
  openssl libssl-dev libelf-dev libdwarf-dev u-boot-tools mtd-utils \
  cpio doxygen liblz4-tool openjdk-8-jre gcc g++ texinfo dosfstools \
  mtools default-jre default-jdk libncurses5 apt-utils wget scons tar \
  rsync git-core libxml2-dev grsync xxd libglib2.0-dev libpixman-1-dev \
  kmod jfsutils reiserfsprogs xfsprogs squashfs-tools pcmciautils \
  quota ppp libtinfo-dev libtinfo5 libstdc++6 gcc-arm-none-eabi vim \
  ssh locales libxinerama-dev libxcursor-dev libxrandr-dev libxi-dev
```

#### 配置 Shell 环境

确保系统默认 Shell 为 `bash`：

```bash
# 检查当前 Shell
ls -l /bin/sh
# 若输出不是 bash，执行以下命令进行修改
sudo dpkg-reconfigure dash
# 在弹出的界面中选择 "No"，将默认 Shell 改为 bash
```

#### 安装 repo 工具（首次使用）

```bash
mkdir -p ~/ohos
curl https://raw.gitcode.com/gitcode-dev/repo/raw/main/repo-py3 > ~/ohos/repo
chmod a+x ~/ohos/repo
pip3 install -i https://repo.huaweicloud.com/repository/pypi/simple requests
```

将 repo 添加到环境变量：

```bash
vim ~/.bashrc               # 编辑环境变量配置
export PATH=~/ohos:$PATH    # 在文件末尾添加 repo 路径
source ~/.bashrc            # 使配置生效
```

---

### 源码获取

> **提示**：源码体积较大（约 50GB+），请确保磁盘空间充足，并保持网络连接稳定。

#### 获取 OpenHarmony 源码

```bash
# OpenHarmony-6.1-LTS
~/ohos/repo init -u https://gitcode.com/openharmony/manifest -b OpenHarmony-6.1-LTS -g ohos:mini --no-repo-verify

# 同步代码
.repo/repo/repo sync -c --force-sync -j8
.repo/repo/repo forall -c 'git lfs pull' -v -j8

# 安装编译器及二进制工具
./build/prebuilts_download.sh

# 安装hb工具
python -m pip install build/hb

```
### 编译构建

```bash

# 选择产品配置
hb set -p gd32f470

# 编译
make -j8

```

