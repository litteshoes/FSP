# -*- coding: utf-8 -*-
# scripts/generate_summary_report.py
import os
import json
import pandas as pd
import logging
import sys
from pathlib import Path

# --- Project Configuration Import ---
project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(project_root)
import config
from scripts import parse_formind_results

# --- Logging Configuration ---
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def find_output_file(directory, extension):
    """Find files with specified extension in directory."""
    files = list(Path(directory).glob('*.' + extension))
    if files:
        return str(files[0])
    return None

def parse_table_with_header(file_path):
    """
    Parse FORMIND output files (e.g., .ba / .bt / .n / .cflux)
    - Skip first two description lines
    - Read third line as column names
    - Return {year: {col: value, ...}}
    """
    if not file_path or not os.path.exists(file_path):
        return {}
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = [ln.strip() for ln in f if ln.strip()]
    if len(lines) < 3:
        return {}
    header = lines[2].split()
    table = {}
    for ln in lines[3:]:
        parts = ln.split()
        if len(parts) != len(header):
            continue
        try:
            year = int(float(parts[0]))
        except ValueError:
            continue
        row = {}
        for k, v in zip(header[1:], parts[1:]):
            try:
                row[k] = float(v)
            except ValueError:
                row[k] = None
        table[year] = row
    return table


def try_find_climate_file_from_results_dir(results_dir):
    """
    Infer climate file path from results directory.
    Compatible with tasks.py directory structure: <job>/formind_inputs/<scenario>/Climate/<file>.txt
    """
    try:
        # Extract scenario name from results_dir: .../formind_outputs/<scenario>
        scenario = os.path.basename(results_dir)
        job_dir = os.path.dirname(os.path.dirname(results_dir))  # Up two levels: .../<job>
        climate_dir = os.path.join(job_dir, 'formind_inputs', scenario, 'Climate')

        # Find climate file
        for p in Path(climate_dir).glob('*.txt'):
            return str(p)

        # If not found, try fallback path (compatible with old structure)
        scenario_dir = os.path.dirname(results_dir)
        old_climate_dir = os.path.join(scenario_dir, 'formind_parameters', 'Climate')
        for p in Path(old_climate_dir).glob('*.txt'):
            return str(p)

    except Exception:
        pass
    return None


def summarize_climate_file(file_path):
    """
    Extract summary statistics from FORMIND climate file. Robust parsing for varying formats:
    - Parse floating point numbers line by line, use maximum column count
    - Output yearly means (simple average) and column counts
    Returns empty dict on failure.
    """
    if not file_path or not os.path.exists(file_path):
        return {}
    try:
        rows = []
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            for ln in f:
                nums = []
                for tok in ln.replace('\t', ' ').split():
                    try:
                        nums.append(float(tok))
                    except ValueError:
                        pass
                if nums:
                    rows.append(nums)
        if not rows:
            return {}
        # 对齐列：以最大列数为准，不足补 None
        maxc = max(len(r) for r in rows)
        cols = [[] for _ in range(maxc)]
                for r in rows:
            r2 = r + [None] * (maxc - len(r))
            for i, v in enumerate(r2):
                if v is not None:
                    cols[i].append(v)
        means = [sum(c) / len(c) if c else None for c in cols]
        # Return mean values for first 6 columns (approximate meanings for correlation analysis, not exact physical fields)
        return {
            "columns": maxc,
            "mean_col_1": means[0] if len(means) > 0 else None,
            "mean_col_2": means[1] if len(means) > 1 else None,
            "mean_col_3": means[2] if len(means) > 2 else None,
            "mean_col_4": means[3] if len(means) > 3 else None,
            "mean_col_5": means[4] if len(means) > 4 else None,
            "mean_col_6": means[5] if len(means) > 5 else None,
        }
    except Exception:
        return {}


