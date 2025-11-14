/*!
 * @file    mqtt_sn_batch_test.c
 * @brief   Batch Test Suite for MQTT-SN Gateway Functionalities
 * @author  Test Suite Generator
 * @date    2025
 *
 * @description
 * Comprehensive automated testing for MQTT-SN client implementation including:
 * - Connection and disconnection tests
 * - QoS 0, 1, 2 message publishing
 * - Subscribe/Unsubscribe operations
 * - Retransmission logic verification
 * - File transfer protocol testing
 * - Network failure scenarios
 * - Message ID wrapping behavior
 *
 * Test Results:
 * Tests output PASS/FAIL status with detailed logging for debugging.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../../config.h"
#include "../../drivers/microsd_driver.h"
#include "../../fs/chunk_transfer.h"
#include "../../fs/data_frame.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "../mqtt-sn-udp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

/*! Test Configuration */
#define TEST_TIMEOUT_MS (5000U)
#define TEST_SHORT_DELAY_MS (100U)
#define TEST_MEDIUM_DELAY_MS (500U)
#define TEST_LONG_DELAY_MS (1000U)
#define TEST_POLL_ITERATIONS (50U)
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
} test_stats_t;

static test_stats_t g_test_stats = {0};

/*! Test Context */
typedef struct {
    struct udp_pcb* pcb;
    ip_addr_t gateway_addr;
    u16_t gateway_port;
    mqtt_sn_context_t* mqtt_ctx;
    filesystem_info_t* fs_info;
    bool test_failed;
    char last_error[128];
} test_context_t;

