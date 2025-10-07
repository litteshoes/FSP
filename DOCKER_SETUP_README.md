# FSP Docker + Blender 完整配置指南

## 🎯 配置完成总结

### ✅ 已完成的工作

#### 1. Blender 4.5.3 安装和配置
- **解压完成**: `blender-4.5.3-linux-x64.tar.xz` → `/home/jry/FSP/tools/blender/`
- **权限设置**: Blender 可执行文件权限正确 (755)
- **路径配置**: 容器内路径 `/home/jry/FSP/tools/blender/blender`

#### 2. Docker 环境配置
- **Docker 安装**: 版本 27.5.1 ✅
- **Docker Compose 安装**: 版本 2.33.0 ✅
- **镜像源配置**: 已配置国内镜像源 ✅
- **用户权限**: Docker 访问权限已配置 ✅

#### 3. 项目配置文件
- **Dockerfile**: 已修改支持外部 Blender (`INSTALL_BLENDER=0`)
- **docker-compose.yaml**: 正确配置 volume 挂载
- **.env**: 环境变量配置完整
- **目录结构**: 项目必要目录已创建

### 📁 当前文件结构
```
/home/jry/FSP/
├── tools/blender/          # Blender 4.5.3 安装目录 ✅
├── .env                     # 环境变量配置 ✅
├── compose.yaml            # Docker Compose 配置 ✅
├── backend/Dockerfile      # 构建配置 (已修改) ✅
├── data/                   # 数据目录 ✅
├── logs/                   # 日志目录 ✅
├── app/                    # 应用目录 ✅
└── BLENDER_SETUP_README.md # Blender配置文档 ✅
```

## 🚀 使用方法

### 启动 FSP 服务
```bash
cd /home/jry/FSP
docker compose up -d
```

### 检查服务状态
```bash
docker compose ps
```

### 查看日志
```bash
# Web 服务日志
docker compose logs web

# Worker 服务日志
docker compose logs worker

# Redis 日志
docker compose logs redis
```

### 停止服务
```bash
docker compose down
```

## ⚙️ 关键配置说明

### Blender 配置
- **本地 Blender**: 使用外部挂载，不在容器内安装
- **环境变量**: `INSTALL_BLENDER=0`
- **容器内路径**: `/home/jry/FSP/tools/blender/blender`

### Docker 镜像源
已配置国内镜像源，提升构建速度：
```json
{
  "registry-mirrors": [
    "https://registry.docker-cn.com",
    "https://hub-mirror.c.163.com",
    "https://mirror.baidubce.com"
  ]
}
```

### Volume 挂载
```yaml
volumes:
  - /home/jry/FSP/tools:/home/jry/FSP/tools:rw    # Blender
  - /home/jry/FSP/data:/home/jry/FSP/data:rw      # 数据
  - /home/jry/FSP/logs:/home/jry/FSP/logs:rw      # 日志
```

## 🔧 故障排除

### Docker 权限问题
如果遇到权限问题，运行：
```bash
sudo chmod 666 /var/run/docker.sock
```

### 网络超时问题
如果构建时遇到网络超时：
1. 检查网络连接
2. 确认镜像源配置正确
3. 考虑使用离线构建方式

### Blender 相关问题
- 检查 Blender 路径: `/home/jry/FSP/tools/blender/blender`
- 运行测试脚本: `./test-blender.sh`
- 确认环境变量设置正确

## 📝 环境要求

### 系统要求
- Ubuntu 24.04 LTS
- Docker 27.5.1+
- Docker Compose 2.33.0+

### 硬件要求
- 建议 4GB+ RAM
- 2GB+ 可用磁盘空间
- 支持虚拟化的 CPU

## 🔍 验证步骤

### 1. 验证 Docker
```bash
docker version
docker compose version
```

### 2. 验证 Blender
```bash
cd /home/jry/FSP
./test-blender.sh
```

### 3. 验证配置
```bash
docker compose config
```

### 4. 启动服务
```bash
docker compose up -d
curl http://localhost:5000  # 测试 Web 服务
```

## 📚 相关文档

- `BLENDER_SETUP_README.md` - Blender 详细配置
- `DOCKER_DEPLOYMENT_README.md` - Docker 部署指南
- `FSP_工作流文档.md` - 项目工作流说明

---

**配置完成时间**: 2025年9月27日
**状态**: ✅ 准备就绪，可以启动服务
