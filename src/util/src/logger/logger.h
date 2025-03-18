#pragma once

// 临时实现
#define LogDebug(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define LogInfo(fmt, ...) printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define LogWarn(fmt,...) printf("[WARN] " fmt "\n", ##__VA_ARGS__)
#define LogError(fmt,...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)