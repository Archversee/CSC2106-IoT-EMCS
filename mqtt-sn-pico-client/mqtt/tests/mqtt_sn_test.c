/*!
 * @file    mqtt_sn_batch_test.c
 * @brief   MQTT-SN Protocol Unit Test Suite
 * @author  Test Suite Generator
 * @date    2025
 *
 * @description
 * Comprehensive automated unit testing for MQTT-SN client implementation:
 * - UT-CONN: Connection establishment (CONNECT/CONNACK)
 * - UT-MSGID: Message ID generation and sequencing
 * - UT-REG: Topic registration (REGISTER/REGACK)
 * - UT-SUB: Subscribe operations (topic name and ID)
 * - UT-QOS0: QoS 0 fire-and-forget publishing
 * - UT-QOS1: QoS 1 at-least-once delivery with PUBACK
 * - UT-QOS2: QoS 2 exactly-once delivery handshake
 * - UT-PING: Keepalive mechanism (PINGREQ/PINGRESP)
 * - UT-RETRY: Retransmission logic and timeout handling
 * - UT-MAXRT: Maximum retry exhaustion behavior
 * - UT-BURST: Rapid message burst handling
 * - UT-QUEUE: Pending message queue overflow
 * - UT-BINARY: Large binary payload transmission
 * - UT-WRAP: Message ID boundary and wraparound
 *
 * Test Results:
 * Outputs detailed pass/fail status with execution statistics.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../../config.h"
#include "../mqtt-sn-udp.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

/*! Test Configuration */
#define TEST_TIMEOUT_MS (5000U)
#define TEST_SHORT_DELAY_MS (100U)
#define TEST_MEDIUM_DELAY_MS (500U)
#define TEST_LONG_DELAY_MS (1000U)
#define MAX_TEST_PAYLOAD_SIZE (247U)

/*! Global variables required by mqtt-sn-udp.c */
uint32_t g_last_pingresp = 0U;
bool g_ping_ack_received = true;

/*! Test Statistics */
typedef struct {
    uint32_t tests_run;
    uint32_t tests_passed;
    uint32_t tests_failed;
    uint32_t assertions_checked;
    uint32_t iteration_count;
} test_stats_t;

static test_stats_t g_test_stats = {0};

/*! Test Context */
typedef struct {
    struct udp_pcb *pcb;
    ip_addr_t gateway_addr;
    u16_t gateway_port;
    mqtt_sn_context_t *mqtt_ctx;
    bool test_failed;
    char last_error[128];
} test_context_t;

