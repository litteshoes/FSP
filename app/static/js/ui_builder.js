// app/static/js/ui_builder.js (v12.1 - Final Function Fix)
let parSchema = null;
let pinSchema = null;
let effectivePftParams = null; // 运行时根据 schema 计算得到的“按PFT定义”的参数集合

async function handleResponse(response) {
    if (!response.ok) {
        const data = await response.json().catch(() => ({ error: `HTTP Status ${response.status}` }));
        throw new Error(data.error);
    }
    return response.json();
}

export async function buildEditor(parContainer, pinContainer, pftCount) {
    let parError = null;
    let pinError = null;
    try {
        parSchema = await fetch(`/api/par-schema/${pftCount}`).then(handleResponse);
    } catch (error) {
        parError = error;
    }
    if (parError) {
        if (parContainer) {
            parContainer.innerHTML = `<p style="color: red; font-weight: bold;">错误: 无法加载编辑器。<br>${parError.message}</p>`;
        }
        return;
    }

    if (pinContainer) {
        try {
            pinSchema = await fetch(`/api/pin-schema/${pftCount}`).then(handleResponse);
        } catch (error) {
            pinError = error; pinSchema = null;
        }
    } else {
        pinSchema = null;
    }

    buildParUI(parContainer, pftCount);
    if (pinContainer) {
        if (pinSchema) {
            buildPinUI(pinContainer, pftCount);
        } else {
            pinContainer.innerHTML = `<div class="text-muted" style="padding:8px 2px;">未能加载 PIN 模板，已降级为默认空样地。${pinError ? `原因：${pinError.message}` : ''}</div>`;
        }
    }
}

// 判断某个参数是否应按 PFT 渲染（支持一维/二维）
function isPerPftParamGlobal(param, pftCount) {
    const val = param && Array.isArray(param.value) ? param.value : null;
    if (!val) return false;
    if (Array.isArray(val[0])) {
        const colCount = Math.max(...val.map(r => Array.isArray(r) ? r.length : 0));
        return colCount >= pftCount && colCount > 1;
    }
    return val.length >= pftCount && val.length > 1;
}

// --- 公共：将 comment 标题（section）映射为统一的分组名 ---
function mapSection(section) {
    const s = String(section || '').toLowerCase();
    if (/geometry/.test(s)) return '几何 Geometry';
    if (/production|growth/.test(s)) return '生产 Production';
    if (/mortality/.test(s)) return '死亡 Mortality';
    if (/environment|climate|temperature|soil|water|carbon/.test(s)) return '环境/气候/水/碳 Environment';
    return '模拟/技术 Simulation';
}

function buildParUI(container, pftCount) {
    if (!parSchema) return;
    // 过滤不希望用户修改的参数块：结果文件开关（myResultFileSwitch.*）
    const generalParamsAll = parSchema.general_params
        .filter(p => p.key !== 'N_Par.Div_MAXGRP')
        .filter(p => !/^myResultFileSwitch\./i.test(p.key || ''));
    // 将“看似通用但实际按PFT定义”的参数移动到 PFT 参数集合中
    const perPftFromGeneral = generalParamsAll.filter(p => isPerPftParamGlobal(p, pftCount));
    const generalParams = generalParamsAll.filter(p => !isPerPftParamGlobal(p, pftCount));
    // 组合得到最终按PFT渲染的参数集合：
    // 注意：parSchema.pft_params 已由后端按维度判断为“按PFT定义”，这里不要再次过滤，避免误排除
    effectivePftParams = [
        ...(parSchema.pft_params || []),
        ...perPftFromGeneral
    ];
    const pftCountParam = parSchema.general_params.find(p => p.key === 'N_Par.Div_MAXGRP');
    let html = '';

    // 分组：优先使用 schema.section（Comment 标题），其次用键前缀兜底
    const mapSection = (section) => {
        const s = String(section || '').toLowerCase();
        if (/geometry/.test(s)) return '几何 Geometry';
        if (/production|growth/.test(s)) return '生产 Production';
        if (/mortality/.test(s)) return '死亡 Mortality';
        if (/environment|climate|temperature|soil|water|carbon/.test(s)) return '环境/气候/水/碳 Environment';
        return '模拟/技术 Simulation';
    };
    const groupOf = (param) => {
        if (param && param.section) return mapSection(param.section);
        const k = String(param?.key || '').toLowerCase();
        if (k.startsWith('n_par.geo_')) return '几何 Geometry';
        if (k.startsWith('n_par.pro_') || k.includes('growth_function_switch')) return '生产 Production';
        if (k.startsWith('n_par.mort_')) return '死亡 Mortality';
        if (k.startsWith('n_par.env_') || k.startsWith('n_par.ref_') || k.startsWith('n_par.temperature_') || k.includes('climate') || k.includes('water') || k.includes('cflux') || k.startsWith('n_par.cpool_')) return '环境/气候/水/碳 Environment';
        return '模拟/技术 Simulation';
    };
    const order = ['模拟/技术 Simulation','环境/气候/水/碳 Environment','几何 Geometry','生产 Production','死亡 Mortality'];
    const grouped = {};
    generalParams.forEach(p => { const g = groupOf(p); (grouped[g] ||= []).push(p); });
    // 确保 N_Par.Div_MAXGRP 在“模拟/技术”组首位
    if (!grouped['模拟/技术 Simulation']) grouped['模拟/技术 Simulation'] = [];
    if (pftCountParam) grouped['模拟/技术 Simulation'].unshift({ ...pftCountParam, value: pftCount });
    // 渲染
    order.forEach(g => {
        const list = grouped[g]; if (!list || !list.length) return;
        html += `<fieldset><legend>${g}</legend><div class="grid-container">`;
        list.forEach(param => { html += createFormControl(param); });
        html += '</div></fieldset>';
    });
    // 提示性信息：结果文件开关已由系统管理
    html += `<div class="text-muted" style="margin:-10px 0 20px 2px; font-size:0.9em;">结果输出文件设置已由系统管理（myResultFileSwitch.*），当前界面不提供修改。</div>`;
    html += `<fieldset><legend>植物功能型 (PFT) 参数</legend><div id="pft-editor-tabs" class="tabs"></div><div id="pft-params-container"></div></fieldset>`;
    container.innerHTML = html;
    renderPftEditor(pftCount);
}

