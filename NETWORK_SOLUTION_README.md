# FSP Docker 网络问题解决方案

## 🔍 问题诊断

当前遇到的问题：
```
Error response from daemon: Get "https://registry-1.docker.io/v2/": dial tcp 74.86.226.234:443: i/o timeout
```

### 问题分析
1. **网络连接正常**: 可以 ping 通 8.8.8.8
2. **DNS 解析正常**: 可以解析 docker.io
3. **Docker Hub 连接超时**: 无法访问 Docker 镜像仓库
4. **镜像源配置**: 已尝试多个国内镜像源，但仍存在连接问题

## 🛠️ 解决方案

### 方案一：网络修复（推荐）

#### 1. 检查网络配置
```bash
# 检查路由
ip route show

# 检查 DNS
cat /etc/resolv.conf

# 测试网络连接
ping -c 3 registry-1.docker.io
```

#### 2. 配置系统代理（如果适用）
```bash
# 创建 Docker 代理配置
sudo mkdir -p /etc/systemd/system/docker.service.d/
sudo tee /etc/systemd/system/docker.service.d/http-proxy.conf > /dev/null << 'EOF'
[Service]
Environment="HTTP_PROXY=http://your-proxy:port"
Environment="HTTPS_PROXY=http://your-proxy:port"
EOF

sudo systemctl daemon-reload
sudo systemctl restart docker
```

#### 3. 使用更可靠的镜像源
```bash
# 尝试这些镜像源
sudo tee /etc/docker/daemon.json > /dev/null << 'EOF'
{
  "registry-mirrors": [
    "https://docker.mirrors.ustc.edu.cn",
    "https://hub-mirror.c.163.com",
    "https://mirror.ccs.tencentyun.com"
  ]
}
EOF

sudo systemctl restart docker
```

### 方案二：离线/预加载方案

#### 1. 预先拉取基础镜像
```bash
# 在有网络的环境中运行
docker pull redis:7-alpine
docker pull python:3.11-slim
docker pull nginx:alpine

# 保存镜像
docker save redis:7-alpine -o redis.tar
docker save python:3.11-slim -o python.tar
docker save nginx:alpine -o nginx.tar
```

#### 2. 在目标环境中加载镜像
```bash
# 传输 tar 文件到目标环境
docker load -i redis.tar
docker load -i python.tar
docker load -i nginx.tar
```

### 方案三：简化部署（已合并）

现已统一使用 `compose.yaml`，无需简化变体：

```bash
cd /home/jry/FSP
docker compose -f compose.yaml up -d
```

#### 2. 手动验证 Blender 功能
```bash
# 测试 Blender 是否可用
cd /home/jry/FSP/tools/blender
./blender --version

# 测试 Python 环境
cd /home/jry/FSP
python3 -c "import sys; print('Python version:', sys.version)"
```

### 方案四：完全本地开发环境

#### 1. 创建本地开发环境
```bash
# 安装 Python 依赖
pip3 install -r requirements.txt

# 启动 Redis（如果需要）
# 使用系统已有的 Redis 或者跳过

# 直接运行 Flask 应用
python3 run.py
```

#### 2. 配置 Blender 路径
确保环境变量正确设置：
```bash
export BLENDER_PATH=/home/jry/FSP/tools/blender/blender
export PYTHONPATH=/home/jry/FSP:$PYTHONPATH
```

## 🔧 当前状态

### ✅ 已完成
- Docker 安装和配置 ✅
- Blender 4.5.3 解压和配置 ✅
- 项目文件结构完整 ✅
- 简化配置创建 ✅

### ⚠️ 待解决问题
- Docker Hub 网络连接 ❌
- 镜像拉取超时 ❌

### 🔄 建议操作顺序

1. **立即可用**: 使用 `compose-simple.yaml` 尝试启动简化服务
2. **网络修复**: 解决网络问题后使用完整配置
3. **离线部署**: 在网络受限环境下使用预加载镜像

## 📞 故障排除

### 如果仍然无法连接 Docker Hub

1. **检查防火墙**:
   ```bash
   sudo ufw status
   sudo iptables -L
   ```

2. **测试不同网络环境**:
   - 尝试连接到不同的 WiFi 网络
   - 检查公司/学校网络策略

3. **使用 VPN** (如果可用):
   ```bash
   # 连接到可访问 Docker Hub 的网络
   ```

4. **联系网络管理员**了解可能的网络限制

### 验证解决方案是否有效

```bash
# 测试 Docker 连接
docker run --rm hello-world

# 测试镜像拉取
docker pull alpine:latest

# 启动简化服务
cd /home/jry/FSP
docker compose -f compose-simple.yaml up -d
docker compose -f compose-simple.yaml ps
```

## 📚 相关文档

- `BLENDER_SETUP_README.md` - Blender 配置详情
- `DOCKER_SETUP_README.md` - 完整 Docker 配置指南
- `compose.yaml` - 统一配置

---

**更新时间**: 2025年9月27日
**状态**: 🔄 网络问题待解决，可使用简化配置
