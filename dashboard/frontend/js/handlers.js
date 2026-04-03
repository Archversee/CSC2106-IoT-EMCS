/**
 * handlers.js — Socket event handlers for run_update and mqtt_message
 * Depends on: socket.js, charts.js, ui.js, topics.js
 *
 * Node roles:
 *   a02, a04       — endpoint arduinos (connect to broker, appear in sidebar)
 *   a03, a05       — relay-only arduinos (never connect to broker; their names
 *                    can still appear as the `node` field in forwarded payloads)
 *   pico           — Pico W endpoint
 *
 * Sidebar devices show endpoint nodes only (a02, a04, pico).
 * Charts, event log, feed, and Arduino tab show ALL nodes.
 */

// Nodes that have sidebar device cards + traffic bars
const SIDEBAR_NODES = ['a02', 'a04', 'pico'];

// Nodes tracked in the compare tab per-node section
const ENDPOINT_NODES = ['a02', 'a04', 'pico'];

// ── Node name → bucket key ────────────────────────────────────────────────
function nodeToBkt(node) {
    if (!node) return null;
    if (node === 'arduino-02') return 'a02';
    if (node === 'arduino-03') return 'a03';
    if (node === 'arduino-04') return 'a04';
    if (node === 'arduino-05') return 'a05';
    if (node.startsWith('pico')) return 'pico';
    return null;
}

// ── run_update ────────────────────────────────────────────────────────────
socket.on('run_update', (data) => {
    const f = data.flooding, r = data.routing;

    // Stat bar
    document.getElementById('s-total').textContent = data.allTime.total;
    document.getElementById('s-flood-pkts').textContent = f.pkts;
    document.getElementById('s-route-pkts').textContent = r.pkts;
    document.getElementById('s-flood-dupes').textContent = f.dupes;
    document.getElementById('s-flood-del').textContent = f.deliveryPct ? f.deliveryPct + '%' : '—';
    document.getElementById('s-route-del').textContent = r.deliveryPct ? r.deliveryPct + '%' : '—';

    // Compare cols
    set('c-f-pkts', f.pkts);
    set('c-f-hops', f.inferredHops);
    set('c-f-dupes', f.dupes);
    set('c-f-gaps', f.gaps);
    set('c-f-del', f.deliveryPct ? f.deliveryPct + '%' : '—');
    set('c-f-intv', f.avgInterval || '—');
    set('c-f-dur', fmtDur(f.duration));
    set('c-r-pkts', r.pkts);
    set('c-r-hops', r.inferredHops);
    set('c-r-dupes', r.dupes);
    set('c-r-gaps', r.gaps);
    set('c-r-del', r.deliveryPct ? r.deliveryPct + '%' : '—');
    set('c-r-intv', r.avgInterval || '—');
    set('c-r-dur', fmtDur(r.duration));

    // Per-node traffic bars (compare tab)
    const maxN = Math.max(...ENDPOINT_NODES.map(n => Math.max(f.perNode[n] || 0, r.perNode[n] || 0)), 1);
    ENDPOINT_NODES.forEach(n => {
        const fv = f.perNode[n] || 0, rv = r.perNode[n] || 0;
        setBar('nb-f-' + n, fv / maxN * 100);
        setBar('nb-r-' + n, rv / maxN * 100);
        set('nv-f-' + n, fv);
        set('nv-r-' + n, rv);
        const fTot = f.pkts + f.gaps || 1, rTot = r.pkts + r.gaps || 1;
        set('del-f-' + n, fv > 0 ? Math.round(fv / fTot * 100) + '%' : '—');
        set('del-r-' + n, rv > 0 ? Math.round(rv / rTot * 100) + '%' : '—');
    });

    // Dupe chart
    dupeChart.data.datasets[0].data = [f.dupes, f.gaps];
    dupeChart.data.datasets[1].data = [r.dupes, r.gaps];
    dupeChart.update();

    // Sidebar device traffic bars
    const at = data.allTime;
    const atMax = Math.max(...SIDEBAR_NODES.map(n => at.perNode[n] || 0), 1);
    SIDEBAR_NODES.forEach(n => {
        setBar('tbar-' + n, (at.perNode[n] || 0) / atMax * 100);
        set('dpkts-' + n, at.perNode[n] || 0);
    });

    // Restore timer after page reload mid-run
    if (data.activeRun && data.activeRun !== activeRun) {
        const runInfo = data[data.activeRun];
        if (runInfo && runInfo.startTime && !runTimerIv) {
            activeRun = data.activeRun;
            runStartMs[data.activeRun] = runInfo.startTime;
            document.getElementById('stop-btn').style.display = 'block';
            document.getElementById('status-' + data.activeRun).textContent = 'Running…';
            document.getElementById('timer-' + data.activeRun).className =
                'run-timer ' + (data.activeRun === 'flooding' ? 'flood' : 'route');
            _startTimerInterval(data.activeRun);
        }
    }

    applyMode(data.currentMode);
});

// ── mqtt_message ──────────────────────────────────────────────────────────
socket.on('mqtt_message', (data) => {
    const { topic, message: msg, time, node, seq, isDupe, gap, isCmd, mode } = data;

    // Always push to topics pane
    topicsIngest(topic, msg, time);

    if (isCmd) {
        addFeed(time, topic, msg, 'cmd');
        return;
    }

    const bkt = nodeToBkt(node);
    const isPico = bkt === 'pico';

    // Update sidebar device status — only nodes that have a card
    if (SIDEBAR_NODES.includes(bkt)) {
        const dot = document.getElementById('dot-' + bkt);
        const devEl = document.getElementById('dev-' + bkt);
        if (dot) dot.className = 'dev-dot on';
        if (devEl) devEl.className = 'dev online';
        const lastEl = document.getElementById('dlast-' + bkt);
        if (lastEl) lastEl.textContent = time;
    }

    // Sensor chart — datasets: [a02, a04, pico] (matches charts.js order)
    if (seq !== null && bkt) {
        sensorChart.data.labels.push(time);
        sensorChart.data.datasets[0].data.push(bkt === 'a02' ? seq : null);
        sensorChart.data.datasets[1].data.push(bkt === 'a04' ? seq : null);
        sensorChart.data.datasets[2].data.push(isPico ? seq : null);
        if (sensorChart.data.labels.length > MAXPTS) {
            sensorChart.data.labels.shift();
            sensorChart.data.datasets.forEach(d => d.data.shift());
        }
        sensorChart.update();
    }

    // Interval chart
    const now = Date.now();
    if (activeRun && lastMsgTime[activeRun] !== null) {
        const intv = (now - lastMsgTime[activeRun]) / 1000;
        if (intv < 60) {
            pushChart(intChart, time,
                activeRun === 'flooding' ? intv : null,
                activeRun === 'routing' ? intv : null
            );
        }
    }
    if (activeRun) lastMsgTime[activeRun] = now;

    // Rate buckets
    if (mode === 'flooding') bucketFlood++;
    else if (mode === 'routing') bucketRoute++;

    // Events tab — all nodes
    addEvent(time, node, topic, seq, isDupe, gap, mode);

    // Live feed — all nodes
    const feedClass = isDupe ? 'dupe' : mode === 'flooding' ? 'flood' : 'route';
    const metaTxt = isDupe ? '⚠ DUPLICATE forward' : gap > 0 ? `⚠ GAP: ${gap} pkt(s) lost` : '';
    addFeed(time, topic, msg, feedClass, mode ? mode.toUpperCase() : '', metaTxt);

    // Arduino / Pico message list tabs — all nodes
    addMsgRow(isPico ? 'list-pico' : 'list-arduino', time, topic, msg);
});