function buildPinUI(container, pftCount) {
    if (!pinSchema) return;
    let html = `<fieldset><legend class="checkbox-group"><input type="checkbox" id="use-default-pin-toggle" checked><label for="use-default-pin-toggle">使用默认 .pin 数据</label></legend><div id="pin-custom-area" style="display:none;"><div class="grid-container">
        <div class="form-group"><label>区域描述</label><div class="control-wrapper"><input type="text" id="pin-regionheader" value="${pinSchema.general[0]?.value || ''}"></div></div>
        <div class="form-group"><label>胸径等级</label><textarea id="pin-dclass" rows="3">${pinSchema.dclass.join(' ')}</textarea></div>
        </div><h3>样地数据</h3><div id="pin-plots-grid">`;
    pinSchema.plots.forEach((plot, plotIndex) => {
        html += `<div class="plot-card"><h4>${plot.name}</h4><p><strong>位置:</strong> ${plot.position.join(' ')}</p>`;
        for (let i = 0; i < pftCount; i++) {
            const n0_row = plot.n0[i] || Array(pinSchema.dclass.length).fill(0);
            html += `<details><summary><strong>PFT ${i + 1}</strong></summary><div class="n0-editor-wrapper"><label>初始树木数:</label><input type="text" class="n0-input" data-plot-index="${plotIndex}" data-pft-index="${i}" value="${n0_row.join(' ')}"></div></details>`;
        }
        html += `</div>`;
    });
    html += `</div></div></fieldset>`;
    container.innerHTML = html;

    // 添加复选框切换事件处理
    const toggleCheckbox = document.getElementById('use-default-pin-toggle');
    const customArea = document.getElementById('pin-custom-area');
    if (toggleCheckbox && customArea) {
        toggleCheckbox.addEventListener('change', (event) => {
            customArea.style.display = event.target.checked ? 'none' : 'block';
        });
    }
}

