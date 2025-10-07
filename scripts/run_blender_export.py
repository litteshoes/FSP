"""
scripts/run_blender_export.py
"""
import bpy, json, sys, os, math
from mathutils import Vector
from pathlib import Path

def log(message): print(f"[BlenderExport] {message}")

def clear_existing_trees(collection_name="GeneratedTrees"):
    log("Clearing existing trees from scene...")
    if bpy.context.mode != 'OBJECT': bpy.ops.object.mode_set(mode='OBJECT')
    gen_collection = bpy.data.collections.get(collection_name)
    if not gen_collection:
        gen_collection = bpy.data.collections.new(collection_name)
        bpy.context.scene.collection.children.link(gen_collection)
        return gen_collection
    for obj in list(gen_collection.objects): bpy.data.objects.remove(obj, do_unlink=True)
    return gen_collection
def _create_principled_material(name: str, rgba=(0.6, 0.6, 0.6, 1.0)):
    mat = bpy.data.materials.get(name)
    if not mat:
        mat = bpy.data.materials.new(name=name)
    mat.use_nodes = True
    nt = mat.node_tree
    # 清空旧节点
    for n in list(nt.nodes):
        nt.nodes.remove(n)
    bsdf = nt.nodes.new('ShaderNodeBsdfPrincipled')
    bsdf.location = (0, 0)
    bsdf.inputs['Base Color'].default_value = rgba
    bsdf.inputs['Roughness'].default_value = 0.6
    out = nt.nodes.new('ShaderNodeOutputMaterial')
    out.location = (200, 0)
    nt.links.new(bsdf.outputs['BSDF'], out.inputs['Surface'])
    return mat

def _get_species_material(species_code: str):
    key = (species_code or 'unknown').lower()
    if 'conifer' in key or key in {'0', '1', '3'}:  # PFT 1 也映射到针叶树
        color = (0.145, 0.274, 0.145, 1.0)  # 深绿
        name = 'Mat_Conifer'
    elif 'broadleaf' in key or key in {'2', '4', '5'}:
        color = (0.172, 0.478, 0.196, 1.0)  # 亮绿
        name = 'Mat_Broadleaf'
    else:
        color = (0.35, 0.52, 0.36, 1.0)
        name = 'Mat_UnknownTree'
    return _create_principled_material(name, color)

def _look_at(obj, target: Vector):
    try:
        direction = (target - obj.location)
        rot_quat = direction.to_track_quat('-Z', 'Y')
        obj.rotation_euler = rot_quat.to_euler()
    except Exception:
        pass

def _world_bounds_for_trees():
    """Compute world-space AABB for generated trees."""
    objs = [o for o in bpy.data.objects if o.name.startswith("tree_")]
    if not objs:
        return None
    deps = bpy.context.evaluated_depsgraph_get()
    min_v = Vector((1e9, 1e9, 1e9))
    max_v = Vector((-1e9, -1e9, -1e9))
    for o in objs:
        ev = o.evaluated_get(deps)
        for corner in o.bound_box:
            w = ev.matrix_world @ Vector(corner)
            min_v.x, min_v.y, min_v.z = min(min_v.x, w.x), min(min_v.y, w.y), min(min_v.z, w.z)
            max_v.x, max_v.y, max_v.z = max(max_v.x, w.x), max(max_v.y, w.y), max(max_v.z, w.z)
    return (min_v, max_v)


def load_tree_models():
    log("Loading preset tree models...")
    templates = {}
    for assets_collection_name in ["TreeAssets_Clean", "TreeAssets"]:
        assets_collection = bpy.data.collections.get(assets_collection_name)
        if assets_collection:
            for obj in assets_collection.objects: templates[obj.name] = obj
            log(f"Loaded {len(assets_collection.objects)} assets from collection '{assets_collection_name}'")
            break
    if not templates: log("Error: Asset collection 'TreeAssets_Clean' or 'TreeAssets' not found")
    return templates