def parse_climate_timeseries(file_path):
    """
    Calculate yearly statistics from FORMIND climate file.
    Assumes first line is header: rain, temperature, irradiance, day_length, PET, co2
    Remaining lines are daily records (~365*N lines).
    - Rain/PET accumulated yearly
    - Other variables averaged yearly
    Returns {year: {rain_sum_mm, pet_sum_mm, temperature_mean_C, irradiance_mean_umol, day_length_mean_h, co2_mean_ppm}}
    Returns {} on failure.
    """
    import re
    if not file_path or not os.path.exists(file_path):
        return {}
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = [ln.strip() for ln in f if ln.strip()]
        if len(lines) < 2:
            return {}
        header = re.split(r"\s+", lines[0])
        # Normalize column names
        def norm(s):
            s = s.lower()
            s = re.sub(r"\[.*?\]", "", s)
            s = s.replace('daylength', 'day_length')
            return s
        cols = [norm(c) for c in header]
        name_to_idx = {c: i for i, c in enumerate(cols)}
        # Fallback to positional mapping if column names missing
        wanted = ['rain', 'temperature', 'irradiance', 'day_length', 'pet', 'co2']
        for i, w in enumerate(wanted):
            if w not in name_to_idx and i < len(cols):
                name_to_idx[w] = i

        # Load data values
        rows = []
        for ln in lines[1:]:
            parts = re.split(r"\s+", ln)
            try:
                rows.append([float(x) for x in parts])
            except Exception:
                continue
        if not rows:
            return {}

        DAYS = 365
        total = len(rows)

        # Process only complete years (multiples of 365 days)
        complete_years = total // DAYS
        actual_years = complete_years

        out = {}
        for y in range(actual_years):
            s, e = y * DAYS, min((y + 1) * DAYS, total)
            seg = rows[s:e]
            if not seg:  # Avoid empty segments
                continue

            # Convert year from 0-based to 1-based
            year_index = y + 1

            def colvals(name):
                j = name_to_idx.get(name)
                return [r[j] for r in seg if j is not None and j < len(r)]

            rain_sum = sum(colvals('rain')) if colvals('rain') else None
            pet_sum = sum(colvals('pet')) if colvals('pet') else None
            def mean(vals):
                return (sum(vals) / len(vals)) if vals else None

            out[year_index] = {
                'rain_sum_mm': rain_sum,
                'pet_sum_mm': pet_sum,
                'temperature_mean_C': mean(colvals('temperature')),
                'irradiance_mean_umol': mean(colvals('irradiance')),
                'day_length_mean_h': mean(colvals('day_length')),
                'co2_mean_ppm': mean(colvals('co2')),
            }
        return out
    except Exception:
        return {}

