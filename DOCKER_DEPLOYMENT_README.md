# FSP项目Docker部署指南

本文档介绍如何使用Docker将FSP（Forest Simulation Platform）项目部署到服务器端。

## 📋 项目概述

FSP是一个基于Flask的森林模拟平台，支持：
- 森林生长模拟（使用FORMIND模型）
- 3D可视化（使用Blender）
- 多情景气候变化分析
- 异步任务处理（使用Celery + Redis）

## 🏗️ 架构设计

```
┌─────────────────┐    ┌─────────────────┐
│   Nginx (80)    │    │    Flask Web    │
│  Reverse Proxy  │◄──►│     (5000)     │
└─────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌─────────────────┐
                       │  Celery Worker  │
                       │   (异步任务)     │
                       └─────────────────┘
                              │
                              ▼
                       ┌─────────────────┐
                       │     Redis       │
                       │  Message Queue  │
                       └─────────────────┘
```

## 📁 项目结构

```
fsp-docker-deployment/
├── backend/
│   └── Dockerfile              # 后端Dockerfile
├── nginx/
│   └── default.conf            # Nginx配置
├── compose.yaml                # 服务编排配置
├── requirements.txt            # Python依赖
├── celery_config.py           # Celery配置
├── deploy.sh                  # 部署脚本
└── DOCKER_DEPLOYMENT_README.md # 本文档
```

## 🚀 快速开始

### 1. 环境准备

确保服务器已安装：
- Docker (版本 >= 20.10)
- Docker Compose (版本 >= 2.0)

```bash
# 检查安装
docker --version
docker compose version
```

### 2. 项目部署

#### 方法一：使用部署脚本（推荐）

```bash
# 克隆项目到服务器
git clone <your-repo-url>
cd fsp-project

# 执行部署
./deploy.sh deploy
```

#### 方法二：手动部署

```bash
# 1. 克隆项目
git clone <your-repo-url>
cd fsp-project

# 2. 创建必要目录
mkdir -p data logs

# 3. 构建镜像
docker compose build

# 4. 启动服务
docker compose up -d

# 5. 检查状态
docker compose ps
```

### 3. 验证部署

```bash
# 检查服务状态
docker compose ps

# 查看日志
docker compose logs -f web

# 访问应用
curl http://localhost/health
```

## 🔧 配置说明

### 环境变量

在 `compose.yaml` 中可以配置以下环境变量：

```yaml
environment:
  - CELERY_BROKER_URL=redis://redis:6379/0
  - CELERY_RESULT_BACKEND=redis://redis:6379/0
  - FLASK_ENV=production
```

### 端口映射

- **80**: Nginx反向代理端口（对外暴露）
- **5000**: Flask应用端口（内部使用）
- **6379**: Redis端口（内部使用）

### 数据卷挂载

```yaml
volumes:
  - ./data:/app/data:rw      # 模拟数据目录
  - ./logs:/app/logs:rw      # 日志目录
  - redis_data:/data         # Redis持久化数据
```

## 📊 服务管理

### 常用命令

```bash
# 启动服务
docker compose up -d

# 停止服务
docker compose down

# 重启服务
docker compose restart

# 查看日志
docker compose logs -f [service_name]

# 进入容器
docker compose exec web bash

# 扩展Worker数量
docker compose up -d --scale worker=3
```

### 服务状态监控

```bash
# 查看所有服务状态
docker compose ps

# 查看资源使用
docker stats

# 查看特定服务日志
docker compose logs -f worker
docker compose logs -f redis
```

## 🔍 故障排除

### 常见问题

1. **端口冲突**
   ```bash
   # 检查端口占用
   netstat -tulpn | grep :80
   # 修改docker-compose.yml中的端口映射
   ```

2. **内存不足**
   ```bash
   # 检查系统内存
   free -h
   # 增加交换空间或服务器内存
   ```

3. **Blender渲染失败**
   ```bash
# 检查Blender是否正确安装
docker compose exec web blender --version
# 查看详细错误日志
docker compose logs worker
   ```

4. **FORMIND模拟失败**
   ```bash
   # 检查 FORMIND 可执行文件（Linux 版）
   docker compose -f compose.yaml exec worker sh -lc 'ls -l /app/tools/formind/bin; /app/tools/formind/bin/formind --help || true'
   # 验证输入文件格式
   ```
### Linux 版 FORMIND 重建（如容器内存在 Windows 版 .exe）

如果日志出现 PermissionError 指向 `formind.exe`，说明容器内为 Windows 可执行，需使用 Docker 多阶段构建编译 Linux 版本（已在 `backend/Dockerfile` 实现）：

```bash
# 只重建并重启 worker（包含 Linux 版 formind）
docker compose -f compose.yaml build --no-cache worker
docker compose -f compose.yaml up -d --no-deps --force-recreate worker

# 验证
docker compose -f compose.yaml exec worker sh -lc 'ls -l /app/tools/formind/bin; /app/tools/formind/bin/formind --help || true'
```


### 日志分析

```bash
# 查看所有服务日志
docker compose logs

# 查看特定时间段的日志
docker compose logs --since "1h" web

# 实时监控日志
docker compose logs -f
```

## 🔧 性能优化

### 1. 调整Worker数量

```yaml
# 在 compose.yaml 中修改
worker:
  deploy:
    replicas: 4  # 根据CPU核心数调整
```

### 2. 调整Gunicorn配置

```yaml
web:
  command: gunicorn --bind 0.0.0.0:5000 --workers 8 --timeout 120 run:app
```

### 3. Redis优化

```yaml
redis:
  command: redis-server --maxmemory 512mb --maxmemory-policy allkeys-lru
```

## 🔐 安全配置

### 1. 使用HTTPS

```nginx
# 在nginx/default.conf中添加
server {
    listen 443 ssl;
    ssl_certificate /path/to/cert.pem;
    ssl_certificate_key /path/to/key.pem;
    # ... 其他配置
}
```

### 2. 限制上传文件大小

```nginx
# nginx/default.conf
client_max_body_size 50M;
```

### 3. 添加防火墙规则

```bash
# 只开放必要端口
ufw allow 80
ufw allow 443
ufw deny 5000  # Flask内部端口
ufw deny 6379  # Redis内部端口
```

## 📈 监控和维护

### 健康检查

```bash
# 添加到nginx配置
location /health {
    access_log off;
    return 200 "healthy\n";
}
```

### 备份策略

```bash
# 备份数据卷
docker run --rm -v fsp_redis_data:/data -v $(pwd):/backup alpine tar czf /backup/redis_backup.tar.gz -C /data .
docker run --rm -v $(pwd)/data:/data -v $(pwd):/backup alpine tar czf /backup/app_data_backup.tar.gz -C /data .
```

### 日志轮转

```bash
# 配置logrotate
/etc/logrotate.d/fsp:
/var/log/fsp/*.log {
    daily
    rotate 7
    compress
    missingok
    notifempty
}
```

## 🚀 更新部署

```bash
# 停止服务
docker compose down

# 拉取最新代码
git pull origin main

# 重建镜像
docker compose build --no-cache

# 启动服务
docker compose up -d

# 清理旧镜像
docker image prune -f
```

## 📞 支持

如果遇到问题，请：

1. 查看日志：`docker compose logs`
2. 检查服务状态：`docker compose ps`
3. 验证配置文件语法
4. 参考本文档的故障排除部分

## 📝 版本信息

- Docker: >= 20.10
- Docker Compose: >= 2.0
- Python: 3.9
- Flask: 2.3.3
- Celery: 5.3.4
- Redis: 7-alpine
- Nginx: alpine
