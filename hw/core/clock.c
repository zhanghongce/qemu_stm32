#include "hw/clock.h"
#include "qemu/notify.h

struct ClockSignalDevice {
    /*< private >*/
    DeviceState qdev;
    /*< public >*/

    clkfreq internal_freq, output_freq, max_internal_freq;
    bool output_enabled;
    NotifierList output_freq_change_notifiers;
};

static clkfreq clk_sig_dev_get_output_freq(ClockSignalDevice *clk)
{
    return clk->output_enabled ? clk->freq : 0;
}

static void clk_sig_dev_recalc_output_freq(ClockSignalDevice *clk)
{
    notifier_list_notify(clk->output_freq_change_notifiers);
}

static void clk_sig_dev_set_output_enabled(ClockSignalDevice *clk, bool output_enabled)
{
    clk->output_enabled = output_enabled;
    clk_sig_dev_recalc_output_freq(clk);

}

static void clk_sig_dev_check_max_freq(ClockSignalDevice *clk)
{
    if(clk->max_freq) {
        if(clk->freq > clk->max_freq) {

        }
    }
}

static void clk_sig_dev_set_max_freq(ClockSignalDevice *clk, clkfreq max_freq)
{
    clk->max_freq = max_freq;
    clk_sig_dev_check_max_freq(clk);
}

static void clk_sig_dev_set_freq(ClockSignalDevice *clk, clkfreq freq)
{
    clk->freq = freq;
    clk_sig_dev_check_max_freq(clk);
    notifier_list_notify(clk->output_freq_change_notifiers);
}

static void clk_sig_dev_instance_init(Object *obj)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(dev);
    clk->freq = 0;
    clk->max_freq = 0;
    clk->output_enabled = true;
}

static void clk_sig_dev_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *k = DEVICE_CLASS(klass);
    ClockSignalDeviceClass *clkclass = CLOCK_SIGNAL_DEVICE_CLASS(klass);
    clkclass->get_output_freq = clock_signal_device_get_output_freq;
    set_output_enabled = clock_signal_device_set_output_enabled
    set_max_freq = check_max_freq
    set_freq
}

static const TypeInfo clock_signal_device_info = {
    .name = TYPE_CLOCK_SIGNAL_DEVICE,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(ClockSignalDevice),
    .instance_init = clk_sig_dev_instance_init,
    .abstract = true,
    .class_size = sizeof(ClockSignalDeviceClass),
    .class_init = clock_signal_device_class_init,
};



struct ClockSignalSource {
    /*< private >*/
    ClockSignalDevice clk;
    /*< public >*/
};

struct ClockTreeNode {
    /*< private >*/
    ClockSignalDevice clk;
    /*< public >*/

    clkfreq input_freq;
    uint32_t multiplier, divisor;

    ClockSignalDevice *input_clock;
};