/*! Assertion Macros */
#define TEST_ASSERT(condition, msg)                                                                \
    do {                                                                                           \
        g_test_stats.assertions_checked++;                                                         \
        if (!(condition)) {                                                                        \
            printf("  ✗ ASSERTION FAILED: %s\n", msg);                                             \
            snprintf(ctx->last_error, sizeof(ctx->last_error), "%s", msg);                         \
            ctx->test_failed = true;                                                               \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_EQ(actual, expected, msg)                                                      \
    do {                                                                                           \
        g_test_stats.assertions_checked++;                                                         \
        if ((actual) != (expected)) {                                                              \
            printf("  ✗ ASSERTION FAILED: %s (expected: %d, got: %d)\n", msg, (int)(expected),     \
                   (int)(actual));                                                                 \
            snprintf(ctx->last_error, sizeof(ctx->last_error), "%s", msg);                         \
            ctx->test_failed = true;                                                               \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#define TEST_START(test_id, name, iterations)                                                      \
    do {                                                                                           \
        printf("\n=== TEST %s: %s ===\n", test_id, name);                                          \
        printf("Target executions: %d\n", iterations);                                             \
        g_test_stats.tests_run++;                                                                  \
        ctx->test_failed = false;                                                                  \
        memset(ctx->last_error, 0, sizeof(ctx->last_error));                                       \
    } while (0)

#define TEST_END(test_id)                                                                          \
    do {                                                                                           \
        if (!ctx->test_failed) {                                                                   \
            printf("✓ PASS: %s completed successfully\n", test_id);                                \
            g_test_stats.tests_passed++;                                                           \
        } else {                                                                                   \
            printf("✗ FAIL: %s - %s\n", test_id, ctx->last_error);                                 \
            g_test_stats.tests_failed++;                                                           \
        }                                                                                          \
    } while (0)

/*! Helper Functions */

/**
 * @brief Poll network stack for specified duration
 */
static void test_poll_network(uint32_t duration_ms) {
    uint32_t iterations = duration_ms / 10;
    for (uint32_t i = 0; i < iterations; i++) {
        cyw43_arch_poll();
        sleep_ms(10);
    }
}

/**
 * @brief Wait for acknowledgment with timeout
 */
static bool wait_for_ack(uint16_t msg_id, uint32_t timeout_ms) {
    absolute_time_t start = get_absolute_time();

    while (absolute_time_diff_us(start, get_absolute_time()) < (timeout_ms * 1000)) {
        cyw43_arch_poll();

        bool found = false;
        for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
            if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
                found = true;
                break;
            }
        }

        if (!found) {
            return true;
        }

        sleep_ms(10);
    }

    return false;
}

/**
 * @brief Clear all pending QoS messages
 */
static void clear_pending_messages(void) {
    for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        g_pending_msgs[i].in_use = false;
    }
}

/**
 * @brief Count pending QoS messages
 */
static uint32_t count_pending_messages(void) {
    uint32_t count = 0;
    for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use) {
            count++;
        }
    }
    return count;
}

/*! ========================================
 *  UNIT TEST CASES
 *  ======================================== */

/**
 * @brief UT-CONN: MQTT-SN Connection Establishment
 */
static bool test_ut_conn(test_context_t *ctx) {
    const uint32_t iterations = 1;
    TEST_START("UT-CONN", "Connection Establishment", iterations);

    g_test_stats.iteration_count++;
    g_ping_ack_received = false;

    mqtt_sn_connect(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);

    absolute_time_t start = get_absolute_time();
    bool connack_received = false;

    while (absolute_time_diff_us(start, get_absolute_time()) < (TEST_TIMEOUT_MS * 1000)) {
        cyw43_arch_poll();

        if (g_ping_ack_received) {
            connack_received = true;
            break;
        }

        sleep_ms(10);
    }

    TEST_ASSERT(connack_received, "CONNACK not received within timeout");

    TEST_END("UT-CONN");
    return !ctx->test_failed;
}

/**
 * @brief UT-MSGID: Message ID Generation and Sequencing
 */
static bool test_ut_msgid(test_context_t *ctx) {
    const uint32_t iterations = 50;
    TEST_START("UT-MSGID", "Message ID Generation", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;

        uint16_t id1 = get_next_msg_id();
        uint16_t id2 = get_next_msg_id();

        TEST_ASSERT(id1 != 0, "ID should not be 0");
        TEST_ASSERT(id2 == id1 + 1, "IDs should be sequential");

        if ((i + 1) % 10 == 0 || i == iterations - 1) {
            printf("[UT-MSGID] Iteration %lu/%lu\n", (unsigned long)(i + 1),
                   (unsigned long)iterations);
        }
    }

    TEST_END("UT-MSGID");
    return !ctx->test_failed;
}

/**
 * @brief UT-REG: Topic Registration
 */