function renderPftEditor(pftCount) {
    const tabsContainer = document.getElementById('pft-editor-tabs');
    const paramsContainer = document.getElementById('pft-params-container');
    if (!tabsContainer || !paramsContainer || !parSchema || !parSchema.pft_params) return;
    let tabsHtml = '', paramsHtml = '';

    for (let i = 0; i < pftCount; i++) {
        tabsHtml += `<button type="button" class="tab-button ${i === 0 ? 'active' : ''}" data-pft-tab="${i}">PFT ${i + 1}</button>`;
        paramsHtml += `<div class="tab-content pft-tab-content ${i === 0 ? 'active' : ''}" data-pft-content="${i}">`;
        paramsHtml += `<div class="form-group" style="grid-column: 1 / -1;"><label class="text-muted">当前 PFT</label><div class="control-wrapper"><strong>PFT ${i + 1}</strong></div></div>`;
        const classify = (param) => {
            // 优先使用 section
            const sec = mapSection(param.section);
            if (sec === '几何 Geometry' || sec === '生产 Production' || sec === '死亡 Mortality') return sec;
            const k = String(param?.key||'').toLowerCase();
            if (k.startsWith('n_par.geo_')) return '几何 Geometry';
            if (k.startsWith('n_par.pro_') || k.includes('growth_function_switch')) return '生产 Production';
            if (k.startsWith('n_par.mort_')) return '死亡 Mortality';
            return '其他 Others';
        };
        const byGroup = { '几何 Geometry':[], '生产 Production':[], '死亡 Mortality':[], '其他 Others':[] };
        (effectivePftParams || []).forEach(param => { byGroup[classify(param)].push(param); });
        const renderGroup = (legend, list) => {
            if (!list.length) return '';
            let s = `<fieldset><legend>${legend}</legend><div class="grid-container">`;
            list.forEach(param => { s += createPftFormControl(param, i); });
            s += '</div></fieldset>';
            return s;
        };
        paramsHtml += renderGroup('几何 Geometry', byGroup['几何 Geometry']);
        paramsHtml += renderGroup('生产 Production', byGroup['生产 Production']);
        paramsHtml += renderGroup('死亡 Mortality', byGroup['死亡 Mortality']);
        paramsHtml += renderGroup('其他 Others', byGroup['其他 Others']);
        paramsHtml += `</div>`;
    }
    tabsContainer.innerHTML = tabsHtml;
    paramsContainer.innerHTML = paramsHtml;

    // 移除调试输出，避免控制台噪声

    // 防御性绑定：限制每个输入仅联动同PFT的对应控件
    paramsContainer.querySelectorAll('.slider, .number-input').forEach(el => {
        const key = el.getAttribute('data-param-key');
        const pftIndex = el.getAttribute('data-pft-index');
        if (!key || pftIndex === null) return;
        el.addEventListener('input', () => {
            const isSlider = el.classList.contains('slider');
            const selector = isSlider
                ? `input.number-input[data-param-key="${key}"][data-pft-index="${pftIndex}"]`
                : `input.slider[data-param-key="${key}"][data-pft-index="${pftIndex}"]`;
            const peer = paramsContainer.querySelector(selector);
            if (peer) peer.value = el.value;
        });
    });

    // 绑定PFT标签切换（作用域内完成，避免与顶层标签互相干扰）
    tabsContainer.addEventListener('click', (e) => {
        const btn = e.target.closest('button.tab-button');
        if (!btn) return;
        e.preventDefault();
        e.stopPropagation();
        tabsContainer.querySelectorAll('button.tab-button').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        paramsContainer.querySelectorAll('.pft-tab-content').forEach(c => { 
            c.classList.remove('active'); 
            c.style.display = 'none';
        });
        const content = paramsContainer.querySelector(`.pft-tab-content[data-pft-content="${btn.dataset.pftTab}"]`);
        if (content) { 
            content.classList.add('active');
            content.style.display = 'grid';
        }
    });
}

function createFormControl(param) {
    const { key, type, value, label_cn, description, unit, range } = param;
    // 再次保护：直接隐藏结果文件开关（避免从schema带入）
    if (/^myResultFileSwitch\./i.test(String(key))) {
        return '';
    }
    if ((type === 'float' || type === 'int') && Array.isArray(range) && range.length === 2) {
        let min = Number(range[0]), max = Number(range[1]);
        // 针对 PFT 数量上限进行强制约束（1..6）
        const loweredKey = String(key || '').toLowerCase();
        if (key === 'N_Par.Div_MAXGRP' || loweredKey.includes('eiv_maxgr')) {
            min = Math.max(1, isNaN(min) ? 1 : min);
            max = 6;
        }
        // 仅当“确认为布尔开关”时才使用复选框：类型为 int 且 0..1，或键/标签表明Flag/Switch/Log
        const isBooleanSwitch = (type === 'int' && min === 0 && max === 1)
            || /^switch\./i.test(String(key))
            || /^log\./i.test(String(key))
            || /\bflag\b/i.test(String(label_cn || ''));
        if (isBooleanSwitch) {
            const checked = Number(value || 0) === 1 ? 'checked' : '';
            return `<div class="form-group" title="${description || ''}">
                        <label for="${key}">${label_cn}<code class="param-key">${key}</code></label>
                        <div class="control-wrapper">
                            <input type="checkbox" id="${key}" data-param-key="${key}" ${checked}>
                            <span class="unit">${unit || ''}</span>
                        </div>
                    </div>`;
        }
        const step = type === 'int' ? 1 : 0.001;
        let cleanValue = Number(value);
        if (isNaN(cleanValue)) cleanValue = min;
        cleanValue = Math.max(min, Math.min(max, cleanValue));
        const roundedValue = type === 'int' ? Math.round(cleanValue) : parseFloat(cleanValue.toFixed(3));
        const precision = type === 'int' ? 0 : 3;
        return `<div class="form-group" title="${description || ''}">
                    <label for="${key}">${label_cn}<code class="param-key">${key}</code></label>
                    <div class="control-wrapper">
                        <input type="range" data-param-key="${key}" min="${min}" max="${max}" value="${roundedValue}" step="${step}" class="slider" tabindex="0">
                        <input type="number" data-param-key="${key}" min="${min}" max="${max}" value="${roundedValue.toFixed(precision)}" step="${step}" class="number-input">
                        <span class="unit">${unit || ''}</span>
                    </div>
                </div>`;
    }
    return `<div class="form-group" title="${description || ''}">
                <label for="${key}">${label_cn}<code class="param-key">${key}</code></label>
                <div class="control-wrapper">
                    <input type="text" id="${key}" data-param-key="${key}" value="${value || ''}" style="width: 100%;">
                </div>
            </div>`;
}