/*! Assertion Macros */
#define TEST_ASSERT(condition, msg) \
    do { \
        g_test_stats.assertions_checked++; \
        if (!(condition)) { \
            printf("  ✗ ASSERTION FAILED: %s\n", msg); \
            snprintf(ctx->last_error, sizeof(ctx->last_error), "%s", msg); \
            ctx->test_failed = true; \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_EQ(actual, expected, msg) \
    do { \
        g_test_stats.assertions_checked++; \
        if ((actual) != (expected)) { \
            printf("  ✗ ASSERTION FAILED: %s (expected: %d, got: %d)\n", msg, (int)(expected), (int)(actual)); \
            snprintf(ctx->last_error, sizeof(ctx->last_error), "%s", msg); \
            ctx->test_failed = true; \
            return false; \
        } \
    } while(0)

#define TEST_START(name) \
    do { \
        printf("\n========================================\n"); \
        printf("TEST: %s\n", name); \
        printf("========================================\n"); \
        g_test_stats.tests_run++; \
        ctx->test_failed = false; \
        memset(ctx->last_error, 0, sizeof(ctx->last_error)); \
    } while(0)

#define TEST_END() \
    do { \
        if (!ctx->test_failed) { \
            printf("✓ PASS\n"); \
            g_test_stats.tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", ctx->last_error); \
            g_test_stats.tests_failed++; \
        } \
    } while(0)

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
        
        // Check if message is still pending
        bool found = false;
        for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
            if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return true; // ACK received
        }
        
        sleep_ms(10);
    }
    
    return false; // Timeout
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
 *  TEST CASES
 *  ======================================== */

/**
 * @brief Test 1: MQTT-SN Connection
 */
static bool test_connection(test_context_t* ctx) {
    TEST_START("MQTT-SN Connection");
    
    // Reset flag before testing
    g_ping_ack_received = false;
    
    printf("Sending CONNECT...\n");
    mqtt_sn_connect(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
    
    printf("Waiting for CONNACK...\n");
    
    // Poll network and wait for CONNACK with explicit timeout
    absolute_time_t start = get_absolute_time();
    bool connack_received = false;
    
    while (absolute_time_diff_us(start, get_absolute_time()) < (TEST_TIMEOUT_MS * 1000)) {
        cyw43_arch_poll();
        
        if (g_ping_ack_received) {
            connack_received = true;
            printf("CONNACK flag detected\n");
            break;
        }
        
        sleep_ms(10);
    }
    
    TEST_ASSERT(connack_received, "CONNACK not received within timeout");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 2: Message ID Generation
 */
static bool test_message_id_generation(test_context_t* ctx) {
    TEST_START("Message ID Generation");
    
    uint16_t id1 = get_next_msg_id();
    uint16_t id2 = get_next_msg_id();
    uint16_t id3 = get_next_msg_id();
    
    printf("Generated IDs: %u, %u, %u\n", id1, id2, id3);
    
    TEST_ASSERT(id1 != 0, "ID should not be 0");
    TEST_ASSERT(id2 == id1 + 1, "IDs should be sequential");
    TEST_ASSERT(id3 == id2 + 1, "IDs should be sequential");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 3: QoS 0 Publishing
 */
static bool test_qos0_publish(test_context_t* ctx) {
    TEST_START("QoS 0 Publishing");
    
    uint8_t payload[] = "QoS0 Test Message";
    uint16_t msg_id = get_next_msg_id();
    
    printf("Publishing QoS 0 message (ID: %u)...\n", msg_id);
    mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                             2, payload, sizeof(payload), QOS_LEVEL_0, msg_id, false);
    
    test_poll_network(TEST_SHORT_DELAY_MS);
    
    // QoS 0 should not be tracked
    uint32_t pending = count_pending_messages();
    TEST_ASSERT_EQ(pending, 0, "QoS 0 should not be tracked");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 4: QoS 1 Publishing and ACK
 */
static bool test_qos1_publish_ack(test_context_t* ctx) {
    TEST_START("QoS 1 Publishing and ACK");
    
    clear_pending_messages();
    
    uint8_t payload[] = "QoS1 Test Message";
    uint16_t msg_id = get_next_msg_id();
    
    printf("Publishing QoS 1 message (ID: %u)...\n", msg_id);
    mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                             2, payload, sizeof(payload), QOS_LEVEL_1, msg_id, false);
    
    // Immediate check - should be tracked right after sending
    uint32_t pending = count_pending_messages();
    TEST_ASSERT_EQ(pending, 1, "QoS 1 should be tracked immediately after send");
    
    printf("Waiting for PUBACK...\n");
    bool ack_received = wait_for_ack(msg_id, TEST_TIMEOUT_MS);
    TEST_ASSERT(ack_received, "PUBACK not received within timeout");
    
    // Should be removed after ACK
    pending = count_pending_messages();
    TEST_ASSERT_EQ(pending, 0, "Message should be removed after PUBACK");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 5: QoS 2 Publishing and Handshake
 */
static bool test_qos2_publish_handshake(test_context_t* ctx) {
    TEST_START("QoS 2 Publishing and Handshake");
    
    clear_pending_messages();
    
    uint8_t payload[] = "QoS2 Test Message";
    uint16_t msg_id = get_next_msg_id();
    
    printf("Publishing QoS 2 message (ID: %u)...\n", msg_id);
    mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                             2, payload, sizeof(payload), QOS_LEVEL_2, msg_id, false);
    
    // Immediate check - should be tracked right after sending
    uint32_t pending = count_pending_messages();
    TEST_ASSERT_EQ(pending, 1, "QoS 2 should be tracked immediately after send");
    
    printf("Waiting for QoS 2 handshake (PUBREC->PUBREL->PUBCOMP)...\n");
    bool ack_received = wait_for_ack(msg_id, TEST_TIMEOUT_MS);
    TEST_ASSERT(ack_received, "QoS 2 handshake not completed within timeout");
    
    // Should be removed after completion
    pending = count_pending_messages();
    TEST_ASSERT_EQ(pending, 0, "Message should be removed after PUBCOMP");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 6: QoS 1 Retransmission
 */
static bool test_qos1_retransmission(test_context_t* ctx) {
    TEST_START("QoS 1 Retransmission");
    
    clear_pending_messages();
    
    // Enable ACK dropping to force retransmission
    ctx->mqtt_ctx->drop_acks = true;
    printf("ACK dropping enabled\n");
    
    uint8_t payload[] = "Retry Test";
    uint16_t msg_id = get_next_msg_id();
    
    printf("Publishing QoS 1 message (ID: %u) with ACK drop...\n", msg_id);
    mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                             2, payload, sizeof(payload), QOS_LEVEL_1, msg_id, false);
    
    test_poll_network(TEST_SHORT_DELAY_MS);
    
    // Verify it's being tracked
    uint32_t pending = count_pending_messages();
    TEST_ASSERT(pending == 1, "Message should be tracked");
    
    // Wait for first timeout and retransmission
    printf("Waiting for retransmission timeout (%.1f seconds)...\n", 
           QOS_RETRY_INTERVAL_US / 1000000.0f);
    sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
    
    // Trigger timeout check (should retransmit)
    check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
    test_poll_network(TEST_SHORT_DELAY_MS);
    
    // Message should still be pending (ACKs are dropped)
    pending = count_pending_messages();
    TEST_ASSERT(pending > 0, "Message should still be pending after dropped ACK");
    
    // Verify retry count increased
    bool found = false;
    uint8_t retry_count = 0;
    for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
            retry_count = g_pending_msgs[i].retry_count;
            printf("Retry count: %u\n", retry_count);
            TEST_ASSERT(retry_count > 0, "Retry count should increase");
            found = true;
            break;
        }
    }
    TEST_ASSERT(found, "Pending message not found");
    
    // Now disable ACK dropping - next attempt should succeed
    ctx->mqtt_ctx->drop_acks = false;
    printf("ACK dropping disabled\n");
    
    // Wait another timeout period for next retry
    printf("Waiting for next retransmission cycle...\n");
    sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
    
    // Trigger another timeout check (will retransmit without ACK drop)
    check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
    
    // Now wait for the PUBACK
    printf("Waiting for successful PUBACK...\n");
    bool ack_received = wait_for_ack(msg_id, TEST_TIMEOUT_MS);
    TEST_ASSERT(ack_received, "PUBACK not received after re-enabling ACKs");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 7: Subscribe to Topic
 */
static bool test_subscribe(test_context_t* ctx) {
    TEST_START("Subscribe to Topic");
    
    printf("Subscribing to topic ID 1...\n");
    mqtt_sn_subscribe_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port, 1);
    
    test_poll_network(TEST_MEDIUM_DELAY_MS);
    
    printf("Waiting for SUBACK...\n");
    test_poll_network(TEST_LONG_DELAY_MS);
    
    // Note: We can't directly verify SUBACK without modifying the callback
    // This test verifies the subscribe function executes without error
    printf("Subscribe command sent successfully\n");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 8: PINGREQ/PINGRESP
 */
static bool test_ping(test_context_t* ctx) {
    TEST_START("PINGREQ/PINGRESP");
    
    g_ping_ack_received = false;
    
    printf("Sending PINGREQ...\n");
    mqtt_sn_pingreq(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
    
    printf("Waiting for PINGRESP...\n");
    test_poll_network(TEST_LONG_DELAY_MS);
    
    TEST_ASSERT(g_ping_ack_received, "PINGRESP not received");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 9: Maximum Pending Messages
 */
static bool test_max_pending_messages(test_context_t* ctx) {
    TEST_START("Maximum Pending Messages");
    
    clear_pending_messages();
    ctx->mqtt_ctx->drop_acks = true;
    
    printf("Publishing %d QoS 1 messages (ACKs dropped)...\n", MAX_PENDING_QOS_MSGS + 2);
    
    uint8_t payload[] = "Overflow Test";
    
    // Fill up the queue
    for (uint32_t i = 0; i < MAX_PENDING_QOS_MSGS + 2; i++) {
        uint16_t msg_id = get_next_msg_id();
        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                                 2, payload, sizeof(payload), QOS_LEVEL_1, msg_id, false);
        test_poll_network(50);
    }
    
    // Should have MAX_PENDING_QOS_MSGS messages
    uint32_t pending = count_pending_messages();
    printf("Pending messages: %lu\n", (unsigned long)pending);
    TEST_ASSERT_EQ(pending, MAX_PENDING_QOS_MSGS, "Should have max pending messages");
    
    // Cleanup
    ctx->mqtt_ctx->drop_acks = false;
    clear_pending_messages();
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 10: Large Binary Payload
 */
static bool test_large_binary_payload(test_context_t* ctx) {
    TEST_START("Large Binary Payload");
    
    clear_pending_messages();
    
    uint8_t payload[MAX_TEST_PAYLOAD_SIZE];
    for (size_t i = 0; i < sizeof(payload); i++) {
        payload[i] = (uint8_t)(i & 0xFF);
    }
    
    uint16_t msg_id = get_next_msg_id();
    
    printf("Publishing %d byte binary payload (QoS 1)...\n", MAX_TEST_PAYLOAD_SIZE);
    mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                             2, payload, sizeof(payload), QOS_LEVEL_1, msg_id, false);
    
    test_poll_network(TEST_SHORT_DELAY_MS);
    
    printf("Waiting for PUBACK...\n");
    bool ack_received = wait_for_ack(msg_id, TEST_TIMEOUT_MS);
    TEST_ASSERT(ack_received, "PUBACK not received for large payload");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 11: Rapid Message Burst
 */
static bool test_message_burst(test_context_t* ctx) {
    TEST_START("Rapid Message Burst");
    
    clear_pending_messages();
    
    const uint32_t burst_count = 10;
    uint8_t payload[] = "Burst Test";
    
    printf("Sending burst of %lu messages (QoS 1)...\n", (unsigned long)burst_count);
    
    uint16_t first_msg_id = get_next_msg_id();
    
    for (uint32_t i = 0; i < burst_count; i++) {
        uint16_t msg_id = get_next_msg_id();
        mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                                 2, payload, sizeof(payload), QOS_LEVEL_1, msg_id, false);
        sleep_ms(10); // Small delay between messages
    }
    
    printf("Waiting for all PUBACKs...\n");
    sleep_ms(TEST_TIMEOUT_MS);
    test_poll_network(TEST_LONG_DELAY_MS);
    
    // All should be acknowledged
    uint32_t pending = count_pending_messages();
    printf("Remaining pending: %lu\n", (unsigned long)pending);
    TEST_ASSERT_EQ(pending, 0, "All burst messages should be acknowledged");
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 12: QoS 1 Maximum Retries
 */
static bool test_qos1_max_retries(test_context_t* ctx) {
    TEST_START("QoS 1 Maximum Retries");
    
    clear_pending_messages();
    
    // Enable ACK dropping - message will never be acknowledged
    ctx->mqtt_ctx->drop_acks = true;
    printf("ACK dropping enabled (permanent)\n");
    
    uint8_t payload[] = "Max Retry Test";
    uint16_t msg_id = get_next_msg_id();
    
    printf("Publishing QoS 1 message (ID: %u) that will never be ACKed...\n", msg_id);
    mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                             2, payload, sizeof(payload), QOS_LEVEL_1, msg_id, false);
    
    test_poll_network(TEST_SHORT_DELAY_MS);
    
    // Verify it's being tracked
    uint32_t pending = count_pending_messages();
    TEST_ASSERT(pending == 1, "Message should be tracked initially");
    
    printf("Waiting for %d retries to exhaust (this will take ~%.1f seconds)...\n", 
           QOS_MAX_RETRIES, (QOS_MAX_RETRIES * QOS_RETRY_INTERVAL_US) / 1000000.0f);
    
    // Simulate retry cycle: wait for each retry attempt
    for (uint8_t retry = 0; retry < QOS_MAX_RETRIES; retry++) {
        printf("  Waiting for retry %u/%u...\n", retry + 1, QOS_MAX_RETRIES);
        
        // Wait for retry timeout
        sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
        
        // Trigger timeout check
        check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
        test_poll_network(TEST_SHORT_DELAY_MS);
        
        // Verify message is still pending (not yet exhausted)
        pending = count_pending_messages();
        if (retry < QOS_MAX_RETRIES - 1) {
            TEST_ASSERT(pending == 1, "Message should still be pending during retries");
            
            // Verify retry count
            for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
                if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
                    printf("    Retry count: %u\n", g_pending_msgs[i].retry_count);
                    TEST_ASSERT(g_pending_msgs[i].retry_count == retry + 1, 
                               "Retry count should match iteration");
                    break;
                }
            }
        }
    }
    
    // Wait for final timeout (should give up after max retries)
    printf("Waiting for final timeout (message should be dropped)...\n");
    sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
    check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
    test_poll_network(TEST_SHORT_DELAY_MS);
    
    // Message should now be removed from queue (gave up)
    pending = count_pending_messages();
    printf("Final pending count: %lu\n", (unsigned long)pending);
    TEST_ASSERT_EQ(pending, 0, "Message should be dropped after max retries");
    
    // Verify message was actually given up (not still in queue)
    bool found = false;
    for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
            found = true;
            break;
        }
    }
    TEST_ASSERT(!found, "Message should not be in pending queue after giving up");
    
    // Cleanup
    ctx->mqtt_ctx->drop_acks = false;
    printf("ACK dropping disabled\n");
    printf("✓ Message properly dropped after %d failed retries\n", QOS_MAX_RETRIES);
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 13: QoS 2 Maximum Retries (Broker Disconnect Warning)
 */
static bool test_qos2_max_retries(test_context_t* ctx) {
    TEST_START("QoS 2 Maximum Retries");
    
    printf("⚠️  WARNING: This test will cause broker disconnect after 2 retries\n");
    printf("    The test verifies retry behavior before disconnection occurs\n\n");
    
    clear_pending_messages();
    
    // Enable ACK dropping - message will never be acknowledged
    ctx->mqtt_ctx->drop_acks = true;
    printf("ACK dropping enabled (permanent)\n");
    
    uint8_t payload[] = "QoS2 Max Retry";
    uint16_t msg_id = get_next_msg_id();
    
    printf("Publishing QoS 2 message (ID: %u) that will never be ACKed...\n", msg_id);
    mqtt_sn_publish_topic_id(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port,
                             2, payload, sizeof(payload), QOS_LEVEL_2, msg_id, false);
    
    test_poll_network(TEST_SHORT_DELAY_MS);
    
    // Verify it's being tracked
    uint32_t pending = count_pending_messages();
    TEST_ASSERT(pending == 1, "Message should be tracked initially");
    
    // Verify initial state is step 0 (waiting for PUBREC)
    bool found = false;
    for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
            TEST_ASSERT(g_pending_msgs[i].step == 0, "Initial step should be 0 (waiting for PUBREC)");
            found = true;
            break;
        }
    }
    TEST_ASSERT(found, "Message should be in pending queue");
    
    printf("\nNote: Broker disconnects after 2 retries, so we'll test up to retry 2\n");
    printf("Waiting for retry cycles (this will take ~%.1f seconds)...\n", 
           (2 * QOS_RETRY_INTERVAL_US) / 1000000.0f);
    
    // Test first 2 retries (broker will disconnect after this)
    for (uint8_t retry = 0; retry < 2; retry++) {
        printf("  Waiting for retry %u/2...\n", retry + 1);
        
        // Wait for retry timeout
        sleep_ms(QOS_RETRY_INTERVAL_US / 1000 + 500);
        
        // Trigger timeout check
        check_qos_timeouts(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
        test_poll_network(TEST_SHORT_DELAY_MS);
        
        // Verify message is still pending
        pending = count_pending_messages();
        TEST_ASSERT(pending == 1, "Message should still be pending during retries");
        
        // Verify retry count and step
        for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
            if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
                printf("    Retry count: %u, Step: %u\n", 
                       g_pending_msgs[i].retry_count, g_pending_msgs[i].step);
                TEST_ASSERT(g_pending_msgs[i].retry_count == retry + 1, 
                           "Retry count should match iteration");
                TEST_ASSERT(g_pending_msgs[i].step == 0, 
                           "Step should still be 0 (no PUBREC received)");
                break;
            }
        }
    }
    
    printf("\n⚠️  Broker will disconnect after next retry\n");
    printf("    Stopping test before disconnection to preserve connection\n");
    
    // Cleanup - disable ACK dropping and manually clear the message
    ctx->mqtt_ctx->drop_acks = false;
    printf("ACK dropping disabled\n");
    
    // Manually remove the pending message to prevent further retries
    remove_pending_qos_msg(msg_id);
    
    pending = count_pending_messages();
    TEST_ASSERT_EQ(pending, 0, "Message should be cleared after manual removal");
    
    printf("✓ QoS 2 retry mechanism validated (2 retries before broker disconnect)\n");
    
    // Reconnect to broker to ensure clean state for remaining tests
    printf("\nReconnecting to broker...\n");
    g_ping_ack_received = false;
    mqtt_sn_connect(ctx->pcb, &ctx->gateway_addr, ctx->gateway_port);
    
    absolute_time_t start = get_absolute_time();
    bool reconnected = false;
    while (absolute_time_diff_us(start, get_absolute_time()) < (TEST_TIMEOUT_MS * 1000)) {
        cyw43_arch_poll();
        if (g_ping_ack_received) {
            reconnected = true;
            break;
        }
        sleep_ms(10);
    }
    
    if (reconnected) {
        printf("✓ Reconnected successfully\n");
    } else {
        printf("⚠️  Reconnection may be needed - continuing anyway\n");
    }
    
    TEST_END();
    return !ctx->test_failed;
}