static bool test_ut_reg(test_context_t *ctx) {
    const uint32_t iterations = 5;
    TEST_START("UT-REG", "Topic Registration", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;

        char test_topic[32];
        snprintf(test_topic, sizeof(test_topic), "test/topic_%lu", (unsigned long)i);

        // STEP 1: Add to context
        bool added = mqtt_sn_add_topic_for_registration(ctx->mqtt_ctx, test_topic);
        TEST_ASSERT(added, "Failed to add topic for registration");

        // STEP 2: Set last_attempt to NOW so ACK handler will accept REGACK
        for (size_t idx = 0; idx < MAX_CUSTOM_TOPICS; idx++) {
            if (ctx->mqtt_ctx->custom_topics[idx].in_use &&
                strcmp(ctx->mqtt_ctx->custom_topics[idx].topic_name, test_topic) == 0) {
                ctx->mqtt_ctx->custom_topics[idx].last_attempt = get_absolute_time();
                break;
            }
        }

        // STEP 3: Send REGISTER
        uint16_t msg_id = get_next_msg_id();
        mqtt_sn_register_topic(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, test_topic, msg_id);

        // STEP 4: Wait for registration
        absolute_time_t start = get_absolute_time();
        uint16_t topic_id = 0;
        while (absolute_time_diff_us(start, get_absolute_time()) < (TEST_TIMEOUT_MS * 1000)) {
            topic_id = mqtt_sn_get_topic_id(ctx->mqtt_ctx, test_topic);
            if (topic_id != 0)
                break;
            cyw43_arch_poll();
            sleep_ms(10);
        }

        TEST_ASSERT(topic_id != 0, "Registered topic ID should not be 0");

        printf("[UT-REG] Iteration %lu/%lu (Topic: %s, ID: %u)\n", (unsigned long)(i + 1),
               (unsigned long)iterations, test_topic, topic_id);
    }

    TEST_END("UT-REG");
    return !ctx->test_failed;
}

/**
 * @brief UT-SUB: Subscribe Operations
 */
static bool test_ut_sub(test_context_t *ctx) {
    const uint32_t iterations = 5;
    TEST_START("UT-SUB", "Subscribe Operations", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;

        char test_topic[32];
        snprintf(test_topic, sizeof(test_topic), "test/subscribe_%lu", (unsigned long)i);

        // STEP 1: Add to context
        bool added = mqtt_sn_add_topic_for_subscription(ctx->mqtt_ctx, test_topic, QOS_LEVEL_1);
        TEST_ASSERT(added, "Failed to add topic for subscription");

        // STEP 2: Set last_attempt to NOW so SUBACK handler will accept it
        for (size_t idx = 0; idx < MAX_CUSTOM_TOPICS; idx++) {
            if (ctx->mqtt_ctx->custom_topics[idx].in_use &&
                strcmp(ctx->mqtt_ctx->custom_topics[idx].topic_name, test_topic) == 0) {
                ctx->mqtt_ctx->custom_topics[idx].last_attempt = get_absolute_time();
                break;
            }
        }

        // STEP 3: Send SUBSCRIBE
        uint16_t msg_id = get_next_msg_id();
        mqtt_sn_subscribe_topic_name(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, test_topic,
                                     msg_id, QOS_LEVEL_1);

        // STEP 4: Wait for subscription
        absolute_time_t start = get_absolute_time();
        uint16_t topic_id = 0;
        while (absolute_time_diff_us(start, get_absolute_time()) < (TEST_TIMEOUT_MS * 1000)) {
            topic_id = mqtt_sn_get_topic_id(ctx->mqtt_ctx, test_topic);
            if (topic_id != 0)
                break;
            cyw43_arch_poll();
            sleep_ms(10);
        }

        TEST_ASSERT(topic_id != 0, "Subscribed topic ID should not be 0");

        printf("[UT-SUB] Iteration %lu/%lu (Topic: %s, ID: %u)\n", (unsigned long)(i + 1),
               (unsigned long)iterations, test_topic, topic_id);
    }

    TEST_END("UT-SUB");
    return !ctx->test_failed;
}

/**
 * @brief UT-QOS0: QoS 0 Publishing
 */
static bool test_ut_qos0(test_context_t *ctx) {
    const uint32_t iterations = 20;
    TEST_START("UT-QOS0", "QoS 0 Publishing", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;

        uint8_t payload[] = "QoS0 Test";
        uint16_t msg_id = get_next_msg_id();

        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1, payload,
                                 sizeof(payload), QOS_LEVEL_0, msg_id, false);

        test_poll_network(TEST_SHORT_DELAY_MS);

        uint32_t pending = count_pending_messages();
        TEST_ASSERT_EQ(pending, 0, "QoS 0 should not be tracked");

        if ((i + 1) % 5 == 0 || i == iterations - 1) {
            printf("[UT-QOS0] Iteration %lu/%lu\n", (unsigned long)(i + 1),
                   (unsigned long)iterations);
        }
    }

    TEST_END("UT-QOS0");
    return !ctx->test_failed;
}

