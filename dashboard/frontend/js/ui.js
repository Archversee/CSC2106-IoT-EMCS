// Generic DOM helpers
function set(id, val) {
    const el = document.getElementById(id);
    if (el) el.textContent = val;
}

function setBar(id, pct) {
    const el = document.getElementById(id);
    if (el) el.style.width = Math.min(100, pct) + '%';
}

function fmtDur(ms) {
    if (!ms) return '—';
    const s = Math.floor(ms / 1000);
    return String(Math.floor(s / 60)).padStart(2, '0') + ':' + String(s % 60).padStart(2, '0');
}

// Live feed
function addFeed(time, topic, msg, cls, meta1 = '', meta2 = '') {
    const feed = document.getElementById('feed');
    const el = document.createElement('div');
    el.className = 'feed-entry ' + cls;
    el.innerHTML = `<div class="feed-t">${time}</div>`
        + `<div class="feed-top">${topic}</div>`
        + `<div class="feed-msg">${msg}</div>`
        + (meta1 || meta2 ? `<div class="feed-meta">${[meta1, meta2].filter(Boolean).join(' · ')}</div>` : '');
    const label = feed.querySelector('.feed-label');
    label ? label.after(el) : feed.prepend(el);
    if (feed.children.length > 82) feed.lastChild.remove();
}

// Event log
function addEvent(time, node, topic, seq, isDupe, gap, mode) {
    const log = document.getElementById('evt-log');
    const el = document.createElement('div');
    el.className = 'evt';
    const badgeClass = isDupe ? 'dupe' : gap > 0 ? 'gap' : 'ok';
    const badgeTxt = isDupe ? 'DUPE' : gap > 0 ? `GAP+${gap}` : 'OK';
    el.innerHTML = `<span class="evt-time">${time}</span>`
        + `<span class="evt-node">${node || '—'}</span>`
        + `<span class="evt-topic">${topic}</span>`
        + `<span class="evt-seq">seq ${seq !== null ? seq : '—'}</span>`
        + `<span class="badge ${badgeClass}">${badgeTxt}</span>`;
    log.prepend(el);
    if (log.children.length > 80) log.lastChild.remove();
}

// Arduino / Pico message rows
function addMsgRow(listId, time, topic, msg) {
    const list = document.getElementById(listId);
    const el = document.createElement('div');
    el.className = 'msg-row';
    el.innerHTML = `<span class="msg-t">${time}</span>`
        + `<span class="msg-top">${topic}</span>`
        + `<span class="msg-val">${msg}</span>`;
    list.prepend(el);
    if (list.children.length > 50) list.lastChild.remove();
}

// Tab switching 
function switchTab(name, btn) {
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('on'));
    document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('on'));
    btn.classList.add('on');
    document.getElementById('pane-' + name).classList.add('on');
}