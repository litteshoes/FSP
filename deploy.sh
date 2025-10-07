#!/bin/bash

# FSP Project Docker Deployment Script
# Usage: ./deploy.sh [build|start|stop|restart|logs|cleanup]

set -e

PROJECT_NAME="fsp"
DOCKER_COMPOSE_FILE="compose.yaml"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if Docker and docker-compose are installed
check_dependencies() {
    if ! command -v docker &> /dev/null; then
        log_error "Docker is not installed, please install Docker first"
        exit 1
    fi

    # Prefer Docker Compose v2 plugin
    if docker compose version &> /dev/null; then
        COMPOSE="docker compose"
    elif command -v docker-compose &> /dev/null; then
        COMPOSE="docker-compose"
        log_warn "Old docker-compose detected, recommend upgrading to Docker Compose v2 (docker compose)"
    else
        log_error "Docker Compose not detected, please install docker compose plugin or docker-compose"
        exit 1
    fi

    log_info "Dependency check passed"
}

# Build images
build() {
    log_info "Building Docker images..."
    $COMPOSE -f $DOCKER_COMPOSE_FILE build --no-cache
    log_info "Docker images built successfully"
}

# Start services
start() {
    log_info "Starting services..."
    $COMPOSE -f $DOCKER_COMPOSE_FILE up -d
    log_info "Services started successfully"

    # Wait for services to be ready
    log_info "Waiting for services to start..."
    sleep 10

    # Check service status
    check_status
}

# Stop services
stop() {
    log_info "Stopping services..."
    $COMPOSE -f $DOCKER_COMPOSE_FILE down
    log_info "Services stopped"
}

# Restart services
restart() {
    log_info "Restarting services..."
    stop
    start
}

# View logs
logs() {
    log_info "Displaying service logs..."
    $COMPOSE -f $DOCKER_COMPOSE_FILE logs -f
}

# Check status
check_status() {
    log_info "Checking service status..."
    $COMPOSE -f $DOCKER_COMPOSE_FILE ps

    # Check web service health
    if curl -f http://localhost/health &> /dev/null; then
        log_info "Web service is running normally"
    else
        log_warn "Web service may not be ready"
    fi
}

# Clean up resources
cleanup() {
    log_warn "Cleaning up Docker resources..."
    $COMPOSE -f $DOCKER_COMPOSE_FILE down -v --rmi all
    docker system prune -f
    log_info "Cleanup completed"
}

# Initial deployment
deploy() {
    log_info "Starting FSP project deployment..."

    # Check dependencies
    check_dependencies

    # Create necessary directories
    mkdir -p data logs

    # Build and start
    build
    start

    log_info "Deployment completed!"
    log_info "Access URL: http://localhost"
    log_info "API URL: http://localhost/api/"
}

# Main function
main() {
    case "${1:-deploy}" in
        "build")
            check_dependencies
            build
            ;;
        "start")
            check_dependencies
            start
            ;;
        "stop")
            stop
            ;;
        "restart")
            check_dependencies
            restart
            ;;
        "logs")
            logs
            ;;
        "status")
            check_status
            ;;
        "cleanup")
            cleanup
            ;;
        "deploy")
            deploy
            ;;
        *)
            echo "Usage: $0 [build|start|stop|restart|logs|status|cleanup|deploy]"
            echo "  build   - Build Docker images"
            echo "  start   - Start services"
            echo "  stop    - Stop services"
            echo "  restart - Restart services"
            echo "  logs    - View logs"
            echo "  status  - Check service status"
            echo "  cleanup - Clean up all resources"
            echo "  deploy  - Full deployment (default)"
            exit 1
            ;;
    esac
}

main "$@"