// --- 关键：补全这个被遗漏的函数定义 ---
function createPftFormControl(param, pftIndex) {
    const { key, label_cn, unit, range, value, description } = param;
    // 支持一维（每个PFT一个值）与二维（多行*多PFT）两种模板
    let pftValue = 0;
    if (Array.isArray(value)) {
        if (Array.isArray(value[0])) {
            pftValue = (value[0] && value[0][pftIndex] !== undefined) ? value[0][pftIndex] : 0;
        } else {
            pftValue = value[pftIndex] !== undefined ? value[pftIndex] : 0;
        }
    }
    const inputId = `${key}-${pftIndex}`;
    if (Array.isArray(range) && range.length === 2) {
         const min = Number(range[0]), max = Number(range[1]);
         // 0/1 参数：使用复选框
         if (min === 0 && max === 1) {
             const checked = Number(pftValue || 0) === 1 ? 'checked' : '';
             return `<div class="form-group" title="${description || ''}">
                       <label for="${inputId}">${label_cn}<code class="param-key">${key}</code></label>
                       <div class="control-wrapper">
                           <input type="checkbox" id="${inputId}" data-param-key="${key}" data-pft-index="${pftIndex}" ${checked}>
                           <span class="unit">${unit || ''}</span>
                       </div>
                   </div>`;
         }
         const step = (String(pftValue).includes('.')) ? 0.01 : 1;
         let cleanValue = parseFloat(pftValue);
         if (isNaN(cleanValue)) cleanValue = min;
         cleanValue = Math.max(min, Math.min(max, cleanValue));
         const roundedValue = Math.round(cleanValue / step) * step;
         return `<div class="form-group" title="${description || ''}">
                   <label for="${inputId}">${label_cn}<code class="param-key">${key}</code></label>
                   <div class="control-wrapper">
                       <input type="range" id="${inputId}" data-param-key="${key}" data-pft-index="${pftIndex}" min="${min}" max="${max}" value="${roundedValue}" step="${step}" class="slider">
                      <input type="number" id="${inputId}-num" data-param-key="${key}" data-pft-index="${pftIndex}" min="${max < 1 ? 0 : min}" max="${max}" value="${roundedValue.toFixed(step === 1 ? 0 : 2)}" step="${step}" class="number-input">
                       <span class="unit">${unit || ''}</span>
                   </div>
               </div>`;
    }
    return `<div class="form-group" title="${description || ''}">
               <label for="${inputId}">${label_cn}<code class="param-key">${key}</code></label>
               <div class="control-wrapper">
                   <input type="text" id="${inputId}" data-param-key="${key}" data-pft-index="${pftIndex}" value="${pftValue}">
               </div>
           </div>`;
}