def generate_forest(scene_data_path, tree_templates, target_collection):
    log(f"Generating forest from {scene_data_path.name}...")
    try:
        with open(scene_data_path, 'r', encoding='utf-8') as f: data = json.load(f)
    except Exception as e:
        log(f"Error: Cannot read or parse JSON file: {e}"); return
    trees_data = data.get("trees", [])
    log(f"Found {len(trees_data)} tree data points, starting processing...")
    if not tree_templates: return
    terrain_obj = bpy.data.objects.get("Terrain")

    cluster_to_model_map = {
        '0': ['conifer_1', 'conifer_1.001'],
        '1': ['conifer_1', 'conifer_1.001'],  # PFT 1 mapped to conifer
        '2': ['broadleaf_2', 'broadleaf_2.002'],
        '3': ['conifer_1', 'conifer_1.001'],
        '4': ['broadleaf_1', 'broadleaf_1.002'],
        '5': ['broadleaf_2', 'broadleaf_2.002'],
        'unknown': ['broadleaf_1']
    }

    depsgraph = bpy.context.evaluated_depsgraph_get()
    terrain_eval = terrain_obj.evaluated_get(depsgraph) if terrain_obj else None
    for i, tree_info in enumerate(trees_data):
        try:
            species_code = tree_info.get("group")
            if species_code is None: species_code = tree_info.get("species", "unknown")
            species_code = str(species_code)
            model_name_list = cluster_to_model_map.get(species_code, cluster_to_model_map['unknown'])
            tree_id_str = str(tree_info.get('tree_id', f'tree_{i}'))
            model_index = abs(hash(tree_id_str)) % len(model_name_list)
            selected_model_name = model_name_list[model_index]
            model_template = tree_templates.get(selected_model_name)
            if not model_template:
                log(f"Warning: Model '{selected_model_name}' not found, skipping."); continue
            pos_x, pos_y = float(tree_info.get("x", 0)), float(tree_info.get("y", 0))
            height = float(tree_info.get("height", 1.0))
            if terrain_eval and terrain_obj:
                ray_origin, ray_direction = Vector((pos_x, pos_y, 1000)), Vector((0, 0, -1))
                hit, location, _, _ = terrain_eval.ray_cast(
                    origin=terrain_obj.matrix_world.inverted() @ ray_origin,
                    direction=terrain_obj.matrix_world.inverted().to_3x3() @ ray_direction
                )
                final_location = terrain_obj.matrix_world @ location if hit else Vector((pos_x, pos_y, 0))
            else:
                final_location = Vector((pos_x, pos_y, 0))
            # Copy object to preserve materials/UV/modifiers; for large resources can reference data only
            new_tree = model_template.copy()
            new_tree.data = model_template.data.copy()
            target_collection.objects.link(new_tree)
            new_tree.location = final_location
            source_height = model_template.dimensions.z
            if source_height > 0.001:
                sf = max(height / source_height, 0.05)
                new_tree.scale = (sf, sf, sf)
            new_tree.rotation_euler.z = (abs(hash(tree_id_str)) % 360) * (math.pi / 180)
            # Ensure material exists (if resource has no material, assign species color material)
            if not new_tree.data.materials or len(new_tree.data.materials) == 0:
                mat = _get_species_material(species_code)
                if len(new_tree.data.materials) == 0:
                    new_tree.data.materials.append(mat)
                else:
                    new_tree.data.materials[0] = mat
        except Exception as e:
            log(f"Error processing tree {i}: {e}, data: {tree_info}")
    # Give terrain a base material to avoid pure gray
    if terrain_obj and (not terrain_obj.data.materials or len(terrain_obj.data.materials) == 0):
        terrain_mat = _create_principled_material('Mat_Terrain', (0.46, 0.44, 0.40, 1.0))
        terrain_obj.data.materials.append(terrain_mat)
    log("Forest generation completed.")
    bpy.context.view_layer.update()

def export_to_gltf(output_path):
    log(f"Preparing to export scene to: {output_path}...")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    bpy.ops.export_scene.gltf(filepath=str(output_path), export_format='GLB', export_apply=True,
                              export_draco_mesh_compression_enable=True, export_draco_mesh_compression_level=6)
    log(f"Successfully exported to {output_path}"); print("EXPORT_SUCCESS")

def render_preview(preview_path: Path):
    try:
        log(f"Preparing to render preview to: {preview_path}...")
        scene = bpy.context.scene
        # Fast render settings
        try:
            scene.render.engine = 'BLENDER_EEVEE'
        except Exception:
            try:
                scene.render.engine = 'EEVEE'
            except Exception:
                scene.render.engine = 'BLENDER_WORKBENCH'
        scene.render.image_settings.file_format = 'PNG'
        scene.render.resolution_x = 1280
        scene.render.resolution_y = 720
        scene.render.resolution_percentage = 70
        # Brighter world background
        if scene.world:
            scene.world.color = (0.95, 0.95, 0.95)
        # Set up camera (if not exists)
        cam = bpy.data.objects.get('FormindPreviewCam')
        if not cam:
            bpy.ops.object.camera_add()
            cam = bpy.context.active_object
            cam.name = 'FormindPreviewCam'
        # Adaptive camera to generated trees
        b = _world_bounds_for_trees()
        if b:
            min_v, max_v = b
            center = (min_v + max_v) * 0.5
            span_vec = (max_v - min_v)
            span = max(span_vec.x, span_vec.y, span_vec.z)
            dist = max(60.0, float(span) * 2.2)
            cam.location = center + Vector((dist, dist, dist))
            _look_at(cam, center)
            cam.data.clip_start = 0.1
            cam.data.clip_end = max(1000.0, dist * 10.0)
        else:
            cam.location = (100.0, 100.0, 100.0)
            cam.rotation_euler = (0.7, 0.0, 0.78)
        scene.camera = cam
        # Lightweight lighting - add sun lamp
        light_data = bpy.data.lights.new(name="Sun", type='SUN')
        light_obj = bpy.data.objects.new(name="Sun", object_data=light_data)
        scene.collection.objects.link(light_obj)
        light_obj.location = (80, 120, 80)
        light_data.energy = 5.0
        preview_path.parent.mkdir(parents=True, exist_ok=True)
        scene.render.filepath = str(preview_path)
        bpy.ops.render.render(write_still=True)
        log(f"Preview render completed: {preview_path}")
    except Exception as e:
        log(f"Preview rendering failed: {e}")

def main():
    log("Blender export script starting.")
    try:
        bpy.context.scene.render.engine = 'BLENDER_WORKBENCH'
        args = sys.argv[sys.argv.index("--") + 1:]
        scene_data_file = Path(args[args.index("--scene_data") + 1])
        output_file = Path(args[args.index("--output") + 1])
        preview_file = None
        if "--preview" in args:
            preview_file = Path(args[args.index("--preview") + 1])
    except Exception as e:
        log(f"Error: Initialization or argument parsing failed: {e}"); sys.exit(1)
    target_collection = clear_existing_trees()
    tree_templates = load_tree_models()
    if tree_templates:
        generate_forest(scene_data_file, tree_templates, target_collection)
        export_to_gltf(output_file)
        if preview_file:
            render_preview(preview_file)
    else:
        log("Error: Failed to load any tree asset models."); sys.exit(1)
    log("Script execution completed.")

main()