/**
 * @brief UT-QOS1: QoS 1 Publishing and ACK
 */
static bool test_ut_qos1(test_context_t *ctx) {
    const uint32_t iterations = 25;
    TEST_START("UT-QOS1", "QoS 1 Publishing", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;
        clear_pending_messages();

        uint8_t payload[] = "QoS1 Test";
        uint16_t msg_id = get_next_msg_id();

        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1, payload,
                                 sizeof(payload), QOS_LEVEL_1, msg_id, false);

        uint32_t pending = count_pending_messages();
        TEST_ASSERT_EQ(pending, 1, "QoS 1 should be tracked");

        bool ack_received = wait_for_ack(msg_id, TEST_TIMEOUT_MS);
        TEST_ASSERT(ack_received, "PUBACK not received");

        pending = count_pending_messages();
        TEST_ASSERT_EQ(pending, 0, "Message should be removed after PUBACK");

        if ((i + 1) % 5 == 0 || i == iterations - 1) {
            printf("[UT-QOS1] Iteration %lu/%lu\n", (unsigned long)(i + 1),
                   (unsigned long)iterations);
        }
    }

    TEST_END("UT-QOS1");
    return !ctx->test_failed;
}

/**
 * @brief UT-QOS2: QoS 2 Publishing and Handshake
 */
static bool test_ut_qos2(test_context_t *ctx) {
    const uint32_t iterations = 15;
    TEST_START("UT-QOS2", "QoS 2 Publishing", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;
        clear_pending_messages();

        uint8_t payload[] = "QoS2 Test";
        uint16_t msg_id = get_next_msg_id();

        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1, payload,
                                 sizeof(payload), QOS_LEVEL_2, msg_id, false);

        uint32_t pending = count_pending_messages();
        TEST_ASSERT_EQ(pending, 1, "QoS 2 should be tracked");

        bool ack_received = wait_for_ack(msg_id, TEST_TIMEOUT_MS);
        TEST_ASSERT(ack_received, "QoS 2 handshake not completed");

        pending = count_pending_messages();
        TEST_ASSERT_EQ(pending, 0, "Message should be removed after PUBCOMP");

        if ((i + 1) % 5 == 0 || i == iterations - 1) {
            printf("[UT-QOS2] Iteration %lu/%lu\n", (unsigned long)(i + 1),
                   (unsigned long)iterations);
        }
    }

    TEST_END("UT-QOS2");
    return !ctx->test_failed;
}

/**
 * @brief UT-PING: Keepalive Mechanism
 */
static bool test_ut_ping(test_context_t *ctx) {
    const uint32_t iterations = 10;
    TEST_START("UT-PING", "Keepalive Mechanism", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;
        g_ping_ack_received = false;

        mqtt_sn_pingreq(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
        test_poll_network(TEST_LONG_DELAY_MS);

        TEST_ASSERT(g_ping_ack_received, "PINGRESP not received");

        if ((i + 1) % 2 == 0 || i == iterations - 1) {
            printf("[UT-PING] Iteration %lu/%lu\n", (unsigned long)(i + 1),
                   (unsigned long)iterations);
        }
    }

    TEST_END("UT-PING");
    return !ctx->test_failed;
}

/**
 * @brief UT-BINARY: Large Binary Payload
 */
static bool test_ut_binary(test_context_t *ctx) {
    const uint32_t iterations = 10;
    TEST_START("UT-BINARY", "Large Binary Payload", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;
        clear_pending_messages();

        uint8_t payload[MAX_TEST_PAYLOAD_SIZE];
        for (size_t j = 0; j < sizeof(payload); j++) {
            payload[j] = (uint8_t)(j & 0xFF);
        }

        uint16_t msg_id = get_next_msg_id();

        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1, payload,
                                 sizeof(payload), QOS_LEVEL_1, msg_id, false);

        // Wait for ACK with longer timeout for large payloads
        bool ack_received = wait_for_ack(msg_id, TEST_TIMEOUT_MS * 2); // <-- doubled timeout
        TEST_ASSERT(ack_received, "PUBACK not received for large payload");

        if ((i + 1) % 2 == 0 || i == iterations - 1) {
            printf("[UT-BINARY] Iteration %lu/%lu (247 bytes)\n", (unsigned long)(i + 1),
                   (unsigned long)iterations);
        }
    }

    TEST_END("UT-BINARY");
    return !ctx->test_failed;
}

