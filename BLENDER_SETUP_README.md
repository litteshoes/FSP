# Blender 4.5.3 配置指南

## 概述
本项目已配置为使用从其他服务器移植的 Blender 4.5.3 版本，无需在Docker构建时下载。

## 完成的工作

### 1. Blender 解压 ✓
- 已将 `blender-4.5.3-linux-x64.tar.xz` 解压到 `/home/jry/FSP/tools/blender/`
- Blender 可执行文件路径：`/home/jry/FSP/tools/blender/blender`
- 文件权限已正确设置（755）

### 2. Docker 配置 ✓
- 修改了 `backend/Dockerfile`，支持使用外部挂载的 Blender
- 更新了 `.env` 环境变量文件
- Docker Compose 配置已包含正确的 volume 挂载

### 3. 环境变量配置 ✓
在 `.env` 文件中配置了：
```bash
# Blender配置 - 使用本地已安装的blender
BLENDER_TIMEOUT=300  # 秒
# Blender已通过volume挂载到容器中，容器内路径为/home/jry/FSP/tools/blender/blender

# Docker构建参数
INSTALL_BLENDER=0  # 设为0，因为我们使用本地已有的blender
```

## Docker 容器配置

### Volume 挂载
Docker Compose 已配置以下 volume 挂载：
```yaml
volumes:
  - /home/jry/FSP/tools:/home/jry/FSP/tools:rw
```

### 环境变量
- `BLENDER_PATH=/home/jry/FSP/tools/blender/blender` （容器内路径）
- `PATH` 已包含 Blender 路径：`/home/jry/FSP/tools/blender`

## 使用方法

### 启动服务
```bash
cd /home/jry/FSP
docker-compose up -d
```

### 验证配置
运行测试脚本：
```bash
cd /home/jry/FSP
./test-blender.sh
```

### 手动测试 Blender
```bash
# 在容器内测试
docker-compose exec web /home/jry/FSP/tools/blender/blender --version

# 或者在宿主机测试（需要安装依赖）
cd /home/jry/FSP/tools/blender
./blender --version
```

## 注意事项

1. **依赖库**：如果在宿主机运行 Blender，需要确保安装了相应的 X11 和图形库
2. **容器内使用**：在 Docker 容器中，Blender 通过 volume 挂载使用，无需额外安装
3. **版本匹配**：当前使用的是 Blender 4.5.3，与项目兼容

## 文件结构
```
/home/jry/FSP/
├── tools/
│   └── blender/           # Blender 4.5.3 安装目录
│       ├── blender        # 主可执行文件
│       ├── blender-launcher
│       └── lib/           # 相关库文件
├── .env                   # 环境变量配置
├── compose.yaml          # Docker Compose 配置
├── backend/Dockerfile    # 构建配置（已修改支持外部 Blender）
└── test-blender.sh       # 测试脚本
```

## 故障排除

如果遇到 Blender 相关问题：

1. 检查 volume 挂载是否正确
2. 确认环境变量设置
3. 运行测试脚本检查 Blender 功能
4. 查看 Docker 容器日志

---
配置完成时间：2025年9月27日
