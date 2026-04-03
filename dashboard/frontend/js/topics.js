/**
 * Tracked topics:
 *   sensors/data          — all nodes
 *   sensors/arduino/data  — arduino only
 *   sensors/pico/data     — pico only
 *   sensors/cmd           — commands
 */

const TOPICS = [
  { id: 'all', label: 'sensors/data', topic: 'sensors/data' },
  { id: 'arduino', label: 'sensors/arduino/data', topic: 'sensors/arduino/data' },
  { id: 'pico', label: 'sensors/pico/data', topic: 'sensors/pico/data' },
  { id: 'cmd', label: 'sensors/cmd', topic: 'sensors/cmd' },
];

// Per-topic message stores (max 60 each)
const topicStore = {};
TOPICS.forEach(t => { topicStore[t.id] = []; });

let activeTopicId = 'all';

// Incoming messages → store
function topicsIngest(topic, msg, time) {
  const match = TOPICS.find(t => t.topic === topic);
  if (!match) return;

  const entry = { time, msg };
  topicStore[match.id].unshift(entry);
  if (topicStore[match.id].length > 60) topicStore[match.id].pop();

  // Update count badge
  const countEl = document.getElementById('tc-' + match.id);
  if (countEl) countEl.textContent = topicStore[match.id].length + ' msgs';

  // Live-append if this topic is active
  if (match.id === activeTopicId) {
    prependTopicMsg(match.id, time, msg);
  }
}

function prependTopicMsg(topicId, time, msg) {
  const stream = document.getElementById('tstream-' + topicId);
  if (!stream) return;
  const el = document.createElement('div');
  el.className = 'topic-msg';
  el.innerHTML = `<span class="msg-t">${time}</span><span class="msg-val">${msg}</span>`;
  stream.prepend(el);
  if (stream.children.length > 60) stream.lastChild.remove();
}

// Switch active topic tab
function switchTopicTab(topicId) {
  activeTopicId = topicId;

  document.querySelectorAll('.topic-tab').forEach(btn => {
    btn.classList.toggle('on', btn.dataset.topicId === topicId);
  });

  // Re-render stream from store
  const stream = document.getElementById('tstream-' + topicId);
  if (!stream) return;
  stream.innerHTML = '';
  topicStore[topicId].forEach(({ time, msg }) => {
    const el = document.createElement('div');
    el.className = 'topic-msg';
    el.innerHTML = `<span class="msg-t">${time}</span><span class="msg-val">${msg}</span>`;
    stream.appendChild(el);
  });
}

// Build the topics pane DOM
function buildTopicsPane() {
  const pane = document.getElementById('pane-topics');
  if (!pane) return;

  // Tab strip
  const tabStrip = document.createElement('div');
  tabStrip.className = 'topic-tabs-row';
  TOPICS.forEach(t => {
    const btn = document.createElement('button');
    btn.className = 'topic-tab' + (t.id === activeTopicId ? ' on' : '');
    btn.dataset.topicId = t.id;
    btn.textContent = t.label;
    btn.onclick = () => switchTopicTab(t.id);
    tabStrip.appendChild(btn);
  });
  pane.appendChild(tabStrip);

  // One stream div per topic (only active one shown via CSS-less trick — all rendered, scroll hidden)
  TOPICS.forEach(t => {
    const wrapper = document.createElement('div');
    wrapper.id = 'twrapper-' + t.id;
    wrapper.style.display = t.id === activeTopicId ? 'block' : 'none';

    const header = document.createElement('div');
    header.className = 'topic-header';
    header.innerHTML = `<span class="topic-name">${t.topic}</span><span class="topic-count" id="tc-${t.id}">0 msgs</span>`;

    const stream = document.createElement('div');
    stream.id = 'tstream-' + t.id;
    stream.className = 'topic-stream';

    wrapper.appendChild(header);
    wrapper.appendChild(stream);
    pane.appendChild(wrapper);
  });
}

// Override switchTopicTab to also toggle wrapper visibility
const _switchTopicTab = switchTopicTab;
function switchTopicTab(topicId) {
  activeTopicId = topicId;

  document.querySelectorAll('.topic-tab').forEach(btn => {
    btn.classList.toggle('on', btn.dataset.topicId === topicId);
  });

  TOPICS.forEach(t => {
    const w = document.getElementById('twrapper-' + t.id);
    if (w) w.style.display = t.id === topicId ? 'block' : 'none';
  });

  // Re-render stream
  const stream = document.getElementById('tstream-' + topicId);
  if (!stream) return;
  stream.innerHTML = '';
  topicStore[topicId].forEach(({ time, msg }) => {
    const el = document.createElement('div');
    el.className = 'topic-msg';
    el.innerHTML = `<span class="msg-t">${time}</span><span class="msg-val">${msg}</span>`;
    stream.appendChild(el);
  });
}

// Init
document.addEventListener('DOMContentLoaded', buildTopicsPane);