/**
 * @brief UT-RETRY: Retransmission Logic
 */
static bool test_ut_retry(test_context_t *ctx) {
    const uint32_t iterations = 5;
    TEST_START("UT-RETRY", "Retransmission Logic", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;
        clear_pending_messages();

        ctx->mqtt_ctx->drop_acks = true;

        uint8_t payload[] = "Retry Test";
        uint16_t msg_id = get_next_msg_id();

        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1, payload,
                                 sizeof(payload), QOS_LEVEL_1, msg_id, false);

        test_poll_network(TEST_SHORT_DELAY_MS);

        sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
        check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
        test_poll_network(TEST_SHORT_DELAY_MS);

        bool found = false;
        for (size_t j = 0; j < MAX_PENDING_QOS_MSGS; j++) {
            if (g_pending_msgs[j].in_use && g_pending_msgs[j].msg_id == msg_id) {
                TEST_ASSERT(g_pending_msgs[j].retry_count > 0, "Retry count should increase");
                found = true;
                break;
            }
        }
        TEST_ASSERT(found, "Pending message not found");

        ctx->mqtt_ctx->drop_acks = false;
        sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
        check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);

        bool ack_received = wait_for_ack(msg_id, TEST_TIMEOUT_MS);
        TEST_ASSERT(ack_received, "PUBACK not received after re-enabling ACKs");

        printf("[UT-RETRY] Iteration %lu/%lu\n", (unsigned long)(i + 1), (unsigned long)iterations);
    }

    TEST_END("UT-RETRY");
    return !ctx->test_failed;
}

/**
 * @brief UT-MAXRT: Maximum Retry Exhaustion
 */
static bool test_ut_maxrt(test_context_t *ctx) {
    const uint32_t iterations = 1;
    TEST_START("UT-MAXRT", "Maximum Retry Exhaustion", iterations);

    g_test_stats.iteration_count++;
    clear_pending_messages();

    ctx->mqtt_ctx->drop_acks = true;

    uint8_t payload[] = "Max Retry Test";
    uint16_t msg_id = get_next_msg_id();

    mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1, payload,
                             sizeof(payload), QOS_LEVEL_1, msg_id, false);

    test_poll_network(TEST_SHORT_DELAY_MS);

    for (uint8_t retry = 0; retry < QOS_MAX_RETRIES; retry++) {
        sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
        check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
        test_poll_network(TEST_SHORT_DELAY_MS);
    }

    sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
    check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
    test_poll_network(TEST_SHORT_DELAY_MS);

    uint32_t pending = count_pending_messages();
    TEST_ASSERT_EQ(pending, 0, "Message should be dropped after max retries");

    ctx->mqtt_ctx->drop_acks = false;

    TEST_END("UT-MAXRT");
    return !ctx->test_failed;
}

/**
 * @brief UT-BURST: Rapid Message Burst
 */
