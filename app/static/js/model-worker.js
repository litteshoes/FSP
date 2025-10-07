// app/static/js/model-worker.js - Web Worker for background model loading
// 注意：当前已禁用，因为Web Worker实现有根本性问题：
// 1. 无法通过postMessage传递复杂的JavaScript对象（如Three.js库）
// 2. 无法通过postMessage返回复杂的3D场景对象
// 3. Web Worker环境无法直接访问DOM或加载CDN资源
//
// 要重新启用Web Worker，需要重新设计架构：
// - 使用SharedArrayBuffer或MessagePort传递数据
// - 在Worker中重新初始化Three.js库（需要CDN访问权限）
// - 或者改用简单的任务分发，不涉及复杂对象传递

let THREE = null;
let loader = null;

// 监听来自主线程的库注入
self.onmessage = function(e) {
    const { type, data, url, id } = e.data;

    if (type === 'MAIN_READY') {
        // 主线程已准备就绪，Worker也准备就绪
        self.postMessage({ type: 'WORKER_READY' });
    } else if (type === 'INIT_LIBS') {
        // 主线程注入Three.js库
        THREE = data.THREE;
        initLoaders();
    } else if (type === 'LOAD_MODEL') {
        if (!loader) {
            self.postMessage({
                type: 'MODEL_ERROR',
                id: id,
                url: url,
                error: '加载器未初始化',
                success: false
            });
            return;
        }

        console.log(`Worker: 开始加载模型 ${url}`);

        loader.load(
            url,
            (gltf) => {
                // 模型加载成功，发送回主线程
                self.postMessage({
                    type: 'MODEL_LOADED',
                    id: id,
                    url: url,
                    scene: gltf.scene,
                    success: true
                });
            },
            (progress) => {
                // 发送加载进度
                if (progress.lengthComputable) {
                    const percent = (progress.loaded / progress.total * 100).toFixed(1);
                    self.postMessage({
                        type: 'LOAD_PROGRESS',
                        id: id,
                        url: url,
                        progress: percent
                    });
                }
            },
            (error) => {
                // 模型加载失败
                self.postMessage({
                    type: 'MODEL_ERROR',
                    id: id,
                    url: url,
                    error: error.message,
                    success: false
                });
            }
        );
    }
};

function initLoaders() {
    if (!THREE) return;

    loader = new THREE.GLTFLoader();
    const dracoLoader = new THREE.DRACOLoader();
    dracoLoader.setDecoderPath('https://unpkg.com/three@0.164.1/examples/jsm/libs/draco/');
    loader.setDRACOLoader(dracoLoader);
}