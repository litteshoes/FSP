# GitHub Setup Guide for FSP

This guide will help you prepare and upload the FSP project to GitHub with Git LFS support.

**⚠️ IMPORTANT UPDATE**: Blender binaries are now excluded from Git and installed automatically via Dockerfile. This significantly reduces repository size and LFS usage.

## Prerequisites

- Git installed (2.x or later)
- Git LFS installed
- GitHub account
- Repository created at https://github.com/litteshoes/FSP

## Step 1: Install Git LFS

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install git-lfs
```

### macOS
```bash
brew install git-lfs
```

### Windows
Download from: https://git-lfs.github.com/

## Step 2: Initialize Git LFS

```bash
# Navigate to project directory
cd /home/jry/FSP

# Initialize Git LFS
git lfs install

# Verify LFS is working
git lfs version
```

## Step 3: Initial Git Setup

```bash
# Initialize git repository (if not already done)
git init

# Add all files
git add .

# Check what will be tracked by LFS
git lfs ls-files

# Make initial commit
git commit -m "Initial commit: FSP Forest Simulation Platform

- FORMIND-based forest ecosystem simulator
- Multi-scenario climate projection support (SSP1-2.6, SSP2-4.5, SSP5-8.5)
- 3D visualization with Blender
- Web-based Flask interface
- Docker deployment configuration
- Blender installed via Dockerfile (not in repository)
"
```

## Step 4: Connect to GitHub

```bash
# Add remote repository
git remote add origin https://github.com/litteshoes/FSP.git

# Verify remote
git remote -v
```

## Step 5: Clean Up LFS Cache (If Previously Used)

If you previously tracked Blender in LFS, clean it up before pushing:

```bash
# Remove Blender from Git cache
git rm --cached -r tools/blender/ 2>/dev/null || true

# Prune old LFS objects
git lfs prune

# Verify what's tracked by LFS
git lfs ls-files

# Commit cleanup
git add .gitignore .gitattributes
git commit -m "chore: exclude Blender from repo, install via Dockerfile"
```

**For detailed LFS cleanup**, see `CLEAR_LFS_GUIDE.md` in the project root.

## Step 6: Push to GitHub

### Option A: Push with LFS (Recommended)

```bash
# Push main branch
git push -u origin main

# If you're using 'master' instead of 'main'
git branch -M main
git push -u origin main
```

### Option B: If repository already exists on GitHub

```bash
# Pull any existing content first
git pull origin main --allow-unrelated-histories

# Resolve any conflicts if they occur
# Then push
git push -u origin main
```

## Step 7: Install Blender in Docker

Since Blender is not in the repository, install it during Docker build:

```bash
# Build Docker image with Blender installation
docker compose build --build-arg INSTALL_BLENDER=1

# Specify custom Blender version (optional)
docker compose build --build-arg INSTALL_BLENDER=1 --build-arg BLENDER_VERSION=4.3.2

# Verify Blender installation in container
docker compose run --rm worker blender-wrapper --version

# Expected output: Blender 4.3.2
```

**Available Blender versions**: See [Blender Release Archive](https://www.blender.org/download/lts/)

**Default version**: 4.3.2 (LTS - Long Term Support)

## Step 8: Verify LFS Upload

After pushing, verify that large files are stored in LFS:

```bash
# List LFS tracked files
git lfs ls-files

