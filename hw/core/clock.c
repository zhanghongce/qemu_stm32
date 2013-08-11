#include "hw/clock.h"
#include "qemu/notify.h
#include "qemu/log.h"

struct ClockSignalDevice {
    /*< private >*/
    DeviceState qdev;
    /*< public >*/

    clkfreq freq, output_freq, max_output_freq;
    bool output_enabled;
    NotifierList output_freq_change_notifiers;
};

static void clk_sig_dev_recalc_output_freq(ClockSignalDevice *clk)
{
    clkfreq old_output_freq = clk->output_freq;
    clk->output_freq = clk->output_enabled ? clk->freq : 0;
    if(clk->output_freq != old_ouput_freq) {
        notifier_list_notify(clk->output_freq_change_notifiers, clk);
    }
}

static void clk_sig_dev_check_max_freq(ClockSignalDevice *clk)
{
    if(clk->max_freq) {
        if(clk->output_freq > clk->max_freq) {
            qemu_log_mask(GUEST_ERROR)
        }
    }
}

static void clk_sig_dev_set_freq(ClockSignalDevice *clk, clkfreq freq)
{
    clk->freq = freq;
    clk_sig_dev_check_max_freq(clk);
    notifier_list_notify(clk->output_freq_change_notifiers, clk);
}

static void clk_sig_dev_get_output_freq(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->max_freq, name, errp);
}

static void clk_sig_dev_get_max_freq(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->max_freq, name, errp);
}

static void clk_sig_dev_set_max_freq(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->max_freq, name, errp);
    clk_sig_dev_check_max_freq(clk);
}

static void clk_sig_dev_get_output_enabled(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->output_enabled, name, errp);
}

static void clk_sig_dev_set_output_enabled(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->output_enabled, name, errp);
    clk_sig_dev_recalc_output_freq(clk);
}

static void clk_sig_dev_instance_init(Object *obj)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(dev);
    clk->freq = 0;
    clk->output_enabled = true;
    clk->output_freq = 0;
    clk->max_output_freq = 0;

    object_property_add(obj, "output-freq", "int",
                        clk_sig_dev_get_output_freq,
                        NULL,
                        NULL, NULL, NULL);
    object_property_add(obj, "max-freq", "int",
                        clk_sig_dev_get_max_freq,
                        clk_sig_dev_set_max_freq,
                        NULL, NULL, NULL);
    object_property_add_bool(obj, "output-enabled",
                             clk_sig_dev_get_output_enabled,
                             clk_sig_dev_set_output_enabled,
                             NULL);
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
