/**
 * socket.js — Socket.IO connection + shared state
 */

const socket = io();

// ── Shared state ──────────────────────────────────────────────────────────
let currentMode = 'flooding';
let activeRun = null;
let runTimerIv = null;
const runStartMs = { flooding: null, routing: null };
const lastMsgTime = { flooding: null, routing: null };

let bucketFlood = 0, bucketRoute = 0;

// ── Connection badges ─────────────────────────────────────────────────────
socket.on('connect', () => {
    const el = document.getElementById('conn');
    el.textContent = '● LIVE'; el.className = 'conn-badge live';
});
socket.on('disconnect', () => {
    const el = document.getElementById('conn');
    el.textContent = '● OFFLINE'; el.className = 'conn-badge';
});

// ── Clock ─────────────────────────────────────────────────────────────────
setInterval(() => {
    document.getElementById('clock').textContent = new Date().toLocaleTimeString('en-GB');
}, 1000);