# Check LFS storage
git lfs env
```

On GitHub, navigate to your repository and check:
- Large files should show "Stored with Git LFS" label
- Repository size should be reasonable (not GB)

## Large Files Being Tracked

The `.gitattributes` file is configured to track:

### Blender Scene Files (User-Created Only)
- `data/scenes/*.blend` - User-created Blender scenes
- **Note**: Blender binary itself is NOT tracked (excluded from Git)

### 3D Models
- `*.glb`, `*.gltf` - Web-ready 3D formats
- `*.fbx`, `*.obj` - Standard 3D formats

### Executables
- `tools/formind/bin/formind` - FORMIND binary
- **Note**: `tools/blender/` directory is excluded, Blender installed via Dockerfile

### Media Files
- Video files (`.mp4`, `.avi`, `.mov`, `.webm`)
- High-res images (`.psd`, `.tif`, `.exr`, `.hdr`)

### Data Files
- Database files (`.db`, `.sqlite`)
- Large data files (`.hdf5`, `.nc`, `.parquet`)

## Troubleshooting

### Issue: Blender not found in Docker container

```bash
# Option 1: Build with Blender installation
docker compose build --no-cache --build-arg INSTALL_BLENDER=1

# Option 2: Mount local Blender (edit docker-compose.yaml)
# volumes:
#   - /usr/local/blender:/app/tools/blender:ro

# Verify installation
docker compose run --rm worker blender-wrapper --version
```

### Issue: "This exceeds GitHub's file size limit"

If you see this error for files other than Blender:

```bash
# Find large files not tracked by LFS
find . -type f -size +100M -not -path "./.git/*" -not -path "./tools/blender/*"

# Add them to .gitattributes
echo "path/to/large/file filter=lfs diff=lfs merge=lfs -text" >> .gitattributes

# Migrate to LFS
git lfs migrate import --include="path/to/large/file"
```

### Issue: "rate limit exceeded"

GitHub has bandwidth limits for LFS. If you hit them:

1. Wait for the limit to reset (usually 1 hour)
2. Consider uploading in batches
3. Use `git push --no-verify` to skip hooks if needed

### Issue: Files already committed without LFS

If you already committed large files without LFS:

```bash
# Migrate existing files to LFS
git lfs migrate import --include="*.blend,*.glb" --everything

# Force push (WARNING: rewrites history)
git push -f origin main
```

## Recommended Workflow

### For Future Commits

```bash
# Check status
git status

# Add specific files or all changes
git add .

# Commit with descriptive message
git commit -m "feat: add new feature"

# Push to GitHub
git push origin main
```

### For Large Files

Before committing large files:

```bash
# Check file size
du -sh path/to/file

# If > 100MB, ensure it's in .gitattributes
grep "filename" .gitattributes

# Add to .gitattributes if needed
echo "path/to/file filter=lfs diff=lfs merge=lfs -text" >> .gitattributes

# Then commit normally
git add .
git commit -m "Add large file with LFS"
git push
```

## GitHub Repository Settings

After uploading, configure your repository:

1. **Add Description**: "Forest Simulation Platform - FORMIND-based ecosystem modeling with 3D visualization"

2. **Add Topics**:
   - `forest-modeling`
   - `ecological-simulation`
   - `climate-change`
   - `python`
   - `docker`
   - `blender`
   - `formind`
   - `3d-visualization`

3. **Enable Features**:
   - ✅ Issues
   - ✅ Discussions
   - ✅ Wiki (optional)
   - ✅ Projects (optional)

4. **Set Up Branch Protection** (Settings → Branches):
   - Protect `main` branch
   - Require pull request reviews
   - Enable status checks

5. **Add GitHub Actions** (optional):
   - Settings → Actions
   - Enable workflows for CI/CD

## Git LFS Storage Limits

GitHub provides:
- **Free accounts**: 1 GB storage, 1 GB/month bandwidth
- **Pro accounts**: 2 GB storage, 2 GB/month bandwidth
- Additional packs available for purchase

**After excluding Blender** (~500MB-1GB savings):
- Your LFS usage should be well under 1 GB
- Mainly tracking: FORMIND binary, user scenes, generated models

Monitor your usage:
- Settings → Billing → Git LFS Data

**Need to clear old LFS data?** See `CLEAR_LFS_GUIDE.md` for detailed instructions.

## Next Steps

After successful upload:

1. ✅ Verify Blender installation in Docker: `docker compose build --build-arg INSTALL_BLENDER=1`
2. ✅ Add a nice README banner/logo
3. ✅ Set up GitHub Actions for CI/CD
4. ✅ Create issue templates
5. ✅ Add pull request template
6. ✅ Set up dependabot for security updates
7. ✅ Add badges to README (build status, coverage, etc.)
8. ✅ Create releases and tags

## Quick Start for Users

New users cloning the repository should:

```bash
# 1. Clone repository
git clone https://github.com/litteshoes/FSP.git
cd FSP

# 2. Install Git LFS and fetch LFS files
git lfs install
git lfs pull

# 3. Build Docker images (includes Blender installation)
docker compose build --build-arg INSTALL_BLENDER=1

# 4. Start services
docker compose up -d

# 5. Access FSP at http://localhost:5000
```

**No manual Blender installation needed!** Everything is automated via Docker.

## Useful Commands

```bash
# Check repository size
git count-objects -vH

# List largest files in repo
git rev-list --objects --all | \
  git cat-file --batch-check='%(objecttype) %(objectname) %(objectsize) %(rest)' | \
  sed -n 's/^blob //p' | \
  sort --numeric-sort --key=2 | \
  tail -20

# Clean up old LFS files (if needed)
git lfs prune

# Fetch LFS files
git lfs fetch

# Pull LFS files
git lfs pull
```

## Support

- Git LFS Documentation: https://git-lfs.github.com/
- GitHub LFS Guide: https://docs.github.com/en/repositories/working-with-files/managing-large-files
- FSP Issues: https://github.com/litteshoes/FSP/issues

---

**Note**: This is a Linux version of FSP. Windows users may need to build FORMIND from source or use WSL2.

