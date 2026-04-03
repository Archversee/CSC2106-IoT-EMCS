const MAXPTS = 25;

const chartCfg = (datasets) => ({
    type: 'line',
    data: { labels: [], datasets },
    options: {
        animation: false, responsive: true, maintainAspectRatio: false,
        plugins: {
            legend: {
                labels: { color: '#6b7a96', font: { family: 'IBM Plex Mono', size: 10 }, boxWidth: 10 }
            }
        },
        scales: {
            x: {
                ticks: { color: '#3d4a62', font: { family: 'IBM Plex Mono', size: 9 }, maxTicksLimit: 5 },
                grid: { color: 'rgba(37,43,56,.8)' }
            },
            y: {
                beginAtZero: true,
                ticks: { color: '#3d4a62', font: { family: 'IBM Plex Mono', size: 9 } },
                grid: { color: 'rgba(37,43,56,.8)' }
            }
        }
    }
});

// Sequence chart - endpoint nodes only (a02, a04, pico)
const sensorChart = new Chart(
    document.getElementById('sensorChart').getContext('2d'),
    chartCfg([
        { label: 'Arduino-02', data: [], borderColor: '#22d3ee', backgroundColor: 'rgba(34,211,238,.05)', borderWidth: 1.5, pointRadius: 3, tension: .3, fill: true },
        { label: 'Arduino-04', data: [], borderColor: '#4ade80', backgroundColor: 'rgba(74,222,128,.05)', borderWidth: 1.5, pointRadius: 3, tension: .3, fill: false },
        { label: 'Pico W', data: [], borderColor: '#fbbf24', backgroundColor: 'rgba(251,191,36,.05)', borderWidth: 1.5, pointRadius: 3, tension: .3, fill: false }
    ])
);

const rateChart = new Chart(
    document.getElementById('rateChart').getContext('2d'),
    chartCfg([
        { label: 'Flooding', data: [], borderColor: '#f97316', backgroundColor: 'rgba(249,115,22,.07)', borderWidth: 1.5, pointRadius: 2, tension: .3, fill: true },
        { label: 'Routing', data: [], borderColor: '#22d3ee', backgroundColor: 'rgba(34,211,238,.07)', borderWidth: 1.5, pointRadius: 2, tension: .3, fill: true }
    ])
);

const dupeChart = new Chart(
    document.getElementById('dupeChart').getContext('2d'),
    {
        type: 'bar',
        data: {
            labels: ['Dupes', 'Gaps'],
            datasets: [
                { label: 'Flooding', data: [0, 0], backgroundColor: 'rgba(249,115,22,.7)' },
                { label: 'Routing', data: [0, 0], backgroundColor: 'rgba(34,211,238,.7)' }
            ]
        },
        options: {
            animation: false, responsive: true, maintainAspectRatio: false,
            plugins: { legend: { labels: { color: '#6b7a96', font: { family: 'IBM Plex Mono', size: 10 }, boxWidth: 10 } } },
            scales: {
                x: { ticks: { color: '#3d4a62', font: { family: 'IBM Plex Mono', size: 9 } }, grid: { color: 'rgba(37,43,56,.8)' } },
                y: { beginAtZero: true, ticks: { color: '#3d4a62', font: { family: 'IBM Plex Mono', size: 9 } }, grid: { color: 'rgba(37,43,56,.8)' } }
            }
        }
    }
);

const intChart = new Chart(
    document.getElementById('intChart').getContext('2d'),
    chartCfg([
        { label: 'Flooding interval (s)', data: [], borderColor: '#f97316', backgroundColor: 'rgba(249,115,22,.05)', borderWidth: 1.5, pointRadius: 2, tension: .3, fill: false },
        { label: 'Routing interval (s)', data: [], borderColor: '#22d3ee', backgroundColor: 'rgba(34,211,238,.05)', borderWidth: 1.5, pointRadius: 2, tension: .3, fill: false }
    ])
);

function pushChart(ch, lbl, ...datasetVals) {
    ch.data.labels.push(lbl);
    datasetVals.forEach((v, i) => ch.data.datasets[i].data.push(v));
    if (ch.data.labels.length > MAXPTS) {
        ch.data.labels.shift();
        ch.data.datasets.forEach(d => d.data.shift());
    }
    ch.update();
}

// Rate flush - every 10 s
setInterval(() => {
    const lbl = new Date().toLocaleTimeString('en-GB');
    pushChart(rateChart, lbl, bucketFlood, bucketRoute);
    bucketFlood = 0; bucketRoute = 0;
}, 10000);