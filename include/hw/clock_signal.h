#include "hw/qdev.h"

typedef uint64_t clkfreq;
typedef void ClockSignalDeviceCB(
        ClockSignalDevice *clk,
        clkfreq new_output_freq,
        void *data);

/* Helper functions for creating and modifying clock signal objects */
clkfreq clock_signal_device_get_output_freq(ClockTreeNode *clk);
clkfreq clock_signal_device_add_notify(ClockSignalDevice *clk,
                                       ClockSignalDeviceCB callback);
void clock_signal_device_set_enabled(ClockTreeNode *clk, bool enabled);

ClockSignalSource *new_clock_signal_source(clkfreq freq, bool enabled);
void clock_signal_source_set_freq(ClockSignalSource *clk, clkfreq freq);

ClockTreeNode *new_clock_tree_node(
                         ClockSignalDevice *input_clock,
                         uint32_t mult, uint32_t div,
                         bool enabled,
                         clkfreq max_freq);
void clock_tree_node_set_input_clock(ClockTreeNode *clk,
                                 ClockSignalDevice *input_clock);
void clock_tree_node_set_scale(ClockTreeNode *clk, uint32_t mult, uint32_t div);



#define TYPE_CLOCK_SIGNAL_DEVICE "clock-signal-device"
#define CLOCK_SIGNAL_DEVICE(obj) \
     OBJECT_CHECK(ClockSignalDevice, (obj), TYPE_CLOCK_SIGNAL_DEVICE)
#define CLOCK_SIGNAL_DEVICE_CLASS(klass) \
   OBJECT_CLASS_CHECK(ClockSignalDeviceClass, (klass), TYPE_CLOCK_SIGNAL_DEVICE)
#define CLOCK_SIGNAL_DEVICE_GET_CLASS(obj) \
     OBJECT_GET_CLASS(ClockSignalDeviceClass, (obj), TYPE_CLOCK_SIGNAL_DEVICE)

#define TYPE_CLOCK_SIGNAL_SOURCE "clock-signal-source"
#define TYPE_CLOCK_TREE_NODE "clock-tree-node"

typedef ClockSignalDevice ClockSignalDevice;
typedef ClockSignalSource ClockSignalSource;
typedef ClockTreeNode ClockTreeNode;

/* Note that the clock signal devices do not have a rest method.
 * They should be reset by their parent device.
 */

struct ClockSignalDeviceClass {
    DeviceClass parent_class;

    clkfreq (*get_output_freq)(ClockSignalDevice *clk);
    void (*add_notify)(ClockSignalDevice *clk, ClockSignalDeviceCB callback);
} ClockSignalDeviceClass;