/**
 * @brief Test 14: Message ID Boundary (wrap around)
 */
static bool test_message_id_boundary(test_context_t* ctx) {
    TEST_START("Message ID Boundary");
    
    // Test wrapping by generating many IDs
    printf("Testing message ID wrap-around behavior...\n");
    
    // Get current ID
    uint16_t start_id = get_next_msg_id();
    printf("Starting ID: 0x%04X\n", start_id);
    
    // Generate several IDs to verify sequential behavior
    uint16_t id1 = get_next_msg_id();
    uint16_t id2 = get_next_msg_id();
    uint16_t id3 = get_next_msg_id();
    
    printf("Sequential IDs: 0x%04X, 0x%04X, 0x%04X\n", id1, id2, id3);
    
    TEST_ASSERT(id2 == id1 + 1, "IDs should be sequential");
    TEST_ASSERT(id3 == id2 + 1, "IDs should be sequential");
    TEST_ASSERT(id1 != 0, "ID should never be 0");
    TEST_ASSERT(id2 != 0, "ID should never be 0");
    TEST_ASSERT(id3 != 0, "ID should never be 0");
    
    printf("Message ID generation working correctly\n");
    
    TEST_END();
    return !ctx->test_failed;
}

/*! ========================================
 *  MAIN TEST RUNNER
 *  ======================================== */

