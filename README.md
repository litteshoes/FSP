# FSP - Forest Simulation Platform

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)
[![Python 3.11+](https://img.shields.io/badge/python-3.11+-blue.svg)](https://www.python.org/downloads/)
[![Docker](https://img.shields.io/badge/docker-ready-brightgreen.svg)](https://www.docker.com/)

**FSP** (Forest Simulation Platform) is an integrated web-based platform for forest ecosystem simulation and 3D visualization, combining the FORMIND ecological model with Blender-based rendering capabilities.

## 🌟 Features

- **Multi-Scenario Climate Projections**: Simulate forest dynamics under SSP1-2.6, SSP2-4.5, and SSP5-8.5 climate scenarios
- **Individual-Based Forest Modeling**: Powered by the FORMIND model from Helmholtz Centre for Environmental Research (UFZ)
- **3D Visualization**: Real-time forest visualization using Blender GLB exports
- **Web-Based Interface**: User-friendly Flask web application
- **Flexible Input Options**: Support for CSV tree inventories, custom PIN files, or template-based initialization
- **Comprehensive Analysis**: Automated statistical reports, biomass tracking, and carbon flux analysis
- **Scalable Architecture**: Docker-based deployment with Celery for distributed task processing

## 📋 Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Configuration](#configuration)
- [Scientific Background](#scientific-background)
- [License](#license)
- [Citation](#citation)
- [Contributing](#contributing)
- [Acknowledgments](#acknowledgments)

## 🚀 Installation

### Prerequisites

- Docker >= 20.10
- Docker Compose v2
- Git with Git LFS installed
- 8GB+ RAM recommended
- 20GB+ free disk space

### Docker Deployment (Recommended)

1. **Clone the repository:**
```bash
git clone https://github.com/litteshoes/FSP.git
cd FSP

# Install Git LFS and fetch large files
git lfs install
git lfs pull
```

2. **Build Docker images with Blender:**
```bash
# Blender is installed automatically during build (NOT included in Git)
docker compose build --build-arg INSTALL_BLENDER=1

# Optional: specify Blender version (default: 4.3.2 LTS)
docker compose build --build-arg INSTALL_BLENDER=1 --build-arg BLENDER_VERSION=4.3.2
```

3. **Prepare runtime environment:**
```bash
mkdir -p data logs
cp docker-env-example.txt .env
# Edit .env if needed
```

4. **Start services:**
```bash
docker compose up -d
```

5. **Verify deployment:**
```bash
docker compose ps
curl http://localhost/health

# Verify Blender installation
docker compose run --rm worker blender-wrapper --version
```

The platform will be available at **http://localhost/**

> **Note**: Blender binaries are NOT included in Git repository. They are automatically downloaded and installed during Docker image build.

### Local Development Setup

For local development without Docker:

1. **Install Python 3.11+:**
```bash
python -m venv .venv
source .venv/bin/activate  # On Windows: .venv\Scripts\activate
pip install -r requirements.txt
```

2. **Install system dependencies:**
   - **FORMIND binary**: See `tools/formind/` directory
   - **Blender 4.3+ LTS**: Download from [blender.org](https://www.blender.org/download/lts/)
   - **Redis server**: For task queue
   - **FFmpeg** (optional): For video generation
   
   **Blender installation example:**
   ```bash
   # Linux
   wget https://mirrors.aliyun.com/blender/release/Blender4.3/blender-4.3.2-linux-x64.tar.xz
   tar -xf blender-4.3.2-linux-x64.tar.xz
   sudo mv blender-4.3.2-linux-x64 /usr/local/blender
   export PATH="/usr/local/blender:$PATH"
   ```

3. **Configure paths** in `config.py`

4. **Start services:**
```bash
# Terminal 1: Redis
redis-server

# Terminal 2: Celery worker
celery -A celery_config worker --loglevel=info

# Terminal 3: Flask app
python run.py
```

## 🎯 Quick Start

### Running Your First Simulation

1. **Access the web interface** at http://localhost

2. **Create a new scenario:**
   - Go to "New Simulation"
   - Choose climate scenario (SSP1-2.6, SSP2-4.5, or SSP5-8.5)
   - Upload tree inventory CSV or use default template

3. **Configure simulation parameters:**
   - Simulation duration (years)
   - Plot dimensions (x, y size in meters)
   - Climate data source

4. **Start simulation** and monitor progress in real-time

5. **View results:**
   - 3D forest visualization (GLB model)
   - Statistical reports
   - Biomass and carbon flux charts
   - Download raw output data

### Example: CSV Tree Inventory Format

```csv
x,y,species,dbh,height
10.5,12.3,Quercus_robur,45.2,23.5
15.8,20.1,Fagus_sylvatica,38.7,21.2
```

Required columns:
- `x`, `y`: Tree position in meters
- `species`: Species name (use underscore for spaces)
- `dbh`: Diameter at breast height (cm)
- `height`: Tree height (m)

## 📁 Project Structure

```
FSP/
├── app/                      # Flask web application
│   ├── routes.py            # Web routes and API endpoints
│   ├── static/              # CSS, JS, generated models
│   └── templates/           # HTML templates
├── backend/                 # Docker and deployment configs
│   ├── Dockerfile           # Multi-stage Docker build
│   └── gunicorn.conf.py     # WSGI server configuration
├── data/                    # Simulation data (gitignored)
│   ├── scenarios/           # User scenarios
│   ├── climate/             # Climate input files
│   └── simulation_results/  # Output files
├── scripts/                 # Utility scripts
│   ├── generate_pin_from_csv.py    # CSV → PIN converter
│   ├── parse_formind_results.py    # Result parser
│   └── extract_tree_statistics.py  # Statistics extractor
├── tools/                   # External tools
│   ├── formind/             # FORMIND model and binaries
│   └── blender/             # Blender installation (auto-installed)
├── research/                # Research and calibration tools
│   └── fit_pft_from_inventory.py   # PFT parameter fitting
├── celery_config.py         # Celery task queue configuration
├── config.py                # Application configuration
├── run.py                   # Flask application entry point
├── compose.yaml             # Docker Compose orchestration
└── requirements.txt         # Python dependencies
```

## ⚙️ Configuration

### Environment Variables

Key variables in `.env` file:

```bash
# Blender Installation
INSTALL_BLENDER=1            # Install Blender in Docker (0=use volume mount)
BLENDER_VERSION=4.3.2        # Blender version to install

# Application
FLASK_ENV=production
BASE_DIR=/home/jry/FSP

# Celery Worker
CELERY_PREFETCH=1
CELERY_MAX_TASKS_PER_CHILD=50
CELERY_RESULT_EXPIRES=3600

# Redis
REDIS_MAXMEMORY=256mb
REDIS_MAXMEMORY_POLICY=allkeys-lru

# Performance
GUNICORN_WORKERS=1
GUNICORN_TIMEOUT=120
DISABLE_BLENDER_EXPORT=0     # Set to 1 to skip 3D export
```

### FORMIND Configuration

Edit PAR/PIN files in `tools/formind/Formind_Model/` to customize:
- Plant functional types (PFTs)
- Growth parameters
- Mortality rates
- Environmental responses

## 🔬 Scientific Background

### FORMIND Model

FORMIND is an individual-based forest growth model developed at the Helmholtz Centre for Environmental Research (UFZ). It simulates:

- **Individual tree dynamics**: Growth, mortality, and recruitment
- **Light competition**: 3D canopy structure and light availability
- **Water balance**: Soil moisture and drought stress
- **Carbon cycling**: Biomass accumulation and decomposition

**Key Publications:**
- Köhler, P., & Huth, A. (1998). The effects of tree species grouping in tropical rainforest modelling. *Ecological Modelling*, 109(3), 263-274.
- Fischer, R., et al. (2016). Lessons learned from applying a forest gap model to understand ecosystem and carbon dynamics of complex tropical forests. *Ecological Modelling*, 326, 124-133.

### Climate Scenarios

FSP supports CMIP6 climate scenarios:
- **SSP1-2.6**: Sustainability pathway (low emissions)
- **SSP2-4.5**: Middle-of-the-road scenario
- **SSP5-8.5**: Fossil-fueled development (high emissions)

Climate data sources:
- WorldClim future projections
- Regional climate models (RCM)
- Custom meteorological data

## 📜 License

This project is licensed under the **GNU Affero General Public License v3.0 or later** (AGPL-3.0-or-later).

This licensing choice is required because:
- **FORMIND**: Licensed under AGPL v3.0 or later
- **Blender**: Licensed under GNU GPL v3.0 or later

For details, see [LICENSE](LICENSE) file.

### Third-Party Components

- **FORMIND**: © Helmholtz Centre for Environmental Research (UFZ), AGPL v3
- **Blender**: © Blender Foundation, GNU GPL v3
- See `tools/formind/license/` and `tools/blender/license/` for full license texts

## 📖 Citation

If you use FSP in your research, please cite:

```bibtex
@software{fsp_2024,
  title = {FSP: Forest Simulation Platform},
  author = {{FSP Development Team}},
  year = {2024},
  url = {https://github.com/litteshoes/FSP},
  version = {1.0.0},
  note = {FORMIND-based forest ecosystem simulator with 3D visualization}
}
```

For FORMIND model citations, see `tools/formind/license/FORMIND_citation.txt`

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Workflow

1. Fork the repository
2. Create feature branch: `git checkout -b feature/amazing-feature`
3. Commit changes: `git commit -m 'Add amazing feature'`
4. Push to branch: `git push origin feature/amazing-feature`
5. Open Pull Request

### Code Style

- Follow PEP 8 for Python code
- Use meaningful variable names
- Add docstrings for functions and classes
- Write comments for complex logic

## 📞 Contact & Support

- **Issues**: Report bugs at [GitHub Issues](https://github.com/litteshoes/FSP/issues)
- **Discussions**: Ask questions at [GitHub Discussions](https://github.com/litteshoes/FSP/discussions)
- **Email**: [Your contact email]

## 🙏 Acknowledgments

- **Helmholtz Centre for Environmental Research (UFZ)** for the FORMIND model
- **Blender Foundation** for the 3D rendering engine
- All contributors and users of this platform

## 📊 Project Status

- ✅ Core simulation engine (FORMIND) integrated
- ✅ Web interface functional
- ✅ Docker deployment ready
- ✅ Multi-scenario climate support
- 🚧 Advanced visualization features (in progress)
- 🚧 API documentation (in progress)
- 📋 Mobile interface (planned)

## 🔗 Links

- **Documentation**: See `/docs` directory (coming soon)
- **FORMIND Official**: [UFZ FORMIND](https://www.ufz.de/formind/)
- **Blender**: [blender.org](https://www.blender.org/)
- **WorldClim**: [worldclim.org](https://www.worldclim.org/)

---

**Made with ❤️ for forest ecology research**

