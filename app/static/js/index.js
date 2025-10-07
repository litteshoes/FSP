// app/static/js/index.js (v15.0 - Restore CSV Upload)
import { buildEditor, getCurrentParValues, getCurrentPinContent } from './ui_builder.js';

async function initializeApp() {
    const parEditorContainer = document.getElementById('par-editor-container');
    const pinEditorContainer = document.getElementById('pin-editor-container');
    const form = document.getElementById('simulation-form');
    const searchInput = document.getElementById('param-search-input');
    const searchClearBtn = document.getElementById('param-search-clear');
    const searchCount = document.getElementById('param-search-count');
    const densitySelect = document.getElementById('param-density-select');

    
    function applyParamSearchFilter() {
        try {
            if (!parEditorContainer) return;
            const query = (searchInput?.value || '').trim().toLowerCase();
            const groups = parEditorContainer.querySelectorAll('.form-group');
            let visibleCount = 0;
            groups.forEach(group => {
                // 仅针对参数编辑区域的表单项
                const label = group.querySelector('label');
                const text = (label ? label.textContent : group.textContent) || '';
                const match = query === '' || text.toLowerCase().includes(query);
                group.classList.toggle('hidden-by-search', !match);
                if (match) visibleCount += 1;
            });
            if (searchCount) {
                if (query) searchCount.textContent = `匹配参数: ${visibleCount}`;
                else searchCount.textContent = `已加载参数: ${groups.length}`;
            }
        } catch (_) {}
    }

    async function rebuildUI(pftCount) {
        if (parEditorContainer) parEditorContainer.innerHTML = '<div class="loader"></div>';
        if (pinEditorContainer) pinEditorContainer.innerHTML = '<div class="loader"></div>';
        try {
            await buildEditor(parEditorContainer, pinEditorContainer, pftCount);
            // 构建完成后，应用当前搜索过滤
            applyParamSearchFilter();
            // applyDensitySetting(); // 密度功能已禁用
        } catch (error) {
            console.error('Error in rebuildUI:', error);
            if (parEditorContainer) parEditorContainer.innerHTML = '<p style="color: red;">加载参数编辑器时出错，请刷新页面重试。</p>';
        }
    }
    
    // --- 使用事件委托统一处理所有交互 ---
    form.addEventListener('click', (event) => {
        const btn = event.target.closest('button.tab-button');
        if (!btn) return;
        const isPftTab = !!btn.closest('#pft-editor-tabs');
        const scopeSelector = isPftTab ? '#pft-editor-tabs' : '.container > .tabs';
        document.querySelectorAll(`${scopeSelector} .tab-button`).forEach(t => t.classList.remove('active'));
        btn.classList.add('active');
        if (isPftTab) {
            const paramsContainer = document.getElementById('pft-params-container');
            if (!paramsContainer) return;
            paramsContainer.querySelectorAll('.pft-tab-content').forEach(c => c.classList.remove('active'));
            const targetContent = paramsContainer.querySelector(`.pft-tab-content[data-pft-content="${btn.dataset.pftTab}"]`);
            if (targetContent) targetContent.classList.add('active');
            return;
        }
        // 顶层页面标签切换
        document.querySelectorAll('.tab-content-wrapper > .tab-content').forEach(c => c.classList.remove('active'));
        const container = document.getElementById(btn.dataset.tab);
        if (container) container.classList.add('active');
    });
    form.addEventListener('input', async (event) => {
        const target = event.target;
        if (target.dataset.paramKey === 'N_Par.Div_MAXGRP') {
            const newPftCount = parseInt(target.value, 10);
            if (newPftCount >= 2 && newPftCount <= 6) await rebuildUI(newPftCount);
        }
        if (target.matches('.slider')) {
            const key = target.dataset.paramKey;
            const pftIndex = target.dataset.pftIndex;
            // 在同一参数块内查找对应输入，避免跨 PFT 误联动
            const scope = target.closest('.form-group') || form;
            let selector = `input.number-input[data-param-key="${key}"]`;
            if (pftIndex !== undefined) selector += `[data-pft-index="${pftIndex}"]`;
            const numberInput = scope.querySelector(selector) || form.querySelector(selector);
            if (numberInput) numberInput.value = target.value;
        }
        if (target.matches('.number-input')) {
            const key = target.dataset.paramKey;
            const pftIndex = target.dataset.pftIndex;
            const scope = target.closest('.form-group') || form;
            let selector = `input.slider[data-param-key="${key}"]`;
            if (pftIndex !== undefined) selector += `[data-pft-index="${pftIndex}"]`;
            const slider = scope.querySelector(selector) || form.querySelector(selector);
            if (slider) slider.value = target.value;
        }
        // 搜索框联动过滤
        if (target === searchInput) {
            applyParamSearchFilter();
        }
    });
    form.addEventListener('change', (event) => {
        const target = event.target;
        // --- PIN模式选择逻辑 ---
        if (target.name === 'pin_mode') {
            // 控制CSV上传区域显示
            document.getElementById('pin-csv-area').style.display = (target.value === 'csv') ? 'block' : 'none';
            // 控制PIN文件上传区域显示
            document.getElementById('pin-upload-area').style.display = (target.value === 'upload') ? 'block' : 'none';
        }

    });
    
    // --- 初始加载 ---
    await rebuildUI(2);

    // 搜索清除
    if (searchClearBtn) {
        searchClearBtn.addEventListener('click', () => {
            if (searchInput) searchInput.value = '';
            applyParamSearchFilter();
        });
    }

    function applyDensitySetting() { /* 密度显示功能已禁用 */ }

    // 密度显示功能已禁用
    // if (densitySelect) {
    //     const saved = localStorage.getItem('par_density');
    //     if (saved && saved !== densitySelect.value) densitySelect.value = saved;
    //     applyDensitySetting();
    //     densitySelect.addEventListener('change', () => {
    //         localStorage.setItem('par_density', densitySelect.value);
    //         applyDensitySetting();
    //     });
    // }

    // --- 表单提交 ---
    form.addEventListener('submit', (event) => {
        event.preventDefault();
        const submitButton = document.getElementById('submit-btn');
        submitButton.disabled = true; submitButton.textContent = '正在提交...';
        
        document.getElementById('par-params-json').value = JSON.stringify(getCurrentParValues());
        
        const selectedPinMode = document.querySelector('input[name="pin_mode"]:checked').value;
        // 根据PIN模式设置PIN内容
        if (selectedPinMode === 'upload') {
            // 上传PIN文件模式，使用空字符串（后端会处理文件）
            document.getElementById('pin-content-textarea').value = '';
        } else {
            // 其他模式（默认或CSV）都使用空字符串
            document.getElementById('pin-content-textarea').value = '';
        }

        const formData = new FormData(form);
        // use_default_pin 将不再需要，因为后端会根据pin_mode来判断
        
        fetch('/start-simulation', { method: 'POST', body: formData })
            .then(res => res.json())
            .then(data => {
                if (data.results_url) window.location.href = data.results_url;
                else {
                    alert('提交失败: ' + (data.error || '未知错误'));
                    submitButton.disabled = false; submitButton.textContent = '开始模拟';
                }
            });
    });


}
initializeApp();