def generate(job_id, formind_output_dir, scenario=None):
    """
    Generate final statistical summary report for a simulation task.
    """
    logger.info("[{}] === Starting statistical summary report generation ===".format(job_id))
    
    # formind_output_dir is now passed from tasks.py
    blender_input_dir = os.path.join(config.SIMULATION_RESULTS_DIR, job_id, 'blender_inputs')
    
    summary_data = {}
    
    # 1. Parse statistical files (.cflux, .ba, .bt, etc.)
    cflux = parse_table_with_header(find_output_file(formind_output_dir, 'cflux'))
    ba = parse_table_with_header(find_output_file(formind_output_dir, 'ba'))
    bt = parse_table_with_header(find_output_file(formind_output_dir, 'bt'))
    stems = parse_table_with_header(find_output_file(formind_output_dir, 'n'))
    dia = parse_table_with_header(find_output_file(formind_output_dir, 'dia'))

    # Parse *_th versions (threshold statistics)
    ba_th = parse_table_with_header(find_output_file(formind_output_dir, 'ba_th'))
    bt_th = parse_table_with_header(find_output_file(formind_output_dir, 'bt_th'))

    # 2. Extract tree summary from .res first (yearly totals and species distribution); fallback to blender_inputs
    tree_summary = {}
    try:
        res_path = parse_formind_results.find_res_file(formind_output_dir)
        import pandas as pd
        df = pd.read_csv(res_path, sep='\t', skiprows=2, low_memory=False)
        df = df.rename(columns={
            'Time': 'year', 'Grp': 'group', 'Species': 'species_code', 'N': 'total_number',
            'BT': 'biomass', 'D': 'dbh', 'H': 'height', 'SV': 'stem_volume',
            'X': 'x', 'Y': 'y', 'ID': 'tree_id', 'CR': 'crown_radius', 'CD': 'crown_diameter',
            'LAI': 'tree_lai', 'Plot': 'plot_number', 'Hec': 'hectar_number'
        })
        grouped = df.groupby('year')
        for year, year_df in grouped:
            try:
                year_int = int(year)
            except Exception:
                continue
            species_count = {}
            for _, row in year_df.iterrows():
                try:
                    sp = str(int(row.get('group')))
                except Exception:
                    sp = 'unknown'
                species_count[sp] = species_count.get(sp, 0) + 1
            tree_summary[year_int] = {
                "total_trees": int(sum(species_count.values())),
                "species_summary": species_count
            }
    except Exception:
        # Fallback: read from blender_inputs (if exists)
        json_files = list(Path(blender_input_dir).glob('*.json')) if os.path.exists(blender_input_dir) else []
        for json_file in json_files:
            try:
                with open(json_file, 'r', encoding='utf-8', errors='ignore') as f:
                    data = json.load(f)
                year = data.get('year')
                if year is None:
                    continue
                species_count = {}
                for tree in data.get('trees', []):
                    species = tree.get('species', 'unknown')
                    species_count[species] = species_count.get(species, 0) + 1
                tree_summary[year] = {
                    "total_trees": data.get('total_trees', len(data.get('trees', []))),
                    "species_summary": species_count
                }
            except Exception:
                continue

    # 3. Integrate all data
    # 3.1 Climate overview and yearly data
    climate_file = try_find_climate_file_from_results_dir(formind_output_dir)
    climate_summary = summarize_climate_file(climate_file) if climate_file else {}
    climate_yearly = parse_climate_timeseries(climate_file) if climate_file else {}

    all_years = set(cflux.keys()) | set(ba.keys()) | set(bt.keys()) | set(stems.keys()) | set(tree_summary.keys()) | set(climate_yearly.keys()) | set(dia.keys())
    # Cumulative variables (rolling yearly)
    cumulative_nep = 0.0
    prev_pools = None  # (biomass, deadwood, soil_fast, soil_slow)

    for year in sorted(list(all_years)):
        # Safe value retrieval
        ba_row = ba.get(year, {})
        bt_row = bt.get(year, {})
        cf_row = cflux.get(year, {})
        n_row = stems.get(year, {})

        # Dynamically collect PFT columns (BiomassPerPFT_i / BasalAreaPerPFT_i / NumberPerPFT_i)
        def collect_by_prefix(row, prefix):
            return {k: v for k, v in row.items() if k.lower().startswith(prefix.lower())}

        # Raw carbon fluxes and carbon pools
        nee = cf_row.get("NEE") if cf_row else None
        gpp = cf_row.get("GPP") if cf_row else None
        r_total = cf_row.get("R_total") if cf_row else cf_row.get("R_TOT") if cf_row else None
        r_b = cf_row.get("R_biomass") if cf_row else None
        r_dw = cf_row.get("R_DeadWood") if cf_row else None
        r_ss = cf_row.get("R_Soil_Slow") if cf_row else None
        r_sf = cf_row.get("R_Soil_Fast") if cf_row else None
        c_bio = cf_row.get("CPool_Biomass") if cf_row else None
        c_dw = cf_row.get("CPool_DeadWood") if cf_row else None
        c_sf = cf_row.get("CPool_Soil_fast") if cf_row else None
        c_ss = cf_row.get("CPool_Soil_slow") if cf_row else None
        aet = cf_row.get("AET") if cf_row else None

        # Derived variables
        nep = (-nee) if isinstance(nee, float) else None
        npp = (gpp - r_b) if (isinstance(gpp, float) and isinstance(r_b, float)) else None
        cue = (npp / gpp) if (isinstance(npp, float) and isinstance(gpp, float) and gpp != 0.0) else None
        if isinstance(nep, float):
            cumulative_nep += nep
        # Biomass turnover time tau ≈ CPool_Biomass / NPP
        tau = None
        try:
            if isinstance(c_bio, float) and isinstance(npp, float) and npp not in (None, 0.0):
                tau = c_bio / npp
        except Exception:
            tau = None

        # Carbon pool changes (adjacent year difference)
        delta_pools = {}
        if all(isinstance(x, float) for x in (c_bio, c_dw, c_sf, c_ss)):
            pools_now = (c_bio, c_dw, c_sf, c_ss)
            if prev_pools is not None:
                delta_pools = {
                    "biomass": pools_now[0] - prev_pools[0],
                    "deadwood": pools_now[1] - prev_pools[1],
                    "soil_fast": pools_now[2] - prev_pools[2],
                    "soil_slow": pools_now[3] - prev_pools[3],
                }
                delta_pools["total"] = sum(delta_pools.values())
            prev_pools = pools_now

        summary_data[year] = {
            "year": year,
            "trees": tree_summary.get(year, {}),
            "carbon_flux": {
                "nee": nee,
                "gpp": gpp,
                "r_total": r_total,
                "r_biomass": r_b,
                "r_deadwood": r_dw,
                "r_soil_slow": r_ss,
                "r_soil_fast": r_sf,
                "biomass_carbon": c_bio,
                "deadwood_carbon": c_dw,
                "soil_fast_carbon": c_sf,
                "soil_slow_carbon": c_ss,
                "aet": aet,
            } if cflux else {},
            "biomass": {
                "basal_area_total": ba_row.get("TotalBasalArea"),
                "basal_area_pft": collect_by_prefix(ba_row, "BasalAreaPerPFT_"),
                "total_biomass": bt_row.get("TotalBiomass"),
                "biomass_pft": collect_by_prefix(bt_row, "BiomassPerPFT_"),
                "stems_total": n_row.get("TotalNumber"),
                "stems_pft": collect_by_prefix(n_row, "NumberPerPFT_")
            },
            "thresholds": {
                "ba_th": ba_th.get(year, {}) if ba_th else {},
                "bt_th": bt_th.get(year, {}) if bt_th else {},
            },
            "structure": {
                "diameter_distribution": dia.get(year, {}) if dia else {}
            },
            "derived": {
                "nep": nep,
                "npp_approx": npp,
                "cue": cue,
                "delta_carbon_pools": delta_pools,
                "cumulative_nep": cumulative_nep,
                "tau": tau,
            },
            "climate": climate_yearly.get(year, {})
        }
    
    # 3.2 Generate scenario-level aggregate metrics
    years_sorted = sorted(summary_data.keys())
    aggregates = {}
    if years_sorted:
        y0, y1 = years_sorted[0], years_sorted[-1]
        try:
            b0 = summary_data[y0]["biomass"].get("total_biomass")
            b1 = summary_data[y1]["biomass"].get("total_biomass")
            cum_nep = summary_data[y1]["derived"].get("cumulative_nep")
            aggregates = {
                "start_year": y0,
                "end_year": y1,
                "delta_total_biomass": (b1 - b0) if (isinstance(b0, float) and isinstance(b1, float)) else None,
                "cumulative_nep": cum_nep,
                "climate_summary": climate_summary,
            }
        except Exception:
            pass

    # 4. Save final summary report
    if scenario:
        report_path = os.path.join(config.SIMULATION_RESULTS_DIR, job_id, 'summary_report_{}.json'.format(scenario))
    else:
        report_path = os.path.join(config.SIMULATION_RESULTS_DIR, job_id, 'summary_report.json')
    with open(report_path, 'w', encoding='utf-8') as f:
        json.dump(summary_data, f, indent=2)

    # 4.1 Save "scientific version" enhanced report (including derived and aggregated metrics)
    if scenario:
        sci_path = os.path.join(config.SIMULATION_RESULTS_DIR, job_id, 'summary_science_{}.json'.format(scenario))
    else:
        sci_path = os.path.join(config.SIMULATION_RESULTS_DIR, job_id, 'summary_science.json')
    with open(sci_path, 'w', encoding='utf-8') as f:
        json.dump({"yearly": summary_data, "aggregates": aggregates}, f, indent=2)
        
    logger.info("[{}] Statistical summary report generated: {}".format(job_id, report_path))
    if scenario:
        logger.info("[{}] Scientific version report generated: {}".format(job_id, sci_path))
    logger.info("[{}] === Statistical report generation completed ===".format(job_id))