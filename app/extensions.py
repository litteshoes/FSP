from flask_caching import Cache
from flask_compress import Compress

# Unified extension instances to avoid circular imports
cache = Cache()
compress = Compress()


