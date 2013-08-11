#include "hw/clock_signal.h"
#include "qemu/notify.h
#include "qemu/log.h"

struct ClockSignalDevice {
    /*< private >*/
    DeviceState qdev;
    /*< public >*/

    clkfreq freq, output_freq, max_output_freq;
    bool output_enabled;
    NotifierList output_change_notifiers;
};

static void clk_sig_dev_recalc_output_freq(ClockSignalDevice *clk)
{
    clkfreq old_output_freq = clk->output_freq;
    clk->output_freq = clk->output_enabled ? clk->freq : 0;
    if(clk->output_freq != old_ouput_freq) {
        notifier_list_notify(clk->output_change_notifiers, clk);
    }
}

static void clk_sig_dev_check_max_freq(ClockSignalDevice *clk)
{
    if(clk->max_freq) {
        if(clk->output_freq > clk->max_freq) {
            qemu_log_mask(GUEST_ERROR,
                    "Clock Signal Device: Clock %s exceeded its maximum"
                    "frequency: output freq=%llu max freq=%llu\n",
                    object_get_canonical_path(OBJECT(clk)),
                    (unsigned long long)clk->output_freq,
                    (unsigned long long)clk->max_freq)
        }
    }
}

/* This is essentially a protected method.  This is to be used by
 * derived classes to set the clock's frequency.
 */
static void clk_sig_dev_set_freq(ClockSignalDevice *clk, clkfreq freq)
{
    clk->freq = freq;
    clk_sig_dev_check_max_freq(clk);
    clk_sig_dev_recalc_output_freq(vlk);
}

static clkfreq clk_sig_dev_get_output_freq(ClockSignalDevice *clk)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    return clk->output_freq;
}

typedef struct ClockSignalNotifier {
    Notifier n;
    ClockSignalDeviceCB callback;
    void *data;
} ClockSignalNotifier;

static clk_sig_dev_output_change_notify_handler(Notifier *notifier, void *data)
{
    ClockSignalDevice clk = (ClockSignalDevice *)data;
    ClockSignalNotifier *csn = DO_UPCAST(ClockSignalNotifier, n, notifier);
    csn->callback(clk, clk->output_freq, csn->data);
}

static clkfreq clk_sig_dev_add_notify(ClockSignalDevice *clk,
                                      ClockSignalDeviceCB callback,
                                      void *opaque)
{
    ClockSignalNotifier *csn = g_malloc0(sizeof(ClockSignalNotifier));
    csn->n.notify = clk_sig_dev_output_change_notify_handler;
    csn->callback = callback;
    csn->data = data;
    notifier_list_add(&clk->output_change_notifiers, csn->n)
}

static void clk_sig_dev_get_output_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->max_freq, name, errp);
}

static void clk_sig_dev_get_max_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->max_freq, name, errp);
}

static void clk_sig_dev_set_max_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->max_freq, name, errp);
    clk_sig_dev_check_max_freq(clk);
}

static void clk_sig_dev_get_enabled_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->output_enabled, name, errp);
}

static void clk_sig_dev_set_enabled_prop(Object *obj, struct Visitor *v, void *opaque,
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
    notifier_list_init(&clk->output_change_notifiers);

    object_property_add(obj, "output-freq", "int",
                        clk_sig_dev_get_output_freq_prop,
                        NULL,
                        NULL, NULL, NULL);
    object_property_add(obj, "max-freq", "int",
                        clk_sig_dev_get_max_freq_prop,
                        clk_sig_dev_set_max_freq_prop,
                        NULL, NULL, NULL);
    object_property_add_bool(obj, "enabled",
                             clk_sig_dev_get_enabled_prop,
                             clk_sig_dev_set_enabled_prop,
                             NULL);
}

static void clk_sig_dev_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *k = DEVICE_CLASS(klass);
    ClockSignalDeviceClass *clkclass = CLOCK_SIGNAL_DEVICE_CLASS(klass);
    clkclass->get_output_freq = clk_sig_dev_get_output_freq;
    clkclass->add_notify = clk_sig_dev_add_notify;
}

static const TypeInfo clock_signal_device_info = {
    .name = TYPE_CLOCK_SIGNAL_DEVICE,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(ClockSignalDevice),
    .instance_init = clk_sig_dev_instance_init,
    .abstract = true,
    .class_size = sizeof(ClockSignalDeviceClass),
    .class_init = clk_sig_dev_class_init,
};






static void clk_sig_src_get_source_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint32(v, &clk->freq, name, errp);
}

static void clk_sig_src_set_source_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    clkfreq new_freq;
    visit_type_uint32(v, &new_freq, name, errp);
    clk_sig_dev_set_freq(clk, new_freq);
}

static void clk_sig_src_instance_init(Object *obj)
{
    object_property_add(obj, "source-freq", "int",
                        clk_sig_src_get_source_freq_prop,
                        clk_sig_src_set_source_freq_prop,
                        NULL, NULL, NULL);
}

static const TypeInfo clock_signal_source_info = {
    .name = TYPE_CLOCK_SIGNAL_SOURCE,
    .parent = TYPE_CLOCK_SIGNAL_DEVICE,
    .instance_size = sizeof(ClockSignalDevice),
    .instance_init = clk_sig_src_instance_init,
    .class_size = sizeof(ClockSignalDeviceClass),
};




struct ClockTreeNode {
    /*< private >*/
    ClockSignalDevice clk;
    /*< public >*/

    clkfreq input_freq;
    uint32_t multiplier, divisor;

    ClockSignalDevice *input_clock;
};

static void clk_tree_node_instance_init(Object *obj)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(dev);
    clk->input_freq = 0;
    clk->multiplier = 1;
    clk->divisor = 1;
    clk->input_clock = NULL;

    object_property_add(obj, "multiplier", "int",
                        clk_sig_dev_get_mult_prop,
                        clk_sig_dev_set_mult_prop,
                        NULL, NULL, NULL);
    object_property_add(obj, "divider", "int",
                        clk_sig_dev_get_max_freq_prop,
                        clk_sig_dev_set_max_freq_prop,
                        NULL, NULL, NULL);
    // We need to be able to trigger the frequency to be updated when
    // the input clock link is changed.  However, the link functionality
    // doesn't seem to allow custom setters.  For now just use a function
    // to do the input clock update.
    //object_property_add_link(obj, "input-clock", TYPE_CLOCK_SIGNAL_DEVICE,
    //                         (Object **)&s->input_clock, NULL);
}

static const TypeInfo clock_tree_node_info = {
    .name = TYPE_CLOCK_TREE_NODE,
    .parent = TYPE_CLOCK_SIGNAL_DEVICE,
    .instance_size = sizeof(ClockTreeNode),
    .instance_init = clk_tree_node_instance_init,
    .class_size = sizeof(ClockSignalDeviceClass),
};




static void clock_signal_register_types(void)
{
    type_register_static(&clock_signal_device_info);
    type_register_static(&clock_signal_source_info);
    type_register_static(&clock_tree_node_info);
}

type_init(clock_signal_register_types)
