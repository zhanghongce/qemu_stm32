#include "hw/qdev.h"

typedef uint64_t clkfreq;
typedef void ClockSignalDeviceCB(
        ClockSignalDevice *clk,
        clkfreq new_output_freq,
        void *opaque);

/* Helper functions for working with the classes */
void new_

#define TYPE_CLOCK_SIGNAL_DEVICE "clock-signal-device"
#define CLOCK_SIGNAL_DEVICE(obj) \
     OBJECT_CHECK(ClockSignalDevice, (obj), TYPE_CLOCK_SIGNAL_DEVICE)
#define CLOCK_SIGNAL_DEVICE_CLASS(klass) \
   OBJECT_CLASS_CHECK(ClockSignalDeviceClass, (klass), TYPE_CLOCK_SIGNAL_DEVICE)
#define CLOCK_SIGNAL_DEVICE_GET_CLASS(obj) \
     OBJECT_GET_CLASS(ClockSignalDeviceClass, (obj), TYPE_CLOCK_SIGNAL_DEVICE)

#define TYPE_CLOCK_SIGNAL_SOURCE "clock-signal-source"
#define CLOCK_SIGNAL_SOURCE(obj) \
     OBJECT_CHECK(ClockSignalSourceClass, (obj), TYPE_CLOCK_SIGNAL_SOURCE)
#define CLOCK_SIGNAL_SOURCE_CLASS(klass) \
   OBJECT_CLASS_CHECK(ClockSignalSourceClass, (klass), TYPE_CLOCK_SIGNAL_SOURCE)
#define CLOCK_SIGNAL_SOURCE_GET_CLASS(obj) \
     OBJECT_GET_CLASS(ClockSignalSourceClass, (obj), TYPE_CLOCK_SIGNAL_SOURCE)

#define TYPE_CLOCK_TREE_NODE "clock-tree-node"
#define CLOCK_TREE_NODE(obj) \
     OBJECT_CHECK(ClockTreeNodeClass, (obj), TYPE_CLOCK_TREE_NODE)
#define CLOCK_TREE_NODE_CLASS(klass) \
   OBJECT_CLASS_CHECK(ClockTreeNodeClass, (klass), TYPE_CLOCK_TREE_NODE)
#define CLOCK_TREE_NODE_GET_CLASS(obj) \
     OBJECT_GET_CLASS(ClockTreeNodeClass, (obj), TYPE_CLOCK_TREE_NODE)

typedef ClockSignalDevice ClockSignalDevice;
typedef ClockSignalSource ClockSignalSource;
typedef ClockTreeNode ClockTreeNode;

/* Note that the clock signal devices do not have a rest method.
 * They should be reset by their parent device.
 */

struct ClockSignalDeviceClass {
    DeviceClass parent_class;

    clkfreq (*get_output_freq)(ClockSignalDevice *clk);
    void (*set_output_enabled)(ClockSignalDevice *clk, bool output_enabled);
    void (*set_max_freq)(ClockSignalDevice *clk, clkfreq max_freq);
    void (*add_notify)(ClockSignalDevice *clk, )

    /* Protected - should only be called by child classes */
    void (*set_freq)(ClockSignalDevice *clk, clkfreq freq);
} ClockSignalDeviceClass;

struct ClockSignalSourceClass {
    ClockSignalDeviceClass parent_class;

    void (*set_freq)(ClockSignalSource *clk, clkfreq freq);
} ClockSignalSourceClass;

struct ClockTreeNodeClass {
    ClockSignalDeviceClass parent_class;

    void (*set_input_clock)(ClockTreeNode *clk,
                            ClockSignalDevice *input_clk);
    void (*set_scale)(ClockTreeNode *clk, uint32_t mult, uint32_t div);
};
