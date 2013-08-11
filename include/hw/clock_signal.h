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

