# app/__init__.py
# -*- coding: utf-8 -*-
from flask import Flask
from dotenv import load_dotenv
import os
import config  # Import global configuration
from .extensions import cache, compress

def create_app():
    """
    Application factory function for creating and configuring Flask app.
    """
    # Create Flask instance. Flask automatically searches for 'static' and 'templates' folders.
    # Load .env file if exists
    load_dotenv(os.path.join(os.path.dirname(os.path.dirname(__file__)), '.env'))
    app = Flask(__name__)
    
    # Load configuration from config.py into app.config
    # This allows access to config via app.config['SIMULATION_YEARS'] in the application
    app.config.from_object(config)
    # Development experience: enable template auto-reload
    app.config['TEMPLATES_AUTO_RELOAD'] = True
    app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0
    
    # Initialize extensions
    cache.init_app(app, config={
        'CACHE_TYPE': os.environ.get('CACHE_TYPE', 'simple'),
        'CACHE_DEFAULT_TIMEOUT': int(os.environ.get('CACHE_DEFAULT_TIMEOUT', '120')),
    })
    compress.init_app(app)

    # Register blueprints
    from . import routes
    app.register_blueprint(routes.bp)
    
    # Disable caching for all responses
    @app.after_request
    def add_no_cache_headers(response):
        response.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, max-age=0'
        response.headers['Pragma'] = 'no-cache'
        response.headers['Expires'] = '0'
        return response
    
    return app