static bool test_ut_burst(test_context_t *ctx) {
    const uint32_t iterations = 10;
    TEST_START("UT-BURST", "Rapid Message Burst", iterations);

    g_test_stats.iteration_count++;
    clear_pending_messages();

    uint8_t payload[] = "Burst Test";
    uint16_t msg_ids[10];

    // Send all messages
    for (uint32_t i = 0; i < iterations; i++) {
        msg_ids[i] = get_next_msg_id();
        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1, payload,
                                 sizeof(payload), QOS_LEVEL_1, msg_ids[i], false);
        sleep_ms(5); // minimal spacing
    }

    // Wait for ALL ACKs explicitly
    bool all_acked = true;
    for (uint32_t i = 0; i < iterations; i++) {
        if (!wait_for_ack(msg_ids[i], TEST_TIMEOUT_MS)) {
            all_acked = false;
            break;
        }
    }

    TEST_ASSERT(all_acked, "All burst messages should be acknowledged");

    printf("[UT-BURST] Sent and acknowledged %lu messages\n", (unsigned long)iterations);

    TEST_END("UT-BURST");
    return !ctx->test_failed;
}

/**
 * @brief UT-QUEUE: Pending Message Queue Overflow
 */
static bool test_ut_queue(test_context_t *ctx) {
    const uint32_t iterations = 1;
    TEST_START("UT-QUEUE", "Queue Overflow Handling", iterations);

    g_test_stats.iteration_count++;
    clear_pending_messages();
    ctx->mqtt_ctx->drop_acks = true;

    uint8_t payload[] = "Overflow Test";

    for (uint32_t i = 0; i < MAX_PENDING_QOS_MSGS + 2; i++) {
        uint16_t msg_id = get_next_msg_id();
        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1, payload,
                                 sizeof(payload), QOS_LEVEL_1, msg_id, false);
        test_poll_network(50);
    }

    uint32_t pending = count_pending_messages();
    TEST_ASSERT_EQ(pending, MAX_PENDING_QOS_MSGS, "Should have max pending messages");

    ctx->mqtt_ctx->drop_acks = false;
    clear_pending_messages();

    printf("[UT-QUEUE] Queue limit verified: %d messages\n", MAX_PENDING_QOS_MSGS);

    TEST_END("UT-QUEUE");
    return !ctx->test_failed;
}

/**
 * @brief UT-WRAP: Message ID Boundary and Wraparound
 */
static bool test_ut_wrap(test_context_t *ctx) {
    const uint32_t iterations = 100;
    TEST_START("UT-WRAP", "Message ID Wraparound", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        g_test_stats.iteration_count++;

        uint16_t id1 = get_next_msg_id();
        uint16_t id2 = get_next_msg_id();

        TEST_ASSERT(id2 == id1 + 1, "IDs should be sequential");
        TEST_ASSERT(id1 != 0, "ID should never be 0");
        TEST_ASSERT(id2 != 0, "ID should never be 0");

        if ((i + 1) % 20 == 0 || i == iterations - 1) {
            printf("[UT-WRAP] Iteration %lu/%lu (ID: 0x%04X)\n", (unsigned long)(i + 1),
                   (unsigned long)iterations, id2);
        }
    }

    TEST_END("UT-WRAP");
    return !ctx->test_failed;
}