export function getCurrentParValues() {
    const values = {};
    if (!parSchema) return values;

    // 1. 收集通用参数
    parSchema.general_params.forEach(param => {
        // 优先选择更精确的输入类型
        let el = document.querySelector(`input.number-input[data-param-key="${param.key}"]`);
        if (!el) el = document.querySelector(`input[type="checkbox"][data-param-key="${param.key}"]`);
        if (!el) el = document.querySelector(`[data-param-key="${param.key}"]`);
        if (el) {
            let val = (el.type === 'checkbox') ? (el.checked ? 1 : 0) : el.value;
            // --- 关键修复：根据schema中定义的类型进行转换 ---
            if (param.type === 'float') val = parseFloat(val);
            else if (param.type === 'int') val = parseInt(val, 10);
            // 对于string类型，我们返回原始值，不加引号
            
            // 特殊处理需要引号的参数
            if (param.key === 'PinFileNameX' || param.type === 'string') {
                 values[param.key] = `"${val.replace(/"/g, '')}"`; // 确保有且仅有一对引号
            } else {
                 values[param.key] = val;
            }
        }
    });

    // 2. 收集PFT参数（包含从general中识别出的按PFT定义的参数）
    const pftCount = parseInt(document.querySelector('[data-param-key="N_Par.Div_MAXGRP"]').value, 10);
    (effectivePftParams || parSchema.pft_params || []).forEach(param => {
        const key = param.key;
        let foundCount = 0;
        const pftValues = [];
        const isTwoDimensional = Array.isArray(param.value) && Array.isArray(param.value[0]);
        const sampleValue = Array.isArray(param.value)
            ? (isTwoDimensional ? (param.value[0]?.[0]) : param.value[0])
            : "0";
        for (let i = 0; i < pftCount; i++) {
            const scope = document.querySelector(`.pft-tab-content[data-pft-content="${i}"]`) || document;
            // 优先读取数字输入，其次滑块，再次复选框/文本
            let inputEl = scope.querySelector(`input.number-input[data-param-key="${key}"][data-pft-index="${i}"]`);
            if (!inputEl) inputEl = scope.querySelector(`input.slider[data-param-key="${key}"][data-pft-index="${i}"]`);
            if (!inputEl) inputEl = scope.querySelector(`input[type="checkbox"][data-param-key="${key}"][data-pft-index="${i}"]`);
            if (!inputEl) inputEl = scope.querySelector(`[data-param-key="${key}"][data-pft-index="${i}"]`);

            if (inputEl) {
                let rawVal = (inputEl.type === 'checkbox') ? (inputEl.checked ? 1 : 0) : inputEl.value;
                const parsed = String(sampleValue).includes('.') ? parseFloat(rawVal) : parseInt(rawVal, 10);
                pftValues.push(parsed);
                foundCount++;
            } else {
                // 回退到模板值（支持一维与二维）
                if (isTwoDimensional) {
                    const fallback = param.value[0]?.[i];
                    pftValues.push(fallback !== undefined ? fallback : 0);
                } else {
                    const fallback = Array.isArray(param.value) ? param.value[i] : undefined;
                    pftValues.push(fallback !== undefined ? fallback : 0);
                }
            }
        }
        if (foundCount === 0) return;

        // 保持与模板相同的维度：一维则返回一维；二维则按行复制
        if (isTwoDimensional) {
            values[key] = param.value.map(() => pftValues.slice());
        } else {
            values[key] = pftValues.slice();
        }
    });
    
    console.log("Collected final PAR values:", JSON.stringify(values, null, 2));
    return values;
}

export function getCurrentPinContent() {
    if (!pinSchema) return "";

    // 检查是否勾选了"使用默认 .pin 数据"
    const useDefaultToggle = document.getElementById('use-default-pin-toggle');
    if (useDefaultToggle && useDefaultToggle.checked) {
        // 如果勾选了使用默认数据，返回空字符串，后端会使用模板文件
        return "";
    }

    let content = "file pinfile\n";
    content += `regionheader = "${document.getElementById('pin-regionheader').value}"\n`;
    content += "dclass =\n" + document.getElementById('pin-dclass').value + '\n';
    content += "\nEND OF GENERAL PART\nSTART OF THE REPEATING PLOT DESCRIPTION PART.\n";

    const pftCount = parseInt(document.querySelector('[data-param-key="N_Par.Div_MAXGRP"]').value, 10);
    const plotCards = document.querySelectorAll('.plot-card');

    plotCards.forEach((card, plotIndex) => {
        const plotData = pinSchema.plots[plotIndex];
        content += `\nblock plot\n name     = "${plotData.name}"\n position = ${plotData.position.join(' ')}\n code 	  = 35\n mel      = 0\n n0       =\n`;
        for (let i = 0; i < pftCount; i++) {
            const n0Input = card.querySelector(`.n0-input[data-pft-index="${i}"]`);
            content += (n0Input ? n0Input.value : Array(pinSchema.dclass.length).fill(0).join(' ')) + '\n';
        }
        content += `seeds    = ${Array(pftCount).fill(0).join(' ')}\n`;
    });
    content += "\nEND OF REPEATING PART\n";
    return content;
}