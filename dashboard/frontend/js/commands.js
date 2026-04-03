/**
 * QoS 0: GET /cmd/qos0?topic=...&message=...
 * QoS 1/2: POST /cmd/qos  { topic, message, qos }
 */

// QoS 0 
function sendCmd(msg, topic = 'sensors/cmd') {
    fetch('/cmd/qos0?topic=' + encodeURIComponent(topic) + '&message=' + encodeURIComponent(msg));
    addFeed(new Date().toLocaleTimeString('en-GB'), topic, '→ ' + msg, 'cmd');
}

// QoS 1 / 2 
function sendCmdQoS(msg, qos, topic = 'sensors/cmd') {
    fetch('/cmd/qos', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ topic, message: msg, qos })
    });
    addFeed(
        new Date().toLocaleTimeString('en-GB'),
        topic,
        '→ ' + msg,
        'cmd',
        `QoS ${qos}`
    );
}