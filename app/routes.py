import os, uuid, json, re
from flask import Blueprint, jsonify, render_template, url_for, current_app, request, send_from_directory
from werkzeug.utils import secure_filename
from .extensions import cache
from tasks import run_simulation_flow

def parse_bool(value, default=False):
    """Parse frontend form values to boolean."""
    if value is None:
        return default
    if isinstance(value, bool):
        return value
    s = str(value).strip().lower()
    if s in ('true', 'on', '1', 'yes', 'y'):
        return True
    if s in ('false', 'off', '0', 'no', 'n'):
        return False
    return default

def extract_schema_from_par_file(par_template_path, pft_count):
    if not os.path.exists(par_template_path): return None
    with open(par_template_path, 'r', encoding='utf-8') as f: content = f.read()
    
    schema = {"general_params": [], "pft_params": []}
    param_pattern = re.compile(r"^\s*(?P<type>float|int|string|array)\s+(?P<key>\S+)")
    
    # Pre-scan: build mapping from key to all parameter information
    param_info_map = {}
    temp_param = None
    in_data_block = False
    current_section = 'Simulation'
    section_header_pattern = re.compile(r"^\s*comment\s*-{3,}\s*(?P<section>[A-Za-z /]+)\s*-{3,}")
    for line in content.splitlines():
        # Identify section header (parameters between section titles belong to that section)
        msec = section_header_pattern.match(line)
        if msec:
            current_section = msec.group('section').strip()
            continue
        match = param_pattern.match(line)
        if match:
            if temp_param: param_info_map[temp_param['key']] = temp_param
            key, param_type = match.group('key'), match.group('type')
            temp_param = {'key': key, 'type': param_type, 'value': [], 'metadata_lines': [], 'dimension': None, 'section': current_section}
        elif temp_param:
            stripped_line = line.strip()
            if stripped_line.startswith('dimension'):
                dims = re.findall(r'\d+', stripped_line)
                temp_param['dimension'] = [int(d) for d in dims]
            elif stripped_line.startswith('data'): in_data_block = True
            elif stripped_line.startswith('end'):
                param_info_map[temp_param['key']] = temp_param
                temp_param = None; in_data_block = False
            elif in_data_block and stripped_line: temp_param['value'].append(stripped_line.split())
            elif not in_data_block and stripped_line: temp_param['metadata_lines'].append(stripped_line)
    if temp_param: param_info_map[temp_param['key']] = temp_param
    
    # Build final schema
    for key, info in param_info_map.items():
        param = {'key': key, 'type': info.get('type'), 'label_cn': key, 'unit': "", 'range': None, 'section': info.get('section')}
        for desc_line in info.get('metadata_lines', []):
            if desc_line.startswith(r'\d'): param['label_cn'] = desc_line.replace(r'\d', '', 1).strip()
            elif desc_line.startswith(r'\u'): param['unit'] = desc_line.replace(r'\u', '', 1).strip()
            elif desc_line.startswith(r'\r'):
                range_vals = re.findall(r"[-+]?\d*\.\d+|\d+", desc_line)
                if len(range_vals) >= 2: param['range'] = [float(range_vals[0]), float(range_vals[1])]
        if info.get('type') == 'array': param['value'] = info.get('value', [])
        else:
            line_match = re.search(f"^\s*(?:float|int|string)\s+{re.escape(key)}\s+(.*)", content, re.M)
            if line_match: param['value'] = line_match.group(1).strip().strip('"')
        
        is_pft_param = info.get('type') == 'array' and info.get('dimension') and len(info['dimension']) > 0 and info['dimension'][-1] == pft_count
        
        if is_pft_param: schema['pft_params'].append(param)
        else: schema['general_params'].append(param)
    return schema

def extract_schema_from_par(pft_count):
    base_dir = current_app.config['EXAMPLE_DATA_DIR']
    primary = os.path.join(base_dir, f'tianmu_{pft_count}pft.par')
    fallbacks = [
        os.path.join(base_dir, 'tianmu.par'),
        os.path.join(base_dir, 'tianmu_2pft.par'),
    ]
    schema = extract_schema_from_par_file(primary, pft_count)
    if schema: return schema
    for fb in fallbacks:
        schema = extract_schema_from_par_file(fb, pft_count)
        if schema:
            try: print(f"[par-schema] Fallback used for PFT={pft_count}: {fb}")
            except Exception: pass
            return schema
    return None

