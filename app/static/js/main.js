// app/static/js/main.js (v3.2 - DOM Timing Fix)
import * as THREE from 'three';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { DRACOLoader } from 'three/addons/loaders/DRACOLoader.js';
import { KTX2Loader } from 'three/addons/loaders/KTX2Loader.js';
import { MeshoptDecoder } from 'three/addons/libs/meshopt_decoder.module.js';

// --- 将所有非DOM变量的初始化保留在顶层 ---
let scene, camera, renderer, controls, currentModel, modelUrls = [], yearMap = {}, yearList = [], summaryData = {}, summaryScience = {}, summaryCompare = {};
let activeLoadToken = 0;
const loader = new GLTFLoader();
const dracoLoader = new DRACOLoader();
dracoLoader.setDecoderPath('https://unpkg.com/three@0.164.1/examples/jsm/libs/draco/');
loader.setDRACOLoader(dracoLoader);

// --- 模型加载优化配置 ---
// 启用Draco几何压缩支持（更稳定）
try {
    if (typeof THREE.DRACOLoader !== 'undefined') {
        const dracoLoader = new THREE.DRACOLoader();
        dracoLoader.setDecoderPath('https://www.gstatic.com/draco/versioned/decoders/1.5.6/');
        loader.setDRACOLoader(dracoLoader);
        console.log('Draco loader initialized successfully');
    }
} catch (e) {
    console.log('Draco loader not available, using fallback');
}

// KTX2和Meshopt支持需要额外的库，这里暂时使用警告而不是错误
console.log('KTX2 loader not available, using fallback (requires additional setup)');
console.log('Meshopt decoder not available (requires additional setup)');

// --- 模型缓存和预加载优化 ---
const modelCache = new Map(); // 缓存已加载的模型（内存缓存，会在页面刷新时清除）

// 持久化缓存到localStorage
function saveModelCache() {
    const cacheData = {};
    modelCache.forEach((value, key) => {
        cacheData[key] = {
            // 现在只缓存模型数据，不再缓存相机位置
            // 注意：3D场景对象无法序列化存储
        };
    });
    localStorage.setItem('formind_model_cache', JSON.stringify(cacheData));
}

function loadModelCache() {
    const cacheData = localStorage.getItem('formind_model_cache');
    if (cacheData) {
        try {
            return JSON.parse(cacheData);
        } catch (e) {
            console.warn('加载缓存失败:', e);
            return {};
        }
    }
    return {};
}

function clearModelCache() {
    // 清除localStorage中的持久化缓存
    localStorage.removeItem('formind_model_cache');
    // 清除内存中的缓存
    modelCache.clear();
    console.log('模型缓存已清除');
    return true;
}

function checkAndCleanCache() {
    // 检查缓存大小，如果过大则自动清理
    const cacheData = localStorage.getItem('formind_model_cache');
    if (cacheData) {
        const sizeInBytes = new Blob([cacheData]).size;
        const sizeInMB = sizeInBytes / (1024 * 1024);

        // 如果缓存超过10MB，自动清理
        if (sizeInMB > 10) {
            console.warn(`缓存大小为 ${sizeInMB.toFixed(2)}MB，超过限制，自动清理`);
            clearModelCache();
            return true;
        }
    }
    return false;
}

// 页面卸载时清理内存缓存（但保留localStorage中的持久化缓存）
window.addEventListener('beforeunload', () => {
    modelCache.clear();
    console.log('页面卸载，已清理内存缓存');
});
const preloadQueue = []; // 预加载队列
let currentPreloadIndex = 0;
let isPreloading = false;
const PRELOAD_COUNT = 3; // 预加载前后3个模型
let modelWorker = null; // Web Worker for background loading

// --- CDN加速配置 ---
const CDN_CONFIG = {
    // 主要CDN列表（按优先级排序）
    primary: [
        'https://cdn.jsdelivr.net',
        'https://unpkg.com',
        'https://cdnjs.cloudflare.com'
    ],
    // 备用CDN
    fallback: [
        'https://fastly.jsdelivr.net',
        'https://rawcdn.githack.com'
    ],
    // 启用CDN加速
    enabled: true,
    // 缓存CDN测试结果
    cdnTestCache: new Map()
};

// --- CDN性能测试 ---
async function testCDNPerformance() {
    if (!CDN_CONFIG.enabled) return null;

    const testUrls = [
        ...CDN_CONFIG.primary,
        ...CDN_CONFIG.fallback
    ];

    const results = await Promise.allSettled(
        testUrls.map(async (cdn) => {
            // 使用一个简单的图片文件来测试CDN可用性，避免CORS和404问题
            const testUrl = `${cdn}/examples/jsm/libs/draco/draco_decoder.js`;
            const start = performance.now();
            try {
                const response = await fetch(testUrl, {
                    method: 'HEAD',
                    mode: 'no-cors' // 避免CORS问题
                });
                const end = performance.now();
                return {
                    cdn,
                    latency: end - start,
                    available: true // 如果没有抛出错误，认为是可用的
                };
            } catch (e) {
                return {
                    cdn,
                    latency: Infinity,
                    available: false
                };
            }
        })
    );

    // 选择最快的可用CDN
    const validResults = results
        .filter(r => r.status === 'fulfilled' && r.value.available)
        .map(r => r.value)
        .sort((a, b) => a.latency - b.latency);

    if (validResults.length > 0) {
        const bestCDN = validResults[0].cdn;
        console.log(`选择最佳CDN: ${bestCDN} (${validResults[0].latency.toFixed(1)}ms)`);
        return bestCDN;
    }

    return null;
}

// --- 获取CDN优化的URL ---
function getCDNOptimizedURL(url) {
    if (!CDN_CONFIG.enabled) return url;

    // 如果是Three.js相关资源，使用CDN
    if (url.includes('three@') || url.includes('unpkg.com') || url.includes('cdn.jsdelivr.net')) {
        // 保持原有的CDN URL
        return url;
    }

    // 对于模型文件，可以考虑使用CDN
    // 但这里保持原路径，因为模型文件通常在本地
    return url;
}

// --- 性能监控 ---
let performanceStats = {
    loadTimes: [],
    cacheHits: 0,
    cacheMisses: 0,
    averageLoadTime: 0
};

function updatePerformanceStats(loadTime, wasCached) {
    performanceStats.loadTimes.push(loadTime);
    if (wasCached) {
        performanceStats.cacheHits++;
    } else {
        performanceStats.cacheMisses++;
    }

    // 保持最近100次记录
    if (performanceStats.loadTimes.length > 100) {
        performanceStats.loadTimes.shift();
    }

    performanceStats.averageLoadTime =
        performanceStats.loadTimes.reduce((a, b) => a + b, 0) / performanceStats.loadTimes.length;

    // 更新性能显示（如果有的话）
    updatePerformanceDisplay();
}

function updatePerformanceDisplay() {
    // 注意：HTML性能面板已隐藏，仅用于开发者工具调试
    // 在控制台输出性能统计，供开发者查看
    console.log('性能统计:', {
        平均加载时间: `${performanceStats.averageLoadTime.toFixed(1)}ms`,
        缓存命中率: `${((performanceStats.cacheHits / (performanceStats.cacheHits + performanceStats.cacheMisses)) * 100).toFixed(1)}%`,
        缓存命中: performanceStats.cacheHits,
        缓存未命中: performanceStats.cacheMisses,
        当前LOD: getLODName(currentLOD),
        预加载状态: isPreloading ? '进行中' : '空闲'
    });

    // 仍然更新HTML元素数据（以备将来需要显示时使用）
    const panel = document.getElementById('performance-panel');
    if (panel) {
        // 更新数据但不显示面板
        const loadTimeEl = document.getElementById('perf-load-time');
        const cacheRateEl = document.getElementById('perf-cache-rate');
        const cacheHitsEl = document.getElementById('perf-cache-hits');
        const cacheMissesEl = document.getElementById('perf-cache-misses');
        const lodEl = document.getElementById('perf-current-lod');
        const preloadEl = document.getElementById('perf-preload-status');

        if (loadTimeEl) loadTimeEl.textContent = `平均加载: ${performanceStats.averageLoadTime.toFixed(1)}ms`;
        if (cacheRateEl) cacheRateEl.textContent = `缓存命中率: ${((performanceStats.cacheHits / (performanceStats.cacheHits + performanceStats.cacheMisses)) * 100).toFixed(1)}%`;
        if (cacheHitsEl) cacheHitsEl.textContent = `缓存命中: ${performanceStats.cacheHits}`;
        if (cacheMissesEl) cacheMissesEl.textContent = `缓存未命中: ${performanceStats.cacheMisses}`;
        if (lodEl) lodEl.textContent = `当前LOD: ${getLODName(currentLOD)}`;
        if (preloadEl) preloadEl.textContent = `预加载: ${isPreloading ? '进行中' : '空闲'}`;
    }
}

