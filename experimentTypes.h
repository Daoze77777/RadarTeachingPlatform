#ifndef EXPERIMENTTYPES_H
#define EXPERIMENTTYPES_H

// 实验类型枚举
enum ExperimentType {
    EXP_NONE = 0,

    // 测距原理实验
    EXP_PULSE_METHOD = 1,         // 脉冲法实验
    EXP_PULSE_METHOD_TEST = 2,    // 脉冲法测试实验
    EXP_FREQUENCY_MODULATION = 3, // 调频法实验
    EXP_FREQUENCY_MODULATION_TEST = 4, // 调频法测试实验
    EXP_PHASE_METHOD = 5,         // 相位法实验
    EXP_PHASE_METHOD_TEST = 6,    // 相位法测试实验
    EXP_RANGE_AMBIGUITY = 7,      // 距离退模糊实验
    EXP_RANGE_AMBIGUITY_TEST = 8, // 距离退模糊测试实验
    EXP_RANGE_TRACKING = 9,       // 距离跟踪实验
    EXP_RANGE_TRACKING_TEST = 10, // 距离跟踪测试实验

    // 测角原理实验
    EXP_ANGLE_PHASE = 101,        // 相位法测角实验
    EXP_ANGLE_PHASE_TEST = 102,   // 相位法测角测试实验
    EXP_ANGLE_AMPLITUDE = 103,    // 振幅法测角实验
    EXP_ANGLE_AMPLITUDE_TEST = 104, // 振幅法测角测试实验
    EXP_ANGLE_TRACKING = 105,     // 角度自动跟踪实验
    EXP_ANGLE_TRACKING_TEST = 106, // 角度自动跟踪测试实验
    EXP_MONOPULSE = 107,          // 单脉冲测角实验
    EXP_MONOPULSE_TEST = 108,     // 单脉冲测角测试实验
    EXP_PHASED_ARRAY = 109,       // 相控阵天线模拟实验
    EXP_PHASED_ARRAY_TEST = 110,  // 相控阵天线模拟测试实验

    // 测速原理实验
    EXP_VELOCITY_AVERAGE = 201,   // 平均法测速实验
    EXP_VELOCITY_AVERAGE_TEST = 202, // 平均法测速测试实验
    EXP_VELOCITY_INSTANT = 203,   // 瞬时测速实验
    EXP_VELOCITY_INSTANT_TEST = 204, // 瞬时测速测试实验
    EXP_VELOCITY_AMBIGUITY = 205, // 脉冲雷达速度模糊实验
    EXP_VELOCITY_AMBIGUITY_TEST = 206, // 脉冲雷达速度模糊测试实验
    EXP_VELOCITY_DEAMBIGUITY = 207, // 脉冲雷达解速度模糊实验
    EXP_VELOCITY_DEAMBIGUITY_TEST = 208 // 脉冲雷达解速度模糊测试实验
};

#endif // EXPERIMENTTYPES_H
