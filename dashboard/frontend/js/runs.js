/**
 * runs.js — Mode switching + run start/stop controls
 * Depends on: socket.js
 */

// ── Mode ──────────────────────────────────────────────────────────────────
function setMode(mode) {
    fetch('/mode/' + mode);
    applyMode(mode);
}

function applyMode(mode) {
    currentMode = mode;
    document.getElementById('pill-flood').className = 'mode-pill' + (mode === 'flooding' ? ' active-flood' : '');
    document.getElementById('pill-route').className = 'mode-pill' + (mode === 'routing' ? ' active-route' : '');
    const led = document.getElementById('mode-led');
    const txt = document.getElementById('mode-txt');
    led.className = 'mode-led ' + (mode === 'flooding' ? 'flood' : 'route');
    txt.style.color = mode === 'flooding' ? 'var(--flood)' : 'var(--route)';
    txt.textContent = mode === 'flooding' ? 'FLOODING' : 'ROUTING';
}

socket.on('mode_change', (data) => applyMode(data.mode));

// ── Run control ───────────────────────────────────────────────────────────
function startRun(mode) {
    fetch('/run/start/' + mode);
    applyMode(mode);
    activeRun = mode;
    runStartMs[mode] = Date.now();
    document.getElementById('stop-btn').style.display = 'block';
    document.getElementById('status-' + mode).textContent = 'Running…';
    document.getElementById('timer-' + mode).className = 'run-timer ' + (mode === 'flooding' ? 'flood' : 'route');
    _startTimerInterval(mode);
}

function _startTimerInterval(mode) {
    clearInterval(runTimerIv);
    runTimerIv = setInterval(() => {
        if (!runStartMs[mode]) return;
        const e = Math.floor((Date.now() - runStartMs[mode]) / 1000);
        document.getElementById('timer-' + mode).textContent =
            String(Math.floor(e / 60)).padStart(2, '0') + ':' + String(e % 60).padStart(2, '0');
    }, 1000);
}

function stopRun() {
    fetch('/run/stop');
    clearInterval(runTimerIv);
    document.getElementById('stop-btn').style.display = 'none';
    if (activeRun) {
        document.getElementById('status-' + activeRun).textContent = 'Done';
        document.getElementById('timer-' + activeRun).className = 'run-timer idle';
        activeRun = null;
    }
}

function resetAll() {
    if (!confirm('Reset all run data?')) return;
    fetch('/reset');
}

function exportData() {
    window.open('/export', '_blank');
}

// ── Run socket events ─────────────────────────────────────────────────────
socket.on('run_started', (data) => {
    activeRun = data.mode;
    runStartMs[data.mode] = data.startTime;
    document.getElementById('stop-btn').style.display = 'block';
    document.getElementById('status-' + data.mode).textContent = 'Running…';
    document.getElementById('timer-' + data.mode).className =
        'run-timer ' + (data.mode === 'flooding' ? 'flood' : 'route');
    _startTimerInterval(data.mode);
});

socket.on('run_stopped', (data) => {
    document.getElementById('status-' + data.mode).textContent = 'Done';
    document.getElementById('timer-' + data.mode).className = 'run-timer idle';
    if (activeRun === data.mode) {
        activeRun = null;
        document.getElementById('stop-btn').style.display = 'none';
    }
});