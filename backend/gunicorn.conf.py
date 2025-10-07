import multiprocessing
import os

bind = os.environ.get("GUNICORN_BIND", "0.0.0.0:5000")

# Low memory optimization: use gevent worker, limit concurrency and connections
worker_class = os.environ.get("GUNICORN_WORKER_CLASS", "gevent")

# 1-2x CPU cores, but limited by memory. Default 1 or 2.
default_workers = 1 if multiprocessing.cpu_count() <= 2 else 2
workers = int(os.environ.get("GUNICORN_WORKERS", str(default_workers)))

# Coroutine concurrency per worker
worker_connections = int(os.environ.get("GUNICORN_WORKER_CONNECTIONS", "1000"))

timeout = int(os.environ.get("GUNICORN_TIMEOUT", "120"))
graceful_timeout = int(os.environ.get("GUNICORN_GRACEFUL_TIMEOUT", "30"))
keepalive = int(os.environ.get("GUNICORN_KEEPALIVE", "15"))

# Lower memory footprint and faster restarts
max_requests = int(os.environ.get("GUNICORN_MAX_REQUESTS", "200"))
max_requests_jitter = int(os.environ.get("GUNICORN_MAX_REQUESTS_JITTER", "20"))

accesslog = os.environ.get("GUNICORN_ACCESSLOG", "/app/logs/gunicorn.access.log")
errorlog = os.environ.get("GUNICORN_ERRORLOG", "/app/logs/gunicorn.error.log")
loglevel = os.environ.get("GUNICORN_LOG_LEVEL", "info")

# Preload app to share read-only memory, reduce overall RSS (works with gevent)
preload_app = True

def post_fork(server, worker):
    # Limit BLAS/OMP threads to avoid overwhelming CPU/memory on small machines
    os.environ.setdefault("OMP_NUM_THREADS", "1")
    os.environ.setdefault("OPENBLAS_NUM_THREADS", "1")
    os.environ.setdefault("MKL_NUM_THREADS", "1")


