# scripts/extract_pin_schema.py (v2.0)
import re, json
from pathlib import Path

PIN_TEMPLATE_PATH = Path("data/example_parameters/tianmu.pin")
OUTPUT_SCHEMA_PATH = Path("app/static/pin_schema.json")

def parse_pin_file(content):
    lines = content.splitlines()
    schema = {"general": [], "dclass": [], "plots": []}
    
    part = "general"
    current_plot = None
    n0_lines = []

    for line in lines:
        line = line.strip()
        if not line: continue
        if "END OF GENERAL PART" in line: part = "plots"; continue
        if "block plot" in line:
            if current_plot:
                current_plot["n0"] = [list(map(int, row.split())) for row in n0_lines]
                schema["plots"].append(current_plot)
            current_plot, n0_lines = {}, []
            continue

        if part == "general":
            if line.startswith('regionheader'):
                schema["general"].append({"key": "regionheader", "value": line.split('=')[1].strip().strip('"')})
            elif line.startswith('dclass'):
                # 跳过 "dclass =" 这一行
                pass
            else:
                try: # 尝试解析为dclass行
                    schema["dclass"].extend([float(d) for d in line.split()])
                except ValueError:
                    pass
        elif part == "plots" and current_plot is not None:
            if line.startswith('name'): current_plot["name"] = line.split('=')[1].strip().strip('"')
            elif line.startswith('position'): current_plot["position"] = [int(p) for p in line.split('=')[1].strip().split()]
            elif line.startswith('n0'): part = "n0"
        elif part == "n0":
            if line.startswith('seeds'):
                part = "plots"
                continue
            n0_lines.append(line)

    if current_plot:
        current_plot["n0"] = [list(map(int, row.split())) for row in n0_lines]
        schema["plots"].append(current_plot)
    return schema

def main():
    print("--- 正在解析 .pin 模板文件... ---")
    content = PIN_TEMPLATE_PATH.read_text(encoding='utf-8')
    schema = parse_pin_file(content)
    OUTPUT_SCHEMA_PATH.write_text(json.dumps(schema, indent=2, ensure_ascii=False), encoding='utf-8')
    print(f"✓ PIN UI蓝图已生成: {OUTPUT_SCHEMA_PATH}")

if __name__ == "__main__": main()