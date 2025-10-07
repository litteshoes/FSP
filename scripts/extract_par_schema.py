# scripts/extract_par_schema.py (版本 5.0 - 终极可靠版)
import re, json
from pathlib import Path

PAR_TEMPLATE_PATH = Path("data/example_parameters/tianmu.par")
OUTPUT_SCHEMA_PATH = Path("app/static/par_schema.json")
PARAMETER_WHITELIST = [
    "TimeEnd", "OutputStep", "N_Par.Mort_mean_19", "N_Par.Mort_FallP",
    "N_Par.Est_NS_3", "N_Par.Pro_GLoss", "N_Par.Geo_HMmean_5", "Switch.DLYR"
]

def parse_par_file(content):
    schema = {"groups": []}
    current_group = None
    lines = content.splitlines()
    param_pattern = re.compile(r"^\s*(?P<type>float|int|string|array)\s+(?P<key>\S+)")

    # --- 1. 预扫描，构建key到所有相关信息的映射 ---
    param_info_map = {}
    temp_param = None
    in_data_block = False

    for line in lines:
        match = param_pattern.match(line)
        if match:
            if temp_param: param_info_map[temp_param['key']] = temp_param
            key = match.group('key')
            temp_param = {'key': key, 'type': match.group('type'), 'value': [], 'metadata_lines': []}
            in_data_block = False
        elif temp_param:
            stripped_line = line.strip()
            if stripped_line.startswith('data'): in_data_block = True
            elif stripped_line.startswith('end'): temp_param = None; in_data_block = False
            elif in_data_block and stripped_line: temp_param['value'].append(stripped_line.split())
            elif not in_data_block: temp_param['metadata_lines'].append(stripped_line)

    if temp_param: param_info_map[temp_param['key']] = temp_param

    # --- 2. 根据白名单和分组构建最终schema ---
    for i, line in enumerate(lines):
        line = line.strip()
        group_match = re.match(r"^comment -+\s*(.*?)\s*-+", line)
        if group_match and "output" not in group_match.group(1).lower():
            group_name = group_match.group(1).strip()
            if not any(g['name'] == group_name for g in schema['groups']):
                current_group = {"name": group_name, "parameters": []}
                schema["groups"].append(current_group)
        
        param_match = param_pattern.match(line)
        if param_match and current_group:
            key = param_match.group('key')
            if key in PARAMETER_WHITELIST and not any(p['key'] == key for p in current_group['parameters']):
                info = param_info_map.get(key, {})
                param = {'key': key, 'type': info.get('type'), 'label_cn': key, 'unit': "", 'range': None}
                
                # 从元数据行提取描述等信息
                for desc_line in info.get('metadata_lines', []):
                    if desc_line.startswith(r'\d'): param['label_cn'] = desc_line.replace(r'\d', '').strip()
                    elif desc_line.startswith(r'\u'): param['unit'] = desc_line.replace(r'\u', '').strip()
                    elif desc_line.startswith(r'\r'):
                        range_vals = re.findall(r"[-+]?\d*\.\d+|\d+", desc_line)
                        if len(range_vals) >= 2: param['range'] = [float(range_vals[0]), float(range_vals[1])]
                
                # 提取值
                if info.get('type') == 'array':
                    param['value'] = info.get('value', [])
                else:
                    value_match = re.search(r"^\s*(?:float|int|string)\s+\S+\s+(.*)", line)
                    if value_match: param['value'] = value_match.group(1).strip().strip('"')
                
                current_group['parameters'].append(param)
    return schema

def main():
    print("--- 正在解析 .par 模板文件 (终极可靠版) ---")
    content = PAR_TEMPLATE_PATH.read_text(encoding='utf-8')
    schema = parse_par_file(content)
    OUTPUT_SCHEMA_PATH.write_text(json.dumps(schema, indent=2, ensure_ascii=False), encoding='utf-8')
    print(f"✓ 成功！UI蓝图已生成: {OUTPUT_SCHEMA_PATH}")

if __name__ == "__main__": main()