// --- LOD (Level of Detail) 系统 ---
const LOD_DISTANCES = {
    HIGH: 50,    // 近距离使用高质量模型
    MEDIUM: 100, // 中距离使用中等质量模型
    LOW: 200     // 远距离使用低质量模型
};
const LOD_QUALITIES = {
    HIGH: 'high',
    MEDIUM: 'medium',
    LOW: 'low'
};

function getLODName(lod) {
    switch (lod) {
        case LOD_QUALITIES.HIGH: return '高';
        case LOD_QUALITIES.MEDIUM: return '中';
        case LOD_QUALITIES.LOW: return '低';
        default: return '未知';
    }
}
let currentLOD = LOD_QUALITIES.HIGH;


// --- 初始化Web Worker ---
// 临时禁用Web Worker，因为当前的实现有根本性问题：
// 1. 无法通过postMessage传递Three.js库对象
// 2. 无法通过postMessage返回复杂的3D场景对象
// 3. Web Worker环境无法直接加载CDN资源
function initModelWorker() {
    // 直接禁用Web Worker，回到主线程加载
    console.log('Web Worker已禁用，使用主线程加载模型');
    modelWorker = null;
}

// --- 使用 DOMContentLoaded 事件来确保所有HTML元素都已加载 ---
document.addEventListener('DOMContentLoaded', async () => {
    // 初始化Web Worker
    initModelWorker();

    // 测试并选择最佳CDN
    try {
        const bestCDN = await testCDNPerformance();
        if (bestCDN) {
            CDN_CONFIG.bestCDN = bestCDN;
            console.log(`CDN优化已启用，使用: ${bestCDN}`);
        }
    } catch (e) {
        console.log('CDN测试失败，使用默认配置');
    }
    
    // --- 步骤1: 在DOM加载完毕后，安全地获取所有HTML元素 ---
    const sceneContainer = document.getElementById('scene-container');
    const loaderElement = document.querySelector('.loader');
    const statsFull = document.getElementById('stats-fullpage');
    // 顶部标签
    const tabModel = document.getElementById('tab-model');
    const tabStats = document.getElementById('tab-stats');
    const btnFirst = document.getElementById('btn-first');
    const btnPlay = document.getElementById('btn-play');
    const btnLast = document.getElementById('btn-last');
    const slider = document.getElementById('timeline-slider');
    const yearDisplay = document.getElementById('year-display');
    const btnClearCache = document.getElementById('btn-clear-cache');
    // Chart canvases
    const chartBA = document.getElementById('chart-basal-area');
    const chartBT = document.getElementById('chart-biomass');
    const chartCF = document.getElementById('chart-cflux');
    const toggleGrid = document.getElementById('toggle-grid');
    const toggleLegend = document.getElementById('toggle-legend');
    const btnExportCharts = document.getElementById('btn-export-charts');
    const sspButtons = document.querySelectorAll('#floating-ssp .ssp-button');
    const infoPanel = document.getElementById('info-panel');
        const compareGrid = document.getElementById('compare-grid');
        const chartsGrid = document.getElementById('charts-grid');
        const modeScenario = document.getElementById('mode-scenario');
        const modeCompare = document.getElementById('mode-compare');
        const cmpClimateSelect = document.getElementById('cmp-climate-select');
        let currentScenarioKey = 'ssp245';
    const body = document.querySelector('body[data-job-id]');
        // 状态：是否正在加载模型、是否处于统计分析页
        let isModelLoading = false;
        let isStatsActive = false;

    // 如果关键元素不存在，则提前退出，避免后续错误
    if (!sceneContainer || !body) {
        console.error("初始化错误：找不到 #scene-container 或 body[data-job-id]。");
        return;
    }

    // --- 步骤2: 将所有使用这些DOM元素的函数定义在同一个作用域内 ---

    function init3DScene() {
        // 检查并清理过大的缓存
        checkAndCleanCache();

        // 加载持久化缓存
        const savedCache = loadModelCache();
        console.log('持久化缓存数据:', savedCache);
        console.log('加载了', Object.keys(savedCache).length, '个模型的缓存数据');

        // 将清除缓存函数暴露到全局，方便在控制台调用
        window.clearModelCache = clearModelCache;
        window.saveModelCache = saveModelCache;
        window.loadModelCache = loadModelCache;

        scene = new THREE.Scene();
        // 采用透明背景，用容器CSS渐变作为背景
        // 画布始终占满容器高度，底部控制栏悬浮叠加
        const availableHeight = sceneContainer.clientHeight;
        camera = new THREE.PerspectiveCamera(60, sceneContainer.clientWidth / availableHeight, 0.1, 1000);
        camera.position.set(0, 0, 100); // 相机位于Z轴上，面向原点
        renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
        renderer.setSize(sceneContainer.clientWidth, availableHeight);
        renderer.setPixelRatio(window.devicePixelRatio);
        sceneContainer.appendChild(renderer.domElement);

        // 启用 KTX2 与 Meshopt（若可用）
        try {
            const ktx2 = new KTX2Loader();
            ktx2.setTranscoderPath('https://unpkg.com/three@0.164.1/examples/jsm/libs/basis/');
            ktx2.detectSupport(renderer);
            loader.setKTX2Loader(ktx2);
            if (MeshoptDecoder) loader.setMeshoptDecoder(MeshoptDecoder);
            console.log('KTX2/Meshopt 已配置');
        } catch (e) {
            console.warn('KTX2/Meshopt 配置失败或不支持', e);
        }

        const hemi = new THREE.HemisphereLight(0xffffff, 0x8899aa, 1.0); scene.add(hemi);
        const dirLight = new THREE.DirectionalLight(0xffffff, 1.8); dirLight.position.set(50, 50, 50); scene.add(dirLight);
        let grid = null; // 已取消网格

        controls = new OrbitControls(camera, renderer.domElement);
        controls.enableDamping = true;
        controls.target.set(0, 0, 0); // 始终围绕场景原点旋转
        // 固定视角旋转缩放范围
        controls.minDistance = 30;
        controls.maxDistance = 250;
        controls.minPolarAngle = Math.PI * 0.1; // 允许更低的视角
        controls.maxPolarAngle = Math.PI * 0.9; // 允许更高的视角
        controls.enablePan = true; // 允许平移以便查看不同角度
        controls.enableKeys = false;
        if (controls.mouseButtons) {
            controls.mouseButtons.RIGHT = THREE.MOUSE.ROTATE;
            controls.mouseButtons.MIDDLE = THREE.MOUSE.DOLLY;
        }
        
        function animate() {
            requestAnimationFrame(animate);
            controls.update();
            renderer.render(scene, camera);

            // LOD检测和切换
            updateLOD();
        }

        // --- LOD检测和切换 ---
        function updateLOD() {
            if (!currentModel || !controls) return;

            const distance = camera.position.distanceTo(controls.target);
            let newLOD = LOD_QUALITIES.HIGH;

            if (distance > LOD_DISTANCES.LOW) {
                newLOD = LOD_QUALITIES.LOW;
            } else if (distance > LOD_DISTANCES.MEDIUM) {
                newLOD = LOD_QUALITIES.MEDIUM;
            }

            if (newLOD !== currentLOD) {
                currentLOD = newLOD;
                console.log(`切换到${getLODName(currentLOD)}质量模型，距离: ${distance.toFixed(1)}`);
                updatePerformanceDisplay();

                // 可以在这里实现模型质量切换逻辑
                // 例如：切换到不同详细程度的模型版本
                // switchLODModel(currentLOD);
            }
        }

        animate();
        
        window.addEventListener('resize', () => {
            const h = sceneContainer.clientHeight;
            camera.aspect = sceneContainer.clientWidth / h;
            camera.updateProjectionMatrix();
            renderer.setSize(sceneContainer.clientWidth, h);
        });
        
        
    }

    // 将模型几何中心对齐到场景原点（供加载与缓存复用）
    function recenterViewOnCurrentModel() {
        if (!currentModel) return;
        const box = new THREE.Box3().setFromObject(currentModel);
        const center = new THREE.Vector3();
        box.getCenter(center);
        currentModel.position.sub(center);
        if (controls) controls.update();
    }

    // 使相机基于模型尺寸自动适配视野并保持模型居中可见
    function fitCameraToObject(object, offset = 1.2) {
        if (!camera || !controls || !object) return;
        const box = new THREE.Box3().setFromObject(object);
        const size = box.getSize(new THREE.Vector3());
        const maxDim = Math.max(size.x, size.y, size.z) || 1;
        const fov = camera.fov * (Math.PI / 180);
        const cameraZ = Math.abs((maxDim / 2) / Math.tan(fov / 2)) * offset;
        camera.position.set(0, 0, cameraZ);
        controls.target.set(0, 0, 0);
        controls.minDistance = Math.max(10, cameraZ * 0.3);
        controls.maxDistance = Math.max(50, cameraZ * 3);
        camera.updateProjectionMatrix();
        controls.update();
    }

    // --- 优化的模型加载函数（带缓存和预加载） ---
    function loadModel(url, instantSwitch = false) {
        const startTime = performance.now();
        const token = ++activeLoadToken;
        if (currentModel) scene.remove(currentModel);
        // 标记正在加载；仅在模型预览页显示加载圆圈
        isModelLoading = true;
        if (loaderElement) {
            loaderElement.style.display = isStatsActive ? 'none' : 'block';
        }

        // 优先从缓存中获取
        if (modelCache.has(url)) {
            const cachedModel = modelCache.get(url);
            if (token !== activeLoadToken) return;
            currentModel = cachedModel.scene.clone();
            scene.add(currentModel);

            // 确保模型居中到场景原点并适配相机视野
            recenterViewOnCurrentModel();
            fitCameraToObject(currentModel);

            isModelLoading = false;
            if (loaderElement) loaderElement.style.display = 'none';

            // 更新性能统计
            const loadTime = performance.now() - startTime;
            updatePerformanceStats(loadTime, true);

            // 触发预加载
            if (!instantSwitch) {
                setTimeout(() => preloadNearbyModels(), 100);
            }
            return;
        }

        // 异步加载模型
        loader.load(url, (gltf) => {
            if (token !== activeLoadToken) return;
            currentModel = gltf.scene;

            // 确保模型居中到场景原点并适配相机视野
            recenterViewOnCurrentModel();
            fitCameraToObject(currentModel);

            // 缓存模型（不保存相机位置，因为现在相机和target是固定的）
            modelCache.set(url, {
                scene: currentModel.clone()
            });

            // 保存到持久化缓存
            saveModelCache();

            scene.add(currentModel);

            isModelLoading = false;
            if (loaderElement) loaderElement.style.display = 'none';

            // 更新性能统计
            const loadTime = performance.now() - startTime;
            updatePerformanceStats(loadTime, false);

            // 触发预加载
            if (!instantSwitch) {
                setTimeout(() => preloadNearbyModels(), 100);
            }
        }, (progress) => {
            if (token !== activeLoadToken) return;
            // 显示加载进度（仅在模型预览页）
            isModelLoading = true;
            if (loaderElement) {
                loaderElement.innerHTML = '';
                loaderElement.style.display = isStatsActive ? 'none' : 'block';
            }
        }, (error) => {
            if (token !== activeLoadToken) return;
            console.error('加载模型时出错:', error);
            isModelLoading = false;
            if (loaderElement) loaderElement.style.display = 'none';
        });
    }

    // --- 预加载邻近模型（主线程预加载） ---
    function preloadNearbyModels() {
        console.log(`preloadNearbyModels called. isPreloading: ${isPreloading}, modelUrls length: ${modelUrls.length}`);

        if (isPreloading || modelUrls.length === 0) {
            console.log('预加载条件不满足，跳过');
            return;
        }

        const currentIndex = parseInt(slider?.value || 0);
        const modelsToPreload = [];

        // 添加前后PRELOAD_COUNT个模型
        for (let i = Math.max(0, currentIndex - PRELOAD_COUNT);
             i <= Math.min(modelUrls.length - 1, currentIndex + PRELOAD_COUNT);
             i++) {
            if (i !== currentIndex && !modelCache.has(modelUrls[i])) {
                modelsToPreload.push({ url: modelUrls[i], index: i });
            }
        }

        console.log(`找到 ${modelsToPreload.length} 个需要预加载的模型，当前索引: ${currentIndex}`);

        if (modelsToPreload.length === 0) {
            console.log('没有需要预加载的模型');
            return;
        }

        isPreloading = true;
        let completed = 0;
        const MAX_CONCURRENT = 3;
        console.log(`开始预加载 ${modelsToPreload.length} 个模型...`);
        updatePerformanceDisplay();

        // 预加载尝试函数，带重试机制
        function attemptPreload(url, index, retryCount) {
            const MAX_RETRIES = 2; // 最大重试次数

            // 创建预加载专用的loader实例，避免干扰主加载进度显示
            const preloadLoader = new GLTFLoader();
            const preloadDracoLoader = new DRACOLoader();
            preloadDracoLoader.setDecoderPath('https://unpkg.com/three@0.164.1/examples/jsm/libs/draco/');
            preloadLoader.setDRACOLoader(preloadDracoLoader);

                preloadLoader.load(url, (gltf) => {
                    // 预加载成功，缓存模型（确保模型居中）
                    const model = gltf.scene;
                    const box = new THREE.Box3().setFromObject(model);
                    const center = new THREE.Vector3();
                    box.getCenter(center);
                    model.position.sub(center);

                    const cacheData = { scene: model.clone() };
                    modelCache.set(url, cacheData);
                    completed++;
                    console.log(`✅ 预加载成功: ${url}, 已加载: ${completed}/${modelsToPreload.length}`);

                if (completed === modelsToPreload.length) {
                    isPreloading = false;
                    console.log(`🎉 预加载完成! 总共预加载了 ${completed} 个模型，缓存中现在有 ${modelCache.size} 个模型`);
                    console.log('缓存中的模型:', Array.from(modelCache.keys()));
                    updatePerformanceDisplay();
                }
            }, (progress) => {
                // 预加载时不显示进度，避免干扰主界面
                if (progress.total && progress.total > 0) {
                    const percent = ((progress.loaded / progress.total) * 100).toFixed(1);
                    console.log(`预加载进度: ${percent}%`);
                } else {
                    console.log(`预加载进度: ${progress.loaded} bytes`);
                }
            }, (error) => {
                console.error(`❌ 预加载模型失败: ${url} (尝试 ${retryCount + 1}/${MAX_RETRIES + 1})`, {
                    error: error.message,
                    status: error.status,
                    statusText: error.statusText,
                    loaded: error.loaded || 0,
                    total: error.total || 0
                });

                // 如果还有重试次数，等待后重试
                if (retryCount < MAX_RETRIES) {
                    const delay = Math.pow(2, retryCount) * 1000; // 指数退避：1s, 2s, 4s
                    console.log(`⏳ ${delay/1000}秒后重试 ${url} (尝试 ${retryCount + 2}/${MAX_RETRIES + 1})`);
                    setTimeout(() => attemptPreload(url, index, retryCount + 1), delay);
                    return;
                }

                // 检查是否是网络错误
                if (error.message.includes('404') || error.message.includes('Not Found')) {
                    console.warn(`模型文件不存在: ${url}，这可能是正常的`);
                } else if (error.message.includes('Network') || error.message.includes('fetch')) {
                    console.warn(`网络错误，可能是服务器限制或网络问题`);
                } else {
                    console.warn(`其他错误类型:`, error);
                }

                completed++;
                if (completed === modelsToPreload.length) {
                    isPreloading = false;
                    const successCount = modelsToPreload.length - (modelsToPreload.length - completed);
                    console.log(`预加载完成! 成功: ${successCount}/${modelsToPreload.length} 个模型`);
                    updatePerformanceDisplay();
                }
            });
        }

        // 开始预加载循环
        modelsToPreload.forEach(({ url, index }, i) => {
            const batch = Math.floor(i / MAX_CONCURRENT);
            setTimeout(() => {
                // 检查模型是否已经被其他请求加载了
                if (modelCache.has(url)) {
                    completed++;
                    if (completed === modelsToPreload.length) {
                        isPreloading = false;
                        console.log(`预加载完成 (所有模型已在缓存中)`);
                        updatePerformanceDisplay();
                    }
                    return;
                }

                // 调用预加载尝试函数
                attemptPreload(url, index, 0);

            }, batch * 300);
        });
    }
    function setupEventListeners() {
        // 底部控件容器根据当前激活的标签页显示/隐藏
        // 顶部两段切换：模型预览、统计分析
        const activateTab = (tab) => {
            [tabModel, tabStats].forEach(b => b && b.classList.remove('active'));
            if (tab) tab.classList.add('active');
            if (tab === tabModel) {
                isStatsActive = false;
                statsFull.style.display = 'none';
                statsFull.classList.remove('active');
                // 显示3D画布与信息面板
                renderer?.domElement && (renderer.domElement.style.display = 'block');
                if (infoPanel) infoPanel.style.display = 'block';
                // 模型预览以填充画布为主，避免页面滚动干扰
                if (document && document.body) {
                    document.body.style.overflowY = 'hidden';
                }
                // 仅在仍在加载时显示加载圆圈
                if (loaderElement) {
                    loaderElement.style.display = isModelLoading ? 'block' : 'none';
                }
            } else if (tab === tabStats) {
                isStatsActive = true;
                // 以网格布局显示统计页，配合左侧非悬浮情景栏
                statsFull.style.display = 'grid';
                statsFull.classList.add('active');
                // 隐藏3D画布和信息面板，避免占位
                renderer?.domElement && (renderer.domElement.style.display = 'none');
                if (infoPanel) infoPanel.style.display = 'none';
                // 隐藏加载圆圈
                if (loaderElement) loaderElement.style.display = 'none';
                // 允许页面滚动以查看所有图表
                if (document && document.body) {
                    document.body.style.overflowY = 'auto';
                }
                // 注意：底部控制栏和性能面板现在由CSS自动隐藏
            }
        };
        if (tabModel) tabModel.addEventListener('click', () => activateTab(tabModel));
        if (tabStats) tabStats.addEventListener('click', () => activateTab(tabStats));

        // 添加调试按钮用于手动触发预加载
        if (typeof window !== 'undefined') {
            window.debugPreload = () => {
                console.log('Manual preload trigger');
                console.log('modelUrls:', modelUrls);
                console.log('modelUrls length:', modelUrls?.length);
                console.log('isPreloading:', isPreloading);
                preloadNearbyModels();
            };
        }
        
        if(sspButtons && sspButtons.length) {
            sspButtons.forEach(button => {
                button.addEventListener('click', (e) => {
                    e.preventDefault();
                    e.stopPropagation();
                    const targetSsp = button.dataset?.ssp;
                    if (!targetSsp) return;
                    sspButtons.forEach(btn => btn.classList.remove('active'));
                    button.classList.add('active');
                    currentScenarioKey = targetSsp;
                    renderCharts(targetSsp);
                    // 同步更新复合气候图
                    try { renderClimateComposite(currentScenarioKey); } catch(e) { }
                });
            });
        }

        // 模式切换：按情景 vs 对比
        function applyMode() {
            const isCompare = modeCompare?.checked;
            if (compareGrid && chartsGrid) {
                compareGrid.style.display = isCompare ? 'block' : 'none';
                chartsGrid.style.display = isCompare ? 'none' : 'grid';
                if (isCompare) {
                    renderCompare();
                    if (Object.keys(summaryCompare).length > 0) {
                        renderCompareScience(summaryScience, summaryCompare);
                    }
                } else {
                    const active = document.querySelector('#floating-ssp .ssp-button.active')?.dataset?.ssp || 'ssp245';
                    renderCharts(active);
                }
            }
        }
        modeScenario?.addEventListener('change', applyMode);
        modeCompare?.addEventListener('change', applyMode);
        cmpClimateSelect?.addEventListener('change', () => renderClimateTS(cmpClimateSelect.value));
        // 初始情景写入
        currentScenarioKey = document.querySelector('#floating-ssp .ssp-button.active')?.dataset?.ssp || 'ssp245';
        
        let playTimer = null;
        const setIndex = (idx, instantSwitch = false) => {
            const clamped = Math.max(0, Math.min(modelUrls.length - 1, idx));
            slider.value = clamped;
            const year = yearList[clamped];
            yearDisplay.textContent = `年份: ${formatYearDisplay(year)}`;
            const url = modelUrls[clamped];
            if (url) loadModel(url, instantSwitch);
        };

        if(slider && yearDisplay) {
            slider.addEventListener('input', () => {
                setIndex(parseInt(slider.value, 10) || 0);
            });
        }

        if (btnFirst) btnFirst.addEventListener('click', () => {
            const prev = (parseInt(slider.value, 10) || 0) - 1;
            setIndex(prev);
        });
        if (btnLast) btnLast.addEventListener('click', () => {
            const next = (parseInt(slider.value, 10) || 0) + 1;
            setIndex(next);
        });
        if (btnPlay) btnPlay.addEventListener('click', () => {
            if (playTimer) { clearInterval(playTimer); playTimer = null; btnPlay.textContent = '▶ 播放'; return; }
            btnPlay.textContent = '⏸ 暂停';
            playTimer = setInterval(() => {
                let next = (parseInt(slider.value, 10) || 0) + 1;
                if (next >= modelUrls.length) next = 0; // 循环
                setIndex(next, true); // 播放时使用instantSwitch避免重复预加载
            }, 1200);
        });

        // 取消网格控制：固定隐藏网格
        // 网格已移除
        // 注释掉光照调整功能
        // if (lightIntensity) lightIntensity.addEventListener('input', () => { dirLight.intensity = parseFloat(lightIntensity.value) || 1.0; });
    }

    function extractYearFromUrl(url) {
        // 兼容 xxx_003.glb / glb003.glb / ..._2014.glb
        const m = url.match(/(\d+)(?=\.glb$)/);
        const num = m ? parseInt(m[1], 10) : NaN;
        if (isNaN(num)) return NaN;
        if (num >= 1900 && num <= 2200) return num;
        return 2014 + num;
    }

    function formatYearDisplay(year) {
        // 显示直接计算的年份数值
        return year.toString();
    }

    function sortModelsByYear(urls) {
        return urls.slice().sort((a, b) => (extractYearFromUrl(a) || 0) - (extractYearFromUrl(b) || 0));
    }

    async function onDataLoaded(data) {
        console.log('=== onDataLoaded 开始 ===');
        console.log('完整数据结构:', data);
        console.log('data.models 类型:', typeof data.models);
        console.log('data.models 内容:', data.models);

        console.log('onDataLoaded called with data:', {
            hasSummary: !!data.summary,
            hasModels: !!data.models,
            modelsLength: data.models?.length || 0,
            hasSummaryScience: !!data.summary_science,
            dataKeys: Object.keys(data || {})
        });

        summaryData = data.summary;
        summaryScience = data.summary_science || {};
        summaryCompare = data.summary_compare || {};
        modelUrls = sortModelsByYear(data.models || []);

        console.log('处理后的modelUrls:', {
            length: modelUrls.length,
            firstFew: modelUrls.slice(0, 3),
            lastFew: modelUrls.slice(-3)
        });

        if (!modelUrls || modelUrls.length === 0) {
            document.getElementById('info-panel').innerHTML = '<h2>错误</h2><p>模拟完成，但未生成任何模型文件。</p>';
            if (loaderElement) loaderElement.style.display = 'none';
            return;
        }

        // 优先从模型文件名提取年份，保证与可视模型对应
        yearList = modelUrls.map(extractYearFromUrl).filter(y => !isNaN(y));
        yearList.forEach((year, index) => { yearMap[index] = year; });

        console.log('Year processing:', {
            yearListLength: yearList.length,
            yearMapSize: Object.keys(yearMap).length,
            firstYear: yearList[0],
            modelUrlsSample: modelUrls.slice(0, 2)
        });

        // 测试缓存机制是否正常
        console.log('当前模型缓存大小:', modelCache.size);
        console.log('当前模型缓存keys:', Array.from(modelCache.keys()));

        slider.min = 0;
        slider.max = modelUrls.length > 0 ? modelUrls.length - 1 : 0;
        slider.value = 0;
        slider.disabled = modelUrls.length <= 1;

        // 默认渲染 ssp245 统计
        renderCharts('ssp245');

        async function probeModelExists(url, timeoutMs = 2000) {
            try {
                const ctrl = new AbortController();
                const t = setTimeout(() => ctrl.abort(), timeoutMs);
                const res = await fetch(url, { method: 'HEAD', signal: ctrl.signal });
                clearTimeout(t);
                if (res.ok) return true;
            } catch (e) {}
            // Fallback: 206 Range 探测
            try {
                const ctrl2 = new AbortController();
                const t2 = setTimeout(() => ctrl2.abort(), timeoutMs);
                const res2 = await fetch(url, { method: 'GET', headers: { 'Range': 'bytes=0-0' }, signal: ctrl2.signal });
                clearTimeout(t2);
                return res2.ok && (res2.status === 206 || res2.status === 200);
            } catch (e2) {
                return false;
            }
        }

        async function pickFirstAvailable(urls, maxScan = 12) {
            for (let i = 0; i < Math.min(urls.length, maxScan); i++) {
                const ok = await probeModelExists(urls[i]);
                if (ok) return i;
            }
            return 0;
        }

        const initialIndex = await pickFirstAvailable(modelUrls, 12);
        const initialYear = yearMap[initialIndex];
        slider.value = initialIndex;
        if (initialYear !== undefined) {
            yearDisplay.textContent = `年份: ${formatYearDisplay(initialYear)}`;
        }
        if (modelUrls[initialIndex]) {
            console.log('Starting initial model load for:', modelUrls[initialIndex]);
            loadModel(modelUrls[initialIndex], true);
            setTimeout(() => {
                console.log('Triggering preload after initial load');
                preloadNearbyModels();
            }, 500);
        }

        // 如果对比模式打开，预渲染科研对比
        if (Object.keys(summaryCompare).length > 0) {
            renderCompareScience(summaryScience, summaryCompare);
        }

        // 初始化气候图表（带错误处理）
        console.log('初始化气候图表...');
        console.log('summaryScience 数据结构:', {
            hasData: !!summaryScience,
            scenarios: Object.keys(summaryScience || {}),
            sampleYear: summaryScience?.ssp245?.yearly ? Object.keys(summaryScience.ssp245.yearly)[0] : '无年份数据',
            sampleClimate: summaryScience?.ssp245?.yearly ? summaryScience.ssp245.yearly[Object.keys(summaryScience.ssp245.yearly)[0]]?.climate : '无气候数据'
        });

        // 初始化气候时间序列（默认气温）
        renderClimateTS('temperature_mean_C');

        // 初始化气候复合图
        renderClimateComposite(currentScenarioKey);

        // 默认进入模型预览页
        tabModel?.click?.();

        // 初始化性能显示
        updatePerformanceDisplay();
    }

    function normalizeSummary(summary) {
        // 兼容两种结构：{ssp245: {year: {...}}} 或 {year: {...}}
        if (!summary) return {};
        const keys = Object.keys(summary);
        if (keys.length && /^ssp/.test(keys[0])) return summary; // 已按情景分组
        return { ssp245: summary };
    }

    function sumObjectValues(obj) {
        return Object.values(obj || {}).reduce((a, b) => a + (Number(b) || 0), 0);
    }

    function renderCharts(sspKey) {
        const byScenario = normalizeSummary(summaryData);
        const data = byScenario[sspKey] || {};
        // 年度轴：只选择“有模型输出”的年份（避免被气候100年扩展为全0）
        const yearsAll = Object.keys(data).map(y => parseInt(y)).sort((a,b)=>a-b);
        const hasSimAt = (y) => {
            const d = data[y] || {};
            const hasB = d?.biomass && (d.biomass.total_biomass != null || Object.keys(d.biomass.biomass_pft||{}).length>0);
            const hasC = d?.carbon_flux && (d.carbon_flux.gpp != null || d.carbon_flux.nee != null);
            return !!(hasB || hasC);
        };
        const years = yearsAll.filter(hasSimAt);
        if (!years.length) return;

        // 格式化年份标签用于图表显示
        const formatYearLabels = (yearArray) => yearArray.map(year => formatYearDisplay(year));

        // 基于每年对象解析 PFT 列
        const mkStackDataset = (label, color, perYearObjs, seriesKeys) => ({
            label, data: years.map(y => seriesKeys.map(k => (perYearObjs[y]?.[k]) ?? 0)), borderColor: color, backgroundColor: color+'55', fill: true
        });

        // 构造 PFT 键数组（按键名排序确保稳定）
        const pftKeysBA = Object.keys(data[years[0]]?.biomass?.basal_area_pft || {}).sort();
        const pftKeysBT = Object.keys(data[years[0]]?.biomass?.biomass_pft || {}).sort();
        const pftKeysN  = Object.keys(data[years[0]]?.biomass?.stems_pft || {}).sort();

        const baTotals = years.map(y => Number(data[y]?.biomass?.basal_area_total) || sumObjectValues(data[y]?.biomass?.basal_area_pft));
        const btTotals = years.map(y => Number(data[y]?.biomass?.total_biomass)  || sumObjectValues(data[y]?.biomass?.biomass_pft));

        const gpp = years.map(y => (data[y]?.carbon_flux?.gpp) ?? 0);
        const nee = years.map(y => (data[y]?.carbon_flux?.nee) ?? 0);
        const bc  = years.map(y => (data[y]?.carbon_flux?.biomass_carbon) ?? 0);

        // 销毁旧图
        chartBA?._chart?.destroy?.(); chartBT?._chart?.destroy?.(); chartCF?._chart?.destroy?.();
        const ctxBA = chartBA?.getContext('2d');
        const ctxBT = chartBT?.getContext('2d');
        const elN = document.getElementById('chart-stems');
        const ctxN  = elN?.getContext('2d');
        const ctxCF = chartCF?.getContext('2d');
        const ctxNPP = document.getElementById('chart-npp-cue')?.getContext('2d');
        const ctxGPPpft = document.getElementById('chart-gpp-pft')?.getContext('2d');
        const ctxNPPpft = document.getElementById('chart-npp-pft')?.getContext('2d');
        const ctxCumNEP = document.getElementById('chart-cum-nep')?.getContext('2d');
        const ctxDeltaPools = document.getElementById('chart-delta-pools')?.getContext('2d');
        const ctxTau = document.getElementById('chart-tau')?.getContext('2d');
        const ctxDia = document.getElementById('chart-dia')?.getContext('2d');
        const ctxMonthlyGPP = document.getElementById('chart-monthly-gpp')?.getContext('2d');

        // BA：堆叠PFT + 总量线
        if (ctxBA) {
            const datasets = pftKeysBA.map((k, i) => ({
                label: k.replace('BasalAreaPerPFT_', 'PFT '),
                data: years.map(y => (data[y]?.biomass?.basal_area_pft?.[k]) ?? 0),
                borderColor: `hsl(${(i*60)%360} 70% 45%)`,
                backgroundColor: `hsl(${(i*60)%360} 70% 45% / 0.35)`,
                fill: true
            }));
            datasets.push({ label: 'Total BA', data: baTotals, borderColor: '#111827', backgroundColor: '#11182755', type: 'line', fill: false, borderWidth: 2 });
            chartBA._chart = new Chart(ctxBA, { type: 'bar', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false, plugins: { legend: { display: toggleLegend ? toggleLegend.checked : true } }, scales: { x: { stacked: true, title: { display: true, text: '年份' } }, y: { stacked: true, beginAtZero: true } } } });
        }

        // Biomass：堆叠PFT + 总量线
        if (ctxBT) {
            const datasets = pftKeysBT.map((k, i) => ({
                label: k.replace('BiomassPerPFT_', 'PFT '),
                data: years.map(y => (data[y]?.biomass?.biomass_pft?.[k]) ?? 0),
                borderColor: `hsl(${(i*60)%360} 70% 45%)`,
                backgroundColor: `hsl(${(i*60)%360} 70% 45% / 0.35)`,
                fill: true
            }));
            datasets.push({ label: 'Total Biomass', data: btTotals, borderColor: '#065f46', backgroundColor: '#065f4655', type: 'line', fill: false, borderWidth: 2 });
            chartBT._chart = new Chart(ctxBT, { type: 'bar', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false, plugins: { legend: { display: toggleLegend ? toggleLegend.checked : true } }, scales: { x: { stacked: true, title: { display: true, text: '年份' } }, y: { stacked: true, beginAtZero: true } } } });
        }

        // Stems：堆叠PFT
        if (ctxN) {
            const datasets = pftKeysN.map((k, i) => ({
                label: k.replace('NumberPerPFT_', 'PFT '),
                data: years.map(y => (data[y]?.biomass?.stems_pft?.[k]) ?? 0),
                borderColor: `hsl(${(i*60)%360} 70% 45%)`,
                backgroundColor: `hsl(${(i*60)%360} 70% 45% / 0.35)`,
                fill: true
            }));
            elN._chart?.destroy?.();
            elN._chart = new Chart(ctxN, { type: 'bar', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false, plugins: { legend: { display: toggleLegend ? toggleLegend.checked : true } }, scales: { x: { stacked: true, title: { display: true, text: '年份' } }, y: { stacked: true, beginAtZero: true } } } });
        }

        // Carbon flux：NEE（负值为碳汇）、GPP、Biomass C
        if (ctxCF) {
            chartCF._chart = new Chart(ctxCF, {
                type: 'line', data: { labels: formatYearLabels(years), datasets: [
                    { label: 'NEE (碳汇为负)', data: nee, borderColor: '#ef4444', backgroundColor: '#ef444433', tension: 0.25, fill: true },
                    { label: 'GPP', data: gpp, borderColor: '#f59e0b', backgroundColor: '#f59e0b33', tension: 0.25, fill: true },
                    { label: 'Biomass C', data: bc, borderColor: '#8b5cf6', backgroundColor: '#8b5cf633', tension: 0.25, fill: true }
                ] },
                options: { responsive: true, maintainAspectRatio: false, plugins: { legend: { display: toggleLegend ? toggleLegend.checked : true } }, scales: { x: { title: { display: true, text: '年份' } }, y: { beginAtZero: false } } }
            });
        }

        // Derived: NPP & CUE
        if (ctxNPP) {
            const npp = years.map(y => (data[y]?.derived?.npp_approx) ?? 0);
            const cue = years.map(y => (data[y]?.derived?.cue) ?? 0);
            const el = document.getElementById('chart-npp-cue');
            el._chart?.destroy?.();
            el._chart = new Chart(ctxNPP, {
                type: 'line', data: { labels: formatYearLabels(years), datasets: [
                    { label: 'NPP (近似)', data: npp, borderColor: '#10b981', backgroundColor: '#10b98133', tension: 0.25, fill: true },
                    { label: 'CUE = NPP/GPP', data: cue, borderColor: '#3b82f6', backgroundColor: '#3b82f633', tension: 0.25, yAxisID: 'y1', fill: false },
                ] }, options: { responsive: true, maintainAspectRatio: false, scales: { x: { title: { display: true, text: '年份' } }, y: { beginAtZero: false, position: 'left' }, y1: { beginAtZero: true, position: 'right', suggestedMax: 1 } } }
            });
        }

        // GPP/NPP 按 PFT 分摊（近似：按 BiomassPerPFT 的份额）
        const shareKeys = Object.keys(data[years[0]]?.biomass?.biomass_pft || {}).sort();
        const sharesByYear = years.map(y => {
            const m = data[y]?.biomass?.biomass_pft || {};
            const tot = Object.values(m).reduce((a,b)=>a + (Number(b)||0), 0) || 1;
            const shares = shareKeys.map(k => (Number(m[k]||0))/tot);
            return shares;
        });
        if (ctxGPPpft) {
            const gppArr = years.map(y => Number(data[y]?.carbon_flux?.gpp) || 0);
            const datasets = shareKeys.map((k,i)=>({
                label: k.replace('BiomassPerPFT_','PFT '),
                data: years.map((_,idx)=> gppArr[idx] * (sharesByYear[idx][i] || 0)),
                backgroundColor: `hsl(${(i*60)%360} 70% 45% / 0.35)`,
                borderColor: `hsl(${(i*60)%360} 70% 45%)`, fill: true
            }));
            const el = document.getElementById('chart-gpp-pft');
            el._chart?.destroy?.();
            el._chart = new Chart(ctxGPPpft, { type: 'bar', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false, scales: { x: { stacked: true }, y: { stacked: true, beginAtZero: true } } } });
        }
        if (ctxNPPpft) {
            const nppArr = years.map(y => Number(data[y]?.derived?.npp_approx) || 0);
            const datasets = shareKeys.map((k,i)=>({
                label: k.replace('BiomassPerPFT_','PFT '),
                data: years.map((_,idx)=> nppArr[idx] * (sharesByYear[idx][i] || 0)),
                backgroundColor: `hsl(${(i*60)%360} 70% 45% / 0.35)`,
                borderColor: `hsl(${(i*60)%360} 70% 45%)`, fill: true
            }));
            const el = document.getElementById('chart-npp-pft');
            el._chart?.destroy?.();
            el._chart = new Chart(ctxNPPpft, { type: 'bar', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false, scales: { x: { stacked: true }, y: { stacked: true, beginAtZero: true } } } });
        }

        // Derived: 累计 NEP
        if (ctxCumNEP) {
            const cum = years.map(y => (data[y]?.derived?.cumulative_nep) ?? 0);
            const el = document.getElementById('chart-cum-nep');
            el._chart?.destroy?.();
            el._chart = new Chart(ctxCumNEP, {
                type: 'line', data: { labels: formatYearLabels(years), datasets: [ { label: '累计NEP', data: cum, borderColor: '#111827', backgroundColor: '#11182722', tension: 0.25, fill: true } ] },
                options: { responsive: true, maintainAspectRatio: false }
            });
        }

        // Δ 碳库（每年）
        if (ctxDeltaPools) {
            const keys = ['biomass','deadwood','soil_fast','soil_slow'];
            const colors = ['#065f46','#92400e','#2563eb','#6b7280'];
            const datasets = keys.map((k,i)=>({ label: k, data: years.map(y => (data[y]?.derived?.delta_carbon_pools?.[k]) ?? 0), backgroundColor: colors[i] }));
            const el = document.getElementById('chart-delta-pools');
            el._chart?.destroy?.();
            el._chart = new Chart(ctxDeltaPools, { type: 'bar', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false, scales: { x: { stacked: true }, y: { stacked: true, beginAtZero: true } } } });
        }

        // 生物量周转时间 tau
        if (ctxTau) {
            const tau = years.map(y => (data[y]?.derived?.tau) ?? null);
            const el = document.getElementById('chart-tau');
            el._chart?.destroy?.();
            el._chart = new Chart(ctxTau, { type: 'line', data: { labels: formatYearLabels(years), datasets: [ { label: 'tau (年)', data: tau, borderColor: '#7c3aed', backgroundColor: '#7c3aed22', tension: 0.25, fill: true } ] }, options: { responsive: true, maintainAspectRatio: false } });
        }

        // 径级分布（起始 vs 结束年）
        if (ctxDia) {
            const start = years[0], end = years[years.length-1];
            const d0 = data[start]?.structure?.diameter_distribution || {};
            const d1 = data[end]?.structure?.diameter_distribution || {};
            const bins = Array.from(new Set([...Object.keys(d0), ...Object.keys(d1)])).sort((a,b)=>parseFloat(a)-parseFloat(b));
            const v0 = bins.map(b => Number(d0[b] || 0));
            const v1 = bins.map(b => Number(d1[b] || 0));
            const el = document.getElementById('chart-dia');
            el._chart?.destroy?.();
            el._chart = new Chart(ctxDia, { type: 'bar', data: { labels: bins, datasets: [
                { label: `起始年(${start})`, data: v0, backgroundColor: '#60a5fa' },
                { label: `结束年(${end})`, data: v1, backgroundColor: '#f59e0b' }
            ] }, options: { responsive: true, maintainAspectRatio: false, scales: { x: { stacked: false, title: { display: true, text: '径级(类中心或下界)' } }, y: { beginAtZero: true } } } });
        }

        // 月度GPP分布模拟（年内动态）
        if (ctxMonthlyGPP) {
            // 基于气候数据和GPP总量模拟月度分布
            // 假设GPP在生长季（4-10月）较高，其他月份较低
            const months = ['1月', '2月', '3月', '4月', '5月', '6月', '7月', '8月', '9月', '10月', '11月', '12月'];

            // 计算年度GPP总量
            const annualGPP = gpp.reduce((sum, val) => sum + val, 0);
            const avgMonthlyGPP = annualGPP / 12;

            // 基于气候数据的月度GPP分布模式
            // 这里使用一个简化的模型：生长季GPP较高，非生长季较低
            const monthlyDistribution = [0.05, 0.05, 0.08, 0.12, 0.15, 0.18, 0.20, 0.18, 0.15, 0.12, 0.08, 0.05];
            const monthlyGPP = monthlyDistribution.map(ratio => avgMonthlyGPP * ratio);

            const el = document.getElementById('chart-monthly-gpp');
            el._chart?.destroy?.();
            el._chart = new Chart(ctxMonthlyGPP, {
                type: 'line',
                data: {
                    labels: months,
                    datasets: [{
                        label: '月度GPP模拟 (年内分布)',
                        data: monthlyGPP,
                        borderColor: '#10b981',
                        backgroundColor: '#10b98133',
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        x: { title: { display: true, text: '月份' } },
                        y: {
                            beginAtZero: true,
                            title: { display: true, text: 'GPP (t_C/ha/month)' },
                            ticks: {
                                callback: function(value) {
                                    return value.toFixed(2);
                                }
                            }
                        }
                    },
                    plugins: {
                        legend: { display: true },
                        tooltip: {
                            callbacks: {
                                label: function(context) {
                                    return `GPP: ${context.parsed.y.toFixed(3)} t_C/ha`;
                                }
                            }
                        }
                    }
                }
            });
        }

        if (btnExportCharts) {
            btnExportCharts.onclick = () => {
                const canvases = ['chart-basal-area','chart-biomass','chart-stems','chart-cflux','chart-gpp-pft','chart-npp-cue','chart-npp-pft','chart-cum-nep','chart-delta-pools','chart-tau','chart-dia','chart-monthly-gpp','cmp-biomass','cmp-delta','cmp-nee','cmp-gpp','cmp-npp','cmp-pft-bars','cmp-dia','cmp-cum-nep','cmp-end-bars']
                    .map(id => document.getElementById(id))
                    .filter(c => c && c._chart);
                canvases.forEach((c, i) => {
                    const a = document.createElement('a');
                    a.href = c._chart.toBase64Image();
                    a.download = `${sspKey}_chart_${i+1}.png`;
                    a.click();
                });
            };
        }

        if (btnClearCache) {
            btnClearCache.onclick = () => {
                if (confirm('确定要清除所有模型缓存吗？这将删除已保存的相机设置和预加载数据。')) {
                    clearModelCache();
                    // 显示成功提示
                    const originalText = btnClearCache.innerHTML;
                    btnClearCache.innerHTML = '✅ 已清除';
                    setTimeout(() => {
                        btnClearCache.innerHTML = originalText;
                    }, 2000);
                }
            };
        }
    }

    function renderCompare() {
        const byScenario = normalizeSummary(summaryData);
        const scenarios = ['ssp126','ssp245','ssp585'].filter(s => byScenario[s]);
        if (!scenarios.length) return;

        // 年度轴：取基准情景的“有模型输出”的年份，避免因气候100年导致大量0
        const base = scenarios.includes('ssp126') ? 'ssp126' : scenarios[0];
        const yearsBaseAll = Object.keys(byScenario[base] || {}).map(y=>parseInt(y)).sort((a,b)=>a-b);
        const hasSim = (s,y) => {
            const d = (byScenario[s]||{})[y] || {};
            const b = d?.biomass && (d.biomass.total_biomass != null || Object.keys(d.biomass.biomass_pft||{}).length>0);
            const c = d?.carbon_flux && (d.carbon_flux.gpp != null || d.carbon_flux.nee != null);
            return !!(b || c);
        };
        const years = yearsBaseAll.filter(y => scenarios.every(s => hasSim(s,y)));

        // 格式化年份标签用于图表显示
        const formatYearLabels = (yearArray) => yearArray.map(year => formatYearDisplay(year));

        const series = (picker) => scenarios.map((s, i) => ({ label: s.toUpperCase(), data: years.map(y => picker((byScenario[s]||{})[y]||{})), borderWidth: 2, borderColor: `hsl(${i*120} 70% 40%)`, backgroundColor: `hsl(${i*120} 70% 40% / 0.25)`, tension: 0.2, fill: true }));

        // 1) 总生物量对比
        const ctxB = document.getElementById('cmp-biomass')?.getContext('2d');
        if (ctxB) {
            const datasets = series(y => (y?.biomass?.total_biomass) ?? 0);
            document.getElementById('cmp-biomass')._chart?.destroy?.();
            document.getElementById('cmp-biomass')._chart = new Chart(ctxB, { type: 'line', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false } });
        }

        // 2) 相对变化（相对 ssp126）
        const base2 = 'ssp126';
        const ctxD = document.getElementById('cmp-delta')?.getContext('2d');
        if (ctxD && byScenario[base2]) {
            const baseArr = years.map(y => (byScenario[base2][y]?.biomass?.total_biomass) ?? 0);
            const datasets = scenarios.filter(s => s!==base).map((s, i) => ({
                label: `${s.toUpperCase()} vs ${base2.toUpperCase()}`,
                data: years.map((y, idx) => {
                    const val = (byScenario[s][y]?.biomass?.total_biomass) ?? 0;
                    const ref = baseArr[idx] || 1e-9;
                    return (val - ref) / ref * 100.0; // %
                }),
                borderColor: `hsl(${i*200} 70% 40%)`, backgroundColor: `hsl(${i*200} 70% 40% / 0.25)`, tension: 0.2, fill: true
            }));
            document.getElementById('cmp-delta')._chart?.destroy?.();
            document.getElementById('cmp-delta')._chart = new Chart(ctxD, { type: 'line', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false, scales: { y: { ticks: { callback: (v)=> v+ '%' } } } } });
        }

        // 3) NEE 对比（负值为碳汇）
        const ctxN = document.getElementById('cmp-nee')?.getContext('2d');
        if (ctxN) {
            const datasets = series(y => (y?.carbon_flux?.nee) ?? 0);
            document.getElementById('cmp-nee')._chart?.destroy?.();
            document.getElementById('cmp-nee')._chart = new Chart(ctxN, { type: 'line', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false } });
        }

        // 3.1) GPP 对比
        const ctxGPP = document.getElementById('cmp-gpp')?.getContext('2d');
        if (ctxGPP) {
            const datasets = series(y => (y?.carbon_flux?.gpp) ?? 0);
            document.getElementById('cmp-gpp')._chart?.destroy?.();
            document.getElementById('cmp-gpp')._chart = new Chart(ctxGPP, { type: 'line', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false } });
        }

        // 3.2) NPP 对比
        const ctxNPPc = document.getElementById('cmp-npp')?.getContext('2d');
        if (ctxNPPc) {
            const datasets = series(y => (y?.derived?.npp_approx) ?? 0);
            document.getElementById('cmp-npp')._chart?.destroy?.();
            document.getElementById('cmp-npp')._chart = new Chart(ctxNPPc, { type: 'line', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false } });
        }

        // 4) PFT 组成对比（起始与结束年饼/条形）
        const ctxP = document.getElementById('cmp-pft-bars')?.getContext('2d');
        if (ctxP) {
            const start = years[0], end = years[years.length-1];
            const mk = (y) => scenarios.map(s => sumObjectValues(byScenario[s][y]?.biomass?.biomass_pft || {}));
            const startVals = mk(start), endVals = mk(end);
            document.getElementById('cmp-pft-bars')._chart?.destroy?.();
            document.getElementById('cmp-pft-bars')._chart = new Chart(ctxP, {
                type: 'bar', data: { labels: scenarios.map(s=>s.toUpperCase()), datasets: [
                    { label: `起始年(${start})`, data: startVals, backgroundColor: '#60a5fa' },
                    { label: `结束年(${end})`, data: endVals, backgroundColor: '#34d399' },
                ] }, options: { responsive: true, maintainAspectRatio: false, scales: { x: { stacked: false }, y: { beginAtZero: true } } }
            });
        }

        // 5) 天气/气候对比（概览：使用 summary_science 中 aggregates.climate_summary 均值列）
        const ctxC = document.getElementById('cmp-climate')?.getContext('2d');
        if (ctxC && Object.keys(summaryScience || {}).length) {
            const labels = ['col1','col2','col3','col4','col5','col6'];
            const pickMeans = (ag) => [
                ag?.mean_col_1 ?? 0,
                ag?.mean_col_2 ?? 0,
                ag?.mean_col_3 ?? 0,
                ag?.mean_col_4 ?? 0,
                ag?.mean_col_5 ?? 0,
                ag?.mean_col_6 ?? 0,
            ];
            const datasets = ['ssp126','ssp245','ssp585'].filter(s=>summaryScience[s]).map((s,i)=>({
                label: s.toUpperCase(),
                data: pickMeans(summaryScience[s]?.aggregates?.climate_summary),
                backgroundColor: `hsl(${i*120} 70% 55% / 0.55)`
            }));
            document.getElementById('cmp-climate')._chart?.destroy?.();
            document.getElementById('cmp-climate')._chart = new Chart(ctxC, {
                type: 'bar', data: { labels, datasets }, options: { responsive: true, maintainAspectRatio: false, scales: { x: { stacked: false }, y: { beginAtZero: false } } }
            });
        }

        // 6) 三情景末年径级分布
        const ctxDIA = document.getElementById('cmp-dia')?.getContext('2d');
        if (ctxDIA) {
            const end = years[years.length-1];
            const labels = [];
            const datasets = scenarios.map((s,i)=>{
                const dist = (byScenario[s][end]?.structure?.diameter_distribution) || {};
                const bins = Object.keys(dist).map(x=>parseFloat(x));
                bins.forEach(b => { if (!labels.includes(b)) labels.push(b); });
                return { label: s.toUpperCase(), data: dist, colorIndex: i };
            });
            labels.sort((a,b)=>a-b);
            const ds = datasets.map(d => ({
                label: d.label,
                data: labels.map(b => Number(d.data[b] || 0)),
                backgroundColor: `hsl(${d.colorIndex*120} 70% 55% / 0.35)`,
            }));
            document.getElementById('cmp-dia')._chart?.destroy?.();
            document.getElementById('cmp-dia')._chart = new Chart(ctxDIA, { type: 'bar', data: { labels, datasets: ds }, options: { responsive: true, maintainAspectRatio: false, scales: { x: { stacked: false, title: { display: true, text: '径级(类中心或下界)' } }, y: { beginAtZero: true } } } });
        }
    }

    // 科研增强：渲染累计NEP与终点差异
    function renderCompareScience(sci, cmp) {
        const scenarios = Object.keys(sci);
        if (!scenarios.length) return;
        const years = Array.from(new Set(scenarios.flatMap(s => Object.keys((sci[s]||{}).yearly || {}).map(y => parseInt(y))))).sort((a,b)=>a-b);

        // 格式化年份标签用于图表显示
        const formatYearLabels = (yearArray) => yearArray.map(year => formatYearDisplay(year));

        // 累计 NEP
        const elCum = document.getElementById('cmp-cum-nep');
        const ctxCum = elCum?.getContext('2d');
        if (ctxCum) {
            const datasets = scenarios.map((s, i) => ({
                label: s.toUpperCase(),
                data: years.map(y => (sci[s]?.yearly?.[String(y)]?.derived?.cumulative_nep) ?? 0),
                borderColor: `hsl(${i*120} 70% 40%)`,
                backgroundColor: `hsl(${i*120} 70% 40% / 0.25)`,
                tension: 0.25, fill: true
            }));
            elCum._chart?.destroy?.();
            elCum._chart = new Chart(ctxCum, { type: 'line', data: { labels: formatYearLabels(years), datasets }, options: { responsive: true, maintainAspectRatio: false } });
        }

        // 终点差异条形
        const elBars = document.getElementById('cmp-end-bars');
        const ctxBars = elBars?.getContext('2d');
        if (ctxBars) {
            const labels = Object.keys(cmp).map(s => s.toUpperCase());
            const biom = Object.keys(cmp).map(s => (cmp[s]?.rel_change_vs_ssp126_biomass_pct) ?? 0);
            const cnep = Object.keys(cmp).map(s => (cmp[s]?.rel_change_vs_ssp126_cum_nep_pct) ?? 0);
            elBars._chart?.destroy?.();
            elBars._chart = new Chart(ctxBars, { type: 'bar', data: { labels, datasets: [
                { label: '终点总生物量 相对SSP1-2.6(%)', data: biom, backgroundColor: '#60a5fa' },
                { label: '累计NEP 相对SSP1-2.6(%)', data: cnep, backgroundColor: '#34d399' },
            ] }, options: { responsive: true, maintainAspectRatio: false, scales: { y: { ticks: { callback: (v)=> v+ '%' } } } } });
        }
    }

    // 气候时间序列对比：同一张图内按情景绘制所选气候指标
    function renderClimateTS(metricKey = 'temperature_mean_C') {
        const el = document.getElementById('cmp-climate-ts');
        if (!el) return;

        const ctx = el.getContext('2d');
        const scenarios = Object.keys(summaryScience || {});

        if (!scenarios.length) {
            console.warn('气候时间序列对比：无可用情景数据');
            ctx.clearRect(0, 0, el.width, el.height);
            ctx.fillStyle = '#666';
            ctx.font = '16px Arial';
            ctx.textAlign = 'center';
            ctx.fillText('暂无气候数据', el.width/2, el.height/2);
            return;
        }

        const years = Array.from(new Set(scenarios.flatMap(s => Object.keys((summaryScience[s]||{}).yearly || {}).map(y => parseInt(y))))).sort((a,b)=>a-b);

        if (!years.length) {
            console.warn('气候时间序列对比：无年份数据');
            ctx.clearRect(0, 0, el.width, el.height);
            ctx.fillStyle = '#666';
            ctx.font = '16px Arial';
            ctx.textAlign = 'center';
            ctx.fillText('暂无年份数据', el.width/2, el.height/2);
            return;
        }

        // 格式化年份标签用于图表显示
        const formatYearLabels = (yearArray) => yearArray.map(year => formatYearDisplay(year));

        const datasets = scenarios.map((s, i) => {
            const scenarioData = summaryScience[s] || {};
            const yearlyData = scenarioData.yearly || {};

            const data = years.map(y => {
                const yearData = yearlyData[String(y)] || {};
                const climateData = yearData.climate || {};
                return climateData[metricKey] ?? null;
            });

            // 过滤掉null值用于计算统计，但保持数据点位置
            const validData = data.filter(v => v !== null && typeof v === 'number');
            const avgValue = validData.length > 0 ? validData.reduce((a,b)=>a+b,0)/validData.length : 0;

            return {
                label: s.toUpperCase(),
                data: data,
                borderColor: `hsl(${i*120} 70% 40%)`,
                backgroundColor: `hsl(${i*120} 70% 40% / 0.25)`,
                tension: 0.2,
                fill: true,
                spanGaps: false
            };
        });

        el._chart?.destroy?.();
        el._chart = new Chart(ctx, {
            type: 'line',
            data: { labels: formatYearLabels(years), datasets },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { display: true },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                const value = context.parsed.y;
                                if (value === null) return `${context.dataset.label}: 无数据`;
                                return `${context.dataset.label}: ${value?.toFixed(2) || 'N/A'}`;
                            }
                        }
                    }
                },
                scales: {
                    y: {
                        beginAtZero: false,
                        title: { display: true, text: getClimateMetricLabel(metricKey) }
                    }
                }
            }
        });

        console.log(`气候时间序列图已渲染: ${metricKey}, 情景: ${scenarios.join(', ')}, 年份: ${years.length}年`);
    }

    function getClimateMetricLabel(metricKey) {
        const labels = {
            'temperature_mean_C': '平均气温 (°C)',
            'rain_sum_mm': '累计降雨 (mm)',
            'irradiance_mean_umol': '平均辐照 (μmol/s/m²)',
            'day_length_mean_h': '平均日照时长 (h)',
            'pet_sum_mm': '累计PET (mm)',
            'co2_mean_ppm': '平均CO₂浓度 (ppm)'
        };
        return labels[metricKey] || metricKey;
    }

    // 气候复合图：单情景，六个变量做 Z 分数标准化后同图绘制
    function renderClimateComposite(sspKey = 'ssp245') {
        const el = document.getElementById('cmp-climate-composite');
        if (!el) return;

        const ctx = el.getContext('2d');
        const scenarioData = summaryScience?.[sspKey] || {};

        if (!scenarioData.yearly) {
            console.warn(`气候复合图：情景 ${sspKey} 无数据`);
            ctx.clearRect(0, 0, el.width, el.height);
            ctx.fillStyle = '#666';
            ctx.font = '16px Arial';
            ctx.textAlign = 'center';
            ctx.fillText(`情景 ${sspKey} 无气候数据`, el.width/2, el.height/2);
            return;
        }

        const yearly = scenarioData.yearly;
        const years = Object.keys(yearly).map(y=>parseInt(y)).sort((a,b)=>a-b);

        if (!years.length) {
            console.warn('气候复合图：无年份数据');
            ctx.clearRect(0, 0, el.width, el.height);
            ctx.fillStyle = '#666';
            ctx.font = '16px Arial';
            ctx.textAlign = 'center';
            ctx.fillText('暂无年份数据', el.width/2, el.height/2);
            return;
        }

        // 格式化年份标签用于图表显示
        const formatYearLabels = (yearArray) => yearArray.map(year => formatYearDisplay(year));

        const series = {
            temperature_mean_C: { label: '气温(年均)', color: '#ef4444' },
            rain_sum_mm: { label: '降雨(年累计)', color: '#3b82f6' },
            irradiance_mean_umol: { label: '辐照(年均)', color: '#f59e0b' },
            day_length_mean_h: { label: '日照时长(年均)', color: '#10b981' },
            pet_sum_mm: { label: 'PET(年累计)', color: '#8b5cf6' },
            co2_mean_ppm: { label: 'CO₂(年均)', color: '#6b7280' }
        };

        function zscore(arr){
            const xs = arr.filter(v=>typeof v==='number' && !isNaN(v) && v !== null);
            if (xs.length === 0) return arr.map(() => 0); // 全部返回0如果没有有效数据
            const mu = xs.reduce((a,b)=>a+b,0)/xs.length;
            const sd = Math.sqrt(xs.reduce((a,b)=>a+(b-mu)*(b-mu),0)/xs.length) || 1;
            return arr.map(v => (typeof v==='number' && !isNaN(v) && v !== null) ? (v-mu)/sd : 0);
        }

        const datasets = Object.entries(series).map(([k,meta]) => {
            const rawData = years.map(y => yearly[y]?.climate?.[k] ?? null);
            const normalizedData = zscore(rawData);

            // 检查是否有有效数据
            const hasValidData = normalizedData.some(v => v !== 0);

            return {
                label: meta.label,
                data: normalizedData,
                borderColor: meta.color,
                backgroundColor: meta.color+'33',
                tension: 0.2,
                fill: false,
                hidden: !hasValidData, // 如果没有有效数据，隐藏这条线
                spanGaps: false
            };
        });

        el._chart?.destroy?.();
        el._chart = new Chart(ctx, {
            type: 'line',
            data: { labels: formatYearLabels(years), datasets },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { display: true },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                const value = context.parsed.y;
                                const originalValue = context.dataset.originalData?.[context.dataIndex];
                                if (originalValue === null || originalValue === undefined) {
                                    return `${context.dataset.label}: 无数据`;
                                }
                                return `${context.dataset.label}: ${originalValue?.toFixed(2) || 'N/A'} (Z分数: ${value?.toFixed(2) || 'N/A'})`;
                            }
                        }
                    }
                },
                scales: {
                    y: {
                        title: { display: true, text: 'Z 分数(标准化)' },
                        beginAtZero: false
                    }
                }
            }
        });

        // 保存原始数据用于tooltip显示
        datasets.forEach((dataset, index) => {
            const rawData = years.map(y => yearly[y]?.climate?.[Object.keys(series)[index]] ?? null);
            dataset.originalData = rawData;
        });

        const validDatasets = datasets.filter(d => !d.hidden);
        console.log(`气候复合图已渲染: ${sspKey}, 变量: ${validDatasets.length}/${datasets.length}, 年份: ${years.length}年`);
    }

    function checkStatus(jobId) {
        console.log('检查任务状态:', jobId);
        fetch(`/status/${jobId}`)
            .then(response => {
                console.log('状态响应状态:', response.status);
                return response.json();
            })
            .then(data => {
                console.log('状态数据:', data);
                if (data.status === 'COMPLETED') {
                    console.log('任务完成，开始初始化');
                    // 你的 results.html 中没有 id 为 status-area 的元素，所以这行可以安全删除
                    // document.getElementById('status-area').style.display = 'none';
                    init3DScene();
                    setupEventListeners();
                    console.log('调用onDataLoaded，数据模型数量:', data.models?.length || 0);
                    onDataLoaded(data);
                } else if (data.status === 'PENDING') {
                    console.log('任务进行中，5秒后重试');
                    setTimeout(() => checkStatus(jobId), 5000);
                } else {
                    console.log('任务失败:', data.message);
                     document.getElementById('info-panel').innerHTML = `<h2>任务失败</h2><p>${data.message || '未知错误'}</p>`;
                     if (loaderElement) loaderElement.style.display = 'none';
                }
            })
            .catch(error => {
                console.error('检查状态时出错:', error);
            });
    }

    // --- 步骤3: 启动整个应用的逻辑 ---
    if (body.dataset.jobId) {
        checkStatus(body.dataset.jobId);
    }
});