# --- Helper Function for PIN Schema Extraction (v3 - Robust) ---
def extract_schema_from_pin(pin_template_path):
    with open(pin_template_path, 'r', encoding='utf-8') as f: content = f.read()
    lines = content.splitlines()
    schema = {"general": [], "dclass": [], "plots": []}
    part, current_plot, n0_lines = "general", None, []
    
    dclass_line_found = False
    for line in lines:
        stripped_line = line.strip()
        if not stripped_line: continue
        if "END OF GENERAL PART" in stripped_line: part = "plots"; continue
        if "block plot" in stripped_line:
            if current_plot:
                current_plot["n0"] = [list(map(int, row.split())) for row in n0_lines if row]
                schema["plots"].append(current_plot)
            current_plot, n0_lines = {}, []
            part = "plot_data"
            continue

        if part == "general":
            if stripped_line.startswith('regionheader'): schema["general"].append({"key": "regionheader", "value": line.split('=')[1].strip().strip('"')})
            elif stripped_line.startswith('dclass'): dclass_line_found = True
            elif dclass_line_found and not stripped_line.startswith('file'):
                try: schema["dclass"].extend([float(d) for d in stripped_line.split()])
                except ValueError: pass
        
        if part == "plot_data" and current_plot is not None:
            if stripped_line.startswith('name'): current_plot["name"] = line.split('=')[1].strip().strip('"')
            elif stripped_line.startswith('position'): current_plot["position"] = [int(p) for p in line.split('=')[1].strip().split()]
            elif stripped_line.startswith('n0'): part = "n0_data"
        
        elif part == "n0_data":
            if stripped_line.startswith('seeds'): part = "plot_data"; continue
            n0_lines.append(stripped_line)
            
    if current_plot:
        current_plot["n0"] = [list(map(int, row.split())) for row in n0_lines if row]
        schema["plots"].append(current_plot)
    return schema

bp = Blueprint('main', __name__)

@bp.route('/')
@cache.cached(timeout=60)
def index(): 
    return render_template('index.html')

@bp.route('/api/par-schema/<int:pft_count>')
@cache.cached(timeout=300)
def get_par_schema(pft_count):
    if not 2 <= pft_count <= 6: 
        return jsonify({"error": "PFT count must be between 2 and 6"}), 400
    schema = extract_schema_from_par(pft_count)
    return jsonify(schema) if schema else jsonify({"error": f"Schema for {pft_count} PFTs not found"}), 404
@bp.route('/api/pin-schema/<int:pft_count>')
@cache.cached(timeout=300)
def get_pin_schema(pft_count):
    if not 2 <= pft_count <= 6: 
        return jsonify({"error": "PFT count must be between 2 and 6"}), 400
    pin_template_path = os.path.join(current_app.config['EXAMPLE_DATA_DIR'], f'tianmu_{pft_count}pft.pin')
    if not os.path.exists(pin_template_path): 
        return jsonify({"error": f"PIN template for {pft_count} PFTs not found"}), 404
    return jsonify(extract_schema_from_pin(pin_template_path))
