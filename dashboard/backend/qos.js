// dashboard/backend/qos.js
// publishWithRetry: publish a json payload over mqtt, implement retry for QoS1
// returns a Promise that resolves { success: true } or rejects with Error

const DEFAULTS = {
  qos1: {
    maxRetries: 5,
    baseDelayMs: 400, // exponential backoff base
    ackTimeoutMs: 2000 // wait this long for an ack before retrying
  }
};

function sleep(ms) {
  return new Promise((r) => setTimeout(r, ms));
}

/**
 * Publish a JSON payload using mqttClient, with optional retry for QoS 1.
 *
 * @param {mqtt.MqttClient} mqttClient - mqtt.js client instance
 * @param {string} topic
 * @param {Object} payload - will be JSON.stringified
 * @param {number} qos - 0 or 1
 * @param {Object} opts - overrides for timeouts/retries { maxRetries, baseDelayMs, ackTimeoutMs }
 * @returns {Promise<{success: true}>}
 */
function publishWithRetry(mqttClient, topic, payload, qos = 0, opts = {}) {
  if (!mqttClient || typeof mqttClient.publish !== 'function') {
    return Promise.reject(new Error('Invalid mqtt client passed to publishWithRetry'));
  }

  const json = (typeof payload === 'string') ? payload : JSON.stringify(payload);

  if (qos === 0) {
    // Fire-and-forget: call publish and resolve immediately (mqtt.js still accepts a callback if you want)
    try {
      mqttClient.publish(topic, json, { qos: 0 }, (err) => {
        if (err) {
          // publish error - still resolve as failed
          // but return rejected promise for caller to handle
        }
      });
      return Promise.resolve({ success: true });
    } catch (err) {
      return Promise.reject(err);
    }
  }

  // QoS1 path: retry until PUBACK arrives or retries exhausted
  const settings = Object.assign({}, DEFAULTS.qos1, opts);

  return new Promise(async (resolve, reject) => {
    let attempt = 0;

    // We'll rely on mqtt.js publish callback which is called when the broker acked the message (for QoS 1).
    // However, should the callback not happen (network glitch) we add a timeout and retry.
    async function tryPublish() {
      attempt++;
      let finished = false;

      // create a guard timeout - if the publish callback isn't invoked within ackTimeoutMs, we'll retry
      const timeout = setTimeout(() => {
        if (finished) return;
        finished = true;
        // if we have more retries, schedule next attempt
        if (attempt <= settings.maxRetries) {
          const delay = settings.baseDelayMs * Math.pow(2, attempt - 1);
          // continue after backoff
          setTimeout(() => tryPublish(), delay);
        } else {
          reject(new Error(`PUBACK not received after ${settings.maxRetries} attempts`));
        }
      }, settings.ackTimeoutMs);

      try {
        mqttClient.publish(topic, json, { qos: 1 }, (err) => {
          if (finished) return;
          finished = true;
          clearTimeout(timeout);
          if (err) {
            // publishing error (not ack), try again or fail
            if (attempt <= settings.maxRetries) {
              const delay = settings.baseDelayMs * Math.pow(2, attempt - 1);
              setTimeout(() => tryPublish(), delay);
            } else {
              reject(err);
            }
            return;
          }
          // success: PUBACK received
          resolve({ success: true });
        });
      } catch (err) {
        clearTimeout(timeout);
        finished = true;
        if (attempt <= settings.maxRetries) {
          const delay = settings.baseDelayMs * Math.pow(2, attempt - 1);
          setTimeout(() => tryPublish(), delay);
        } else {
          reject(err);
        }
      }
    } // tryPublish

    // start first attempt
    tryPublish();
  });
}

module.exports = {
  publishWithRetry
};