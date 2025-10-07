# config.py - Configuration module for FSP
import os
import shutil
from dotenv import load_dotenv

# --- Project Root Directory ---
# In Docker environment, working directory is /app
load_dotenv(os.path.join(os.path.dirname(__file__), '.env'))
# Prioritize environment variable, fallback to current file directory if invalid
_base_dir_env = os.environ.get('BASE_DIR')
if _base_dir_env and os.path.isdir(_base_dir_env):
    BASE_DIR = _base_dir_env
else:
    BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# =============================================================================
# 1. External Tools Configuration
# =============================================================================
TOOLS_DIR = os.path.join(BASE_DIR, 'tools')

# Blender configuration (priority: env var > project path > system PATH)
if os.name == 'nt':
    _default_blender = os.path.join(TOOLS_DIR, 'blender', 'blender.exe')
else:
    _default_blender = os.path.join(TOOLS_DIR, 'blender', 'blender')

_blender_from_env = os.environ.get('BLENDER_PATH')
_blender_candidates = []
if _blender_from_env:
    _blender_candidates.append(_blender_from_env)
_blender_candidates.append(_default_blender)

_blender_resolved = next(
    (os.path.abspath(p) for p in _blender_candidates if p and os.path.exists(p) and os.access(p, os.X_OK)),
    None
)
if _blender_resolved is None:
    _which_blender = shutil.which('blender')
    _blender_resolved = _which_blender if _which_blender else os.path.abspath(_default_blender)

BLENDER_EXECUTABLE = _blender_resolved
BLENDER_PROJECT_FILE = os.environ.get('BLENDER_PROJECT_FILE', os.path.join(BASE_DIR, 'data', 'scenes', 'my_forest_scene.blend'))
BLENDER_EXPORT_SCRIPT = os.environ.get('BLENDER_EXPORT_SCRIPT', os.path.join(BASE_DIR, 'scripts', 'run_blender_export.py'))

# FORMIND configuration
FORMIND_DIR = os.environ.get('FORMIND_DIR', os.path.join(TOOLS_DIR, 'formind'))
FORMIND_MODEL_DIR = os.environ.get('FORMIND_MODEL_DIR', os.path.join(FORMIND_DIR, 'Formind_Model'))
FORMIND_BIN_DIR = os.environ.get('FORMIND_BIN_DIR', os.path.join(FORMIND_DIR, 'bin'))
# FORMIND executable (cross-platform support; env var override; existence fallback)
_formind_from_env = os.environ.get('FORMIND_EXECUTABLE')
_formind_candidates = []
if _formind_from_env:
    _formind_candidates.append(_formind_from_env)
# Prioritize container-installed executable to avoid host mount permission issues
_formind_candidates.append('/usr/local/bin/formind')
_formind_candidates.append(os.path.join(FORMIND_BIN_DIR, 'formind'))
_formind_candidates.append(os.path.join(FORMIND_BIN_DIR, 'formind.exe'))

def _is_executable(path: str) -> bool:
    try:
        return path and os.path.exists(path) and os.access(path, os.X_OK)
    except Exception:
        return False

# On non-Windows platforms, prioritize Linux executables and filter out .exe files
if os.name == 'nt':
    FORMIND_EXECUTABLE = next((p for p in _formind_candidates if _is_executable(p)), _formind_candidates[0])
else:
    linux_first = [p for p in _formind_candidates if not p.endswith('.exe')]
    FORMIND_EXECUTABLE = next((p for p in linux_first if _is_executable(p)), linux_first[0])


# FFmpeg configuration (for video generation)
FFMPEG_EXECUTABLE = os.environ.get('FFMPEG_PATH', 'ffmpeg')
# =============================================================================
# 2. Data and Template Paths
# =============================================================================
DATA_DIR = os.environ.get('DATA_DIR', os.path.join(BASE_DIR, 'data'))

# Example parameter files directory
EXAMPLE_DATA_DIR = os.environ.get('EXAMPLE_DATA_DIR', os.path.join(DATA_DIR, 'example_parameters'))

# User uploaded files temporary directory
USER_UPLOADS_DIR = os.environ.get('USER_UPLOADS_DIR', os.path.join(DATA_DIR, 'user_uploads'))

# Simulation results root directory
SIMULATION_RESULTS_DIR = os.environ.get('SIMULATION_RESULTS_DIR', os.path.join(DATA_DIR, 'simulation_results'))

# Web application final model storage location
WEB_MODELS_DIR = os.environ.get('WEB_MODELS_DIR', os.path.join(BASE_DIR, 'app', 'static', 'models'))
WEB_VIDEOS_DIR = os.environ.get('WEB_VIDEOS_DIR', os.path.join(BASE_DIR, 'app', 'static', 'videos'))


# =============================================================================
# 3. Simulation Parameters
# =============================================================================
# Default simulation years, can be overridden by UI
YEARS_TO_RENDER = range(87)  # [0, 1, 2, ..., 86] corresponds to 2014-2100

# =============================================================================
# 4. Auto-create Directories
# =============================================================================
for directory in [
    os.path.join(DATA_DIR, 'scenes'),
    EXAMPLE_DATA_DIR,
    USER_UPLOADS_DIR,
    SIMULATION_RESULTS_DIR,
    WEB_MODELS_DIR
]:
    os.makedirs(directory, exist_ok=True)