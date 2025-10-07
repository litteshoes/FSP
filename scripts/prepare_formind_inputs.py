import os, re, logging, sys, shutil, json

project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(project_root)
import config
from scripts import generate_pin_from_csv

logger = logging.getLogger(__name__)

class ParFileModifier:
    def __init__(self, template_path):
        with open(template_path, 'r', encoding='utf-8') as f: self.content = f.read()
    def set_value(self, key, value):
        pattern = re.compile(f"^(?P<prefix>(?:float|int|string)\s+{re.escape(key)}\s+)(?P<value>.*)$", re.M | re.I)
        value_str = str(value)
        self.content, count = pattern.subn(r"\g<prefix>" + value_str, self.content)
        if count == 0: 
            logger.warning(f"Parameter '{key}' not found")
    def set_array_value(self, key, values_2d_list):
        pattern = re.compile(f"(array\s+{re.escape(key)}.*?data\s*?)(.*?)(end)", re.DOTALL | re.I)
        match = pattern.search(self.content)
        if match:
            prefix, _, suffix = match.groups()
            new_data_lines = "\n" + "\n".join(["\t" + "\t".join(map(str, row)) for row in values_2d_list]) + "\n"
            self.content = self.content.replace(match.group(0), f"{prefix}{new_data_lines}{suffix}", 1)
        else: 
            logger.warning(f"Array parameter '{key}' not found")
    
    def save(self, output_path):
        with open(output_path, 'w', encoding='utf-8', newline='\n') as f: 
            f.write(self.content)

def prepare(job_id, pft_count, use_defaults, custom_data):
    job_project_dir = os.path.join(config.SIMULATION_RESULTS_DIR, job_id, 'formind_project')
    params_dir = os.path.join(job_project_dir, 'formind_parameters')
    climate_dir = os.path.join(params_dir, 'Climate')
    for d in [params_dir, climate_dir, os.path.join(job_project_dir, 'results')]: os.makedirs(d, exist_ok=True)
    
    try:
        par_template_path = os.path.join(config.EXAMPLE_DATA_DIR, f'tianmu_{pft_count}pft.par')
        modifier = ParFileModifier(par_template_path)
        par_params = custom_data.get('par_params', {})
        for key, value in par_params.items():
            if key == 'N_Par.Div_MAXGRP': continue
            if isinstance(value, list): modifier.set_array_value(key, value)
            else: modifier.set_value(key, value)
        modifier.set_value('TimeEnd', len(config.YEARS_TO_RENDER))
        modifier.set_value('PinFileNameX', '"simulation.pin"')
        modifier.set_value('N_Par.Div_MAXGRP', pft_count)
        climate_block_pattern = re.compile(r"(array\s+N_Par.Climate_File.*?data\s*?)(.*?)(end)", re.DOTALL | re.IGNORECASE)
        new_climate_data = "\n\t./Climate/climate.txt\n"
        modifier.content, count = climate_block_pattern.subn(rf"\1{new_climate_data}\3", modifier.content)
        output_par_path = os.path.join(params_dir, "simulation.par")
        modifier.save(output_par_path)
        
        output_pin_path = os.path.join(params_dir, "simulation.pin")
        pin_csv_filepath = custom_data.get('pin_csv_filepath')
        pin_content_from_textarea = custom_data.get('pin_content')
        if pin_csv_filepath:
            pin_content = generate_pin_from_csv.generate(pin_csv_filepath, pft_count)
            with open(output_pin_path, 'w', encoding='utf-8') as f: f.write(pin_content)
        elif not use_defaults['pin'] and pin_content_from_textarea:
            with open(output_pin_path, 'w', encoding='utf-8') as f: f.write(pin_content_from_textarea)
        else:
            pin_template_path = os.path.join(config.EXAMPLE_DATA_DIR, f'tianmu_{pft_count}pft.pin')
            shutil.copy(pin_template_path, output_pin_path)
        
        output_climate_path = os.path.join(climate_dir, "climate.txt")
        if use_defaults['climate']:
            # Use default SSP245 climate data
            shutil.copy(os.path.join(config.EXAMPLE_DATA_DIR, 'climate_ssp245_100y.txt'), output_climate_path)
        else:
            climate_filepath = custom_data.get('climate_filepath')
            if not climate_filepath: raise FileNotFoundError("Custom climate mode but no file path provided.")
            shutil.copy(climate_filepath, output_climate_path)
            os.remove(climate_filepath)
        return output_par_path
    except Exception as e:
        logger.error(f"[{job_id}] Error during data preparation: {e}")
        raise