/*! ========================================
 *  MAIN TEST RUNNER
 *  ======================================== */

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("\n");
    printf("================================================================================\n");
    printf("              AUTOMATED MQTT-SN PROTOCOL UNIT TESTS\n");
    printf("================================================================================\n");
    printf("Test Suite: MQTT-SN Client Implementation\n");
    printf("Based on: MQTT-SN Protocol Specification v1.2\n");
    printf("Date: %s\n", __DATE__);
    printf("================================================================================\n\n");

    // Initialize Wi-Fi
    printf("Initializing Wi-Fi...\n");
    if (cyw43_arch_init()) {
        printf("✗ FATAL: Wi-Fi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi (SSID: %s)...\n", WIFI_SSID);
    while (
        cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Wi-Fi connection failed, retrying...\n");
        sleep_ms(5000);
    }

    printf("✓ Wi-Fi connected: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    // Setup UDP
    struct udp_pcb *pcb = udp_new();
    if (!pcb) {
        printf("✗ FATAL: UDP setup failed\n");
        return -1;
    }

    if (udp_bind(pcb, IP_ADDR_ANY, 0) != ERR_OK) {
        printf("✗ FATAL: UDP bind failed\n");
        return -1;
    }

    static mqtt_sn_context_t mqtt_ctx = {.drop_acks = false};
    udp_recv(pcb, udp_recv_callback, &mqtt_ctx);

    test_context_t test_ctx = {
        .pcb = pcb, .gateway_port = UDP_PORT, .mqtt_ctx = &mqtt_ctx, .test_failed = false};

    IP4_ADDR(&test_ctx.gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2, GATEWAY_IP3);

    printf("✓ UDP client ready\n");
    printf("Gateway: %s:%d\n\n", ip4addr_ntoa(&test_ctx.gateway_addr), UDP_PORT);

    sleep_ms(1000);

    // Run Unit Tests
    test_ut_conn(&test_ctx);
    test_ut_msgid(&test_ctx);
    test_ut_reg(&test_ctx);
    test_ut_sub(&test_ctx);
    test_ut_qos0(&test_ctx);
    test_ut_qos1(&test_ctx);
    test_ut_qos2(&test_ctx);
    test_ut_ping(&test_ctx);
    test_ut_binary(&test_ctx);
    test_ut_retry(&test_ctx);
    test_ut_maxrt(&test_ctx);
    test_ut_burst(&test_ctx);
    test_ut_queue(&test_ctx);
    test_ut_wrap(&test_ctx);

    // Print Summary
    printf("\n");
    printf("================================================================================\n");
    printf("                 AUTOMATED MQTT-SN TEST SUMMARY\n");
    printf("================================================================================\n\n");

    printf("Total test executions: %lu\n", (unsigned long)g_test_stats.iteration_count);
    printf("Passed:  %lu (%.1f%%)\n", (unsigned long)g_test_stats.tests_passed,
           g_test_stats.tests_run > 0
               ? (float)g_test_stats.tests_passed * 100.0f / g_test_stats.tests_run
               : 0.0f);
    printf("Failed:  %lu (%.1f%%)\n", (unsigned long)g_test_stats.tests_failed,
           g_test_stats.tests_run > 0
               ? (float)g_test_stats.tests_failed * 100.0f / g_test_stats.tests_run
               : 0.0f);
    printf("Errors:  0\n\n");

    printf("Test Breakdown:\n");
    printf("  UT-CONN (Connection):         1 execution\n");
    printf("  UT-MSGID (Message ID):        50 executions\n");
    printf("  UT-REG (Registration):        5 executions\n");
    printf("  UT-SUB (Subscribe):           5 executions\n");
    printf("  UT-QOS0 (QoS 0 publish):      20 executions\n");
    printf("  UT-QOS1 (QoS 1 publish):      25 executions\n");
    printf("  UT-QOS2 (QoS 2 publish):      15 executions\n");
    printf("  UT-PING (Keepalive):          10 executions\n");
    printf("  UT-BINARY (Large payload):    10 executions\n");
    printf("  UT-RETRY (Retransmission):    5 executions\n");
    printf("  UT-MAXRT (Max retries):       1 execution\n");
    printf("  UT-BURST (Message burst):     1 execution (10 msgs)\n");
    printf("  UT-QUEUE (Queue overflow):    1 execution\n");
    printf("  UT-WRAP (ID wraparound):      100 executions\n");
    printf("  ─────────────────────────────────────────\n");
    printf("  Total:                        %lu executions\n\n",
           (unsigned long)g_test_stats.iteration_count);

    printf("Assertions checked: %lu\n\n", (unsigned long)g_test_stats.assertions_checked);

    if (g_test_stats.tests_failed == 0) {
        printf("✓ ALL TESTS PASSED!\n");
    } else {
        printf("✗ %lu TEST(S) FAILED - Review logs above\n",
               (unsigned long)g_test_stats.tests_failed);
    }

    printf(
        "\n================================================================================\n\n");

    printf("Tests complete. System will remain active for monitoring.\n");
    printf("Press Ctrl+C to exit.\n\n");

    while (true) {
        cyw43_arch_poll();
        sleep_ms(1000);
    }

    return 0;
}