#ifndef CLOCK_SIGNAL_H
#define CLOCK_SIGNAL_H

#include "hw/qdev.h"

typedef struct ClockSignalDeviceClass ClockSignalDeviceClass;
typedef struct ClockSignalDevice ClockSignalDevice;
typedef struct ClockSignalSource ClockSignalSource;
typedef struct ClockTreeNode ClockTreeNode;

typedef uint64_t clkfreq;
typedef void ClockSignalDeviceCB(
        ClockSignalDevice *clk,
        clkfreq new_output_freq,
        void *data);

/* Helper functions for creating and modifying clock signal objects */
clkfreq clock_signal_get_output_freq(ClockSignalDevice *clk);
void clock_signal_add_notify(ClockSignalDevice *clk,
                                       ClockSignalDeviceCB *callback,
                                       void *data);
void clock_signal_set_enabled(ClockSignalDevice *clk, bool enabled);
bool clock_signal_is_enabled(ClockSignalDevice *clk);
void clock_signal_set_max_freq(ClockSignalDevice *clk, clkfreq freq);

ClockSignalSource *new_clock_signal_source(Object *parent, const char *name, clkfreq freq, bool enabled);
void clock_signal_source_set_freq(ClockSignalSource *clk, clkfreq freq);

ClockTreeNode *new_clock_tree_node(
                         Object *parent, const char *name,
                         ClockSignalDevice *input_clock,
                         uint32_t mul, uint32_t div,
                         bool enabled);
void clock_node_set_input_clock(ClockTreeNode *clk,
                                 ClockSignalDevice *input_clock);
void clock_node_set_scale(ClockTreeNode *clk, uint32_t mul, uint32_t div);



#define TYPE_CLOCK_SIGNAL_DEVICE "clock-signal-device"
#define CLOCK_SIGNAL_DEVICE(obj) \
     OBJECT_CHECK(ClockSignalDevice, (obj), TYPE_CLOCK_SIGNAL_DEVICE)
#define CLOCK_SIGNAL_DEVICE_CLASS(klass) \
   OBJECT_CLASS_CHECK(ClockSignalDeviceClass, (klass), TYPE_CLOCK_SIGNAL_DEVICE)
#define CLOCK_SIGNAL_DEVICE_GET_CLASS(obj) \
     OBJECT_GET_CLASS(ClockSignalDeviceClass, (obj), TYPE_CLOCK_SIGNAL_DEVICE)

#define TYPE_CLOCK_SIGNAL_SOURCE "clock-signal-source"
#define CLOCK_SIGNAL_SOURCE(obj) \
     OBJECT_CHECK(ClockSignalSource, (obj), TYPE_CLOCK_SIGNAL_SOURCE)

#define TYPE_CLOCK_TREE_NODE "clock-tree-node"
#define CLOCK_TREE_NODE(obj) \
     OBJECT_CHECK(ClockTreeNode, (obj), TYPE_CLOCK_TREE_NODE)

/* Note that the clock signal devices do not have a reset method.
 * They should be reset by their parent device.
 */

struct ClockSignalDeviceClass {
    DeviceClass parent_class;

    clkfreq (*get_output_freq)(ClockSignalDevice *clk);
    void (*add_notify)(
            ClockSignalDevice *clk,
            ClockSignalDeviceCB *callback,
            void *data);
};

#endif