@bp.route('/start-simulation', methods=['POST'])
def start_simulation():
    job_id = str(uuid.uuid4())
    job_upload_dir = os.path.join(current_app.config['USER_UPLOADS_DIR'], job_id)
    os.makedirs(job_upload_dir, exist_ok=True)
    try:
        form_data = request.form
        user_par_params = json.loads(form_data.get('par_params', '{}'))
        pft_count = int(user_par_params.get('N_Par.Div_MAXGRP', 2))

        # Strict and explicit branch logic for PIN mode handling
        selected_pin_mode = form_data.get('pin_mode', 'default')
        pin_content = form_data.get('pin_file_content', '')
        # Determine whether to use default PIN based on pin_mode
        use_default_pin = (selected_pin_mode == 'default')
        pin_csv_filepath = None
        pin_file_filepath = None
        if selected_pin_mode == 'csv':
            if 'pin_csv_file' not in request.files or request.files['pin_csv_file'].filename == '':
                return jsonify({'error': 'CSV mode selected but no tree CSV file uploaded.'}), 400
            pin_csv_file = request.files['pin_csv_file']
            filename = secure_filename(pin_csv_file.filename)
            pin_csv_filepath = os.path.join(job_upload_dir, filename)
            pin_csv_file.save(pin_csv_filepath)
        elif selected_pin_mode == 'upload':
            if 'pin_file' not in request.files or request.files['pin_file'].filename == '':
                return jsonify({'error': 'PIN upload mode selected but no PIN file uploaded.'}), 400
            pin_file = request.files['pin_file']
            filename = secure_filename(pin_file.filename)
            pin_file_filepath = os.path.join(job_upload_dir, filename)
            pin_file.save(pin_file_filepath)

        # Compatible with hidden field value="true", default to example climate if missing/empty
        use_default_climate = parse_bool(form_data.get('use_default_climate'), default=True)
        climate_filepath = None
        if not use_default_climate:
            if 'climate_file' not in request.files or request.files['climate_file'].filename == '':
                return jsonify({'error': 'Default climate unchecked but no climate file uploaded.'}), 400
            climate_file = request.files['climate_file']
            filename = secure_filename(climate_file.filename)
            climate_filepath = os.path.join(job_upload_dir, filename)
            climate_file.save(climate_filepath)
        
        task_kwargs = {
            'job_id': job_id, 'pft_count': pft_count,
            'use_defaults': {'pin': use_default_pin, 'climate': use_default_climate},
            'custom_data': {
                'par_params': user_par_params,
                'pin_content': pin_content,
                'pin_csv_filepath': pin_csv_filepath,
                'pin_file_filepath': pin_file_filepath,
                'climate_filepath': climate_filepath
            }
        }
        run_simulation_flow.apply_async(kwargs=task_kwargs)
    except Exception as e: 
        return jsonify({'error': f'Server error processing request: {e}'}), 500
    return jsonify({
        'status': 'processing', 
        'job_id': job_id, 
        'results_url': url_for('main.show_results', job_id=job_id)
    })

@bp.route('/results/<job_id>')
def show_results(job_id): 
    return render_template('results.html', job_id=job_id)

@bp.route('/status/<job_id>')
@cache.cached(timeout=5)
def get_status(job_id):
    job_dir = os.path.join(current_app.config['SIMULATION_RESULTS_DIR'], job_id)
    success_file = os.path.join(job_dir, '_SUCCESS')
    if os.path.exists(success_file):
        summary_data, model_urls = {}, []
        summary_file_path = os.path.join(job_dir, 'summary_report.json')
        summary_science_path = os.path.join(job_dir, 'summary_science.json')
        summary_compare_path = os.path.join(job_dir, 'summary_compare.json')
        if os.path.exists(summary_file_path):
            with open(summary_file_path, 'r', encoding='utf-8') as f: summary_data = json.load(f)
        summary_science = {}
        if os.path.exists(summary_science_path):
            try:
                with open(summary_science_path, 'r', encoding='utf-8') as f: summary_science = json.load(f)
            except Exception:
                summary_science = {}
        summary_compare = {}
        if os.path.exists(summary_compare_path):
            try:
                with open(summary_compare_path, 'r', encoding='utf-8') as f: summary_compare = json.load(f)
            except Exception:
                summary_compare = {}
        models_dir = os.path.join(current_app.config['WEB_MODELS_DIR'], job_id)
        if os.path.exists(models_dir):
            model_files = sorted([f for f in os.listdir(models_dir) if f.endswith('.glb')])
            model_urls = [url_for('static', filename=f"models/{job_id}/{mf}") for mf in model_files]
        return jsonify({
            'status': 'COMPLETED', 
            'models': model_urls, 
            'summary': summary_data, 
            'summary_science': summary_science, 
            'summary_compare': summary_compare
        })
    return jsonify({'status': 'PENDING'})