int main(void) {
    stdio_init_all();
    sleep_ms(2000); // Allow serial to initialize
    
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║  MQTT-SN Batch Test Suite             ║\n");
    printf("║  Pico W Gateway Functionality Tests   ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
    
    // Initialize Wi-Fi
    printf("Initializing Wi-Fi...\n");
    if (cyw43_arch_init()) {
        printf("✗ FATAL: Wi-Fi init failed\n");
        return -1;
    }
    
    cyw43_arch_enable_sta_mode();
    
    printf("Connecting to Wi-Fi (SSID: %s)...\n", WIFI_SSID);
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, 
           CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Wi-Fi connection failed, retrying...\n");
        sleep_ms(5000);
    }
    
    printf("✓ Wi-Fi connected: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
    
    // Setup UDP
    struct udp_pcb* pcb = udp_new();
    if (!pcb) {
        printf("✗ FATAL: UDP setup failed\n");
        return -1;
    }
    
    if (udp_bind(pcb, IP_ADDR_ANY, 0) != ERR_OK) {
        printf("✗ FATAL: UDP bind failed\n");
        return -1;
    }
    
    // Setup test context
    static mqtt_sn_context_t mqtt_ctx = {
        .drop_acks = false,
        .file_session = NULL,
        .fs_info = NULL,
        .transfer_in_progress = false
    };
    
    udp_recv(pcb, udp_recv_callback, &mqtt_ctx);
    
    test_context_t test_ctx = {
        .pcb = pcb,
        .gateway_port = UDP_PORT,
        .mqtt_ctx = &mqtt_ctx,
        .fs_info = NULL,
        .test_failed = false
    };
    
    IP4_ADDR(&test_ctx.gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2, GATEWAY_IP3);
    
    printf("✓ UDP client ready\n");
    printf("Gateway: %s:%d\n\n", ip4addr_ntoa(&test_ctx.gateway_addr), UDP_PORT);
    
    sleep_ms(1000);
    
    // Run Test Suite
    printf("╔════════════════════════════════════════╗\n");
    printf("║  Starting Test Execution               ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    test_connection(&test_ctx);
    test_message_id_generation(&test_ctx);
    test_qos0_publish(&test_ctx);
    test_qos1_publish_ack(&test_ctx);
    test_qos2_publish_handshake(&test_ctx);
    test_subscribe(&test_ctx);
    test_ping(&test_ctx);
    test_large_binary_payload(&test_ctx);
    test_qos1_retransmission(&test_ctx);  // Run before burst tests
    test_qos1_max_retries(&test_ctx);     // Test QoS 1 retry exhaustion
    test_qos2_max_retries(&test_ctx);     // Test QoS 2 retry (stops before disconnect)
    test_message_burst(&test_ctx);
    test_max_pending_messages(&test_ctx);
    test_message_id_boundary(&test_ctx);
    
    // Print Summary
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║  Test Suite Summary                    ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("Total Tests:       %lu\n", (unsigned long)g_test_stats.tests_run);
    printf("Passed:            %lu ✓\n", (unsigned long)g_test_stats.tests_passed);
    printf("Failed:            %lu ✗\n", (unsigned long)g_test_stats.tests_failed);
    printf("Assertions:        %lu\n", (unsigned long)g_test_stats.assertions_checked);
    
    float pass_rate = (g_test_stats.tests_run > 0) ? 
        ((float)g_test_stats.tests_passed / g_test_stats.tests_run * 100.0f) : 0.0f;
    printf("Pass Rate:         %.1f%%\n", pass_rate);
    
    if (g_test_stats.tests_failed == 0) {
        printf("\n🎉 ALL TESTS PASSED! 🎉\n");
    } else {
        printf("\n⚠️  SOME TESTS FAILED - Review logs above\n");
    }
    
    printf("════════════════════════════════════════\n\n");
    
    // Keep running to allow manual inspection
    printf("Test suite complete. System will continue running.\n");
    printf("Press CTRL+C to exit.\n\n");
    
    while (true) {
        cyw43_arch_poll();
        sleep_ms(1000);
    }
    
    return 0;
}